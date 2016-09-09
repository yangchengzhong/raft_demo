include ../../inc.mk

ifeq ($(OS),)
OS := $(shell uname)
endif

ifeq ($(V),)
CMD_PREFIX := @
else
CMD_PREFIX :=
endif

BASE_COMMON = .
ALL_INCLUDE = ./
LIBUV_SRC_PATH = ./
LIBUV_INCLUDES =  $(ALL_INCLUDE)/libuv/tree.h \
				  $(ALL_INCLUDE)/libuv/uv-darwin.h \
				  $(ALL_INCLUDE)/libuv/uv-errno.h \
				  $(ALL_INCLUDE)/libuv/uv-unix.h \
				  $(ALL_INCLUDE)/libuv/uv.h \
				  $(LIBUV_SRC_PATH)/queue.h \
				  $(LIBUV_SRC_PATH)/uv-common.h \
				 $(LIBUV_SRC_PATH)/uv-threadpool.h \
				 $(LIBUV_SRC_PATH)/uv-version.h 

LIBUV_SRCS = $(LIBUV_SRC_PATH)/fs-poll.c \
			 $(LIBUV_SRC_PATH)/inet.c \
			 $(LIBUV_SRC_PATH)/threadpool.c \
			 $(LIBUV_SRC_PATH)/uv-common.c \
			 $(LIBUV_SRC_PATH)/version.c \
			 $(LIBUV_SRC_PATH)/async.c \
			 $(LIBUV_SRC_PATH)/core.c \
			 $(LIBUV_SRC_PATH)/darwin.c \
			 $(LIBUV_SRC_PATH)/darwin-proctitle.c \
			 $(LIBUV_SRC_PATH)/dl.c \
			 $(LIBUV_SRC_PATH)/fs.c \
			 $(LIBUV_SRC_PATH)/fsevents.c \
			 $(LIBUV_SRC_PATH)/getnameinfo.c \
			 $(LIBUV_SRC_PATH)/getaddrinfo.c \
			 $(LIBUV_SRC_PATH)/kqueue.c \
			 $(LIBUV_SRC_PATH)/loop-watcher.c \
			 $(LIBUV_SRC_PATH)/loop.c \
			 $(LIBUV_SRC_PATH)/pipe.c \
			 $(LIBUV_SRC_PATH)/poll.c \
			 $(LIBUV_SRC_PATH)/process.c \
			 $(LIBUV_SRC_PATH)/proctitle.c \
			 $(LIBUV_SRC_PATH)/signal.c \
			 $(LIBUV_SRC_PATH)/stream.c \
			 $(LIBUV_SRC_PATH)/tcp.c \
			 $(LIBUV_SRC_PATH)/thread.c \
			 $(LIBUV_SRC_PATH)/timer.c \
			 $(LIBUV_SRC_PATH)/tty.c \
			 $(LIBUV_SRC_PATH)/udp.c 

LIBUV_OBJS = $(LIBUV_SRCS:%.c=%.o)

TARGET = libuv.a

ifeq ($(CC),)
ifeq ($(OS), Darwin)
CC = clang
else
CC = gcc
endif
endif

ifeq ($(NDEBUG),)
DEBUG = -g -DDEBUG -O0 -DLOGTOFILE_ENABLE#-DDEBUG_DISABLE
# -DTRACE_ENABLE
else
DEBUG = -O3
endif

CINCLUDE = \
        -I/usr/include \
        -I/usr/local/include \
		-I$(ALL_INCLUDE)/libuv \
		-I$(LIBUV_SRC_PATH)/ \

CLIB = \
        -L/usr/local/lib \
        
BASE_CFLAGS = -w -Wall -std=gnu99 -c $(DEBUG) $(CINCLUDE)

ifeq ($(OS), Darwin)
OBJS_DIR = mac_objs
LFLAGS = -Wall $(DEBUG) $(CLIB) -lz -lcrypto -lssl
CFLAGS = $(BASE_CFLAGS)
PROF_FLAGS =
else
OBJS_DIR = linux_objs
LFLAGS = -Wall $(DEBUG) $(CLIB) -lm -ldl -lz -lpthread -lrt -lcrypto -lssl -luuid -lc -lstdc++
CFLAGS = $(BASE_CFLAGS) -fPIC
PROF_FLAGS =
#PROF_FLAGS = -pg
endif

