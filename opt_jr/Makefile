CXXFLAGS  += -Wall -std=c++11

EXE= OPT_JR_CPP

OBJS  = main.o optjrparameters.o helper.o containers.o 

.DEFAULT_GOAL  = all

all: $(EXE)

$(EXE) : main.o optjrparameters.o helper.o containers.o
	$(CXX) $(CXXFLAGS) -o $(EXE) main.o optjrparameters.o helper.o containers.o

main.o : src/main.cpp
	$(CXX) $(CXXFLAGS) -c src/main.cpp

optjrparameters.o : src/optjrparameters.cpp
		$(CXX) $(CXXFLAGS) -c src/optjrparameters.cpp

helper.o : src/helper.cpp
		$(CXX) $(CXXFLAGS) -c src/helper.cpp

containers.o : src/containers.cpp
		$(CXX) $(CXXFLAGS) -c src/containers.cpp


clean :
	$(RM) $(OBJS)

distclean : clean
	$(RM) $(EXE)
	$(RM) $(OBJS)
