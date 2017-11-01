COMPILER=gcc

OTLIB_C := $(wildcard ./otlib/*.c)
OTLIB_H := $(wildcard ./include/otlib/*.h)

PLATFORM_STDC_C := $(wildcard ./platform/stdc/*.c)
PLATFORM_POSIX_C := $(wildcard ./platform/posix_c/*.c)

APP_TEST_C := $(wildcard ./apps/test/app/*.c)


SOURCES_STDC := $(OTLIB_C) $(PLATFORM_STDC_C)
SOURCES_POSIXC := $(OTLIB_C) $(APP_TEST_C) $(PLATFORM_POSIX_C)

#HEADERS := $(OTLIB_H)

SEARCH := -I./include -I./apps/_common -I./apps/test/app


#FLAGS = -std=gnu99 -O -g -Wall
FLAGS = -std=gnu99 -O3

all: test


test: test.o
	$(COMPILER) test.o -L. -lotfs -o ./bin/otfs-test

test.o: libotfs
	$(COMPILER) $(FLAGS) $(SEARCH) -c $(APP_TEST_C)

libotfs: libotfs.o
	$(eval OBJS := $(shell ls ./*.o))
	ar -rcs libotfs.a $(OBJS)
	ranlib libotfs.a

libotfs.o: $(SOURCES)
	$(COMPILER) $(FLAGS) $(SEARCH) -c $(SOURCES)

clean:
	rm -rf ./*.o
	rm -f ./*.gch
	rm -f ./argtable/*.gch
	rm -f ./cJSON/*.gch
	rm -f ./main/*.gch
