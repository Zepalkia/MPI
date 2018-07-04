OBJ_DIR = obj
SRC_DIR = src
INC_DIR = inc

C_FILES := $(wildcard $(SRC_DIR)/*.c)
O_FILES := $(addprefix $(OBJ_DIR)/, $(notdir $(C_FILES:.c=.o)))
I_FILES := $(wildcard $(INC_DIR)/*.h)

CC = mpiCC
CFLAGS = -W -Wall
LFLAGS = -lm
CREATE = mkdirs
EXEC = main

all:  $(CREATE) $(EXEC)

mkdirs:
	mkdir -p $(OBJ_DIR)

main: src/main.c $(O_FILES) $(I_FILES)
	$(CC) $(CFLAGS) -o $@ $< $(LFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INC_DIR)/header.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJ_DIR)/* main
