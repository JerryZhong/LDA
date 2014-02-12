CXX=g++
#CXXFLAGS= -O2 
#CXXFLAGS= -g -Wall
CXXFLAGS= -g
SRC_DIRS=.
INCLUDE_DIRS=-I ./include
all:lda
#LIBS=-lgmpxx -lgmp
SRC=$(foreach srcdir, $(SRC_DIRS), $(wildcard $(srcdir)/*.cpp))
OBJS:=$(patsubst %.cpp, %.o, $(SRC))
LDAOBJS:=$(filter-out ./elda.o,$(OBJS))
ELDAOBJS:=$(filter-out ./lda.o,$(OBJS))
vpath %.h $(INCLUDE_DIRS)
vpath %.cpp $(SRC_DIRS)


lda: $(LDAOBJS)
	$(CXX) $(CXXFLAGS)     $^ -o  $@  #$(LIBS)

$(OBJS):%.o:%.cpp  
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $<  -o $@ 

.PHONY : clean
clean:
	-@rm -rf $(OBJS)  lda
