LIBTOOL=libtool

TARGET      ?= libotfs
TARGETDIR   ?= pkg
EXT_DEF     ?= -DBOARD_posix_a -DAPP_modbus_master -DOT_FEATURE_DLL_SECURITY=0
EXT_INC     ?= 
EXT_LIBS    ?= 

BUILDDIR    := ./build
SRCEXT      := c
DEPEXT      := d
OBJEXT      := o

# Note: Potentially need to add OTEAX to LIBMODULES
DEFAULT_INC := ./include
LIBMODULES  := 
#PLATFORMS   := $(shell find ./platform -type d)
PLATFORMS   := ./platform/posix_c
SUBMODULES  := main otlib $(PLATFORMS)

INC         := -I$(DEFAULT_INC) -I/usr/local/include
INCDEP      := -I$(DEFAULT_INC)
LIB         := -Wl,-Bstatic -L./ -L/usr/local/lib -lJudy

# Global vars that get exported to sub-makefiles
OTFS_CC	    := gcc
OTFS_CFLAGS := -std=gnu99 -O3 -pthread
OTFS_DEF    := $(EXT_DEF)
OTFS_INC    := $(INC) $(EXT_INC)
OTFS_LIB    := $(LIB) $(EXT_LIBS)

# Export the following variables to the shell: will affect submodules
export OTFS_CC
export OTFS_CFLAGS
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
	@cp /usr/local/include/Judy.h ./$(TARGETDIR)

#Build the static library
#Note: testing with libtool now, which may be superior to ar
$(TARGET).a: $(SUBMODULES) $(LIBMODULES)
	$(eval LIBTOOL_OBJ := $(shell find . -type f -name "*.$(OBJEXT)"))
#	$(eval LIBTOOL_INC := $(patsubst $(BUILDDIR)%, $./%, $(OTFS_INC)) )
#	$(eval LIBTOOL_LIB := $(patsubst $(BUILDDIR)%, $./%, $(OTFS_LIB)) )
	$(LIBTOOL) -static -o $(TARGET).a /usr/local/lib/libJudy.a $(LIBTOOL_OBJ)
#	$(LIBTOOL) --tag=CC --mode=link $(OTFS_CC) -all-static -g -O3 $(OTFS_INC) $(OTFS_LIB) -o $(TARGET).a $(LIBTOOL_OBJ)
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

