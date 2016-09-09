SUBDIRS := proto util common raft
RM = rm -rf
MAKE = make --no-print-directory

all: $(TARGET)
	@./configure.sh
	@for dir in $(SUBDIRS); \
	do $(MAKE) -C $$dir all || exit 1; \
	done

proto: $(TARGET)
	@./configure.sh
	$(MAKE) -C proto

clean:
	@./configure.sh
	@for dir in $(SUBDIRS); do make -C $$dir clean|| exit 1; done
	@$(RM) -f inc.mk 

.PHONY: all clean proto
