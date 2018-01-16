# Default Configuration
TARGET      ?= libotfs
EXT_DEF     ?= -DOT_FEATURE_DLL_SECURITY=0
EXT_INC     ?= 
EXT_LIBS    ?= 

DEFAULT_INC := ./include
LIBMODULES  := 
BUILDDIR    := ./build
SRCEXT      := c
DEPEXT      := d
OBJEXT      := o

# Conditional Settings per Target
ifeq ($(TARGET),libotfs)
	OTFS_CC	    := gcc
	OTFS_LIBTOOL:= libtool
	OTFS_CFLAGS := -std=gnu99 -O3 -pthread
	OTFS_DEF    := $(EXT_DEF)
	OTFS_INC    := -I$(DEFAULT_INC) -I/usr/local/include $(EXT_INC)
	OTFS_LIB    := -Wl,-Bstatic -L./ -L/usr/local/lib -lJudy $(EXT_LIBS)
	PLATFORM    := ./platform/posix_c
	TARGETDIR   ?= pkg

else ifeq ($(TARGET),libotfs.c2000)
	C2000_WARE  ?= /Applications/ti/c2000/C2000Ware_1_00_02_00
	TICC_DIR    ?= /Applications/ti/ccsv7/tools/compiler/ti-cgt-c2000_17.9.0.STS

	OTFS_CC	    := cl2000
	OTFS_LIBTOOL:= ar2000
	OTFS_CFLAGS := --c99 -O2 -v28 -ml -mt -g --cla_support=cla0 --float_support=fpu32 --vcu_support=vcu0 
	OTFS_DEF    := -DAPP_modbus_slave_c2000 -DBOARD_C2000_null -D__TMS320F2806x__ -D__C2000__ -D__TI_C__ -D__NO_SECTIONS__ $(EXT_DEF)
	OTFS_INC    := -I$(TICC_DIR)/include -I$(C2000_WARE) -I$(DEFAULT_INC) $(EXT_INC)
	OTFS_LIB    := -Wl,-Bstatic -L$(TICC_DIR)/lib -L./ $(EXT_LIBS)
	PLATFORM    := ./platform/c2000
	TARGETDIR   ?= pkg

else
	error "TARGET set to unknown value: $(TARGET)"
endif

# Export the following variables to the shell: will affect submodules
export OTFS_CC
export OTFS_CFLAGS
export OTFS_DEF
export OTFS_INC
export OTFS_LIB

# Derived Parameters
SUBMODULES  := main otlib $(PLATFORM)
INCDEP      := -I$(DEFAULT_INC)

# Global vars that get exported to sub-makefiles
all: $(TARGET)
lib: $(TARGET).a
remake: cleaner all

install: 
	@cp ./pkg/$(TARGET).a ./$(TARGETDIR)
	@cp -R ./include/* ./$(TARGETDIR)
	@cp ./main/otfs.h ./$(TARGETDIR)
	@cp /usr/local/include/Judy.h ./$(TARGETDIR)

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
libotfs.a: $(SUBMODULES) $(LIBMODULES)
	$(eval LIBTOOL_OBJ := $(shell find $(BUILDDIR) -type f -name "*.$(OBJEXT)"))
#	$(eval LIBTOOL_INC := $(patsubst $(BUILDDIR)%, $./%, $(OTFS_INC)) )
#	$(eval LIBTOOL_LIB := $(patsubst $(BUILDDIR)%, $./%, $(OTFS_LIB)) )

#   BSD/Mac version of libtool
	$(OTFS_LIBTOOL) -static -o $(TARGET).a /usr/local/lib/libJudy.a $(LIBTOOL_OBJ)

#   GNU/Linux version of libtool
#	$(OTFS_LIBTOOL) --tag=CC --mode=link $(OTFS_CC) -all-static -g -O3 $(OTFS_INC) $(OTFS_LIB) -o $(TARGET).a $(LIBTOOL_OBJ)

#   Traditional ar method (alternative to libtool)
#	ar -rcs ./$(TARGET).a $(LIBTOOL_OBJ)
#	ranlib $(TARGET).a
	
	@mv $(TARGET).a $(TARGETDIR)/


#Build the static library
#Note: testing with libtool now, which may be superior to ar
libotfs.c2000.a: $(SUBMODULES) $(LIBMODULES)
	$(eval LIBTOOL_OBJ := $(shell find $(BUILDDIR)/libotfs.c2000 -type f -name "*.$(OBJEXT)"))
	ar2000 -a $(TARGET).a $(LIBTOOL_OBJ)
	@mv $(TARGET).a $(TARGETDIR)/

#Library dependencies (not in otfs sources)
$(LIBMODULES): %: 
	cd ./../$@ && $(MAKE) all

#libotfs submodules
$(SUBMODULES): %: $(LIBMODULES) directories
	$(eval MKFILE := $(notdir $@))
	cd ./$@ && $(MAKE) -f $(MKFILE).mk obj

#Non-File Targets
.PHONY: all lib remake clean cleaner

