#-------------------------------------------------------------------------------
# Flags for compiler, assembler and profiler
#-------------------------------------------------------------------------------

# Shell used by make - we need bash to successfully generate dependencies
SHELL = /bin/bash

# Compiler flags
CFLAGS  += -Wall -Wa,-acdhlms=$(<:.dep=.lst) $(DEFINES) $(DFLAGS) $(INCDIR)
CXXFLAGS  += -Wall -Wa,-acdhlms=$(<:.dep=.lst) $(DEFINES) $(DFLAGS) $(INCDIR)

# Assembler flags
ASFLAGS += -Wall -Wa,-acdhlms=$(<:.dep=.lst) -x assembler-with-cpp $(DEFINES) $(DFLAGS) $(INCDIR)

# Profiler flags
CFLAGS_PROF = $(CFLAGS) -finstrument-functions
CXXFLAGS_PROF = $(CXXFLAGS) -finstrument-functions


#-------------------------------------------------------------------------------
# GCC cross-compiler tools
#-------------------------------------------------------------------------------

CC      = $(CROSS_FW)gcc
CXX     = $(CROSS_FW)g++
LD      = $(CROSS_FW)ld
AR      = $(CROSS_FW)ar
STRIP   = $(CROSS_FW)strip
SIZE    = $(CROSS_FW)size
OBJCOPY = $(CROSS_FW)objcopy
OBJDUMP = $(CROSS_FW)objdump
GDB     = $(CROSS_FW)gdb
NM      = $(CROSS_FW)nm
DDD     = ddd


#-------------------------------------------------------------------------------
# Variables to be used to create and evaluate rules
#-------------------------------------------------------------------------------

# Derive target and object file names from source file lists
OBJSC         = $(patsubst %.c, $(OBJDIR)/%.o, $(notdir $(SRCSC)))
OBJSCXX       = $(patsubst %.cpp, $(OBJDIR)/%.o, $(notdir $(SRCSCXX)))
OBJSC_PROF    = $(patsubst %.c, $(OBJDIR)/%.o, $(notdir $(SRCSC_PROF)))
OBJSCXX_PROF  = $(patsubst %.cpp, $(OBJDIR)/%.o, $(notdir $(SRCSCXX_PROF)))
OBJSA         = $(patsubst %.S, $(OBJDIR)/%.o, $(notdir $(SRCSA)))
OBJSa         = $(patsubst %.s, $(OBJDIR)/%.o, $(notdir $(SRCSa)))

# Get list of automatic dependencies which are to be excluded from build
EXCLDEPS    = $(patsubst %,%\|,$(strip $(EXCL_AUTO_DEP_FILES)))
EXCLDEPS   += last_sed_OR_pattern


#-------------------------------------------------------------------------------
# Path and name of the ".dep" file
#-------------------------------------------------------------------------------

MAINDEP = $(OBJDIR)/.dep


#-------------------------------------------------------------------------------
# Include content of the ".dep" file with rules to compile application and
# handle dependencies
#-------------------------------------------------------------------------------

-include $(MAINDEP)


#-------------------------------------------------------------------------------
# Rule to create the ".dep" file with rules to compile application and handle
# dependencies. Its content then becomes a part of this make file.
#-------------------------------------------------------------------------------

