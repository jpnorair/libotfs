COMPILER=gcc

OTLIB_C := $(wildcard ./otlib/*.c)
OTLIB_H := $(wildcard ./include/otlib/*.h)

PLATFORM_STDC_C := $(wildcard ./platform/stdc/*.c)
PLATFORM_POSIX_C := $(wildcard ./platform/posix_c/*.c)

APP_TEST_C := $(wildcard ./app/test/*.c)


SOURCES_STDC := $(OTLIB_C) $(APP_TEST_C) $(PLATFORM_STDC_C)
SOURCES_POSIXC := $(OTLIB_C) $(APP_TEST_C) $(PLATFORM_POSIX_C)

HEADERS := $(ARGTABLE_H) $(cJSON_H) $(MAIN_H)

SEARCH := -I./include -I./apps/_common -I./apps/test/app


#FLAGS = -std=gnu99 -O -g -Wall
FLAGS = -std=gnu99 -O3

all: otfs_lib test



otfs_lib: hmqtt.o
	$(eval OBJS := $(shell ls ./*.o))
	$(COMPILER) $(FLAGS) $(OBJS) -L$(PAHO_LIBS) -L/usr/local/lib -luriparser -lssl -lpaho-mqtt3c-static -lpaho-mqtt3cs-static -o hmqtt.out

hmqtt.o: $(SOURCES) $(HEADERS)
	$(COMPILER) $(FLAGS) $(SEARCH) -c $(SOURCES) $(HEADERS)

clean:
	rm -rf ./*.o
	rm -f ./*.gch
	rm -f ./argtable/*.gch
	rm -f ./cJSON/*.gch
	rm -f ./main/*.gch
