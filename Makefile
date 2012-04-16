CC = g++ -m64
CFLAGS = -O0 -g3 -Wall -W -I./include/ -I/opt/local/include -DDEBUG_MODE -DUSING_GRAPH_DEBUG
LDLIBS = `pkg-config libgvc --libs`
target = gperl

objs = build/main.o \
	build/gperl.o \
	build/token.o \
	build/parse.o \
	build/graph.o \
	build/compiler.o\
	build/vm.o

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

build/graph.o : src/graph.cpp
	$(CC) $(CFLAGS) -o $@ -c $^

build/compiler.o : src/compiler.cpp
	$(CC) $(CFLAGS) -o $@ -c $^

build/vm.o : src/vm.cpp
	$(CC) $(CFLAGS) -o $@ -c $^

.PHONY: clean
clean:
	$(RM) -rf $(objs) $(target) *~
