#ifndef HITABLE_LIST_H
#define HITABLE_LIST_H

#include"hitable.h"

class hitable_list :public hitable{
public:
	hitable_list(){}
	hitable_list(hitable **l, int n) :list(l), list_size(n){}
	virtual bool hit(const ray &r, float tmin, float tmax, hit_record &rec) const;
	hitable **list;
	int list_size;
};
//hitable_list�е�hit���� �ǵ���ÿ��hitable��Ա�����hit���� ����¼����ĵ㵽rec�� ��û������ ����false
bool hitable_list::hit(const ray &r, float t_min, float t_max, hit_record &rec) const{
	hit_record temp_rec;
	bool hit_anything = false;
	float closest_so_far = t_max;
	for (int i = 0; i < list_size; i++){ //�����б��е�ÿһ������ �ø������߼��
		if (list[i]->hit(r, t_min, closest_so_far, temp_rec)){
			hit_anything = true;
			closest_so_far = temp_rec.t;
			rec = temp_rec;
			rec.mat_ptr = list[i]->mt; //�ؼ����� ����ʱ��¼�����е�Ĳ��� 
		}
	}
	return hit_anything;
}

#endif