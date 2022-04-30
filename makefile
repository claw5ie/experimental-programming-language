WFLAGS=-Wall -Wextra -pedantic
OFLAGS=-std=c++11 -g
OUTPUT=main.out

all: debug_build

debug_build: ./main.cpp ./src/Tokenizer.cpp ./src/Parser.cpp
	g++ $(WFLAGS) $(OFLAGS) -o $(OUTPUT) $^
