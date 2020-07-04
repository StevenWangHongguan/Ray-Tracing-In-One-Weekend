#ifndef MATERIAL_H
#define MATERIAL_H

#include"hitable.h" // 此处应该包含还是声明处理？
#include"ray.h"
#include"random.h"

class material{ //材质 抽象基类 只有一个scatter散射方法 
public:
	virtual bool scatter
		(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered) const = 0;
};

//在单位球中随机产生一个点 借以产生随机的反射光线
vec3 random_in_unit_sphere(){
	vec3 p;
	do{
		//随机生成一个三个分量都介于-1到1的点(向量)
		p = 2.0*vec3(Rand(e), Rand(e), Rand(e)) - vec3(1, 1, 1);
	} while (dot(p, p) >= 1.0); //保证在单位长度半径的球体内
	return p;
}

//朗波特漫反射模型（哑光材质） 
class lambertian :public material{
public:
	lambertian(const vec3 &a) :albedo(a){}
	virtual bool scatter(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered) const 
	{
		vec3 target = rec.p + rec.normal + random_in_unit_sphere();
		scattered = ray(rec.p, target - rec.p); //反射光线  其实此处的光线方向可以直接设置为rec.normal+random_in_unit_sphere()
		attenuation = albedo; //反射时记录衰减率 
		return true;
	}
	vec3 albedo; //反射率 或者说光线的衰减率 上一节课是采用50% 
};

//镜面反射函数
vec3 reflect(const vec3 &v, const vec3 &n){ //入射光线(的方向) 平面法向量
	return v - 2 * dot(v, n) * n;
}

//镜面反射模型（金属材质）
class metal :public material{
public:
	metal(const vec3 &a,float f = 0):albedo(a){ 
		if (f < 0) fuzz = 0;
		else if (f < 1) fuzz = f;
		else fuzz = 1;
	}
	virtual bool scatter(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered) const {
		//*由于我们在光线检测的函数中 已经实现了法向量的单位化了
		//*所以此处无需对法向量进行处理 只要处理入射光线的单位化
		vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);//获得反射光线的方向
		//此时reflected反射光线的方向向量已经是个单位向量了
		scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere()); //反射光线随机偏移一点点 造成模糊效果
		//根据模糊系数 反射光线进行随机的偏移 
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0); 
		//上式代表 如果反射光线和法向量同向 即入射光线垂直照射到镜面 反射光线会原路返回 
		//就不会反射到其他物体上了 点积=0 返回false
	}

	vec3 albedo;
	float fuzz ; //镜面模糊系数 球体越大 模糊系数越大
};

//求折射光线 若存在折射光线 返回true 否则返回false 代表是全反射
bool refract(const vec3 &v, const vec3 &n, float ni_over_nt, vec3 &refracted){ //ni_over_nt相对折射率
	vec3 uv = unit_vector(v);//入射光线单位化
	float dt = dot(uv, n); 
	float discriminant = 1.0 - ni_over_nt*ni_over_nt*(1 - dt*dt);
	//根号里面的内容小于零 说明折射光线的方向向量无实根 即没有出现折射光线 出现的是全反射
	if (discriminant > 0){ 
		refracted = ni_over_nt*(uv - n*dt) - n*sqrt(discriminant);
		return true;
	}
	else//全反射
		return false;
}

//反射率近似求解公式 根据cosine值和相对折射率计算
float schlick(float cosine, float ref_idx){
	float r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0*r0;
	return r0 + (1 - r0)*pow((1 - cosine), 5);
}

//电介质材质
class dielectric :public material{
public:
	dielectric(float ri) :ref_idx(ri){}
	virtual bool scatter(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered) const{
		vec3 outward_normal;  //交点的外法线
		vec3 reflected = reflect(r_in.direction(), rec.normal); //取得反射光线的方向向量
		float ni_over_nt; //相对折射率 入射光介质的折射率/折射光介质的折射率
		attenuation = vec3(1.0, 1.0, 1.0); //对于电解质 不会吸收光线(衰减) 只会反射和折射
		vec3 refracted; //折射光线
		float reflect_prob; //反射率
		float cosine; //用来计算反射率的cos参数 下面的if else只是为了求出两种情况下的cosine 以及相对折射率ni_over_nt
		if (dot(r_in.direction(), rec.normal) > 0){ //从介质进入空气
			outward_normal = -rec.normal;
			ni_over_nt = ref_idx;  //相对折射率为介质折射率/1.0（空气）
			cosine = dot(r_in.direction(), rec.normal) / r_in.direction().length();
		}
		else{ //空气进入介质
			outward_normal = rec.normal;
			ni_over_nt = 1.0 / ref_idx;
			cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();//cos取正值
		}
		//使用上面求得的outward_normal，cosine和相对折射率ni_over_nt来求折射光线
		if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)){//存在折射光线
			//计算镜面的反射率 不同的入射光线会有不同的反射率(后续会随机选择是采用反射光线还是折射光线)
			reflect_prob = schlick(cosine, ref_idx);
		}
		else{//出现全反射
			reflect_prob = 1.0; //全反射的反射率为1 没有折射造成的能量损失
		}
		//进行采样选取 每次我们只选择反射或者折射 而不是将二者混合 由于是随机选取的 最后结果仍然是反射折射的合理混合
		if (Rand(e) < reflect_prob){ //随机产生一个0-1的数 若小于反射率 采用反射光线 （反射率越大 越可能发生反射 反射率越小 越可能发生折射 反射率为1的时候 代表全反射 则只会有反射光线 无折射光线）
			scattered = ray(rec.p, reflected); //此情况已经包含了全反射 因为reflected_prob=1
		}
		else{ //否则 采用折射光线
			scattered = ray(rec.p, refracted);
		}
		return true;
	}

	float ref_idx; //介质的折射率
};

#endif