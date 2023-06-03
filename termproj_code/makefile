.PHONY: clean

term_proj: get_path.o interface.o bmp_IO.o image_client.o image_functions.o
	        gcc -o term_proj get_path.o interface.o bmp_IO.o image_client.o image_functions.o -lcurses -lm

get_path.o: get_path.c term_proj.h
	        gcc -c -o get_path.o get_path.c
interface.o: interface.c term_proj.h
	                gcc -c -o interface.o interface.c
bmp_IO.o: bmp_IO.c term_proj.h
	                gcc -c -o bmp_IO.o bmp_IO.c
image_client.o: image_client.c term_proj.h
	                gcc -c -o image_client.o image_client.c
image_functions.o: image_functions.c term_proj.h
	                        gcc -c -o image_functions.o image_functions.c
clean:
	        rm -f *.o

