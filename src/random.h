#include<random>
using namespace std;
default_random_engine e; //默认的随机数生成引擎
uniform_real_distribution<float> Rand(0, 1);//随机数分布 类型是实数 范围为0-1