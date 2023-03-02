all: GeneticAlgorithm

Genetic.o: Genetic.cpp Genetic.hpp
	$(CXX) -c $< $(CXXFLAGS)

BooleanMatrix.o: BooleanMatrix.cpp BooleanMatrix.hpp
	$(CXX) -c $< $(CXXFLAGS)

main.o: main.cpp BooleanMatrix.cpp BooleanMatrix.hpp Genetic.cpp Genetic.hpp
	$(CXX) -c $< $(CXXFLAGS)

GeneticAlgorithm: main.o BooleanMatrix.o Genetic.o
	$(CXX) -o $@ main.o BooleanMatrix.o Genetic.o
 
clean:
	rm *.o GeneticAlgorithm