include ../Makefile.config

INCLUDE=$(BASE_INCLUDE) -I. -I.. -I../..
LIB_PATH=$(BASE_LIB_PATH)
LIB=$(BASE_LIB) -lprotobuf -lboost_system
USER_LIB=../utils/libutils.a ../protocpp/libprotocpp.a ../service_core/libservice_core.a ../boost_network/libboost_network.a

SRCS=$(wildcard */*.cpp)
OBJS=$(SRCS:.cpp=.o)
DEPS=$(SRCS:.cpp=.d)

NAME=Robot
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
