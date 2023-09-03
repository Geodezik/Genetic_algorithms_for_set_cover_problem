all: GeneticAlgorithm.o

Genetic.o: Genetic.cpp Genetic.hpp
	g++ -O3 -c $< $(CXXFLAGS)

BooleanMatrix.o: BooleanMatrix.cpp BooleanMatrix.hpp
	g++ -O3 -c $< $(CXXFLAGS)

main.o: main.cpp BooleanMatrix.cpp BooleanMatrix.hpp Genetic.cpp Genetic.hpp
	g++ -O3 -c $< $(CXXFLAGS)

GeneticAlgorithm.o: main.o BooleanMatrix.o Genetic.o
	g++ -O3 -o $@ main.o BooleanMatrix.o Genetic.o
 
clean:
	rm *.o
