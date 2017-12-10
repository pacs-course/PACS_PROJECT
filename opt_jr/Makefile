
############# EDITABLE PART #############

# compiler options
CXX = g++
CXXFLAGS += -Wall -std=c++11 -I/usr/include/mysql -fopenmp

# name of executable
EXEC=OPT_JR_CPP

#source directory
SRCDIR = src/
#object file directory (it expects an existing directory)
ODIR= obj/

#libraries
LDLIBS += -lmysqlclient





############# NOT EDITABLE PART #############


#src files
SRCS= $(wildcard $(SRCDIR)*.cpp)
#header files
HEADERS=$(wildcard $(SRCDIR)*.hh)

#object files
_OBJ = $(SRCS:.cpp=.o)
OBJS = $(patsubst $(SRCDIR)%, $(ODIR)%, $(_OBJ))

.PHONY= all clean distclean doc

.DEFAULT_GOAL  = all

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) -o $@ $^ $(LDLIBS) $(CXXFLAGS)

$(ODIR)%.o: $(SRCDIR)%.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)




clean:
	-rm -f $(OBJS)

distclean: clean
	-rm -f $(EXEC)
