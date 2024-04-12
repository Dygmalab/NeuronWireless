#-------------------------------------------------------------------------------
# Commonly used executables
#-------------------------------------------------------------------------------

# Remove file or directory
RM = rm -f -r
# Create directory
MKDIR = mkdir -p
# Copy file or
CP = cp -f -r

#-------------------------------------------------------------------------------
# Initial definitions for variables intended to be extended or redefined later
#-------------------------------------------------------------------------------
# BSP Target board variant
BSP_TARGET_BOARD = UNKNOWN

# Include search directories
INCDIR =

# C sources
SRCSC =

# C++ sources
SRCSCXX =

# Assembler sources
SRCSA =
SRCSa =

# Build defines
DEFINES =

# Automatic dependencies to be excluded from build
EXCL_AUTO_DEP_FILES =

# Flags common to compiler, assembler and linker
COMMONFLAGS =

# C Compiler flags
CFLAGS =

# C++ Compiler flags
CXXFLAGS =

# Assembler flags
ASFLAGS =

# Debug flags
DFLAGS =

# Linker flags
LFLAGS =

# Library directories
LIBDIR = ./

# Libraries
LIBS =

#-------------------------------------------------------------------------------
# Debug/release mode
#-------------------------------------------------------------------------------

# Debug/release mode dedicated directory name
MODE_DEBUG_DIR_NAME = debug
MODE_RELEASE_DIR_NAME = release

# Settings for debug/release mode
ifneq ($(MAKECMDGOALS),debug)
  MODE = release
  MODE_DIR_NAME = $(MODE_RELEASE_DIR_NAME)
else
  MODE = debug
  MODE_DIR_NAME = $(MODE_DEBUG_DIR_NAME)
  DFLAGS += -g
  DFLAGS += -DDEBUG

endif


#-------------------------------------------------------------------------------
# Build process output
#-------------------------------------------------------------------------------

# Name of the directory dedicated to object, dependencies and list files
OBJ_DIR_NAME = obj

# Directory dedicated to object, dependencies and list files
OBJDIR = ./$(MODE_DIR_NAME)/$(OBJ_DIR_NAME)

# Application binary output directory
BINARY_OUTPUT_DIR = ./$(MODE_DIR_NAME)

# Application binary output (path and application name)
BINARY_OUTPUT = $(BINARY_OUTPUT_DIR)/$(APP_NAME)

