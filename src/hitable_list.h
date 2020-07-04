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
//hitable_list中的hit方法 是调用每个hitable成员自身的hit方法 并记录最近的点到rec中 若没有命中 返回false
bool hitable_list::hit(const ray &r, float t_min, float t_max, hit_record &rec) const{
	hit_record temp_rec;
	bool hit_anything = false;
	float closest_so_far = t_max;
	for (int i = 0; i < list_size; i++){ //对于列表中的每一个物体 用该条光线检测
		if (list[i]->hit(r, t_min, closest_so_far, temp_rec)){
			hit_anything = true;
			closest_so_far = temp_rec.t;
			rec = temp_rec;
			rec.mat_ptr = list[i]->mt; //关键步骤 命中时记录下命中点的材质 
		}
	}
	return hit_anything;
}

#endif