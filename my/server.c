// =====================================================================================
// 
//       Filename:  mysocket.c
//
//    Description:  
//
//        Version:  1.0
//        Created:  2013年07月30日 19时43分40秒
//       Revision:  cgxr
//       Compiler:  gcc
//
//         Author:  hanyaorong, cgxryy@gmail.com
//        Company:  Class 1201 of Computer Network Technology
// 
// =====================================================================================


#include "my.h"
#include "server.h"

void log_err(char *string,int line)
{
	int 	fd;
	char 	buf[256];
	int 	ret;
	char 	now[256];

	getcwd(now, 256);
	chdir("/home/chang/changgong");
	fd = open("error_log.txt", O_RDWR|O_CREAT|O_APPEND, 0664);

	fprintf(stderr,"line: %d ",line);
	ret = sprintf(buf, "line: %d %s error\n",line, string);
	write(fd, buf, ret);
	perror(string);

	close(fd);
	chdir(now);
	exit(1);
}

int main(int argc, char *argv[])
{
	int 			client_fd;
	socklen_t 	 	client_len;
	int 			pid;
	int 			optval;
	int 			socket_fd;
	struct sockaddr_in 	ser_addr;
	struct sockaddr_in 	cli_addr;

	//初始化套接字
	memset(&ser_addr, 0, sizeof(struct sockaddr_in));
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(4507);
	ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	//创建
	socket_fd = socket(AF_INET,SOCK_STREAM,0);
	if (socket_fd < 0)
	{
		log_err("socket",__LINE__);
	}
	
	 //设置套接字，使可以重新绑定端口
	optval = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (void*)&optval, sizeof(int)) < 0)
	log_err("setsockopt",__LINE__);
	
	//套接字绑定
	if (bind(socket_fd, (struct sockaddr *)&ser_addr, sizeof(struct sockaddr_in)) < 0)
		log_err("bind",__LINE__);

	//监听
	if (listen(socket_fd, 128) < 0)
	      log_err("listen",__LINE__);

	//接受连接
	client_len = sizeof(struct sockaddr_in);
	while (1)
	{	
		client_fd = accept(socket_fd, (void*)&cli_addr,&client_len);	
		if (client_fd < 0)	      
		      log_err("accept",__LINE__);
	
		printf("有个客户端请求连接,ip:%s\n",inet_ntoa(cli_addr.sin_addr));
		//子进程处理连接请求
		if ((pid = fork()) == 0)
		{
			printf("已连接到客户端，正在子进程处理...\n");
			server(client_fd, &cli_addr);//处理函数，处理接发数据
		}
	}

	close(socket_fd);
	return 0;

}

void sys_log(struct sockaddr_in *cli_addr, unsigned int cmd_num,int flag)
{
	time_t 		t;
	struct tm 	*timenow;
	char 		cmd[10];
	int 		fd;
	char 		buf[256];
	int 		ret;
	char 		now[256];

	getcwd(now, 256);
	chdir("/home/chang/changgong");

	fd = open("sys_log.txt", O_RDWR|O_CREAT|O_APPEND, 0664);
	switch (cmd_num)
	{
		case LS:
			strcpy(cmd, "ls");
			break;
		case GET:
			strcpy(cmd, "get");
			break;
		case PUT:
			strcpy(cmd, "put");
			break;
		case QUIT:
			strcpy(cmd, "quit");
			break;
		case CD:
			strcpy(cmd, "cd");
			break;
	}

	time(&t);
	timenow = localtime(&t);
 	if (flag == 0)
	{
		ret = sprintf(buf, "%s %s connected us.\n", asctime(timenow), inet_ntoa(cli_addr->sin_addr));
		write(fd, buf, ret);
	}
	else
	{
		ret = sprintf(buf, "%s input %s\n", inet_ntoa(cli_addr->sin_addr), cmd);
	write(fd, buf, ret);
	}

	close(fd);
	chdir(now);
}

