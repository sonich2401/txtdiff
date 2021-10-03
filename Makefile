CC = g++

ifndef UNAME
UNAME = $(shell uname)

PIXEL_GAME_ENGINE_ARGS = -lX11 -lGL -lpthread -lstdc++fs -std=c++17
CC_ARGS = -Wno-unused-result -Wno-write-strings -Ofast $(PIXEL_GAME_ENGINE_ARGS)
DEFINES_DEF = DATE="\"`date`\"" VER=\"$(VERSION)\" PROG_NAME=\"$(BUILD)\" OS_CLEAR=\"$(OS_CLEAR)\" FORCE_EXPERIMENTAL_FS
endif

ODIR := build
ROOT_DIRECTORY=.
SOURCES := ${shell find ${ROOT_DIRECTORY} -type d -print}
INCLUDE := ./include

C_FILES := $(foreach dir,$(SOURCES),  $(wildcard $(dir)/*.c) ) $(wildcard *.c)
CPP_FILES := $(foreach dir,$(SOURCES),  $(wildcard $(dir)/*.cpp) ) #$(wildcard *.cpp)
H_FILES := $(foreach dir,$(SOURCES),  $(wildcard $(dir)/*.h) ) $(wildcard *.h)

ASM_FILES := $(foreach dir,$(SOURCES),  $(wildcard $(dir)/*.asm) ) $(wildcard *.asm)
S_FILES := $(foreach dir,$(SOURCES),  $(wildcard $(dir)/*.s) ) $(wildcard *.s)

O_FILES = $(abspath $(addprefix $(ODIR)/, $(CPP_FILES:.cpp=.o)))




VERSION := 0.1
DEFINE_PREFIX = -
OS_CLEAR = clear

ifndef $(BUILD)
	BUILD := $(shell basename $(CURDIR)).linux
endif

FORCE_EXPERIMENTAL_FS=true

ifndef $(DEBUG)
DEBUG = false
endif

ifeq ($(DEBUG), true)
DEFINES_DEF += DEBUG=$(DEBUG)
CC_ARGS += -g
endif

DEFINES = $(foreach def,$(DEFINES_DEF), $(DEFINE_PREFIX)D$(def))

ifeq ($(UNAME), Windows)
DEFINES = ""
endif


$(ODIR)/./%.o : %.cpp
	@echo $(notdir $(basename $*)).cpp
	@mkdir -p $(dir $@)
	@$(CC) -c $*.cpp -o $@ -I $(INCLUDE) $(CC_ARGS) $(DEFINES)


.PHONY: all
.PHONY: message
.PHONY: compile
.PHONY: run

all: compile

compile: message $(addprefix $(ODIR)/, $(CPP_FILES:.cpp=.o)) | $(ODIR)
	@$(CC) $(O_FILES) -o $(BUILD) -I $(INCLUDE) $(CC_ARGS) $(DEFINES) 

message:
	@echo Building ...
	@echo "\tDebug Build = $(DEBUG)"
	@echo "\tTarget = $(UNAME)"
	@echo "\tVersion = $(VERSION)"
	@echo "\tC++ Files to Compile = $(words $(CPP_FILES))"
	
$(ODIR):
	@mkdir -p $@

run:
	@./$(BUILD)

clean:
	@rm -rf $(ODIR)
	@rm -rf $(BUILD)

debug:
	@$(MAKE) DEBUG=true --no-print-directory
release:
	@$(MAKE) --no-print-directory