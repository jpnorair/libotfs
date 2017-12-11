CC=gcc

TARGET      ?= libotfs
TARGETDIR   ?= pkg
EXT_DEF     ?= -DBOARD_posix_a -DAPP_modbus_master -DOT_FEATURE_DLL_SECURITY=0
EXT_INC     ?= 
EXT_LIBS    ?= 

# Note: Potentially need to add OTEAX to LIBMODULES
DEFAULT_DEF := 
DEFAULT_INC := ./include
LIBMODULES  := 
#PLATFORMS   := $(shell find ./platform -type d)
PLATFORMS   := ./platform/posix_c
SUBMODULES  := main otlib $(PLATFORMS)

BUILDDIR    := ./build

SRCEXT      := c
DEPEXT      := d
OBJEXT      := o

#CFLAGS      := -std=gnu99 -O -g -Wall -pthread
CFLAGS      := -std=gnu99 -O3 -pthread
INC         := -I$(DEFAULT_INC) -I/usr/local/include
INCDEP      := -I$(DEFAULT_INC)
LIB         := -Wl,-Bstatic -lJudy -L./ -L/usr/local/lib
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
lib: $(TARGET).a
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

#Packaging stage: copy/move files to pkg output directory
$(TARGET): $(TARGET).a
	@cp -R ./include/* ./$(TARGETDIR)
	@cp ./main/otfs.h ./$(TARGETDIR)

#Build the static library
#Note: testing with libtool now, which may be superior to ar
$(TARGET).a: $(SUBMODULES) $(LIBMODULES)
	@cd $(BUILDDIR)
	$(eval LIBTOOL_OBJ := $(shell find . -type f -name "*.$(OBJEXT)"))
	$(eval LIBTOOL_INC := $(patsubst ./%, $../%, $(OTFS_INC)) )
	$(eval LIBTOOL_LIB := $(patsubst ./%, $../%, $(OTFS_LIB)) )
#	libtool -o $(TARGET).a -static $(OBJECTS)
	libtool -static --mode=link gcc -g -O3 -o $(TARGET).la $(LIBTOOL_OBJ) $(LIBTOOL_INC) $(LIBTOOL_LIB)
	@mv $(TARGET).a $(TARGETDIR)/

#Library dependencies (not in otfs sources)
$(LIBMODULES): %: 
	cd ./../$@ && $(MAKE) all

#libotfs submodules
$(SUBMODULES): %: $(LIBMODULES) directories
	$(eval MKFILE := $(notdir $@))
	cd ./$@ && $(MAKE) -f $(MKFILE).mk obj

#Non-File Targets
.PHONY: all remake clean cleaner

