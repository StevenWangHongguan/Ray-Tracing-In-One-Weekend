#ifndef CAMERA_H
#define CAMERA_H

#include"ray.h"
#include"vec3.h"

const float M_PI = acos(-1);

//ȡ���Թ���Դ��ΪԲ�ĵ�һ����λԲ���ϵ������ ģ�¾�ͷ(��Ȧ)������
vec3 random_in_unit_disk(){
	vec3 p;
	do{
		p = 2.0*vec3(Rand(e), Rand(e), 0) - vec3(1, 1, 0); 
	} while (dot(p, p) >= 1.0);
	return p;
}

//�����
class camera{
public:
	//vfov vertical view of field ����ڴ�ֱ�����ϵĸ����Ǵ�С aspect����Ļ�Ŀ�߱�
	//lookfrom�������Դ�� looat��ʾ���߷����һ���� vup����������ϵ��y�����ϵĵ�λ����
	//aperture�����Ȧ(��ͷ)��ֱ�� focus_dist������(��Դ������ƽ��ľ���) ��ʵ����ԭ���������Ļ�ľ���dis
	camera(vec3 lookfrom,vec3 lookat,vec3 vup,float vfov,float aspect,float aperture,float focus_dist){
		lens_radius = aperture / 2;
		float theta = vfov*M_PI/ 180; //���Ƕ�ת��Ϊ������
		float half_height = tan(theta / 2); //��Ļ�İ��
		float half_width = aspect*half_height; //��Ļ�İ��
		origin = lookfrom;
		//ʹ��uvw����ϵ ����µ���Ļ��ƽ��
		w = unit_vector(lookfrom - lookat);
		u = unit_vector(cross(vup, w));
		v = cross(w, u);
		//-w�������߷����ƶ�һ����λ���� �������µ���Ļ���� �Լ�ƫ����(��������ͽ���)
		lower_left_corner = origin - half_width*focus_dist*u - half_height*focus_dist*v - focus_dist * w;  //�������û����� Ϊʲô�µ���Ļ�����½�λ��Ҫ�����focus_dis
		//��ʵ �߶�half_height��half_width���Ѿ�Ҫ����һ������focus_dist�� ��Ϊԭ��Ĭ���������1û�г� ������(����)����1 ��Ӧ���������half_height������
		//Ҳ����˵ ���಻һ�� ��Ļ�ĳ��ȺͿ��Ҳ��һ����
		horizontal = 2*half_width*focus_dist*u;
		vertical = 2*half_height*focus_dist*v;
	}
	ray get_ray(float s, float t){ //����0-1�Ĳ���s,t����ƫ�� ȡ��ÿһ������
		vec3 rd = lens_radius*random_in_unit_disk(); //��Ȧ�İ뾶���Ե�λԲ���������һ������ �õ�ƫ�Ƶ�λ��
		vec3 offset = u*rd.x() + v*rd.y();//ת�Ƶ������ƽ���Ͻ���ƫ��
		return ray(origin+offset, lower_left_corner+s*horizontal + t*vertical-origin-offset);
	}
	vec3 origin; //���Դ��
	vec3 lower_left_corner; //��Ļ���½�
	vec3 horizontal;//ˮƽƫ����
	vec3 vertical;//��ֱƫ����
	vec3 u, v, w; //�õ�λ����������ʾ���ƽ�� uˮƽ���� v��ֱ���� wΪ���߷��� ��ƽ�淨�����ĸ���
	float lens_radius;//��Ȧ�뾶
};

#endif