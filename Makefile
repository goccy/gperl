CC = g++
CFLAGS = -O2 -Wall -g3 -W -I./include/ -DSTATIC_TYPING_MODE #-DDEBUG_MODE #-DUSING_GRAPH_DEBUG
#CFLAGS = -O0 -g3 -gdwarf-2 -Wall -W -fpermissive -I./include/ -I/opt/local/include/ -DSTATIC_TYPING_MODE -DDEBUG_MODE -DUSING_GRAPH_DEBUG
LDLIBS = `pkg-config libgvc --libs` -lpthread
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
	build/array.o\
	build/undef.o

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

build/array.o : src/array.cpp
	$(CC) $(CFLAGS) -o $@ -c $^

build/undef.o : src/undef.cpp
	$(CC) $(CFLAGS) -o $@ -c $^

.PHONY: clean
clean:
	$(RM) -rf $(objs) $(target) *~
