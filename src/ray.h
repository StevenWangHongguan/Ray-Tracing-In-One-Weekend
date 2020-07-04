#ifndef RAY_H
#define RAY_H
#include"vec3.h"

class ray{
public:
	ray(){}
	ray(const vec3 &a, const vec3 &b) { A = a; B = b; }
	vec3 origin() const { return A; }
	vec3 direction() const { return B; }
	vec3 point_at_parameter(float t) const { return A + t*B; }
	//讲道理这两个数据成员应该声明为private的
	vec3 A;
	vec3 B;

};
#endif