$(MAINDEP): $(OBJDIR)
	@for SRC in $(SRCSC); do \
	    BASE=$${SRC##*/}; \
	    echo $(OBJDIR)/$${BASE%.*}.dep: $$SRC; \
	    echo -e \\t'@$$(CC) -MM -MG $$(CFLAGS) $$< | sed -e "s,[^: ]*:,$$@:," | sed -e "s,$$(EXCLDEPS),  ,g" > $$@'; \
	    echo $(OBJDIR)/$${BASE%.*}.o: $(OBJDIR)/$${BASE%.*}.dep; \
	    echo -e \\t'$$(CC) -c $$(CFLAGS) -o $$@ '"$$SRC"; \
	    echo -include $(OBJDIR)/$${BASE%.*}.dep; \
	done > $(MAINDEP)
	@for SRC in $(SRCSCXX); do \
	    BASE=$${SRC##*/}; \
	    echo $(OBJDIR)/$${BASE%.*}.dep: $$SRC; \
	    echo -e \\t'@$$(CXX) -MM -MG $$(CXXFLAGS) $$< | sed -e "s,[^: ]*:,$$@:," | sed -e "s,$$(EXCLDEPS),  ,g" > $$@'; \
	    echo $(OBJDIR)/$${BASE%.*}.o: $(OBJDIR)/$${BASE%.*}.dep; \
	    echo -e \\t'$$(CXX) -c $$(CXXFLAGS) -o $$@ '"$$SRC"; \
	    echo -include $(OBJDIR)/$${BASE%.*}.dep; \
	done >> $(MAINDEP)
	@for SRC in $(SRCSA); do \
	    BASE=$${SRC##*/}; \
	    echo $(OBJDIR)/$${BASE%.*}.dep: $$SRC; \
	    echo -e \\t'@$$(CC) -MM -MG $$(ASFLAGS) $$< | sed -e "s,[^: ]*:,$$@:," | sed -e "s,$$(EXCLDEPS),  ,g" > $$@'; \
	    echo $(OBJDIR)/$${BASE%.*}.o: $(OBJDIR)/$${BASE%.*}.dep; \
	    echo -e \\t'$$(CC) -c $$(ASFLAGS) -o $$@ '"$$SRC"; \
	    echo -include $(OBJDIR)/$${BASE%.*}.dep; \
	done >> $(MAINDEP)
	@for SRC in $(SRCSa); do \
	    BASE=$${SRC##*/}; \
	    echo $(OBJDIR)/$${BASE%.*}.dep: $$SRC; \
	    echo -e \\t'@$$(CC) -MM -MG $$(ASFLAGS) $$< | sed -e "s,[^: ]*:,$$@:," | sed -e "s,$$(EXCLDEPS),  ,g" > $$@'; \
	    echo $(OBJDIR)/$${BASE%.*}.o: $(OBJDIR)/$${BASE%.*}.dep; \
	    echo -e \\t'$$(CC) -c $$(ASFLAGS) -o $$@ '"$$SRC"; \
	    echo -include $(OBJDIR)/$${BASE%.*}.dep; \
	done >> $(MAINDEP)
	@for SRC in $(SRCSC_PROF); do \
	    BASE=$${SRC##*/}; \
	    echo $(OBJDIR)/$${BASE%.*}.dep: $$SRC; \
	    echo -e \\t'@$$(CC) -MM -MG $$(CFLAGS_PROF) $$< | sed -e "s,[^: ]*:,$$@:," | sed -e "s,$$(EXCLDEPS),  ,g" > $$@'; \
	    echo $(OBJDIR)/$${BASE%.*}.o: $(OBJDIR)/$${BASE%.*}.dep; \
	    echo -e \\t'$$(CC) -c $$(CFLAGS_PROF) -o $$@ '"$$SRC"; \
	    echo -include $(OBJDIR)/$${BASE%.*}.dep; \
	done >> $(MAINDEP)
	@for SRC in $(SRCSCXX_PROF); do \
	    BASE=$${SRC##*/}; \
	    echo $(OBJDIR)/$${BASE%.*}.dep: $$SRC; \
	    echo -e \\t'@$$(CXX) -MM -MG $$(CXXFLAGS_PROF) $$< | sed -e "s,[^: ]*:,$$@:," | sed -e "s,$$(EXCLDEPS),  ,g" > $$@'; \
	    echo $(OBJDIR)/$${BASE%.*}.o: $(OBJDIR)/$${BASE%.*}.dep; \
	    echo -e \\t'$$(CXX) -c $$(CXXFLAGS_PROF) -o $$@ '"$$SRC"; \
	    echo -include $(OBJDIR)/$${BASE%.*}.dep; \
	done >> $(MAINDEP)

#-------------------------------------------------------------------------------
# Rule to create object's directory
#-------------------------------------------------------------------------------

$(OBJDIR):
	$(MKDIR) $(OBJDIR)

