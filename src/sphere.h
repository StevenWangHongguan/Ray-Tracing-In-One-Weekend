#ifndef SPHRE_H
#define SHERE_H

#include"hitable.h" //hitableͷ�ļ�����һ���������еļ�¼ �Լ�һ��hitable������

class sphere :public hitable{ //�˴�û�����̳з�ʽ Ĭ����˽�м̳� ��Ȼ��Ҫ����Ϊpublic����ȷ
public:
	sphere(){}
	//�˹��캯���ؼ� m��ʼ�������material
	sphere(vec3 cen, float r,material* m) :center(cen), radius(r), hitable(m){}
	virtual bool hit(const ray &r, float tmin, float tmax, hit_record &rec) const;
	vec3 center;
	float radius;
	
};

bool sphere::hit(const ray &r, float tmin, float tmax, hit_record &rec) const {
	vec3 oc = r.origin() - center; //A-C ����Դ������-����Բ������ ��ù�Դָ��Բ�ĵ�����
	float a = dot(r.direction(), r.direction());
	float b = 2.0 * dot(oc,r.direction());
	float c = dot(oc, oc) - radius*radius;
	float discriminant = b*b - 4*a*c; //�б�ʽ
	if (discriminant > 0){
		float temp = (-b - sqrt(discriminant)) / (2.0*a); //��С�ĸ�t1
		if (temp > tmin && temp < tmax){
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);//����λ��
			rec.normal = (rec.p - center) / radius; //��õ�λ������
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
	return false; //�ﵽ�˴� Ҫô�б�ʽС��0 Ҫô������Ľⶼ������Ҫ��
}

#endif