int server(int client_fd, struct sockaddr_in *cli_addr)
{
	int 		size;
	unsigned int 	cmd_num;
	char 		*file;
	char 		cmd_path[256] = {"/home/chang/changgong"};
	
	sys_log(cli_addr, cmd_num, 0);
	
	while (1)
	{ 	
		//接收命令序号
		size = recv(client_fd, (char*)&cmd_num, sizeof(unsigned int), 0);
		
		cmd_num = ntohl(cmd_num);
		sys_log(cli_addr, cmd_num, 1);
		
		printf("接收到数据%d字节",size);
		if (size < 0)
			log_err("recv",__LINE__);
		if (size == 0)
		{
			printf("客户端断开连接...\n");
			return -1;
		}
		if (cmd_num == QUIT)
		{
			printf("客户端主动断开连接...\n"); 
			return -1;
		}	
		
		file = malloc(256);
		memset(file, 0, 256);
		
		//接收命令行命令和参数
		size = recv(client_fd, file, 256, 0);
		if (size < 256)
		      log_err("recv", __LINE__);
		printf("接受到文件名数据%d字节\n",size);
		printf("客户端输入的%s\n", (char*)file);
		printf("序号为%u\n", cmd_num);

		command_choice(client_fd, cmd_num, file, cmd_path);

		free(file);
	}
}

void command_choice(int client_fd, unsigned int command, char *file, char *cmd_path)
{
	printf("进入command_choice()...\n");
	switch (command)
	{
		case LS:	
			command_ls(client_fd, file, cmd_path);
			break;
		case GET:
			command_get(client_fd, file, cmd_path);
			break;
		case PUT:
			command_put(client_fd, file, cmd_path);
			break;
		case CD:
			command_cd(client_fd, file, cmd_path);
			break;
	}
}

int command_cd(int client_fd, char *file, char *cmd_path)
{
	char 	temp[256][256] = {"\0"};
	int 	ret; 
	char 	now[256];
	char 	cd[256];
	char 	name[256];//传给客户端用来改命令行显示

	strcpy(cd, cmd_path);
		
	getcwd(now, 256);
	
	//拆分整个命令行
	depart_file(file, temp);
	//从[1]开始，因为[0]是ls/get/put...
	if (strncmp(temp[1], "..", 2) == 0)
	{	
		if (strcmp(cmd_path, "/home/chang/changgong") == 0)
		      printf("已经是ftp根目录...\n");
		else
		      find_path(cmd_path, cd);
	}
	else if (strcmp(temp[1], "/") == 0)
	      strcpy(cd, "/home/chang/changgong");
	else
	{
		strcat(cd, "/");
		strcat(cd, temp[1]);
	}
	

	printf("cd切换的目录的是%s\n", cd);
	ret = chdir(cd);
	if (ret == -1)
	{ 
		printf("没有这个目录，无法切换...\n");
		return 0;
	}
	else
	     	chdir(now);

	find_cd(cd, name);
	ret = send(client_fd, name, 256, 0);
	if (ret == -1)
	      log_err("cd send", __LINE__);
	strcpy(cmd_path, cd);//将处理完的cd目录交给cmd_path
	
	return 1;
}

void find_cd(char *cmd_path, char *name)
{
	char 	admin[256];
	char 	*move = admin;
	int 	admin_len;

	move = getlogin();
	admin_len = strlen(move);
	move = cmd_path + 6 + admin_len;//指针移动到/home/admin/处
	strcpy(name, move);
}

void command_ls(int client_fd, char *file, char *cmd_path)
{
	DIR 		*dir;
	struct dirent 	*into;
	char 		filename[1024];
	int 		i = 0;
	int 		ret;
	char 		workspace[256] = "/home/chang/cgxr/my";//程序运行目录
	char 		space[256];

	printf("进入command_ls()...\n");
	memset((char*)&filename, '\0', (size_t)sizeof(filename));

	getcwd(workspace, 256);
	printf("cd后处理路径%s\n",cmd_path);

	find_path(cmd_path, space);
	chdir(space);
	dir = opendir(cmd_path);
	if (dir == NULL)
	      my_err("opendir",__LINE__);
	
	//读取文件名写入一维数组
	while ((into = readdir(dir)) != NULL)
	{
		strcat(filename, into->d_name);
		strcat(filename, "\t");
		i++;
	}
	
	//发送一维数组过去
	printf("开始发送...\n");
	ret = send(client_fd, (char *)&filename, 1024, 0);
	if (ret == -1)
	      log_err("my_send", __LINE__);
	if (ret < sizeof(filename))
	      printf("数据发送不完整...错误...");
	
	chdir(workspace);
	closedir(dir);
}

