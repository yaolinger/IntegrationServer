include ../Makefile.config

INCLUDE=$(BASE_INCLUDE) -I. -I.. -I../.. -I../third_module/include
LIB_PATH=$(BASE_LIB_PATH) -L../third_module/lib
LIB=$(BASE_LIB) -lprotobuf
USER_LIB=../utils/libutils.a ../protocpp/libprotocpp.a

SRCS=$(wildcard */*.cpp */*.zjx)
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
