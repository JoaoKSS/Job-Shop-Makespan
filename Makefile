CXX = g++
CXXFLAGS = -Wall -O3 -std=c++11

all: jobshop

jobshop: main.o JobShop.o
	$(CXX) $(CXXFLAGS) -o jobshop main.o JobShop.o

main.o: main.cpp JobShop.h
	$(CXX) $(CXXFLAGS) -c main.cpp

JobShop.o: JobShop.cpp JobShop.h
	$(CXX) $(CXXFLAGS) -c JobShop.cpp

clean:
	rm -f *.o jobshop
