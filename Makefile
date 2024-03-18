# Makefile

main:
	g++ -std=c++11 -o program main.cpp -pthread -lncurses -lGL -lglut -lGLU
