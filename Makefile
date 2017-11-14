COMPILER=gcc

OTLIB_C := $(wildcard ./otlib/*.c)
OTLIB_H := $(wildcard ./include/otlib/*.h)

PLATFORM_STDC_C := $(wildcard ./platform/stdc/*.c)
PLATFORM_POSIX_C := $(wildcard ./platform/posix_c/*.c)

APP_NULLPOSIX := $(wildcard ./apps/null_posix/app/*.c)

FS_DEFAULT := ./apps/null_posix/app/fs_default_startup.c

SOURCES_STDC := $(OTLIB_C) $(PLATFORM_STDC_C)
SOURCES_POSIXC := $(OTLIB_C) $(PLATFORM_POSIX_C)


#HEADERS := $(OTLIB_H)

SEARCH := -I./ \
          -I./include \
          -I./apps/_common \
          -I./apps/null_posix \
          -I./libs/OTEAX

DEFINES := -DBOARD_POSIX_SIM -D__NO_SECTIONS__


#FLAGS = -std=gnu99 -O -g -Wall
FLAGS = -std=gnu99 -O3

all: nullposix

lib: clean libotfs


fs_default.o: clean libotfs
	$(COMPILER) $(FLAGS) $(DEFINES) $(SEARCH) -c $(FS_DEFAULT) -o fs_default.o

nullposix: liboteax libotfs nullposix.o
	$(COMPILER) main.o -L. -L./libs/OTEAX -loteax -lotfs -o ./bin/otfs-test

nullposix.o:
	$(COMPILER) $(FLAGS) $(DEFINES) $(SEARCH) -c $(APP_NULLPOSIX)

libotfs: libotfs.o
	$(eval OBJS := $(shell ls ./*.o))
	ar -rcs libotfs.a $(OBJS)
	ranlib libotfs.a

libotfs.o: $(SOURCES_POSIXC)
	$(COMPILER) $(FLAGS) $(DEFINES) $(SEARCH) -c $(SOURCES_POSIXC) otfs.c

liboteax:
	cd ./libs/OTEAX && $(MAKE) all && $(MAKE) install

clean:
	rm -rf ./*.o
	rm -f ./*.gch
