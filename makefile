BIN = coursework1
CC = g++
FLAGS = -std=c++11 -stdlib=libc++
INC = -I ./include
LIB_PATH = ./lib/
SYS_LIB = -lz
LOC_LIB = $(LIB_PATH)libGLEW.a $(LIB_PATH)libglfw3.a $(LIB_PATH)libassimp.3.1.1.dylib
FRAMEWORKS = -framework Cocoa -framework OpenGL -framework IOKit
SRC = main.cpp gl_util.cpp

all: compile

compile:
	${CC} ${FLAGS} ${FRAMEWORKS} -o ${BIN} ${SRC} ${INC} ${LOC_LIB} ${SYS_LIB}