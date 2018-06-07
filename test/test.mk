TARGET      := test

OTFS_CC	    ?= gcc
OTFS_CFLAGS ?= -std=gnu99 -O3
OTFS_DEF    ?= 
OTFS_INC    ?= 
OTFS_LIB    ?= 

BUILDDIR    := .
TARGETDIR   := .
SRCEXT      := c
DEPEXT      := d
OBJEXT      := o
LIB         := -L./../pkg -lotfs $(patsubst -L./%, -L./../%, $(OTFS_LIB)) 
INC         := -I./../pkg $(patsubst -I./%, -I./../%, $(OTFS_INC)) 
INCDEP      := $(INC)

SOURCES     := $(shell find . -type f -name "*.$(SRCEXT)")
OBJECTS     := $(patsubst ./%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

# Need to specify compiler input flags because TI compiler is stupid (doesn't abide by documentation)
ifeq ($(OTFS_CC),gcc)
	CCOUT = -o 
else
	CCOUT = --output_file=
endif



all: resources $(TARGET)
obj: $(OBJECTS)
remake: cleaner all


#Copy Resources from Resources Directory to Target Directory
resources: directories

#Make the Directories
directories:
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(BUILDDIR)

#Clean only Objects
clean:
	@$(RM) -rf $(BUILDDIR)/*.o

#Full Clean, Objects and Binaries
cleaner: clean
#	@$(RM) -rf $(TARGETDIR)

#Pull in dependency info for *existing* .o files
-include $(OBJECTS:.$(OBJEXT)=.$(DEPEXT))

#Direct build of the test app with objects
$(TARGET): $(OBJECTS)
	$(OTFS_CC) -o $(TARGETDIR)/$(TARGET) $^ $(LIB)

#Compile Stages
$(BUILDDIR)/%.$(OBJEXT): ./%.$(SRCEXT)
	@mkdir -p $(dir $@)
ifeq ($(OTFS_CC),gcc)
	$(OTFS_CC) $(OTFS_CFLAGS) $(OTFS_DEF) $(INC) $(LIB) -o $(basename $@) $<
else
	$(OTFS_CC) $(OTFS_CFLAGS) $(OTFS_DEF) $(INC) -c $(CCOUT)$@ $<
endif

#Non-File Targets
.PHONY: all remake clean cleaner resources

