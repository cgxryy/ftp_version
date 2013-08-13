//函数名：left_batch
//描述：提取后缀
//参数：string完整文件名，batch提取出的后缀名
//返回值：无
#include <string.h>

void left_batch(char *string, char *batch)
{
	char *move;

	while (*move != '\0')
	      move++;
	while (*move != '.')
	      move--;
	strcpy(batch, move);
}
