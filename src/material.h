#ifndef MATERIAL_H
#define MATERIAL_H

#include"hitable.h" // �˴�Ӧ�ð���������������
#include"ray.h"
#include"random.h"

class material{ //���� ������� ֻ��һ��scatterɢ�䷽�� 
public:
	virtual bool scatter
		(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered) const = 0;
};

//�ڵ�λ�����������һ���� ���Բ�������ķ������
vec3 random_in_unit_sphere(){
	vec3 p;
	do{
		//�������һ����������������-1��1�ĵ�(����)
		p = 2.0*vec3(Rand(e), Rand(e), Rand(e)) - vec3(1, 1, 1);
	} while (dot(p, p) >= 1.0); //��֤�ڵ�λ���Ȱ뾶��������
	return p;
}

//�ʲ���������ģ�ͣ��ƹ���ʣ� 
class lambertian :public material{
public:
	lambertian(const vec3 &a) :albedo(a){}
	virtual bool scatter(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered) const 
	{
		vec3 target = rec.p + rec.normal + random_in_unit_sphere();
		scattered = ray(rec.p, target - rec.p); //�������  ��ʵ�˴��Ĺ��߷������ֱ������Ϊrec.normal+random_in_unit_sphere()
		attenuation = albedo; //����ʱ��¼˥���� 
		return true;
	}
	vec3 albedo; //������ ����˵���ߵ�˥���� ��һ�ڿ��ǲ���50% 
};

//���淴�亯��
vec3 reflect(const vec3 &v, const vec3 &n){ //�������(�ķ���) ƽ�淨����
	return v - 2 * dot(v, n) * n;
}

//���淴��ģ�ͣ��������ʣ�
class metal :public material{
public:
	metal(const vec3 &a,float f = 0):albedo(a){ 
		if (f < 0) fuzz = 0;
		else if (f < 1) fuzz = f;
		else fuzz = 1;
	}
	virtual bool scatter(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered) const {
		//*���������ڹ��߼��ĺ����� �Ѿ�ʵ���˷������ĵ�λ����
		//*���Դ˴�����Է��������д��� ֻҪ����������ߵĵ�λ��
		vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);//��÷�����ߵķ���
		//��ʱreflected������ߵķ��������Ѿ��Ǹ���λ������
		scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere()); //����������ƫ��һ��� ���ģ��Ч��
		//����ģ��ϵ�� ������߽��������ƫ�� 
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0); 
		//��ʽ���� ���������ߺͷ�����ͬ�� ��������ߴ�ֱ���䵽���� ������߻�ԭ·���� 
		//�Ͳ��ᷴ�䵽������������ ���=0 ����false
	}

	vec3 albedo;
	float fuzz ; //����ģ��ϵ�� ����Խ�� ģ��ϵ��Խ��
};

//��������� ������������� ����true ���򷵻�false ������ȫ����
bool refract(const vec3 &v, const vec3 &n, float ni_over_nt, vec3 &refracted){ //ni_over_nt���������
	vec3 uv = unit_vector(v);//������ߵ�λ��
	float dt = dot(uv, n); 
	float discriminant = 1.0 - ni_over_nt*ni_over_nt*(1 - dt*dt);
	//�������������С���� ˵��������ߵķ���������ʵ�� ��û�г���������� ���ֵ���ȫ����
	if (discriminant > 0){ 
		refracted = ni_over_nt*(uv - n*dt) - n*sqrt(discriminant);
		return true;
	}
	else//ȫ����
		return false;
}

//�����ʽ�����⹫ʽ ����cosineֵ����������ʼ���
float schlick(float cosine, float ref_idx){
	float r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0*r0;
	return r0 + (1 - r0)*pow((1 - cosine), 5);
}

//����ʲ���
class dielectric :public material{
public:
	dielectric(float ri) :ref_idx(ri){}
	virtual bool scatter(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered) const{
		vec3 outward_normal;  //������ⷨ��
		vec3 reflected = reflect(r_in.direction(), rec.normal); //ȡ�÷�����ߵķ�������
		float ni_over_nt; //��������� �������ʵ�������/�������ʵ�������
		attenuation = vec3(1.0, 1.0, 1.0); //���ڵ���� �������չ���(˥��) ֻ�ᷴ�������
		vec3 refracted; //�������
		float reflect_prob; //������
		float cosine; //�������㷴���ʵ�cos���� �����if elseֻ��Ϊ�������������µ�cosine �Լ����������ni_over_nt
		if (dot(r_in.direction(), rec.normal) > 0){ //�ӽ��ʽ������
			outward_normal = -rec.normal;
			ni_over_nt = ref_idx;  //���������Ϊ����������/1.0��������
			cosine = dot(r_in.direction(), rec.normal) / r_in.direction().length();
		}
		else{ //�����������
			outward_normal = rec.normal;
			ni_over_nt = 1.0 / ref_idx;
			cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();//cosȡ��ֵ
		}
		//ʹ��������õ�outward_normal��cosine�����������ni_over_nt�����������
		if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)){//�����������
			//���㾵��ķ����� ��ͬ��������߻��в�ͬ�ķ�����(���������ѡ���ǲ��÷�����߻����������)
			reflect_prob = schlick(cosine, ref_idx);
		}
		else{//����ȫ����
			reflect_prob = 1.0; //ȫ����ķ�����Ϊ1 û��������ɵ�������ʧ
		}
		//���в���ѡȡ ÿ������ֻѡ����������� �����ǽ����߻�� ���������ѡȡ�� �������Ȼ�Ƿ�������ĺ�����
		if (Rand(e) < reflect_prob){ //�������һ��0-1���� ��С�ڷ����� ���÷������ ��������Խ�� Խ���ܷ������� ������ԽС Խ���ܷ������� ������Ϊ1��ʱ�� ����ȫ���� ��ֻ���з������ ��������ߣ�
			scattered = ray(rec.p, reflected); //������Ѿ�������ȫ���� ��Ϊreflected_prob=1
		}
		else{ //���� �����������
			scattered = ray(rec.p, refracted);
		}
		return true;
	}

	float ref_idx; //���ʵ�������
};

#endif