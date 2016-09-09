include ../inc.mk

APP_EXE=raft_demo
SUB_MODULES= raft_lib .

HFILE = $(foreach dir,$(SUB_MODULES),-I $(dir))
CFILE = $(foreach dir,$(SUB_MODULES), $(wildcard $(dir)/*.c))
OBJ=$(CFILE:.c=.o)
DEL_OBJ = $(foreach dir, $(SUB_MODULES), $(wildcard $(dir)/*.o))

CFLAGS+= ${HFILE}

all: $(APP_EXE)

%.o:%.c Makefile ../inc.mk
	@echo "CC $<"
	$(CMD_PREFIX)$(CC) $(CFLAGS) -c $< -o $@

$(APP_EXE) : $(OBJ) 
	@echo -e "make $(RED) $(APP_EXE) $(RES)"
	@$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS) $(LIBS)

clean:
	@echo -e "do make clean"
	@$(RM) $(DEL_OBJ) $(APP_EXE) core.* log

