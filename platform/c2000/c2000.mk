MODNAME      := c2000

# Global vars that get exported from primary makefile
OTFS_CC	    ?= cl2000
OTFS_CFLAGS ?= --c99 -O2
OTFS_DEF    ?= -DBOARD_c2000 -DOT_FEATURE_DLL_SECURITY=0 -D__C2000__ -D__TI_C__ -D__NO_SECTIONS__
OTFS_INC    ?= 
OTFS_LIB    ?= 

BUILDDIR    := ../../$(OTFS_BUILDDIR)/$(MODNAME)
TARGETDIR   := .
SRCEXT      := c
DEPEXT      := d
OBJEXT      := o
LIB         := $(OTFS_LIB)
INC         := $(patsubst -I./%, -I./../../%, $(OTFS_INC)) 
INCDEP      := $(INC)

SOURCES     := $(shell find . -type f -name "*.$(SRCEXT)")
OBJECTS     := $(patsubst ./%, $(BUILDDIR)/%, $(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

# Need to specify compiler input flags because TI compiler is stupid (doesn't abide by documentation)
ifneq (,$(findstring gcc,$(OTFS_CC)))
	CCOUT = -o 
else
	CCOUT = --output_file=
endif



all: resources $(MODNAME)
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
	@$(RM) -rf $(BUILDDIR)

#Full Clean, Objects and Binaries
cleaner: clean
	@$(RM) -rf $(TARGETDIR)

#Pull in dependency info for *existing* .o files
-include $(OBJECTS:.$(OBJEXT)=.$(DEPEXT))

#Direct build of the test app with objects
$(MODNAME): $(OBJECTS)
	$(OTFS_CC) -o $(TARGETDIR)/$(MODNAME) $^ $(LIB)

#Compile Stages
$(BUILDDIR)/%.$(OBJEXT): ./%.$(SRCEXT)
	@mkdir -p $(dir $@)
ifeq ($(OTFS_CC),gcc)
	$(OTFS_CC) $(OTFS_CFLAGS) $(OTFS_DEF) $(INC) -c -o $@ $<
	@$(OTFS_CC) $(OTFS_CFLAGS) $(OTFS_DEF) $(INCDEP) -MM ./$*.$(SRCEXT) > $(BUILDDIR)/$*.$(DEPEXT)
	@cp -f $(BUILDDIR)/$*.$(DEPEXT) $(BUILDDIR)/$*.$(DEPEXT).tmp
	@sed -e 's|.*:|$(BUILDDIR)/$*.$(OBJEXT):|' < $(BUILDDIR)/$*.$(DEPEXT).tmp > $(BUILDDIR)/$*.$(DEPEXT)
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.$(DEPEXT).tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.$(DEPEXT)
	@rm -f $(BUILDDIR)/$*.$(DEPEXT).tmp
else
	$(OTFS_CC) $(OTFS_CFLAGS) $(OTFS_DEF) $(INC) -c $(CCOUT)$@ $<
endif

#Non-File Targets
.PHONY: all remake clean cleaner resources

