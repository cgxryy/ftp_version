#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>

#define QUIT 	0
#define LS 	1
#define GET 	2
#define PUT 	3
#define CD 	4
#define HELP 	9

void 	depart_file(char *buf, char filename[][256]);
int  	file_exist(char *file, char *name);
void 	pick_name(char *file, char *name);
void 	find_path(char *file, char *path);
void 	my_err(char *string, int line);
void 	path_deal(char *path);
void 	left_batch(char *string, char *batch);

