// =====================================================================================
// 
//       Filename:  client.c
//
//    Description:  
//
//        Version:  1.0
//        Created:  2013年08月02日 15时41分36秒
//       Revision:  none
//       Compiler:  g++
//
//         Author:  hanyaorong, cgxryy@gmail.com
//        Company:  Class 1201 of Computer Network Technology
// 
// =====================================================================================

#include "my.h"
#include "client.h"

void information()
{
	printf("./XXX IP地址 \n\t./client 192.168.100.1 (例子)\n");
}


void my_scanf(char *string)
{
	while ((*string = (char)getchar()) != '\n')
	      string++;
	*string = '\0';
}

void my_help(void)
{
	printf("\n\tcommand:");
	printf("\n\t\tls:  ls");
	printf("\n\t\tget: 1.get  xxx.mp3  aaa.txt\n\t\t     2.get  *.pdf  *.doc");
	printf("\n\t\tput: put  /home/chang/xxx.txt");
	printf("\n\t\tcd:  1.cd  first\n\t\t     2.cd  ..");
	printf("\n\t\tquit:quit");
	printf("\n\t\thelp:help\n");
}

int main(int argc, char *argv[])
{
	struct sockaddr_in 	sock;
	int 			sock_fd;
	int 			ret;

	if ( argc != 2)
	{
		information();
		return -1;
	}
	
	//初始化地址结构
	memset(&sock, 0, sizeof(struct sockaddr_in));
	sock.sin_family = AF_INET;
	sock.sin_port = htons(4507);

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd < 0)
	      my_err("socket",__LINE__);
	ret = inet_aton(argv[1], &sock.sin_addr);
	if (ret <= 0)
	{
		printf("IP不可用\n");
		information();
		return -1;
	}
	//连接到目的IP
	if (connect(sock_fd, (struct sockaddr*)&sock, sizeof(struct sockaddr)) < 0)
	      my_err("connect", __LINE__);

	client(sock_fd);

	return 0;
}

int 	client(int sock_fd)
{
	my_help();
	deal_server(sock_fd);
	return 0;
}

int 	deal_server(int sock_fd)
{
	int 		ret;
	char 		file[80][256] = {"\0"};
	char  		buf[1024];
	char 		command[256] = "\0";
	unsigned int 	cmd_num;
	int 		i = 0;
	char 		name[256] = "changgong";

	while (1)
	{
		printf("\nftp:%s/>> ",name);		
		for( i = 0; i < 80; i++)
		      file[i][0] = '\0';
		i = 0;//每次把file中清0，依次作为有无参数判断方法
		
		my_scanf(command);

		if (strncmp(command,"quit", 4) == 0 || strncmp(command, "exit", 4) == 0)
		      cmd_num = QUIT;
		else if (strncmp(command,"ls", 2) == 0)
		      cmd_num = LS;
		else if (strncmp(command,"get", 3) == 0)
		      cmd_num = GET;
		else if (strncmp(command,"put", 3) == 0)
		      cmd_num = PUT;
		else if (strncmp(command,"cd", 2) == 0)
		      cmd_num = CD;
		else if (strncmp(command, "help", 4) == 0)
		      cmd_num = HELP;
		else
		{
			printf("错误输入，重新输入");
			my_help();
			memset(command, '\0', 256);
			cmd_num = 111;//不是命令序号即可
			continue;
		}

		printf("发送命令为%s,编号%u\n", command, cmd_num);
	
		//发送前检验命令后面参数是否存在(help,quit不需要参数，跳过)
		if (cmd_num == GET || cmd_num == CD || cmd_num == PUT) 
		{
			depart_file(command, file);//参数都拆开	
			if (file[1][0] == '\0')
			{
				printf("command need least one argv\n");
				continue;//无参返回
			}
		}

		//发送命令序号
		if (command[0] != '\0' && cmd_num != HELP) 
		{
			cmd_num = htonl(cmd_num);
			ret = send(sock_fd, (char*)&cmd_num,sizeof(unsigned int), 0);
			if (ret < sizeof(unsigned int))
			{
				printf("发送时丢失数据包...错误...\n");
				return -1;
			}
		}
		else
		{
			my_help();
			continue;
		}
		
		//由命令编号决定发送什么内容
		cmd_num = ntohl(cmd_num);
		
		//发送整体命令行过去
		if (command[0] != '\0' && cmd_num != HELP)
		{	
			ret = send(sock_fd, command, 256, 0);
			if (ret < 256)
			{
				printf("发送时丢失数据包...错误...\n");
				return -1;
			}
		}
		else
		{ 
			my_help();
			continue;
		}

		//根据发送内容不同，接受下一个包方式不同
		switch (cmd_num)
		{	
			case QUIT:
				return 0;
			case LS:
				client_ls(sock_fd, buf, file);
				break;
			case GET:
				client_get(sock_fd, buf, file);
				break;
			case PUT:
				client_put(sock_fd, buf, file[1]);
				break;
			case CD:
				client_cd(sock_fd, name);
				break;
		}

	}
}

