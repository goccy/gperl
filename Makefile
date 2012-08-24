CC = g++-4.2
READLINE_DIR = lib/greadline
#CFLAGS = -O2 -g -Wall -W -fomit-frame-pointer -I./include/ -I$(READLINE_DIR)/include #-DSTATIC_TYPING_MODE #-DDEBUG_MODE #-DUSING_JIT #-DUSING_GRAPH_DEBUG
CFLAGS = -O0 -g3 -gdwarf-2 -Wall -W -fpermissive -I./include/ -I$(READLINE_DIR)/include -I/opt/local/include/ -DDEBUG_MODE -DUSING_GRAPH_DEBUG
READLINE_CFLAGS= -O2 -Wall -W -I$(READLINE_DIR)/include/
LDLIBS = -L/usr/local/lib/x86_64 -ljit #`pkg-config libgvc --libs` -lpthread
target = gperl

objs = build/main.o \
	build/gperl.o \
	build/token.o \
	build/parse.o \
	build/ast.o \
	build/graph.o \
	build/compiler.o\
	build/memory.o\
	build/vm.o \
	build/gen_vm.o\
	build/string.o\
	build/array.o\
	build/undef.o\
	build/jit.o\
	$(READLINE_DIR)/build/greadline.o\
	$(READLINE_DIR)/build/keyword.o\
	$(READLINE_DIR)/build/term.o\
	$(READLINE_DIR)/build/complete.o\
	$(READLINE_DIR)/build/history.o\
	$(READLINE_DIR)/build/mem.o

.PHONY: all
all: $(target)

$(target): $(objs)
	$(CC) -o $@ $^ $(LDLIBS)

build/main.o : src/main.cpp
	$(CC) $(CFLAGS) -o $@ -c $^

build/gperl.o : src/gperl.cpp
	$(CC) $(CFLAGS) -o $@ -c $^

build/token.o : src/token.cpp
	$(CC) $(CFLAGS) -o $@ -c $^

build/parse.o : src/parse.cpp
	$(CC) $(CFLAGS) -o $@ -c $^

build/ast.o : src/ast.cpp
	$(CC) $(CFLAGS) -o $@ -c $^

build/graph.o : src/graph.cpp
	$(CC) $(CFLAGS) -o $@ -c $^

build/compiler.o : src/compiler.cpp
	$(CC) $(CFLAGS) -o $@ -c $^

build/memory.o : src/memory.cpp
	$(CC) $(CFLAGS) -o $@ -c $^

build/vm.o : src/vm.cpp
	$(CC) $(CFLAGS) -o $@ -c $^

build/gen_vm.o : src/gen_vm.cpp
	$(CC) $(CFLAGS) -o $@ -c $^

build/string.o : src/string.cpp
	$(CC) $(CFLAGS) -o $@ -c $^

build/array.o : src/array.cpp
	$(CC) $(CFLAGS) -o $@ -c $^

build/undef.o : src/undef.cpp
	$(CC) $(CFLAGS) -o $@ -c $^

build/jit.o : src/jit.cpp
	$(CC) $(CFLAGS) -o $@ -c $^

$(READLINE_DIR)/build/greadline.o : $(READLINE_DIR)/src/greadline.c
	$(CC) $(READLINE_CFLAGS) -o $@ -c $^

$(READLINE_DIR)/build/complete.o : $(READLINE_DIR)/src/complete.c
	$(CC) $(READLINE_CFLAGS) -o $@ -c $^

$(READLINE_DIR)/build/keyword.o : $(READLINE_DIR)/src/keyword.c
	$(CC) $(READLINE_CFLAGS) -o $@ -c $^

$(READLINE_DIR)/build/term.o : $(READLINE_DIR)/src/term.c
	$(CC) $(READLINE_CFLAGS) -o $@ -c $^

$(READLINE_DIR)/build/history.o : $(READLINE_DIR)/src/history.c
	$(CC) $(READLINE_CFLAGS) -o $@ -c $^

$(READLINE_DIR)/build/mem.o : $(READLINE_DIR)/src/mem.c
	$(CC) $(READLINE_CFLAGS) -o $@ -c $^

.PHONY: clean
clean:
	$(RM) -rf $(objs) $(target) *~
