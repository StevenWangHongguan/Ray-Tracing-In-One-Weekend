#ifndef CAMERA_H
#define CAMERA_H

#include"ray.h"
#include"vec3.h"

const float M_PI = acos(-1);

//取得以光线源点为圆心的一个单位圆盘上的随机点 模仿镜头(光圈)的作用
vec3 random_in_unit_disk(){
	vec3 p;
	do{
		p = 2.0*vec3(Rand(e), Rand(e), 0) - vec3(1, 1, 0); 
	} while (dot(p, p) >= 1.0);
	return p;
}

//相机类
class camera{
public:
	//vfov vertical view of field 相机在垂直方向上的俯仰角大小 aspect是屏幕的宽高比
	//lookfrom代表相机源点 looat表示视线方向的一个点 vup是世界坐标系中y轴向上的单位向量
	//aperture代表光圈(镜头)的直径 focus_dist代表焦距(光源到成像平面的距离) 其实就是原来相机离屏幕的距离dis
	camera(vec3 lookfrom,vec3 lookat,vec3 vup,float vfov,float aspect,float aperture,float focus_dist){
		lens_radius = aperture / 2;
		float theta = vfov*M_PI/ 180; //将角度转化为弧度制
		float half_height = tan(theta / 2); //屏幕的半高
		float half_width = aspect*half_height; //屏幕的半宽
		origin = lookfrom;
		//使用uvw坐标系 获得新的屏幕的平面
		w = unit_vector(lookfrom - lookat);
		u = unit_vector(cross(vup, w));
		v = cross(w, u);
		//-w是向视线方向移动一个单位向量 以下是新的屏幕坐标 以及偏移量(包含方向和焦距)
		lower_left_corner = origin - half_width*focus_dist*u - half_height*focus_dist*v - focus_dist * w;  //此条语句没有理解 为什么新的屏幕的左下角位置要多乘上focus_dis
		//其实 高度half_height和half_width就已经要乘上一个焦距focus_dist了 因为原来默认是深度是1没有乘 如果深度(焦距)不是1 就应该在上面的half_height处乘上
		//也就是说 焦距不一样 屏幕的长度和宽度也不一样了
		horizontal = 2*half_width*focus_dist*u;
		vertical = 2*half_height*focus_dist*v;
	}
	ray get_ray(float s, float t){ //传递0-1的参数s,t进行偏移 取得每一条光线
		vec3 rd = lens_radius*random_in_unit_disk(); //光圈的半径乘以单位圆盘上随机的一个方向 得到偏移的位置
		vec3 offset = u*rd.x() + v*rd.y();//转移到相机的平面上进行偏移
		return ray(origin+offset, lower_left_corner+s*horizontal + t*vertical-origin-offset);
	}
	vec3 origin; //相机源点
	vec3 lower_left_corner; //屏幕左下角
	vec3 horizontal;//水平偏移量
	vec3 vertical;//垂直偏移量
	vec3 u, v, w; //用单位正交基来表示相机平面 u水平方向 v竖直方向 w为视线方向 即平面法向量的负向
	float lens_radius;//光圈半径
};

#endif