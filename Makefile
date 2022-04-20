CURRENT_DIR := $(shell dirname $(abspath $(lastword $(MAKEFILE_LSIT))))

CC = gcc
RM = rm

CFLAGS := -Wall -O2 -g -Wno-unused-function

LLC = llc
CLANG = clang

CLANG_FLAGS := -O2
LLVM_FLAGS := -march=bpf -mcpu=probe


BIN_TARGET = $(CURRENT_DIR)/srv6manager
BIN_SRC_FILES := $(CURRENT_DIR)/main.c

DT4_SRC := $(CURRENT_DIR)/srv6_dt4.c
DT4_OBJ ?= $(DT4_SRC:%.c=%.o)

.PHONY: all
all: bin_target ebpf_target

.PHONY: bin_target
bin_target: 
	@echo "====== Make bin target start ======"
	$(CC) $(CFLAGS) $(BIN_SRC_FILES) -o $(BIN_TARGET) 
	@echo "====== Make bin target done ======"


.PHONY: ebpf_target
ebpf_target: 
	@echo "====== Make eBPF target start ======"
	$(CLANG) $(CLANG_FLAGS) -emit-llvm -c $(DT4_SRC) -o - | $(LLC) $(LLVM_FLAGS) -filetype=obj -o $(DT4_OBJ)
	@echo "====== Make eBPF target done ======"


