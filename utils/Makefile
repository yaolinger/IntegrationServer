include ../Makefile.config

LIB = $(BASE_LIB)
LIB_PATH = $(BASE_LIB_PATH)
INCLUDE = $(BASE_INCLUDE) -I. -I.. -I/usr/integration_server_third/include/zdb

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:%.cpp=%.o)
DEPS = $(SRCS:%.cpp=%.d)

NAME = utils
TARGET = lib$(NAME).a

all : $(TARGET)

-include $(DEPS)

%.o : %.cpp
	$(CXX) $(VERSION) $(CXXFLAGS) $(INCLUDE) -c $< -o $@ -MMD -MP

clean:
	rm *.d *.o $(TARGET) -rf

distclean:
	@echo "$(NAME) nothing to do ~~"

$(TARGET) : $(OBJS)
	rm $(TARGET) -rf
	ar -crs $(TARGET) $(OBJS)


.PHONY: all clean distclean
