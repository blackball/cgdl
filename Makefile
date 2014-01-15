CC = gcc
CFLAGS = -I./
LIBS = -lm -lopencv_core -lopencv_highgui -lopencv_imgproc

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

cgdel-demo: example.o cgdl.o mat.o
	gcc cgdl.o mat.o example.o -o cgdl-demo $(LIBS)

clean: 
	rm -rf *.o cgdl-demo