void command_get(int client_fd, char *file, char *cmd_path)
{
	char 	filename[256][256] = {"\0"};
	char 	batch[80][10] = {"\0"};
	int 	i;
	char 	batchsign[10];
	char 	normalsign[10];
	char 	end[10];

	printf("进入command_get()...\n");
	depart_file(file, filename);
	for (i = 1; filename[i][0] != '\0'; i++)
	{
		if (strncmp(filename[i], "*.", 2) != 0)	
		{
			memset(normalsign, 'n', 10);
			sleep(1);
			send(client_fd, normalsign, 10, 0);
			sleep(1);
			send(client_fd, filename[i], 256, 0);
			sleep(1);
			my_get(client_fd, filename[i], cmd_path);
		} 
		else
		{
			memset(batchsign, 'b', 10);
			sleep(1);
			send(client_fd, batchsign, 10, 0);
			batch_get(client_fd, filename[i], batch, cmd_path);
		}
	}
	
	memset(end, '\t', 10);
	sleep(1);
	send(client_fd, end, 10, 0);
	printf("已发送结束标志过去.......\n");

}

void batch_get(int client_fd, char file[], char batch[][10], char *cmd_path)
{
	printf("进入batch_get()\n");
	find_batch(batch, file);
	match_batch(client_fd, batch, cmd_path);
}

int find_batch(char batch[][10], char file[])
{
	char 	temp_batch[10];
	int 	i = 0, j = 0;//i是batch专用
	
	printf("进入find_batch()\n");
	left_batch(file, temp_batch);

	//若batch其中有这种后缀则不存入
/*	while (batch[j][0] != '\0')
	{
		if (strcmp(batch[j], temp_batch) == 0)
	      	{
		      flag = 1;
		      break;
		}
		j++;
	}
*/	
	while (batch[i][0] != '\0')
	      i++;
	strcpy(batch[i], temp_batch);
	
	return 1;
}

void match_batch(int client_fd, char batch[][10], char *cmd_path)
{
	DIR 		*dir;
	struct dirent 	*into;
	struct stat 	info;
	char 		former_path[256];
	char 		*move;
	int 		i;//循环batch专用
	char 		batchfile[256] = "\0";
	
	printf("进入match_batch()...\n");
	getcwd(former_path, 256);
	chdir(cmd_path);

	if ((dir = opendir(cmd_path)) == NULL)
	      log_err("opendir", __LINE__);

	while ((into = readdir(dir)) != NULL)
	{
		if (lstat(into->d_name, &info) == -1)
		      log_err("lstat", __LINE__);

		if (S_ISDIR(info.st_mode))
		      continue;//匹配文件后缀时跳过目录
		else
		{
			move = into->d_name;
			while (*move != '\0')
			      move++;
			while (*move != '.')
			      move--;
			printf("into->d_name:%s\tmove:%s\n", into->d_name, move);
			for ( i = 0; batch[i][0] != '\0'; i++)
			{	
				if (strcmp(batch[i], move) == 0)
				{
					printf("成功进入比较发送阶段()...\n");
					memset(batchfile, '\0', 256);
					strcpy(batchfile, into->d_name);
					sleep(1);
					send(client_fd, batchfile, 256, 0);
					sleep(1);
					my_get(client_fd, into->d_name, cmd_path);
				}
			}
		}
	}
	memset(batchfile, '\t', 256);
	sleep(1);
	send(client_fd, batchfile, 256, 0);
	chdir(former_path);
	closedir(dir);
}

