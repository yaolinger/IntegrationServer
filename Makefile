SUBDIRS = utils proto protocpp boost_network service_core single_server test

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
