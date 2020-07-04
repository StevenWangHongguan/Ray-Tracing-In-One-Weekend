#ifndef SPHRE_H
#define SHERE_H

#include"hitable.h" //hitable头文件包含一个光线命中的记录 以及一个hitable抽象类

class sphere :public hitable{ //此处没声明继承方式 默认是私有继承 果然需要声明为public才正确
public:
	sphere(){}
	//此构造函数关键 m初始化父类的material
	sphere(vec3 cen, float r,material* m) :center(cen), radius(r), hitable(m){}
	virtual bool hit(const ray &r, float tmin, float tmax, hit_record &rec) const;
	vec3 center;
	float radius;
	
};

bool sphere::hit(const ray &r, float tmin, float tmax, hit_record &rec) const {
	vec3 oc = r.origin() - center; //A-C 光线源点向量-球体圆心向量 获得光源指向圆心的向量
	float a = dot(r.direction(), r.direction());
	float b = 2.0 * dot(oc,r.direction());
	float c = dot(oc, oc) - radius*radius;
	float discriminant = b*b - 4*a*c; //判别式
	if (discriminant > 0){
		float temp = (-b - sqrt(discriminant)) / (2.0*a); //较小的根t1
		if (temp > tmin && temp < tmax){
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);//交点位置
			rec.normal = (rec.p - center) / radius; //获得单位法向量
			return true;
		}
		temp = (-b + sqrt(discriminant)) / (2.0*a);
		if (temp > tmin && temp < tmax){
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - center) / radius; 
			return true;
		}
	}
	return false; //达到此处 要么判别式小于0 要么两个点的解都不符合要求
}

#endif