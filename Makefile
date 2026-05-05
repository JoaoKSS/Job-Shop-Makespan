CXX = g++
CXXFLAGS = -Wall -O3 -std=c++11

all: jobshop generate_results

jobshop: main.o JobShop.o
	$(CXX) $(CXXFLAGS) -o jobshop main.o JobShop.o

generate_results: generate_results.o JobShop.o
	$(CXX) $(CXXFLAGS) -o generate_results generate_results.o JobShop.o

main.o: main.cpp JobShop.h
	$(CXX) $(CXXFLAGS) -c main.cpp

generate_results.o: generate_results.cpp JobShop.h
	$(CXX) $(CXXFLAGS) -c generate_results.cpp

JobShop.o: JobShop.cpp JobShop.h
	$(CXX) $(CXXFLAGS) -c JobShop.cpp

clean:
	rm -f *.o jobshop generate_results
