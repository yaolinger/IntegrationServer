include ../Makefile.config

INCLUDE=$(BASE_INCLUDE) -I. -I.. -I../.. -I/usr/integration_server_third/include/zdb
LIB_PATH=$(BASE_LIB_PATH)
LIB=$(BASE_LIB) -lprotobuf -lboost_system -lzdb
USER_LIB=../utils/libutils.a ../protocpp/libprotocpp.a

SRCS=$(wildcard */*.cpp */*.zjx)
OBJS=$(SRCS:.cpp=.o)
DEPS=$(SRCS:.cpp=.d)

NAME=test
TARGET=$(NAME).out

all : $(TARGET)

-include $(DEPS)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(VERSION) -c $< -o $@ -MMD -MP


$(TARGET) : $(OBJS) $(USER_LIB)
	$(CXX) $^ $(LIB_PATH) $(LIB) $(USER_LIB) -o $@

clean :
	rm */*.d */*.o $(TARGET) -f

distclean :
	@echo "$(NAME) nothing to do~~"

.PHONY : all clean distclean
