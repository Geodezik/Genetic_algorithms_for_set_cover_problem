all: GeneticAlgorithm

Genetic.o: Genetic.cpp Genetic.hpp
	$(CXX) -c $< $(CXXFLAGS)

main.o: main.cpp Genetic.cpp Genetic.hpp
	$(CXX) -c $< $(CXXFLAGS)

GeneticAlgorithm: main.o Genetic.o
	$(CXX) -o $@ main.o Genetic.o
 
clean:
	rm *.o GeneticAlgorithm