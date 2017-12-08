CC=gcc

TARGET      ?= libotfs
TARGETDIR   ?= bin
EXT_DEF     ?= -DBOARD_posix_a -DAPP_modbus_master -DOT_FEATURE_DLL_SECURITY=0
EXT_INC     ?= 
EXT_LIBS    ?= 

# Note: Potentially need to add OTEAX to LIBMODULES
DEFAULT_DEF := 
DEFAULT_INC := ./include
LIBMODULES  := 
#PLATFORMS   := $(shell find ./platform -type d)
PLATFORMS   := ./platform/posix_c
SUBMODULES  := otlib $(PLATFORMS)

BUILDDIR    := build

SRCEXT      := c
DEPEXT      := d
OBJEXT      := o

#CFLAGS      := -std=gnu99 -O -g -Wall -pthread
CFLAGS      := -std=gnu99 -O3 -pthread
INC         := -I$(DEFAULT_INC)
INCDEP      := -I$(DEFAULT_INC)
LIB         := -lotfs -L.
OTFS_DEF    := $(DEFAULT_DEF) $(EXT_DEF)
OTFS_INC    := $(INC) $(EXT_INC)
OTFS_LIB    := $(LIB) $(EXT_LIBS)

#OBJECTS     := $(shell find $(BUILDDIR) -type f -name "*.$(OBJEXT)")
#MODULES     := $(SUBMODULES) $(LIBMODULES)


# Export the following variables to the shell: will affect submodules
export OTFS_DEF
export OTFS_INC
export OTFS_LIB

all: $(TARGET)
lib: libotfs.a
remake: cleaner all


directories:
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(BUILDDIR)

#Clean only Objects
clean:
	@$(RM) -rf $(BUILDDIR)

#Full Clean, Objects and Binaries
cleaner: clean
	@$(RM) -rf $(TARGETDIR)

#Linker -- only for building library test suite
$(TARGET): libotfs.a
	$(CC) $(CFLAGS) -I. $(OTFS_INC) -c -o $(BUILDDIR)/libotfs-test.o ./libotfs-test.c
	$(CC) $(CFLAGS) $(OTFS_DEF) -o $(TARGETDIR)/$(TARGET) $(OBJECTS) $(OTFS_LIB)

#Build the static library
#Note: testing with libtool now, which may be superior to ar
libotfs.a: $(SUBMODULES) $(LIBMODULES)
	$(eval OBJECTS := $(shell find $(BUILDDIR) -type f -name "*.$(OBJEXT)"))
	libtool -o libotfs.a -static $(OBJECTS)

#Library dependencies (not in otfs sources)
$(LIBMODULES): %: 
	cd ./../$@ && $(MAKE) all

#libotfs submodules
$(SUBMODULES): %: $(LIBMODULES) directories
	$(eval MKFILE := $(notdir $@))
	cd ./$@ && $(MAKE) -f $(MKFILE).mk obj

#Non-File Targets
.PHONY: all remake clean cleaner

