#ifndef VEC3_H
#define VEC3_H

#include<math.h>
#include<stdlib.h>
#include<iostream>

class vec3{
public:
	vec3(){}
	vec3(float e0, float e1, float e2){ e[0] = e0; e[1] = e1; e[2] = e2; }

	float x(){ return e[0]; }
	float y(){ return e[1]; }
	float z(){ return e[2]; }
	float r(){ return e[0]; }
	float g(){ return e[1]; }
	float b(){ return e[2]; }

	const vec3& operator+() const { return *this; }// '+'号
	vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }//'-'号
	float operator[](int i) const { return e[i]; }
	float operator[](int i) { return e[i]; }

	vec3& operator+=(const vec3 &v2);
	vec3& operator-=(const vec3 &v2);
	vec3& operator*=(const vec3 &v2);
	vec3& operator/=(const vec3 &v2);
	vec3& operator*=(const float &t);
	vec3& operator/=(const float &t);

	float length() const { return sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]); }
	float squared_length()const { return e[0] * e[0] + e[1] * e[1] + e[2] * e[2]; }
	void make_unit_vector();//向量归一化

	float e[3];
};
inline vec3 operator+(const vec3 &v1, const vec3 &v2){
	return vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}
inline vec3 operator-(const vec3 &v1, const vec3 &v2){
	return vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}
//乘除法用于颜色的运算 不可用于位置运算
inline vec3 operator*(const vec3 &v1, const vec3 &v2){
	return vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
}
inline vec3 operator/(const vec3 &v1, const vec3 &v2){
	return vec3(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
}
//向量与实数的乘除法
inline vec3 operator*(const vec3 &v1, float t){
	return vec3(v1.e[0] *t, v1.e[1] * t, v1.e[2] * t);
}
inline vec3 operator*(float t, const vec3 &v1){
	return v1*t;
}
inline vec3 operator/(const vec3 &v1, float t){
	return vec3(v1.e[0] / t, v1.e[1] / t, v1.e[2] / t);
}
//几何运算 点积叉积
inline float dot(const vec3 &v1, const vec3 &v2){
	return v1.e[0] * v2.e[0]+ v1.e[1] * v2.e[1]+ v1.e[2] * v2.e[2];
}
inline vec3 cross(const vec3 &v1, const vec3 &v2){
	return vec3( (v1.e[1]*v2.e[2]-v1.e[2]*v2.e[1]),
		(-(v1.e[0]*v2.e[2]-v1.e[2]*v2.e[0])),
		(v1.e[0]*v2.e[1]-v1.e[1]*v2.e[0]) );
}
inline vec3 unit_vector(const vec3 &v){
	return v / v.length();
}
void vec3::make_unit_vector(){
	e[0] /= length();
	e[1] /= length();
	e[2] /= length();
}
#endif