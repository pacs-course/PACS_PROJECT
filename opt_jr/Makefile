CXXFLAGS  += -Wall -std=c++11 -I/usr/include/mysql

EXE= OPT_JR_CPP

OBJS  = main.o optjrparameters.o optjrParam_helper.o readConfigurationFile.o db.o application.o read_app_file.o batch.o debugmessage.o

.DEFAULT_GOAL  = all

all: $(EXE)

$(EXE) : main.o optjrparameters.o helper.o readConfigurationFile.o db.o application.o read_app_file.o batch.o debugmessage.o
	$(CXX) $(CXXFLAGS) -o $(EXE) main.o optjrparameters.o helper.o readConfigurationFile.o db.o application.o read_app_file.o batch.o debugmessage.o  -lmysqlclient

main.o : src/main.cpp
	$(CXX) $(CXXFLAGS) -c src/main.cpp

optjrparameters.o : src/optjrparameters.cpp
	$(CXX) $(CXXFLAGS) -c src/optjrparameters.cpp

optjrParam_helper..o : src/optjrParam_helper.cpp
	$(CXX) $(CXXFLAGS) -c src/optjrParam_helper.cpp

readConfigurationFile.o : src/readConfigurationFile.cpp
	$(CXX) $(CXXFLAGS) -c src/readConfigurationFile.cpp

db.o : src/db.cpp
	$(CXX) $(CXXFLAGS) -c src/db.cpp

application.o : src/application.cpp
	$(CXX) $(CXXFLAGS) -c src/application.cpp

read_app_file.o : src/read_app_file.cpp
	$(CXX) $(CXXFLAGS) -c src/read_app_file.cpp

batch.o : src/batch.cpp
	$(CXX) $(CXXFLAGS) -c src/batch.cpp

debugmessage.o : src/debugmessage.cpp
	$(CXX) $(CXXFLAGS) -c src/debugmessage.cpp



clean :
	$(RM) $(OBJS)

distclean : clean
	$(RM) $(EXE)
	$(RM) $(OBJS)
