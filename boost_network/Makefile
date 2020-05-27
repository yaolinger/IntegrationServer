include ../Makefile.config

LIB = $(BASE_LIB)
LIB_PATH = $(BASE_LIB_PATH)
INCLUDE = $(BASE_INCLUDE) -I. -I..

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:%.cpp=%.o)
DEPS = $(SRCS:%.cpp=%.d)

TARGET = libboostnetwork.a

all : $(TARGET)

-include $(DEPS)

%.o:%.cpp
	$(CXX) $(VERSION) $(CXXFLAGS) $(INCLUDE) -c $< -o $@ -MMD -MP

clean:
	rm *.d *.o $(TARGET) -rf

$(TARGET) : $(OBJS)
	@rm $(TARGET) -rf
	@ar -crs $(TARGET) $(OBJS)

.PHONY: all clean echo

