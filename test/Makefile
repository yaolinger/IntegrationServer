include ../Makefile.config

INCLUDE=$(BASE_INCLUDE) -I. -I.. -I../..
LIB_PATH=$(BASE_LIB_PATH)
LIB=$(BASE_LIB)
USER_LIB=../utils/libutils.a

SRCS=$(wildcard */*.cpp)
OBJS=$(SRCS:.cpp=.o)
DEPS=$(SRCS:.cpp=.d)

TARGET=Test.out

all : $(TARGET)

-include $(DEPS)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(VERSION) -c $< -o $@ -MMD -MP


$(TARGET) : $(OBJS) $(USER_LIB)
	$(CXX) $^ $(LIB_PATH) $(LIB) $(USER_LIB) -o $@

clean :
	rm */*.d */*.o $(TARGET) -f

.PHONY : all clean
