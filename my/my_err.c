//函数名：my_err
//描述：错误提示并退出
//参数：string错误的函数名，line错误的位置
//返回值：无
#include "my.h"
void my_err(char *string,int line)
{
	fprintf(stderr,"line: %d ",line);
	perror(string);
	exit(1);
}