ifeq ($(OS), Darwin)
CLIB = -L/System/Library/Frameworks/JavaVM.framework/Versions/CurrentJDK/Libraries
else
CLIB = -L/opt/jdk/jre/lib/amd64 -L/opt/jdk/jre/lib/amd64/server

endif

LIBUV_LINUX_OBJS = \
$(OBJS_DIR)/libuvsrc-fs-poll.o \
$(OBJS_DIR)/libuvsrc-inet.o \
$(OBJS_DIR)/libuvsrc-threadpool.o \
$(OBJS_DIR)/libuvsrc-uv-common.o \
$(OBJS_DIR)/libuvsrc-version.o \
$(OBJS_DIR)/libuvsrc-unix-async.o \
$(OBJS_DIR)/libuvsrc-unix-core.o \
$(OBJS_DIR)/libuvsrc-unix-dl.o \
$(OBJS_DIR)/libuvsrc-unix-fs.o \
$(OBJS_DIR)/libuvsrc-unix-getnameinfo.o \
$(OBJS_DIR)/libuvsrc-unix-getaddrinfo.o \
$(OBJS_DIR)/libuvsrc-unix-loop-watcher.o \
$(OBJS_DIR)/libuvsrc-unix-loop.o \
$(OBJS_DIR)/libuvsrc-unix-pipe.o \
$(OBJS_DIR)/libuvsrc-unix-poll.o \
$(OBJS_DIR)/libuvsrc-unix-process.o \
$(OBJS_DIR)/libuvsrc-unix-signal.o \
$(OBJS_DIR)/libuvsrc-unix-stream.o \
$(OBJS_DIR)/libuvsrc-unix-tcp.o \
$(OBJS_DIR)/libuvsrc-unix-thread.o \
$(OBJS_DIR)/libuvsrc-unix-timer.o \
$(OBJS_DIR)/libuvsrc-unix-tty.o \
$(OBJS_DIR)/libuvsrc-unix-udp.o \
$(OBJS_DIR)/libuvsrc-unix-proctitle.o \
$(OBJS_DIR)/libuvsrc-unix-linux-core.o \
$(OBJS_DIR)/libuvsrc-unix-linux-inotify.o \
$(OBJS_DIR)/libuvsrc-unix-linux-syscalls.o \

LIBUV_MAC_OBJS = \
$(OBJS_DIR)/libuvsrc-fs-poll.o \
$(OBJS_DIR)/libuvsrc-inet.o \
$(OBJS_DIR)/libuvsrc-threadpool.o \
$(OBJS_DIR)/libuvsrc-uv-common.o \
$(OBJS_DIR)/libuvsrc-version.o \
$(OBJS_DIR)/libuvsrc-unix-async.o \
$(OBJS_DIR)/libuvsrc-unix-core.o \
$(OBJS_DIR)/libuvsrc-unix-dl.o \
$(OBJS_DIR)/libuvsrc-unix-fs.o \
$(OBJS_DIR)/libuvsrc-unix-fsevents.o \
$(OBJS_DIR)/libuvsrc-unix-getnameinfo.o \
$(OBJS_DIR)/libuvsrc-unix-getaddrinfo.o \
$(OBJS_DIR)/libuvsrc-unix-loop-watcher.o \
$(OBJS_DIR)/libuvsrc-unix-loop.o \
$(OBJS_DIR)/libuvsrc-unix-pipe.o \
$(OBJS_DIR)/libuvsrc-unix-poll.o \
$(OBJS_DIR)/libuvsrc-unix-process.o \
$(OBJS_DIR)/libuvsrc-unix-signal.o \
$(OBJS_DIR)/libuvsrc-unix-stream.o \
$(OBJS_DIR)/libuvsrc-unix-tcp.o \
$(OBJS_DIR)/libuvsrc-unix-thread.o \
$(OBJS_DIR)/libuvsrc-unix-timer.o \
$(OBJS_DIR)/libuvsrc-unix-tty.o \
$(OBJS_DIR)/libuvsrc-unix-udp.o \
$(OBJS_DIR)/libuvsrc-unix-proctitle.o \
$(OBJS_DIR)/libuvsrc-unix-darwin-proctitle.o \
$(OBJS_DIR)/libuvsrc-unix-darwin.o \
$(OBJS_DIR)/libuvsrc-unix-kqueue.o \


