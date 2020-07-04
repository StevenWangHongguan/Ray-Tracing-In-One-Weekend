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

//可检测到(命中)的物体 有自己的检测函数 同时拥有自己的材质
class hitable{
public:
	hitable() = default;
	hitable(material* m) :mt(m){}
	virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const = 0;
	//抽象基类中的数据成员
	material* mt ; //材质
};

#endif