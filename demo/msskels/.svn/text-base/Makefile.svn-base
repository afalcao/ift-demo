# Diretórios de acesso 

INC=../../include
LIB=../../lib
BIN=.

INCLUDES= -I$(INC) -I$(OPF_DIR)/include

#FLAGS= -g -Wall 
FLAGS= -O3 -Wall

$(LIB)/libift.a:
	cd ../../; make ; cd -


 $@.c: $(LIB)/libift.a $@.c 
	make
	gcc $(FLAGS) $@.c -o $(BIN)/$@ $(INCLUDES) -L$(LIB) -lift -lm 