void client_ls(int sock_fd, char *buf, char file[][256])
{
	char 	filename[256][256] = {"\0"};
	int 	ret;

	ret = recv(sock_fd, (char*)buf, 1024, 0);
	printf("已经接收到%d字节\n", ret);
	if (ret < 0)
	     	my_err("recv",__LINE__);
	if (ret == 0)
		printf("异常断开连接...\n");
				
	depart_file(buf,filename);
	show_ls(filename);
}

void show_ls(char filename[][256])
{
	int  	maxlen = 60;
	int  	nowlen = 0;
	int  	i = 0,linemax = 0,j;
	char 	*move;
	
	maxlen = strlen(filename[i]);
						
	while ( filename[i][0] != '\0')
	{
	
		if (maxlen < strlen(filename[i+1]))  
		      maxlen = strlen(filename[i+1]);
		i++;
	}
							
	i = 0;
	linemax = maxlen;
	while ( filename[i][0] != '\0')
	{
		nowlen = strlen(filename[i]);
		if ((linemax + 2 ) > 60)
		{
			printf("\n");
			move = filename[i];
			while (*move != '\0')
			      move++;
			while (*move != '.' && move != filename[i])
			      move--;
			if (strcmp(filename[i], move) != 0 && strcmp((move-1), "..") != 0)
				printf("%s",filename[i]);
			else 	
			{
				printf("\033[40;34m\033[5m");
				printf("%s", filename[i]);
				printf("\033[0m");
			}
			for ( j = 0; j < maxlen - nowlen + 2; j++)
			     printf(" ");
			linemax = maxlen;
			i++;
		}
		else
		{      
			move = filename[i];
			while (*move != '\0')
			      move++;
			while (*move != '.' && move != filename[i])
			      move--;
			if (strcmp(filename[i], move) != 0 && strcmp((move-1), "..") != 0)
				printf("%s",filename[i]);
			else 	
			{
				printf("\033[40;34m\033[5m");
				printf("%s", filename[i]);
				printf("\033[0m");
			}
			for ( j = 0; j < maxlen - nowlen + 2; j++)
				   printf(" ");
			linemax += (maxlen + 2);
			i++;
		}
	}
	printf("\n");
}

void client_get(int sock_fd, char *buf, char file[][256])
{
	char 	sign[10];
	char 	filename[256];

	printf("进入command_get()...\n");
	while (1)
	{
		printf("已回到循环开头...\n");
		memset(sign, 0, 10);
		recv(sock_fd, sign, 10, 0);
		sleep(1);
		if (strncmp(sign, "\t\t\t", 3) == 0)
		{
			printf("接收结束标志....\n");
			break;
		}
	
		memset(filename, 0, 256);	
		if (strncmp(sign, "nnn", 3) == 0)
		{
			printf("匹配normal()...\n");
			recv(sock_fd, filename, 256, 0);
			sleep(1);
			client_my_get(sock_fd, buf, filename);
		}
		else if (strncmp(sign, "bbb", 3) == 0)
		{
			while (1)
			{
				printf("匹配batch()...\n");
				recv(sock_fd, filename, 256, 0);
				sleep(1);
				if (strncmp(filename, "\t\t\t", 3) == 0)
				{
					printf("已退出batch循环...\n");
					break;
				}
				printf("进入batch匹配的get()...\n");
				client_my_get(sock_fd, buf, filename);
			}
		}
	}

}