void my_get(int client_fd, char *file, char *cmd_path)
{
	int 	send_ret = 1024;
	int 	read_ret = 1024;//每次实际发送大小,为了循环第一次判断初值为大于0的值
	int  	file_fd;//打开的文件描述符
	char 	former_path[256];
	char 	*receive = former_path;
	char 	*buf;
	double 	sum = 0.0;
	double 	local_sum = 0.0;

	buf = malloc(1024);

	printf("进入my_get()...\n");
	
	//保存之前工作路径
	receive = getcwd(receive, 256);
	if (receive == NULL)
	      log_err("getcwd", __LINE__);
	
	//转换到文件所在目录
	chdir(cmd_path);
	printf("已经转到%s目录\n", cmd_path);

	//开始读出总大小
	file_fd = open(file, O_RDWR, 0644);
	if (file_fd == -1)
	{
		printf("找不到\n");
	}
	while (read_ret > 0)
	{
		read_ret = read(file_fd, buf, 1024);
		sum += read_ret;
	}
	close(file_fd);
	send(client_fd, &sum, 8, 0);
	sleep(1);

	//开始发送
	file_fd = open(file, O_RDWR, 0644);
	if (file_fd == -1)
	      log_err("open", __LINE__);
	
	//全部发送完时ret为0
	while (1)
	{
		//buf所指空间第一个字符为标志位，表示是否传完
		memset(buf, 0, 1024);
		read_ret = read(file_fd, buf, 1024);
		if (read_ret == -1)
		      log_err("read", __LINE__);
		//停止判断标志
		if (read_ret <= 0)
		{
			memset(buf, '\t', 1024);
			send(client_fd, buf, 1024, 0);
			break;
		}
		//read_ret实际读出值，也是要发送出的大小
		send_ret = send(client_fd, buf, read_ret, 0);
		if (send_ret == -1)
		      log_err("send", __LINE__);	
		local_sum += send_ret;
		printf("客户端下载%s %.lf%%\n", file, local_sum/sum*100);
	}
	
	free(buf);
	close(file_fd);
	chdir(former_path);
}

void command_put(int client_fd, char *file, char *cmd_path)
{
	int 	recv_ret;
	char 	path[256];
	char 	*buf;

	buf = malloc(1024);
	printf("进入command_put()...\n");
	find_path(file, path);
	my_put(client_fd, file, path, cmd_path);
	free(buf);
}

void my_put(int client_fd, char *file, char *path, char *cmd_path)
{
	int 	file_fd;
	int 	recv_ret = 1;
	int 	write_ret = 1;
	int 	i = 0;//写入次数
	char 	former_path[256];
	char 	name[256];
	char 	*receive = former_path;
	char 	*buf;
	int 	sign = 0;
	double 	sum = 0;
	double 	local_sum = 0;

	buf = malloc(1024);

	printf("进入my_put()...\n");
	//获取文件名准备最后恢复到开始目录
	receive = getcwd(receive, 256);
	if (receive == NULL)
	      log_err("getcwd", __LINE__);
	
	//确定工作路径,1表示在changgong为根的根目录，0表示处理过的目录
	receive = file;
	while (*receive != '\0')
	      receive++;
	while (*receive != *file)
	{
		receive--;
		if (*receive == '.')	
		{
			sign = 1;
			break;
		}
	}

	receive--;
	if (sign == 0)
	      chdir("/home/chang/changgong/none");
	else if (strcmp(receive, "mp4") == 0)
	      chdir("/home/chang/changgong/mp4");
	else if (strcmp(receive, "mp3") == 0)
	      chdir("/home/chang/changgong/mp3");
	else if (strcmp(receive, "doc") == 0)
	      chdir("/home/chang/changgong/doc");
	else if (strcmp(receive, "jpg") == 0)
	      chdir("/home/chang/changgong/jpg");
	else 
	      chdir("/home/chang/changgong");
	//把文件名字提取出来，作为open参数
	pick_name(file,name);
	printf("该文件为%s\n", name);

	//接收文件总大小数值
	recv(client_fd, &sum, 8, 0);

	//创建文件写入数据
	file_fd = open(name, O_CREAT|O_EXCL|O_RDWR, 0664);
	if (file_fd == -1)
	      log_err("open",__LINE__);
	while (1)
	{
		memset(buf, 0, 1024);
		recv_ret = recv(client_fd, buf, 1024, 0);
		if (recv_ret == -1)
		      log_err("recv", __LINE__);
		if (strncmp(buf, "\t\t\t", 3) == 0)
		{
			write_ret = 0;
			break;
		}
		write_ret = write(file_fd, buf, recv_ret);
		local_sum +=  write_ret;
		printf("接收上传数据%s %.lf%%\n", name, local_sum/sum*100);
		if (write_ret == -1)
		      log_err("write", __LINE__);
		i++;
	}
	free(buf);
	close(file_fd);
	chdir(former_path);
}


