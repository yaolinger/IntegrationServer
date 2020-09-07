SUBDIRS = utils proto protocpp boost_network logic_work service_core single_server robot test

all :
	@for dir in $(SUBDIRS); \
	do\
	  $(MAKE) -C $$dir;\
	done

clean :
	@for dir in $(SUBDIRS); \
	do\
	  $(MAKE) -C $$dir clean;\
	done

distclean :
	@for dir in $(SUBDIRS); \
	do\
	  $(MAKE) -C $$dir distclean;\
	done

.PHONY: all clean
