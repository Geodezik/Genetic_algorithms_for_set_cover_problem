all: GeneticAlgorithm.o

BCGA.o: BCGA.cpp BCGA.hpp
	g++ -O3 -c $< $(CXXFLAGS)

BooleanMatrix.o: BooleanMatrix.cpp BooleanMatrix.hpp
	g++ -O3 -c $< $(CXXFLAGS)

main.o: main.cpp BooleanMatrix.cpp BooleanMatrix.hpp BCGA.cpp BCGA.hpp
	g++ -O3 -c $< $(CXXFLAGS)

GeneticAlgorithm.o: main.o BooleanMatrix.o BCGA.o
	g++ -O3 -o $@ main.o BooleanMatrix.o BCGA.o
 
clean:
	rm *.o
