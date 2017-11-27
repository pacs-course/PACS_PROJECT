CXXFLAGS  += -Wall -std=c++11 -I/usr/include/mysql

EXE= OPT_JR_CPP

OBJS  = main.o optjrparameters.o optjrParam_helper.o readConfigurationFile.o db.o application.o read_app_file.o batch.o debugmessage.o bounds.o invokePredictor.o invokePredictor_helper.o search.o objectiveFunction.o utility.o candidates.o

.DEFAULT_GOAL  = all

all: $(EXE)

$(EXE) : main.o optjrparameters.o helper.o readConfigurationFile.o db.o application.o read_app_file.o batch.o debugmessage.o bounds.o invokePredictor.o invokePredictor_helper.o search.o objectiveFunction.o utility.o candidates.o
	$(CXX) $(CXXFLAGS) -fopenmp -o $(EXE) main.o optjrparameters.o helper.o readConfigurationFile.o db.o application.o read_app_file.o batch.o debugmessage.o bounds.o invokePredictor.o invokePredictor_helper.o search.o objectiveFunction.o utility.o candidates.o -lmysqlclient

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
	$(CXX) $(CXXFLAGS) -c  src/batch.cpp

debugmessage.o : src/debugmessage.cpp
	$(CXX) $(CXXFLAGS) -c src/debugmessage.cpp

bounds.o : src/bounds.cpp
	$(CXX) $(CXXFLAGS) -fopenmp -c src/bounds.cpp

invokePredictor.o : src/invokePredictor.cpp
	$(CXX) $(CXXFLAGS) -c src/invokePredictor.cpp

invokePredictor_helper.o : src/invokePredictor_helper.cpp
	$(CXX) $(CXXFLAGS) -c src/invokePredictor_helper.cpp

search.o : src/search.cpp
	$(CXX) $(CXXFLAGS) -c src/search.cpp

objectiveFunction.o : src/objectiveFunction.cpp
	$(CXX) $(CXXFLAGS) -c src/objectiveFunction.cpp

utility.o : src/utility.cpp
	$(CXX) $(CXXFLAGS) -c src/utility.cpp

candidates.o : src/candidates.cpp
	$(CXX) $(CXXFLAGS) -c src/candidates.cpp








clean :
	$(RM) $(OBJS)

distclean : clean
	$(RM) $(EXE)
	$(RM) $(OBJS)
