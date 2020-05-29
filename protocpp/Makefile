include ../Makefile.config

INCLUDE = $(BASE_INCLUDE) -I. -I..

SRCS = $(wildcard *.cc */*.cc)
OBJS = $(SRCS:.cc=.o)
DEPS = $(SRCS:.cc=.d)

TARGET = libprotocpp.a

all:$(TARGET)

-include $(DEPS)

%.o : %.cc
	$(CXX) $(VERSION) $(CXXFLAGS) $(INCLUDE) -c $< -o $@ -MMD -MP

$(TARGET) : $(OBJS)
	@rm -f $(TARGET)
	@ar -crs $(TARGET) $(OBJS)

clean:
	rm -f */*.o */*.d *.o *.d $(TARGET)

distclean:
	rm -f */*.cc */*.h *.hpp *.cc

.PHONY: all clean distclean

