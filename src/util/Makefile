include ../inc.mk

APP_EXE=libutil.a

SUB_MODULES= . res_parser 

HFILE = $(foreach dir,$(SUB_MODULES),-I $(dir))
CFILE = $(foreach dir,$(SUB_MODULES), $(wildcard $(dir)/*.c))
OBJ=$(CFILE:.c=.o)
DEL_OBJ = $(foreach dir, $(SUB_MODULES), $(wildcard $(dir)/*.o))

all: $(APP_EXE) 

%.o:%.c Makefile
	@echo "CC $<"
	$(CMD_PREFIX)$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@ 

$(APP_EXE) : $(OBJ) 
	@echo -e "make $(RED) $(APP_EXE) $(RES)"
	@ar r $(APP_EXE) $^ 

clean:
	@echo -e "do make clean"
	@$(RM) $(DEL_OBJ) $(APP_EXE)
