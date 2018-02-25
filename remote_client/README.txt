README.txt

Command to build the main.cpp file:
	g++ main.cpp -o main -I/usr/include `pkg-config --cflags opencv` `pkg-config --libs opencv`