//函数名：pick_name
//描述：从一个存有绝对路径的字符串中提取文件名出来
//参数：file绝对路径，name文件名
//返回值：无
#include <string.h>

void pick_name(char *file, char *name)
{
	char 	*move = file;
	while (*move != '\0')
	      move++;
	while (*move != '/')
	      move--;
	move++;
	strcpy(name, move);
}


