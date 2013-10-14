
INC=../../include
LIB=../../lib

#CFLAGS= -g -O1 -Wall -I$(INC)
CFLAGS= -O3 -Wall -I$(INC) -DNDEBUG

all: watershed_heap watershed_rgqueue

watershed_rgqueue: watershed_rgqueue.o rgqueue.o $(LIB)/libift.a
	gcc  $^ -o $@ -lm 

watershed_heap: watershed_heap.o $(LIB)/libift.a
	gcc  $^ -o $@ -lm 

$(LIB)/libift.a:
	cd ../; make ; cd -

clean:
	rm -f *.o watershed_heap watershed_rgqueue

# ./watershed_rgqueue flower.pgm flower_grad.pgm flower-seeds.txt
# ./watershed_heap flower.pgm flower_grad.pgm flower-seeds.txt

# valgrind --leak-check=yes ./watershed_rgqueue flower.pgm flower_grad.pgm flower-seeds.txt
