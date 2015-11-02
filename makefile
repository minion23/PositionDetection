all: test.cpp
        g++ -ggdb -pthread test.cpp `pkg-config --cflags opencv` -o test`pkg-config --libs opencv`
clean:
          $(RM) test
