//函数名：depart_file
//描述：把一维字符数组中的字符串以空格、换行符为界分割存入二维字符数组
//参数：buf一维字符数组，filename二维字符数组
//返回值：无

void depart_file(char *buf, char filename[][256])
{
	char 	*move = buf;
	int 	i = 0, j = 0;

	while (*move != '\0')
	{
		if (*move == '\t' || *move == ' ')
		{
			filename[i][j] = '\0';
			i++;
			j = 0;
			move++;
		}
		filename[i][j] = *move;
		move++;
		j++;
	}
}

