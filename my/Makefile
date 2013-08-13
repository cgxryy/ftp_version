objects = server.o depart_file.o left_batch.o pick_name.o\
	  my_err.o find_path.o path_deal.o file_exist.o
server: $(objects)
	gcc -o server $(objects) -Wall
file_exist.o: file_exist.c 
	gcc -c file_exist.c -Wall
my_err.o: my_err.c 
	gcc -c my_err.c -Wall
path_deal.o: path_deal.c
	gcc -c path_deal.c -Wall
find_path.o: find_path.c
	gcc -c find_path.c -Wall
my_err.o: my_err.c
	gcc -c my_err.c -Wall
pick_name.c: pick_name.c
	gcc -c pick_name.c -Wall
left_batch.o: left_batch.c
	gcc -c left_batch.c -Wall
depart_file.o: depart_file.c
	gcc -c depart_file.c -Wall
server.o: server.c server.h 
	gcc -c server.c -Wall
clean:
	rm server.o depart_file.o left_batch.o pick_name.o my_err.o find_path.o path_deal.o file_exist.o
