include ../inc.mk
ifeq ($(OS),)
OS := $(shell uname)
endif

PB_TOOL=protoc-c

GENERATE_FILES=$(wildcard *.[hc])

all: 
	@echo -e "make $(RED) proto $(RES)"
	${PB_TOOL} --c_out=. raft.proto       
	@cp *.[hc] $(DEMO)

clean:
	@echo -e "do make clean"
	@echo ${GENERATE_FILES}
	@$(RM) $(DEMO)/raft.pb-c.[hc]
	$(RM) *.[hc] 


.PHONY: all clean 
