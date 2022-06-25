NAME = allo
CC = gcc
FLAGS = -std=c99 -pedantic -g
FLAGS+= -Wall -Wno-unused-parameter -Wextra -Werror=vla -Werror

PHONY := __all

__all: install

install:
	mkdir /usr/include/memoryhandles && cp -r * /usr/include/memoryhandles
