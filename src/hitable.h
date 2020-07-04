#ifndef HITABLE_H
#define HITABLE_H

#include"ray.h"

class material;
struct hit_record{
	float t;
	vec3 p;
	vec3 normal;
	material* mat_ptr;
};

//�ɼ�⵽(����)������ ���Լ��ļ�⺯�� ͬʱӵ���Լ��Ĳ���
class hitable{
public:
	hitable() = default;
	hitable(material* m) :mt(m){}
	virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const = 0;
	//��������е����ݳ�Ա
	material* mt ; //����
};

#endif