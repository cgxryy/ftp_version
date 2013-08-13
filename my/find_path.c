//函数名：find_path
//描述：找到文件绝对路径的上级目录
//参数：file绝对路径，path上级目录
//返回值：无
#include <string.h>

void find_path(char *file, char *path)
{
	char *move = path;

	strcpy(path ,file);
	while (*move != '\0')
	      move++;
	while (*move != '/')
	      move--;
	*move = '\0';
}
