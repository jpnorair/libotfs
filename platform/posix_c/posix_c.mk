CC=gcc

MODNAME      := posix_c
OTFS_DEF    ?= 
OTFS_INC    ?=
OTFS_LIB    ?= 

#CFLAGS      := -std=gnu99 -O -g -Wall
CFLAGS      := -std=gnu99 -O3 -fPIC

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
	$(CC) -o $(TARGETDIR)/$(MODNAME) $^ $(LIB)

#Compile Stages
$(BUILDDIR)/%.$(OBJEXT): ./%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(OTFS_DEF) $(INC) -c -o $@ $<
	@$(CC) $(CFLAGS) $(OTFS_DEF) $(INCDEP) -MM ./$*.$(SRCEXT) > $(BUILDDIR)/$*.$(DEPEXT)
	@cp -f $(BUILDDIR)/$*.$(DEPEXT) $(BUILDDIR)/$*.$(DEPEXT).tmp
	@sed -e 's|.*:|$(BUILDDIR)/$*.$(OBJEXT):|' < $(BUILDDIR)/$*.$(DEPEXT).tmp > $(BUILDDIR)/$*.$(DEPEXT)
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.$(DEPEXT).tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.$(DEPEXT)
	@rm -f $(BUILDDIR)/$*.$(DEPEXT).tmp

#Non-File Targets
.PHONY: all remake clean cleaner resources

