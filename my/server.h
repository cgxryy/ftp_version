#include <time.h>

int 	server(int client_fd, struct sockaddr_in *cli_addr);
void 	command_choice(int client_fd, unsigned int command, char *file, char *cmd_path);
void 	command_ls(int client_fd, char *file, char *cmd_path);
void 	my_get(int client_fd, char *file, char *cmd_path);
void 	command_get(int client_fd, char *file, char *cmd_path);
void 	command_put(int client_fd, char *file, char *cmd_path);
int  	command_cd(int client_fd, char *file, char *cmd_path);
void 	my_put(int client_fd, char *file, char *path, char *cmd_path);
void 	find_cd(char *cmd_path, char *name);
void 	batch_get(int client_fd, char file[], char batch[][10], char *cmd_path);
void 	match_batch(int client_fd, char batch[][10], char *cmd_path);
int 	find_batch(char batch[][10], char file[]);
void 	sys_log(struct sockaddr_in *cli_addr, unsigned int cmd_num, int flag);
void 	log_err(char *string,int line);

