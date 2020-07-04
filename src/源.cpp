#include<iostream>
#include<string>
#include<fstream>
#include"sphere.h"
#include"hitable_list.h"
#include"material.h"
#include"camera.h"

using namespace std; //在random.h头文件中使用过一次namespace std 此处是否重复？ 是的 不可重复使用命名空间

const float max = 0x3f3f3f3f;

vec3 color(const ray &r,hitable *world, int depth){
	hit_record rec;
	//最小值为啥要改为0.001?
	if (world->hit(r, 0.001, max, rec)){ //hit检索world列表中所有的物体 并记录最近的光线交点
		ray scattered;//散射光线 在递归的过程中会一直改变
		vec3 attenuation;//光线衰减率(三个方向) 在返回的时候起作用 
		//并在scatter调用的过程中取得特定材质的值
		//此处是调试的关键 若不在Hitable类中定义一个material* 并在hitable_list中的hit方法记录material
		//这里的rec.mat_ptr根本不存在
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)){
			return attenuation*color(scattered, world, depth + 1);//回溯时衰减 attenuation为衰减度 一般等于材质的反射率
		}
		else{
			return vec3(0, 0, 0); //超过递归次数 直接返回一个黑色像素点
		}
	}
	else{ //根据光线映射到屏幕的y轴线性插值混色 即背景色
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5*(unit_direction.y() + 1.0);
		return (1.0 - t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);  
	}
}

//随机生成场景
hitable *random_scene(){
	int n = 500;
	hitable **list = new hitable*[n + 1];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(vec3(0.5, 0.5, 0.5))); //代表地面的最大球体
	int i = 1;
	for (int a = -11; a < 11; a++){
		for (int b = -11; b < 11; b++){
			float choose_mat = Rand(e); //生成一个代表材质的随机数
			vec3 center(a + 0.9*Rand(e), 0.2, b + 0.9*Rand(e)); //同一水平线上 左右前后放置
			if ((center - vec3(4.0, 0.2, 2.0)).length() > 0.9){ //距离某一点足够远的地方
				if (choose_mat < 0.8){ //漫反射材质 出现此种材质的概率最大
					list[i++] = new sphere
						(center, 0.2,  //球心、半径、材质(反射系数)
						new lambertian(vec3( Rand(e)*Rand(e), Rand(e)*Rand(e), Rand(e)*Rand(e) ))
						);
				}
				else if(choose_mat<0.95){ //金属材质
					list[i++] = new sphere
						(center, 0.2,  //球心、半径、材质(反射系数)、模糊系数
						new metal( vec3( 0.5*(1 + Rand(e)), 0.5*(1 + Rand(e)), 0.5*(1 + Rand(e))), 0.5*Rand(e) )
						);
				}
				else{ //玻璃材质(电解质)
					list[i++] = new sphere(center, 0.2, new dielectric(1.5));
				}
			}
		}
	}
	//中间 左边 右边的大球 分别是电解质、哑光和金属类
	list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
	list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
	list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

	return new hitable_list(list, i);
}

int main()
{
	ofstream file("封面图.ppm");
	if (file.is_open()){
		int nx = 800;
		int ny = 400;
		int ns = 100; //抗锯齿 使用采样处理 采样100个点
		file << "P3\n" << nx << " " << ny << "\n255\n";
		hitable *world = random_scene();
		//由两个点确定视线方向
		vec3 lookfrom(13, 2, 3); 
		vec3 lookat(0, 0, 0);
		float dist_to_focus = (lookfrom - lookat).length(); //确定焦距
		float aperture = 0.0;//光圈大小(直径) 光圈值越大 散焦模糊效果越明显
		camera cam(lookfrom,lookat,vec3(0,1,0),20,float(nx)/float(ny),aperture,0.7*dist_to_focus); //确定相机
		for (int j = ny - 1; j >= 0; j--){ //j,i循环的作用就是从上到下，从左到右找到一个像素点 映射到屏幕上的每一条光线
			for (int i = 0; i < nx; i++){  //因为像素是相对于生成ppm文件的分辨率 而屏幕是实际建模中的三维空间中的一个屏幕 
				vec3 col(0, 0, 0);         //通过i/nx,j/ny可以映射到(通过偏移量和屏幕尺寸)屏幕上从上到下 从左到右的“每一条光线”
				//对于每个小方格采样100个点	//由于ppm文件是从上到下、从左到右读取颜色 所以j与i的起始量也根据此顺序来设置 以找到对应的光线
				for (int s = 0; s < ns; s++){ 
					float u = float(i+Rand(e)) / float(nx);
					float v = float(j+Rand(e)) / float(ny);
					ray r = cam.get_ray(u, v);
					col = col + color(r, world, 0); //累计颜色值 最后平均
				}
				col = col/float(ns);
				col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));  //伽马校正
				int ir = int(255.99*col.e[0]);  //将0-1映射到0-255的离散值
				int ig = int(255.99*col.e[1]);
				int ib = int(255.99*col.e[2]);
				file << ir << " " << ig << " " << ib << "\n";
			}
		}
		file.close();
		cout << "complished" << endl;
	}
	else cerr << "error" << endl;

	return 0;
}