ifeq ($(OS), Darwin)
OBJS = $(LIBUV_MAC_OBJS) 
else
OBJS = $(LIBUV_LINUX_OBJS) 
endif

all : $(OBJS_DIR) $(TARGET) 

$(OBJS_DIR) :
	mkdir $(OBJS_DIR)

$(TARGET) : $(OBJS) 
	@echo "CC $@"
	@ar r $(TARGET) $^ && mv ${TARGET} $(PROJECT_HOME)/dep/

#libuv

$(OBJS_DIR)/libuvsrc-fs-poll.o : $(BASE_COMMON)/fs-poll.c
	@echo "CC $<"
	$(CMD_PREFIX)$(CC) $(CFLAGS) -c $< -o $@

$(OBJS_DIR)/libuvsrc-inet.o : $(BASE_COMMON)/inet.c
	@echo "CC $<"
	$(CMD_PREFIX)$(CC) $(CFLAGS) -c $< -o $@

$(OBJS_DIR)/libuvsrc-threadpool.o : $(BASE_COMMON)/threadpool.c
	@echo "CC $<"
	$(CMD_PREFIX)$(CC) $(CFLAGS) -c $< -o $@

$(OBJS_DIR)/libuvsrc-uv-common.o : $(BASE_COMMON)/uv-common.c
	@echo "CC $<"
	$(CMD_PREFIX)$(CC) $(CFLAGS) -c $< -o $@

$(OBJS_DIR)/libuvsrc-version.o : $(BASE_COMMON)/version.c
	@echo "CC $<"
	$(CMD_PREFIX)$(CC) $(CFLAGS) -c $< -o $@

$(OBJS_DIR)/libuvsrc-unix-async.o : $(BASE_COMMON)/async.c
	@echo "CC $<"
	$(CMD_PREFIX)$(CC) $(CFLAGS) -c $< -o $@

$(OBJS_DIR)/libuvsrc-unix-core.o : $(BASE_COMMON)/core.c
	@echo "CC $<"
	$(CMD_PREFIX)$(CC) $(CFLAGS) -c $< -o $@

$(OBJS_DIR)/libuvsrc-unix-dl.o : $(BASE_COMMON)/dl.c
	@echo "CC $<"
	$(CMD_PREFIX)$(CC) $(CFLAGS) -c $< -o $@

$(OBJS_DIR)/libuvsrc-unix-fs.o : $(BASE_COMMON)/fs.c
	@echo "CC $<"
	$(CMD_PREFIX)$(CC) $(CFLAGS) -c $< -o $@

$(OBJS_DIR)/libuvsrc-unix-fsevents.o : $(BASE_COMMON)/fsevents.c
	@echo "CC $<"
	$(CMD_PREFIX)$(CC) $(CFLAGS) -c $< -o $@

$(OBJS_DIR)/libuvsrc-unix-getnameinfo.o : $(BASE_COMMON)/getnameinfo.c
	@echo "CC $<"
	$(CMD_PREFIX)$(CC) $(CFLAGS) -c $< -o $@

$(OBJS_DIR)/libuvsrc-unix-getaddrinfo.o : $(BASE_COMMON)/getaddrinfo.c
	@echo "CC $<"
	$(CMD_PREFIX)$(CC) $(CFLAGS) -c $< -o $@

$(OBJS_DIR)/libuvsrc-unix-loop-watcher.o : $(BASE_COMMON)/loop-watcher.c
	@echo "CC $<"
	$(CMD_PREFIX)$(CC) $(CFLAGS) -c $< -o $@

$(OBJS_DIR)/libuvsrc-unix-loop.o : $(BASE_COMMON)/loop.c
	@echo "CC $<"
	$(CMD_PREFIX)$(CC) $(CFLAGS) -c $< -o $@

