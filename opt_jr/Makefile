CXXFLAGS  += -Wall -std=c++11 -I/usr/include/mysql

EXE= OPT_JR_CPP

OBJS  = main.o optjrparameters.o helper.o containers.o db.o application.o read_app_file.o

.DEFAULT_GOAL  = all

all: $(EXE)

$(EXE) : main.o optjrparameters.o helper.o containers.o db.o application.o read_app_file.o
	$(CXX) $(CXXFLAGS) -o $(EXE) main.o optjrparameters.o helper.o containers.o db.o application.o read_app_file.o  -lmysqlclient

main.o : src/main.cpp
	$(CXX) $(CXXFLAGS) -c src/main.cpp

optjrparameters.o : src/optjrparameters.cpp
	$(CXX) $(CXXFLAGS) -c src/optjrparameters.cpp

helper.o : src/helper.cpp
	$(CXX) $(CXXFLAGS) -c src/helper.cpp

containers.o : src/containers.cpp
	$(CXX) $(CXXFLAGS) -c src/containers.cpp

db.o : src/db.cpp
	$(CXX) $(CXXFLAGS) -c src/db.cpp

application.o : src/application.cpp
	$(CXX) $(CXXFLAGS) -c src/application.cpp

read_app_file.o : src/read_app_file.cpp
	$(CXX) $(CXXFLAGS) -c src/read_app_file.cpp 



clean :
	$(RM) $(OBJS)

distclean : clean
	$(RM) $(EXE)
	$(RM) $(OBJS)
