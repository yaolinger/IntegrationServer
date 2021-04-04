include ../Makefile.config

INCLUDE=$(BASE_INCLUDE) -I. -I.. -I../.. $(ZDB_INCLUDE) 
LIB_PATH=$(BASE_LIB_PATH)
#LIB=$(BASE_LIB) -lprotobuf -lboost_system -lzdb
#LIB=$(BASE_LIB)  $(PROTO_LIB) -lboost_system -lzdb
LIB=$(BASE_LIB) $(PROTO_LIB) $(BOOST_LIB) $(ZDB_LIB) -lmysqlclient
USER_LIB=../utils/libutils.a ../protocpp/libprotocpp.a ../logic_work/liblogic_work.a

SRCS=$(wildcard */*.cpp)
OBJS=$(SRCS:.cpp=.o)
DEPS=$(SRCS:.cpp=.d)

NAME=test
TARGET=$(NAME).out

all : $(TARGET)

-include $(DEPS)

%.o : %.cpp
	$(CXX) $(CXXDEFINE) $(CXXFLAGS) -Wno-unused-variable $(INCLUDE) $(VERSION) -c $< -o $@ -MMD -MP

$(TARGET) : $(OBJS) $(USER_LIB)
	$(CXX) $^ $(LIB_PATH) $(LIB) $(USER_LIB) -o $@

clean :
	rm */*.d */*.o $(TARGET) -f

distclean :
	@echo "$(NAME) nothing to do~~"

.PHONY : all clean distclean
