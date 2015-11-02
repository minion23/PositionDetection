all: main.cpp
		g++ -ggdb -pthread main.cpp `pkg-config --cflags opencv` -o test`pkg-config --libs opencv`
clean:
		$(RM) test
