EXAMPLE_MOD_DIR := $(USERMOD_DIR)

# Add all C files to SRC_USERMOD.
SRC_USERMOD += $(EXAMPLE_MOD_DIR)/examplemodule.c
#SRC_USERMOD_CXX += $(EXAMPLE_MOD_DIR)/test.cpp
# We can add our module folder to include paths if needed
# This is not actually needed in this example.
CFLAGS_USERMOD += -I$(EXAMPLE_MOD_DIR)
CXXFLAGS_USERMOD += -I /usr/include/eigen3/ 
CEXAMPLE_MOD_DIR := $(USERMOD_DIR)
LDFLAGS_USERMOD += -lstdc++
CXXFLAGS_USERMOD += -I$(USERMOD_DIR) -std=c++11
