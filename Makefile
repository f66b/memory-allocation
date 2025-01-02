CC=gcc

# uncomment to compile in 32bits mode (require gcc-*-multilib packages
# on Debian/Ubuntu)
#HOST32= -m32

SHELL=/bin/bash

CFLAGS+= $(HOST32) -Wall -Werror -std=gnu18 -g
# uncomment and adapt value to your needs. Without, default to 8kib
#CFLAGS+= -DMEMORY_SIZE=128000
CFLAGS+= -DDEBUG
# pour tester avec ls
CFLAGS+= -fPIC
LDFLAGS= $(HOST32)
TESTS=test_init testsperso
PROGRAMS=memshell $(TESTS)


.PHONY: clean all test_ls

all: $(PROGRAMS)

test: all
	for file in $(TESTS);do ./$$file; done

%.o: %.c
	$(CC) -c $(CFLAGS) -MMD -MF .$@.deps -o $@ $<

# dépendences des binaires
$(PROGRAMS) libmalloc.so: %: mem.o common.o

-include $(wildcard .*.deps)

# seconde partie du sujet
libmalloc.so: malloc_stub.o
	$(CC) -shared -Wl,-soname,$@ $^ -o $@

test_ls: libmalloc.so
	LD_PRELOAD=./libmalloc.so ls

# nettoyage
clean:
	$(RM) *.o $(PROGRAMS) libmalloc.so .*.deps

reindent:
	clang-format -style="{BasedOnStyle: llvm, IndentWidth: 4, AllowShortFunctionsOnASingleLine: None}" -i \
                $$(find $(CURDIR) -iname ".#*" -prune -o \( -iname "*.h" -o -iname "*.c" \) -print)
	find . -iname ".#*" -prune -o \
		\( -iname "*.h" -o -iname "*.c" -o -iname Makefile \) -print0 |\
	while read -d "" file ; do \
		if grep -sq '[[:space:]]\+$$' "$$file"; then \
			sed -e 's/[[:space:]]\+$$//' -i "$$file" ;\
		fi; \
	done
