#include<iostream>
#include<string>
#include<fstream>
#include"sphere.h"
#include"hitable_list.h"
#include"material.h"
#include"camera.h"

using namespace std; //��random.hͷ�ļ���ʹ�ù�һ��namespace std �˴��Ƿ��ظ��� �ǵ� �����ظ�ʹ�������ռ�

const float max = 0x3f3f3f3f;

vec3 color(const ray &r,hitable *world, int depth){
	hit_record rec;
	//��СֵΪɶҪ��Ϊ0.001?
	if (world->hit(r, 0.001, max, rec)){ //hit����world�б������е����� ����¼����Ĺ��߽���
		ray scattered;//ɢ����� �ڵݹ�Ĺ����л�һֱ�ı�
		vec3 attenuation;//����˥����(��������) �ڷ��ص�ʱ�������� 
		//����scatter���õĹ�����ȡ���ض����ʵ�ֵ
		//�˴��ǵ��ԵĹؼ� ������Hitable���ж���һ��material* ����hitable_list�е�hit������¼material
		//�����rec.mat_ptr����������
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)){
			return attenuation*color(scattered, world, depth + 1);//����ʱ˥�� attenuationΪ˥���� һ����ڲ��ʵķ�����
		}
		else{
			return vec3(0, 0, 0); //�����ݹ���� ֱ�ӷ���һ����ɫ���ص�
		}
	}
	else{ //���ݹ���ӳ�䵽��Ļ��y�����Բ�ֵ��ɫ ������ɫ
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5*(unit_direction.y() + 1.0);
		return (1.0 - t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);  
	}
}

//������ɳ���
hitable *random_scene(){
	int n = 500;
	hitable **list = new hitable*[n + 1];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(vec3(0.5, 0.5, 0.5))); //���������������
	int i = 1;
	for (int a = -11; a < 11; a++){
		for (int b = -11; b < 11; b++){
			float choose_mat = Rand(e); //����һ��������ʵ������
			vec3 center(a + 0.9*Rand(e), 0.2, b + 0.9*Rand(e)); //ͬһˮƽ���� ����ǰ�����
			if ((center - vec3(4.0, 0.2, 2.0)).length() > 0.9){ //����ĳһ���㹻Զ�ĵط�
				if (choose_mat < 0.8){ //��������� ���ִ��ֲ��ʵĸ������
					list[i++] = new sphere
						(center, 0.2,  //���ġ��뾶������(����ϵ��)
						new lambertian(vec3( Rand(e)*Rand(e), Rand(e)*Rand(e), Rand(e)*Rand(e) ))
						);
				}
				else if(choose_mat<0.95){ //��������
					list[i++] = new sphere
						(center, 0.2,  //���ġ��뾶������(����ϵ��)��ģ��ϵ��
						new metal( vec3( 0.5*(1 + Rand(e)), 0.5*(1 + Rand(e)), 0.5*(1 + Rand(e))), 0.5*Rand(e) )
						);
				}
				else{ //��������(�����)
					list[i++] = new sphere(center, 0.2, new dielectric(1.5));
				}
			}
		}
	}
	//�м� ��� �ұߵĴ��� �ֱ��ǵ���ʡ��ƹ�ͽ�����
	list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
	list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
	list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

	return new hitable_list(list, i);
}

int main()
{
	ofstream file("����ͼ.ppm");
	if (file.is_open()){
		int nx = 800;
		int ny = 400;
		int ns = 100; //����� ʹ�ò������� ����100����
		file << "P3\n" << nx << " " << ny << "\n255\n";
		hitable *world = random_scene();
		//��������ȷ�����߷���
		vec3 lookfrom(13, 2, 3); 
		vec3 lookat(0, 0, 0);
		float dist_to_focus = (lookfrom - lookat).length(); //ȷ������
		float aperture = 0.0;//��Ȧ��С(ֱ��) ��ȦֵԽ�� ɢ��ģ��Ч��Խ����
		camera cam(lookfrom,lookat,vec3(0,1,0),20,float(nx)/float(ny),aperture,0.7*dist_to_focus); //ȷ�����
		for (int j = ny - 1; j >= 0; j--){ //j,iѭ�������þ��Ǵ��ϵ��£��������ҵ�һ�����ص� ӳ�䵽��Ļ�ϵ�ÿһ������
			for (int i = 0; i < nx; i++){  //��Ϊ���������������ppm�ļ��ķֱ��� ����Ļ��ʵ�ʽ�ģ�е���ά�ռ��е�һ����Ļ 
				vec3 col(0, 0, 0);         //ͨ��i/nx,j/ny����ӳ�䵽(ͨ��ƫ��������Ļ�ߴ�)��Ļ�ϴ��ϵ��� �����ҵġ�ÿһ�����ߡ�
				//����ÿ��С�������100����	//����ppm�ļ��Ǵ��ϵ��¡������Ҷ�ȡ��ɫ ����j��i����ʼ��Ҳ���ݴ�˳�������� ���ҵ���Ӧ�Ĺ���
				for (int s = 0; s < ns; s++){ 
					float u = float(i+Rand(e)) / float(nx);
					float v = float(j+Rand(e)) / float(ny);
					ray r = cam.get_ray(u, v);
					col = col + color(r, world, 0); //�ۼ���ɫֵ ���ƽ��
				}
				col = col/float(ns);
				col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));  //٤��У��
				int ir = int(255.99*col.e[0]);  //��0-1ӳ�䵽0-255����ɢֵ
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