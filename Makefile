LIB=./lib
INCLUDE=./include
BIN=./bin
SRC=./src
OBJ=./obj
DEMO=./demo
#FLAGS= -g -Wall -D _DEBUG
FLAGS= -O3 -Wall

INCLUDES=-I$(INCLUDE)

all: libift 
	@echo
	@echo "libift.a built..."
	@echo "DONE."

libift: $(LIB)/libift.a

$(LIB)/libift.a: \
$(OBJ)/common.o \
$(OBJ)/comptime.o \
$(OBJ)/set.o \
$(OBJ)/image.o \
$(OBJ)/realheap.o \
$(OBJ)/subgraph.o \
$(OBJ)/color.o \
$(OBJ)/feature.o \
$(OBJ)/opf.o \
$(OBJ)/morphology.o \
$(OBJ)/cimage.o \
$(OBJ)/dimage.o \
$(OBJ)/seeds.o \
$(OBJ)/adjacency.o \
$(OBJ)/gqueue.o \

	ar csr $(LIB)/libift.a \
$(OBJ)/common.o \
$(OBJ)/comptime.o \
$(OBJ)/set.o \
$(OBJ)/image.o \
$(OBJ)/realheap.o \
$(OBJ)/subgraph.o \
$(OBJ)/color.o \
$(OBJ)/feature.o \
$(OBJ)/opf.o \
$(OBJ)/morphology.o \
$(OBJ)/cimage.o \
$(OBJ)/dimage.o \
$(OBJ)/seeds.o \
$(OBJ)/adjacency.o \
$(OBJ)/gqueue.o \


$(OBJ)/common.o: $(SRC)/common.c
	gcc $(FLAGS) -c $(SRC)/common.c $(INCLUDES) -o $(OBJ)/common.o

$(OBJ)/comptime.o: $(SRC)/comptime.c
	gcc $(FLAGS) -c $(SRC)/comptime.c $(INCLUDES) -o $(OBJ)/comptime.o

$(OBJ)/set.o: $(SRC)/set.c
	gcc $(FLAGS) -c $(SRC)/set.c $(INCLUDES) -o $(OBJ)/set.o

$(OBJ)/image.o: $(SRC)/image.c
	gcc $(FLAGS) -c $(SRC)/image.c $(INCLUDES) -o $(OBJ)/image.o

$(OBJ)/cimage.o: $(SRC)/cimage.c
	gcc $(FLAGS) -c $(SRC)/cimage.c $(INCLUDES) -o $(OBJ)/cimage.o

$(OBJ)/dimage.o: $(SRC)/dimage.c
	gcc $(FLAGS) -c $(SRC)/dimage.c $(INCLUDES) -o $(OBJ)/dimage.o

$(OBJ)/seeds.o: $(SRC)/seeds.c
	gcc $(FLAGS) -c $(SRC)/seeds.c $(INCLUDES) -o $(OBJ)/seeds.o

$(OBJ)/adjacency.o: $(SRC)/adjacency.c
	gcc $(FLAGS) -c $(SRC)/adjacency.c $(INCLUDES) -o $(OBJ)/adjacency.o

$(OBJ)/gqueue.o: $(SRC)/gqueue.c
	gcc $(FLAGS) -c $(SRC)/gqueue.c $(INCLUDES) -o $(OBJ)/gqueue.o

$(OBJ)/realheap.o: $(SRC)/realheap.c
	gcc $(FLAGS) -c $(SRC)/realheap.c $(INCLUDES) -o $(OBJ)/realheap.o

$(OBJ)/subgraph.o: $(SRC)/subgraph.c
	gcc $(FLAGS) -c $(SRC)/subgraph.c $(INCLUDES) -o $(OBJ)/subgraph.o

$(OBJ)/feature.o: $(SRC)/feature.c
	gcc $(FLAGS) -c $(SRC)/feature.c $(INCLUDES) -o $(OBJ)/feature.o

$(OBJ)/morphology.o: $(SRC)/morphology.c
	gcc $(FLAGS) -c $(SRC)/morphology.c $(INCLUDES) -o $(OBJ)/morphology.o

$(OBJ)/opf.o: $(SRC)/opf.c
	gcc $(FLAGS) -c $(SRC)/opf.c $(INCLUDES) -o $(OBJ)/opf.o

$(OBJ)/color.o: $(SRC)/color.c
	gcc $(FLAGS) -c $(SRC)/color.c $(INCLUDES) -o $(OBJ)/color.o


clean:
	rm -f $(LIB)/lib*.a; rm -f $(OBJ)/*.o; rm -f $(BIN)/*




