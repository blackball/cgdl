CC = gcc
LIBS = -lm -lopencv_core -lopencv_highgui -lopencv_imgproc

cgdel-demo: example.o cgdl.o mat.o
	gcc cgdl.o mat.o example.o -o cgdl-demo $(LIBS)

clean: 
	rm -rf *.o cgdl-demo