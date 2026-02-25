CXX = g++
CXXFLAGS = -std=c++17

FILE = ida
PDBCREATOR_1 = utils/bfs_1
PDBCREATOR_2 = utils/bfs_2

all: compile_databases compile_ida generate_databases

compile_ida: 
	$(CXX) $(CXXFLAGS) -o $(FILE) $(FILE).cpp

compile_databases: 
	$(CXX) $(CXXFLAGS) -o $(PDBCREATOR_1) $(PDBCREATOR_1).cpp
	$(CXX) $(CXXFLAGS) -o $(PDBCREATOR_2) $(PDBCREATOR_2).cpp

generate_databases:
	./$(PDBCREATOR_1)
	./$(PDBCREATOR_2)