$(OBJS_DIR)/libuvsrc-unix-pipe.o : $(BASE_COMMON)/pipe.c
	@echo "CC $<"
	$(CMD_PREFIX)$(CC) $(CFLAGS) -c $< -o $@

$(OBJS_DIR)/libuvsrc-unix-poll.o : $(BASE_COMMON)/poll.c
	@echo "CC $<"
	$(CMD_PREFIX)$(CC) $(CFLAGS) -c $< -o $@

$(OBJS_DIR)/libuvsrc-unix-process.o : $(BASE_COMMON)/process.c
	@echo "CC $<"
	$(CMD_PREFIX)$(CC) $(CFLAGS) -c $< -o $@

$(OBJS_DIR)/libuvsrc-unix-signal.o : $(BASE_COMMON)/signal.c
	@echo "CC $<"
	$(CMD_PREFIX)$(CC) $(CFLAGS) -c $< -o $@

$(OBJS_DIR)/libuvsrc-unix-stream.o : $(BASE_COMMON)/stream.c
	@echo "CC $<"
	$(CMD_PREFIX)$(CC) $(CFLAGS) -c $< -o $@

$(OBJS_DIR)/libuvsrc-unix-tcp.o : $(BASE_COMMON)/tcp.c
	@echo "CC $<"
	$(CMD_PREFIX)$(CC) $(CFLAGS) -c $< -o $@

$(OBJS_DIR)/libuvsrc-unix-thread.o : $(BASE_COMMON)/thread.c
	@echo "CC $<"
	$(CMD_PREFIX)$(CC) $(CFLAGS) -c $< -o $@


$(OBJS_DIR)/libuvsrc-unix-timer.o : $(BASE_COMMON)/timer.c
	@echo "CC $<"
	$(CMD_PREFIX)$(CC) $(CFLAGS) -c $< -o $@

$(OBJS_DIR)/libuvsrc-unix-tty.o : $(BASE_COMMON)/tty.c
	@echo "CC $<"
	$(CMD_PREFIX)$(CC) $(CFLAGS) -c $< -o $@

$(OBJS_DIR)/libuvsrc-unix-udp.o : $(BASE_COMMON)/udp.c
	@echo "CC $<"
	$(CMD_PREFIX)$(CC) $(CFLAGS) -c $< -o $@

$(OBJS_DIR)/libuvsrc-unix-proctitle.o : $(BASE_COMMON)/proctitle.c
	@echo "CC $<"
	$(CMD_PREFIX)$(CC) $(CFLAGS) -c $< -o $@


ifeq ($(OS), Darwin)

#libuv for mac
$(OBJS_DIR)/libuvsrc-unix-darwin-proctitle.o : $(BASE_COMMON)/darwin-proctitle.c
	@echo "CC $<"
	$(CMD_PREFIX)$(CC) $(CFLAGS) -c $< -o $@

$(OBJS_DIR)/libuvsrc-unix-darwin.o : $(BASE_COMMON)/darwin.c
	@echo "CC $<"
	$(CMD_PREFIX)$(CC) $(CFLAGS) -c $< -o $@

$(OBJS_DIR)/libuvsrc-unix-kqueue.o : $(BASE_COMMON)/kqueue.c
	@echo "CC $<"
	$(CMD_PREFIX)$(CC) $(CFLAGS) -c $< -o $@

else
#libuv for linux
$(OBJS_DIR)/libuvsrc-unix-linux-core.o : $(BASE_COMMON)/linux-core.c
	@echo "CC $<"
	$(CMD_PREFIX)$(CC) $(CFLAGS) -c $< -o $@
    
$(OBJS_DIR)/libuvsrc-unix-linux-inotify.o : $(BASE_COMMON)/linux-inotify.c
	@echo "CC $<"
	$(CMD_PREFIX)$(CC) $(CFLAGS) -c $< -o $@
    
$(OBJS_DIR)/libuvsrc-unix-linux-syscalls.o : $(BASE_COMMON)/linux-syscalls.c
	@echo "CC $<"
	$(CMD_PREFIX)$(CC) $(CFLAGS) -c $< -o $@

endif

clean :
	@rm -rf $(OBJS_DIR) $(PROJECT_HOME)/dep/$(TARGET)