int client_my_get(int sock_fd, char *buf, char *file)
{
	int 	file_fd;//新建的文件的文件描述符
	char  	path[256] = "/home/";
	char 	former_path[256];
	char 	*receive = former_path;
	long 	recv_ret = 1;
	long 	write_ret = 1;
	double 	sum = 0.0;
	double 	local_sum = 0.0;

	printf("进入my_get()...\n");
	
	//接收文件总大小数值
	recv_ret = recv(sock_fd, &sum, 8, 0);
	sleep(1);
	if (recv_ret == -1)
	      return 0;

	//最后需恢复成原来工作路径
        receive = getcwd(receive, 256);
	if (receive == NULL)
	      my_err("getcwd", __LINE__);
	
	//把接收的文件送到当前用户的receive_ftp目录下
	receive = getlogin();
	strcat(path, receive);
	chdir(path);

	if ((file_exist(path, "receive_ftp")) == 0)
		mkdir("receive_ftp",0766);

	strcat(path, "/receive_ftp");	
	printf("当前目录为%s\n", path);
	chdir(path);
	

	//创建文件，写入
	file_fd = open(file, O_CREAT|O_EXCL|O_RDWR, 0666);
	if (file_fd == -1)
	{
		printf("文件已存在，已跳过\n");
		return 0;
	}

	while (1)
	{
		memset(buf, 0, 1024);
		recv_ret = recv(sock_fd, buf, 1024, 0);
		if (recv_ret == -1)
		      my_err("recv",__LINE__);
		if (strncmp(buf, "\t\t\t", 3) == 0)
		      break;
		write_ret = write(file_fd, buf, recv_ret);
		if (write_ret == -1)
		      my_err("write",__LINE__);
		local_sum += (double)write_ret;
		printf("下载进度 %s %.0lf%%\n", file, local_sum/sum*100);
	}

	close(file_fd);
	chdir(former_path);

	return 1;
}

void client_put(int sock_fd, char *buf, char *file)
{
	char 	former_path[256];
	char 	*receive = former_path;
	char 	file_path[256];

	receive = getcwd(receive, 256);
	if (receive == NULL)
	      my_err("getcwd", __LINE__);
	//文件所在目录存入file_path
	find_path(file, file_path);
	
	chdir(file_path);
	printf("已经转到%s目录\n",file_path);
	
	//发送全'-'字符的数据过去表示不是目录
	client_my_put(sock_fd, buf, file);
	chdir(former_path);
}

void client_my_put(int sock_fd, char *buf, char *file)
{
	int 		read_ret = 1;
	int 		send_ret = 1;
	int 		file_fd;
	double 		sum = 0;//发送文件总大小
	double 		local_sum = 0;//文件读取当前大小
	char 		name[80];

	pick_name(file, name);
	//读出文件总大小发给服务端
	file_fd = open(file, O_RDWR, 0644);
	if (file_fd == -1)
	      my_err("open", __LINE__);
	while (read_ret > 0)
	{
		read_ret = read(file_fd, buf, 1024);
		sum += read_ret;
	}
	close(file_fd);
	send(sock_fd, &sum, 8, 0);

	//读出文件发出
	file_fd = open(file, O_RDWR, 0644);
	if (file_fd == -1)
	      my_err("open", __LINE__);
	while (1)
	{
		memset(buf, 0, 1024);
		read_ret = read(file_fd, buf, 1024);
		if (read_ret == -1)
		      my_err("read", __LINE__);
		//停止判断标志
		if (read_ret <= 0)
		{
			memset(buf, '\t', 1024);
			send(sock_fd, buf, 1024, 0);
			break;
		}
		send_ret = send(sock_fd, buf, read_ret, 0);
		local_sum += send_ret;
		printf("上传进度%s %.lf%%\n", name, local_sum/sum*100);
		if (send_ret == -1)
		      my_err("send", __LINE__);
	}
	close(file_fd);
}

void client_cd(int sock_fd, char *name)
{
	int 	ret;
	unsigned short  flag;

	ret = recv(sock_fd, (char*)&flag, sizeof(unsigned short), 0);
	if (ret < sizeof(unsigned short))
	      my_err("recv ",__LINE__);
	flag = ntohs(flag);
	if (flag == 1)
	{      
		ret = recv(sock_fd, name, 256, 0);
		if (ret == -1)
		      my_err("cd recv", __LINE__);

	}
}
