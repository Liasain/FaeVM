CC := clang-15
CFLAGS := -std=gnu2x -g -Wall -Wextra

NAME := main

SRC_DIR := src
LIB_DIR := lib
OBJ_DIR := obj

SRC = $(wildcard $(SRC_DIR)/*.c)
LIB = $(wildcard $(LIB_DIR)/*.h)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

all: $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(LIB)
	$(CC) $(CFLAGS) -c $< -I$(LIB_DIR) -o $@

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ)

.PHONY: clean
clean:
	rm -f $(OBJ) $(NAME)

