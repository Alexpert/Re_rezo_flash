CC=gcc
CFLAGS=-g -Wall

LIB_DIR=lib
SRC_DIR=src
BIN_DIR=bin
OBJ_DIR=obj

libs: $(LIB_DIR)/libarray_list.a $(LIB_DIR)/libfon.a
exec:	$(BIN_DIR)/client $(BIN_DIR)/server
all: libs exec

# Make Libraries
$(LIB_DIR)/libarray_list.a: $(OBJ_DIR)/array_list.o
	rm -f $(LIB_DIR)/libarray_list.a
	ar -r $(LIB_DIR)/libarray_list.a $(OBJ_DIR)/array_list.o

$(LIB_DIR)/libfon.a: $(OBJ_DIR)/fon.o
	rm -f $(LIB_DIR)/libfon.a
	ar -r $(LIB_DIR)/libfon.a $(OBJ_DIR)/fon.o

#Make objects
$(OBJ_DIR)/client.o: $(SRC_DIR)/client.c
	$(CC) -c $(SRC_DIR)/client.c
	mv client.o $(OBJ_DIR)/client.o

$(OBJ_DIR)/server.o: $(SRC_DIR)/server.c
	$(CC) -c $(SRC_DIR)/server.c
	mv server.o $(OBJ_DIR)/server.o

$(OBJ_DIR)/fon.o: $(LIB_DIR)/fon.h $(LIB_DIR)/fon.c
	$(CC) -c $(LIB_DIR)/fon.c $(CFLAGS)
	mv fon.o $(OBJ_DIR)

$(OBJ_DIR)/array_list.o: $(LIB_DIR)/array_list.h $(LIB_DIR)/array_list.c
	$(CC) -c $(LIB_DIR)/array_list.c $(CFLAGS)
	mv array_list.o $(OBJ_DIR)

#Make tests
$(BIN_DIR)/client: $(OBJ_DIR)/client.o $(LIB_DIR)/libfon.a
	$(CC) -o $(BIN_DIR)/client $(OBJ_DIR)/client.o -L. $(LIB_DIR)/libfon.a -lm

$(BIN_DIR)/server: $(OBJ_DIR)/server.o $(LIB_DIR)/libfon.a
	$(CC) -o $(BIN_DIR)/server $(OBJ_DIR)/server.o -L. $(LIB_DIR)/libfon.a -lm


clean:
	rm $(LIB_DIR)/*.a
	rm $(OBJ_DIR)/*
	rm $(BIN_DIR)/*
