# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/reisen/project/signal

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/reisen/project/signal/tbBuild

# Include any dependencies generated for this target.
include CMakeFiles/tokenBucket.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/tokenBucket.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/tokenBucket.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/tokenBucket.dir/flags.make

CMakeFiles/tokenBucket.dir/main.c.o: CMakeFiles/tokenBucket.dir/flags.make
CMakeFiles/tokenBucket.dir/main.c.o: /home/reisen/project/signal/main.c
CMakeFiles/tokenBucket.dir/main.c.o: CMakeFiles/tokenBucket.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/reisen/project/signal/tbBuild/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/tokenBucket.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/tokenBucket.dir/main.c.o -MF CMakeFiles/tokenBucket.dir/main.c.o.d -o CMakeFiles/tokenBucket.dir/main.c.o -c /home/reisen/project/signal/main.c

CMakeFiles/tokenBucket.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/tokenBucket.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/reisen/project/signal/main.c > CMakeFiles/tokenBucket.dir/main.c.i

CMakeFiles/tokenBucket.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/tokenBucket.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/reisen/project/signal/main.c -o CMakeFiles/tokenBucket.dir/main.c.s

CMakeFiles/tokenBucket.dir/tokenBucket.c.o: CMakeFiles/tokenBucket.dir/flags.make
CMakeFiles/tokenBucket.dir/tokenBucket.c.o: /home/reisen/project/signal/tokenBucket.c
CMakeFiles/tokenBucket.dir/tokenBucket.c.o: CMakeFiles/tokenBucket.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/reisen/project/signal/tbBuild/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/tokenBucket.dir/tokenBucket.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/tokenBucket.dir/tokenBucket.c.o -MF CMakeFiles/tokenBucket.dir/tokenBucket.c.o.d -o CMakeFiles/tokenBucket.dir/tokenBucket.c.o -c /home/reisen/project/signal/tokenBucket.c

CMakeFiles/tokenBucket.dir/tokenBucket.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/tokenBucket.dir/tokenBucket.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/reisen/project/signal/tokenBucket.c > CMakeFiles/tokenBucket.dir/tokenBucket.c.i

CMakeFiles/tokenBucket.dir/tokenBucket.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/tokenBucket.dir/tokenBucket.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/reisen/project/signal/tokenBucket.c -o CMakeFiles/tokenBucket.dir/tokenBucket.c.s

# Object files for target tokenBucket
tokenBucket_OBJECTS = \
"CMakeFiles/tokenBucket.dir/main.c.o" \
"CMakeFiles/tokenBucket.dir/tokenBucket.c.o"

# External object files for target tokenBucket
tokenBucket_EXTERNAL_OBJECTS =

tokenBucket: CMakeFiles/tokenBucket.dir/main.c.o
tokenBucket: CMakeFiles/tokenBucket.dir/tokenBucket.c.o
tokenBucket: CMakeFiles/tokenBucket.dir/build.make
tokenBucket: CMakeFiles/tokenBucket.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/reisen/project/signal/tbBuild/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable tokenBucket"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/tokenBucket.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/tokenBucket.dir/build: tokenBucket
.PHONY : CMakeFiles/tokenBucket.dir/build

CMakeFiles/tokenBucket.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/tokenBucket.dir/cmake_clean.cmake
.PHONY : CMakeFiles/tokenBucket.dir/clean

CMakeFiles/tokenBucket.dir/depend:
	cd /home/reisen/project/signal/tbBuild && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/reisen/project/signal /home/reisen/project/signal /home/reisen/project/signal/tbBuild /home/reisen/project/signal/tbBuild /home/reisen/project/signal/tbBuild/CMakeFiles/tokenBucket.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/tokenBucket.dir/depend

