//函数名：file_exist
//描述：检查某个文件名在某个目录下是否存在
//参数：file目录，name文件名
//返回值：有返回1，无返回0
#include "my.h"

int  file_exist(char *file,char *name)
{
	DIR 		*dir;
	struct dirent 	*into;
	int 		flag = 0;

	dir = opendir(file);
	if (dir == NULL)
	      my_err("opendir", __LINE__);

	while ((into = readdir(dir)) != NULL)
	{
		if (strcmp(into->d_name, name) == 0)
		      flag = 1;
	}

	closedir(dir);
	if (flag == 0)
	      return 0;
	else 
	      return 1;
}


