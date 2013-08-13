//函数名：path_deal
//描述：剔除原绝对路径的/home/chang,把后面的接到/home/chang/changgong后面
//参数：path需要变换的路经
//返回值：无
#include "my.h"
void path_deal(char *path)
{
	char *move = path + 11;//指针移动到chang的后面
	char temp[256];
	
	strcpy(temp, move);
	strcpy(path, "/home/chang/changgong");
	strcat(path, move);

	printf("此时路径为%s\n", path);
}
