CC = gcc
CFLAGS = -g -Wall

.PHONY: all all-global all-local clean clean-global clean-local global local

LOCAL_SRC := $(wildcard *.c)
LOCAL_OBJ := $(patsubst %.c, %, $(LOCAL_SRC))

all all-local local: $(LOCAL_OBJ)

clean clean-local: 
	-rm $(LOCAL_OBJ)

SETS := $(wildcard ../set*)
GLOBAL_SRC := $(wildcard $(SETS)/*.c)
GLOBAL_OBJ := $(patsubst %.c, %, $(GLOBAL_SRC))

all-global global: $(GLOBAL_OBJ)

clean-global:
	-rm $(GLOBAL_OBJ)
