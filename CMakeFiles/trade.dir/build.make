# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.27

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
CMAKE_COMMAND = /opt/homebrew/Cellar/cmake/3.27.7/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/Cellar/cmake/3.27.7/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/yumin/desktop/ts

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/yumin/desktop/ts

# Include any dependencies generated for this target.
include CMakeFiles/trade.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/trade.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/trade.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/trade.dir/flags.make

CMakeFiles/trade.dir/main.cpp.o: CMakeFiles/trade.dir/flags.make
CMakeFiles/trade.dir/main.cpp.o: main.cpp
CMakeFiles/trade.dir/main.cpp.o: CMakeFiles/trade.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/yumin/desktop/ts/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/trade.dir/main.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/trade.dir/main.cpp.o -MF CMakeFiles/trade.dir/main.cpp.o.d -o CMakeFiles/trade.dir/main.cpp.o -c /Users/yumin/desktop/ts/main.cpp

CMakeFiles/trade.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/trade.dir/main.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/yumin/desktop/ts/main.cpp > CMakeFiles/trade.dir/main.cpp.i

CMakeFiles/trade.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/trade.dir/main.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/yumin/desktop/ts/main.cpp -o CMakeFiles/trade.dir/main.cpp.s

# Object files for target trade
trade_OBJECTS = \
"CMakeFiles/trade.dir/main.cpp.o"

# External object files for target trade
trade_EXTERNAL_OBJECTS =

trade: CMakeFiles/trade.dir/main.cpp.o
trade: CMakeFiles/trade.dir/build.make
trade: /usr/local/lib/libboost_system.dylib
trade: CMakeFiles/trade.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/yumin/desktop/ts/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable trade"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/trade.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/trade.dir/build: trade
.PHONY : CMakeFiles/trade.dir/build

CMakeFiles/trade.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/trade.dir/cmake_clean.cmake
.PHONY : CMakeFiles/trade.dir/clean

CMakeFiles/trade.dir/depend:
	cd /Users/yumin/desktop/ts && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/yumin/desktop/ts /Users/yumin/desktop/ts /Users/yumin/desktop/ts /Users/yumin/desktop/ts /Users/yumin/desktop/ts/CMakeFiles/trade.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/trade.dir/depend

