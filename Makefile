# Default Configuration
TARGET      ?= $(shell uname -srm | sed -e 's/ /-/g')
EXT_DEF     ?= 
EXT_INC     ?= 
EXT_LIBS    ?= 
VERSION     ?= 0.1.0

DEFAULT_INC := ./include
LIBMODULES  := OTEAX libjudy
SRCEXT      := c
DEPEXT      := d
OBJEXT      := o
THISMACHINE := $(shell uname -srm | sed -e 's/ /-/g')
THISSYSTEM	:= $(shell uname -s)

# Conditional Settings per Target
ifeq ($(TARGET),$(THISMACHINE))
	ifeq ($(THISSYSTEM),Darwin)
	# Mac can't do conditional selection of static and dynamic libs at link time.
	#	PRODUCT_LIBS := libotfs.$(THISSYSTEM).dylib libotfs.$(THISSYSTEM).a
	PRODUCT_LIBS := libotfs.$(THISSYSTEM).a
	else ifeq ($(THISSYSTEM),Linux)
		PRODUCT_LIBS := libotfs.$(THISSYSTEM).so libotfs.$(THISSYSTEM).a
	else
		error "THISSYSTEM set to unknown value: $(THISSYSTEM)"
	endif
	BUILDDIR    := build/$(THISMACHINE)
	PRODUCTDIR  := pkg/$(THISMACHINE)
	PACKAGEDIR  ?= ./../_hbpkg/$(THISMACHINE)/libotfs.$(VERSION)
	OTFS_CC	    := gcc
	OTFS_LIBTOOL:= libtool
	OTFS_CFLAGS := -std=gnu99 -O3 -pthread -fPIC
	OTFS_DEF    := $(EXT_DEF)
	OTFS_INC    := -I$(DEFAULT_INC) -I./../_hbpkg/$(TARGET)/libjudy -I./../_hbpkg/$(TARGET)/liboteax $(EXT_INC)
	OTFS_LIB    := $(patsubst -I%,-L%,$(OTFS_INC)) -ljudy -loteax $(EXT_LIBS)
	PLATFORM    := ./platform/posix_c

else ifeq ($(TARGET),c2000)
	BUILDDIR    := build/$(TARGET)
	PRODUCTDIR  := pkg/$(TARGET)
	PRODUCT_LIBS:= libotfs.c2000
	PACKAGEDIR  ?= ./../_hbpkg/$(TARGET)/libotfs.$(VERSION)
	C2000_WARE  ?= /Applications/ti/c2000/C2000Ware_1_00_02_00
	TICC_DIR    ?= /Applications/ti/ccsv7/tools/compiler/ti-cgt-c2000_17.9.0.STS
	OTFS_CC	    := cl2000
	OTFS_LIBTOOL:= ar2000
	OTFS_CFLAGS := --c99 -O2 -v28 -ml -mt -g --cla_support=cla0 --float_support=fpu32 --vcu_support=vcu0 
	OTFS_DEF    := -DAPP_csip_c2000 -DBOARD_C2000_null -D__TMS320F2806x__ -D__C2000__ -D__TI_C__ -D__NO_SECTIONS__ $(EXT_DEF)
	OTFS_INC    := -I$(TICC_DIR)/include -I$(C2000_WARE) -I$(DEFAULT_INC) $(EXT_INC)
	OTFS_LIB    := -Wl,-Bstatic -L$(TICC_DIR)/lib -L./ $(EXT_LIBS)
	PLATFORM    := ./platform/c2000

else
	error "TARGET set to unknown value: $(TARGET)"
endif

# Export the following variables to the shell: will affect submodules
OTFS_BUILDDIR := $(BUILDDIR)
OTFS_PRODUCTDIR := $(PRODUCTDIR)
export OTFS_BUILDDIR
export OTFS_PRODUCTDIR
export OTFS_CC
export OTFS_CFLAGS
export OTFS_DEF
export OTFS_INC
export OTFS_LIB

# Derived Parameters
SUBMODULES  := main otlib otsys $(PLATFORM)
INCDEP      := -I$(DEFAULT_INC)

# Global vars that get exported to sub-makefiles
all: $(PRODUCT_LIBS) test
lib: $(PRODUCT_LIBS)
remake: cleaner all

install: 
	@rm -rf $(PACKAGEDIR)
	@mkdir -p $(PACKAGEDIR)
	@cp -R ./$(PRODUCTDIR)/* ./$(PACKAGEDIR)/
	@rm -f $(PACKAGEDIR)/../libotfs
	@ln -s libotfs.$(VERSION) ./$(PACKAGEDIR)/../libotfs
	
directories:
	@mkdir -p $(PRODUCTDIR)
	@mkdir -p $(BUILDDIR)
	@cp -R ./include/* ./$(PRODUCTDIR)/
	@cp -R ./main/otfs.h ./$(PRODUCTDIR)/

#Clean only Objects
clean:
	@$(RM) -rf $(BUILDDIR)
	@$(RM) -rf $(PRODUCTDIR)

#Full Clean, Objects and Binaries
cleaner: clean
	@$(RM) -rf pkg

# Test
test: $(PRODUCT_LIBS)
	$(eval MKFILE := $(notdir $@))
	cd ./$@ && $(MAKE) -f $(MKFILE).mk obj

#Build the static library
#There are several supported variants.
libotfs.Darwin.a: $(SUBMODULES) $(LIBMODULES)
	$(eval LIBTOOL_OBJ := $(shell find $(BUILDDIR) -type f -name "*.$(OBJEXT)"))
	$(OTFS_LIBTOOL) -static -o libotfs.a ./../_hbpkg/$(TARGET)/libjudy/libjudy.a $(LIBTOOL_OBJ)
	@mv libotfs.a $(PRODUCTDIR)/

libotfs.Linux.a: $(SUBMODULES) $(LIBMODULES)
	$(eval LIBTOOL_OBJ := $(shell find $(BUILDDIR) -type f -name "*.$(OBJEXT)"))
	ar -rcs $(PRODUCTDIR)/libotfs.a $(LIBTOOL_OBJ)
	ranlib $(PRODUCTDIR)/libotfs.a
#	$(OTFS_LIBTOOL) --tag=CC --mode=link $(OTFS_CC) -all-static -g -O3 $(OTFS_INC) $(OTFS_LIB) -o libotfs.a $(LIBTOOL_OBJ)
#	@mv libotfs.a $(PRODUCTDIR)/

libotfs.c2000.a: $(SUBMODULES) $(LIBMODULES)
	$(eval LIBTOOL_OBJ := $(shell find $(BUILDDIR) -type f -name "*.$(OBJEXT)"))
	ar2000 -a libotfs.a $(LIBTOOL_OBJ)
	@mv libotfs.a $(PRODUCTDIR)/

#Build Shared Library
libotfs.Linux.so: $(SUBMODULES) $(LIBMODULES)
	$(eval LIBTOOL_OBJ := $(shell find $(BUILDDIR) -type f -name "*.$(OBJEXT)"))
	$(OTFS_CC) -shared -fPIC -Wl,-soname,libotfs.so.1 -o $(PRODUCTDIR)/libotfs.so.$(VERSION) $(LIBTOOL_OBJ) -lc
	
	
#Library dependencies (not in otfs sources)
$(LIBMODULES): %: 
	cd ./../$@ && $(MAKE) all && $(MAKE) install

#libotfs submodules
$(SUBMODULES): %: $(LIBMODULES) directories
	$(eval MKFILE := $(notdir $@))
	cd ./$@ && $(MAKE) -f $(MKFILE).mk obj

#Non-File Targets
.PHONY: all lib remake clean cleaner

