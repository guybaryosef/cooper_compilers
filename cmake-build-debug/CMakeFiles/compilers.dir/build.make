# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/guy/Downloads/programs/clion-2018.3.4/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/guy/Downloads/programs/clion-2018.3.4/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/guy/projects/cooper/2019_spring/compilers

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/guy/projects/cooper/2019_spring/compilers/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/compilers.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/compilers.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/compilers.dir/flags.make

CMakeFiles/compilers.dir/front-end/lexer/ltests/chars.c.o: CMakeFiles/compilers.dir/flags.make
CMakeFiles/compilers.dir/front-end/lexer/ltests/chars.c.o: ../front-end/lexer/ltests/chars.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/guy/projects/cooper/2019_spring/compilers/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/compilers.dir/front-end/lexer/ltests/chars.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/compilers.dir/front-end/lexer/ltests/chars.c.o   -c /home/guy/projects/cooper/2019_spring/compilers/front-end/lexer/ltests/chars.c

CMakeFiles/compilers.dir/front-end/lexer/ltests/chars.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/compilers.dir/front-end/lexer/ltests/chars.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/guy/projects/cooper/2019_spring/compilers/front-end/lexer/ltests/chars.c > CMakeFiles/compilers.dir/front-end/lexer/ltests/chars.c.i

CMakeFiles/compilers.dir/front-end/lexer/ltests/chars.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/compilers.dir/front-end/lexer/ltests/chars.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/guy/projects/cooper/2019_spring/compilers/front-end/lexer/ltests/chars.c -o CMakeFiles/compilers.dir/front-end/lexer/ltests/chars.c.s

CMakeFiles/compilers.dir/front-end/lexer/ltests/kw.c.o: CMakeFiles/compilers.dir/flags.make
CMakeFiles/compilers.dir/front-end/lexer/ltests/kw.c.o: ../front-end/lexer/ltests/kw.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/guy/projects/cooper/2019_spring/compilers/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/compilers.dir/front-end/lexer/ltests/kw.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/compilers.dir/front-end/lexer/ltests/kw.c.o   -c /home/guy/projects/cooper/2019_spring/compilers/front-end/lexer/ltests/kw.c

CMakeFiles/compilers.dir/front-end/lexer/ltests/kw.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/compilers.dir/front-end/lexer/ltests/kw.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/guy/projects/cooper/2019_spring/compilers/front-end/lexer/ltests/kw.c > CMakeFiles/compilers.dir/front-end/lexer/ltests/kw.c.i

CMakeFiles/compilers.dir/front-end/lexer/ltests/kw.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/compilers.dir/front-end/lexer/ltests/kw.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/guy/projects/cooper/2019_spring/compilers/front-end/lexer/ltests/kw.c -o CMakeFiles/compilers.dir/front-end/lexer/ltests/kw.c.s

CMakeFiles/compilers.dir/front-end/lexer/ltests/num.c.o: CMakeFiles/compilers.dir/flags.make
CMakeFiles/compilers.dir/front-end/lexer/ltests/num.c.o: ../front-end/lexer/ltests/num.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/guy/projects/cooper/2019_spring/compilers/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/compilers.dir/front-end/lexer/ltests/num.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/compilers.dir/front-end/lexer/ltests/num.c.o   -c /home/guy/projects/cooper/2019_spring/compilers/front-end/lexer/ltests/num.c

CMakeFiles/compilers.dir/front-end/lexer/ltests/num.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/compilers.dir/front-end/lexer/ltests/num.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/guy/projects/cooper/2019_spring/compilers/front-end/lexer/ltests/num.c > CMakeFiles/compilers.dir/front-end/lexer/ltests/num.c.i

CMakeFiles/compilers.dir/front-end/lexer/ltests/num.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/compilers.dir/front-end/lexer/ltests/num.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/guy/projects/cooper/2019_spring/compilers/front-end/lexer/ltests/num.c -o CMakeFiles/compilers.dir/front-end/lexer/ltests/num.c.s

CMakeFiles/compilers.dir/front-end/lexer/ltests/op.c.o: CMakeFiles/compilers.dir/flags.make
CMakeFiles/compilers.dir/front-end/lexer/ltests/op.c.o: ../front-end/lexer/ltests/op.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/guy/projects/cooper/2019_spring/compilers/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/compilers.dir/front-end/lexer/ltests/op.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/compilers.dir/front-end/lexer/ltests/op.c.o   -c /home/guy/projects/cooper/2019_spring/compilers/front-end/lexer/ltests/op.c

CMakeFiles/compilers.dir/front-end/lexer/ltests/op.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/compilers.dir/front-end/lexer/ltests/op.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/guy/projects/cooper/2019_spring/compilers/front-end/lexer/ltests/op.c > CMakeFiles/compilers.dir/front-end/lexer/ltests/op.c.i

CMakeFiles/compilers.dir/front-end/lexer/ltests/op.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/compilers.dir/front-end/lexer/ltests/op.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/guy/projects/cooper/2019_spring/compilers/front-end/lexer/ltests/op.c -o CMakeFiles/compilers.dir/front-end/lexer/ltests/op.c.s

CMakeFiles/compilers.dir/front-end/lexer/my_test/lexer-tester.c.o: CMakeFiles/compilers.dir/flags.make
CMakeFiles/compilers.dir/front-end/lexer/my_test/lexer-tester.c.o: ../front-end/lexer/my_test/lexer-tester.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/guy/projects/cooper/2019_spring/compilers/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object CMakeFiles/compilers.dir/front-end/lexer/my_test/lexer-tester.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/compilers.dir/front-end/lexer/my_test/lexer-tester.c.o   -c /home/guy/projects/cooper/2019_spring/compilers/front-end/lexer/my_test/lexer-tester.c

CMakeFiles/compilers.dir/front-end/lexer/my_test/lexer-tester.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/compilers.dir/front-end/lexer/my_test/lexer-tester.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/guy/projects/cooper/2019_spring/compilers/front-end/lexer/my_test/lexer-tester.c > CMakeFiles/compilers.dir/front-end/lexer/my_test/lexer-tester.c.i

CMakeFiles/compilers.dir/front-end/lexer/my_test/lexer-tester.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/compilers.dir/front-end/lexer/my_test/lexer-tester.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/guy/projects/cooper/2019_spring/compilers/front-end/lexer/my_test/lexer-tester.c -o CMakeFiles/compilers.dir/front-end/lexer/my_test/lexer-tester.c.s

CMakeFiles/compilers.dir/front-end/lexer/lexer.c.o: CMakeFiles/compilers.dir/flags.make
CMakeFiles/compilers.dir/front-end/lexer/lexer.c.o: ../front-end/lexer/lexer.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/guy/projects/cooper/2019_spring/compilers/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object CMakeFiles/compilers.dir/front-end/lexer/lexer.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/compilers.dir/front-end/lexer/lexer.c.o   -c /home/guy/projects/cooper/2019_spring/compilers/front-end/lexer/lexer.c

CMakeFiles/compilers.dir/front-end/lexer/lexer.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/compilers.dir/front-end/lexer/lexer.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/guy/projects/cooper/2019_spring/compilers/front-end/lexer/lexer.c > CMakeFiles/compilers.dir/front-end/lexer/lexer.c.i

CMakeFiles/compilers.dir/front-end/lexer/lexer.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/compilers.dir/front-end/lexer/lexer.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/guy/projects/cooper/2019_spring/compilers/front-end/lexer/lexer.c -o CMakeFiles/compilers.dir/front-end/lexer/lexer.c.s

CMakeFiles/compilers.dir/front-end/lexer/lheader2.c.o: CMakeFiles/compilers.dir/flags.make
CMakeFiles/compilers.dir/front-end/lexer/lheader2.c.o: ../front-end/lexer/lheader2.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/guy/projects/cooper/2019_spring/compilers/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building C object CMakeFiles/compilers.dir/front-end/lexer/lheader2.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/compilers.dir/front-end/lexer/lheader2.c.o   -c /home/guy/projects/cooper/2019_spring/compilers/front-end/lexer/lheader2.c

CMakeFiles/compilers.dir/front-end/lexer/lheader2.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/compilers.dir/front-end/lexer/lheader2.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/guy/projects/cooper/2019_spring/compilers/front-end/lexer/lheader2.c > CMakeFiles/compilers.dir/front-end/lexer/lheader2.c.i

CMakeFiles/compilers.dir/front-end/lexer/lheader2.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/compilers.dir/front-end/lexer/lheader2.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/guy/projects/cooper/2019_spring/compilers/front-end/lexer/lheader2.c -o CMakeFiles/compilers.dir/front-end/lexer/lheader2.c.s

CMakeFiles/compilers.dir/front-end/parser/tests/exprtests.c.o: CMakeFiles/compilers.dir/flags.make
CMakeFiles/compilers.dir/front-end/parser/tests/exprtests.c.o: ../front-end/parser/tests/exprtests.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/guy/projects/cooper/2019_spring/compilers/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building C object CMakeFiles/compilers.dir/front-end/parser/tests/exprtests.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/compilers.dir/front-end/parser/tests/exprtests.c.o   -c /home/guy/projects/cooper/2019_spring/compilers/front-end/parser/tests/exprtests.c

CMakeFiles/compilers.dir/front-end/parser/tests/exprtests.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/compilers.dir/front-end/parser/tests/exprtests.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/guy/projects/cooper/2019_spring/compilers/front-end/parser/tests/exprtests.c > CMakeFiles/compilers.dir/front-end/parser/tests/exprtests.c.i

CMakeFiles/compilers.dir/front-end/parser/tests/exprtests.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/compilers.dir/front-end/parser/tests/exprtests.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/guy/projects/cooper/2019_spring/compilers/front-end/parser/tests/exprtests.c -o CMakeFiles/compilers.dir/front-end/parser/tests/exprtests.c.s

CMakeFiles/compilers.dir/front-end/parser/tests/my_test.c.o: CMakeFiles/compilers.dir/flags.make
CMakeFiles/compilers.dir/front-end/parser/tests/my_test.c.o: ../front-end/parser/tests/my_test.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/guy/projects/cooper/2019_spring/compilers/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building C object CMakeFiles/compilers.dir/front-end/parser/tests/my_test.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/compilers.dir/front-end/parser/tests/my_test.c.o   -c /home/guy/projects/cooper/2019_spring/compilers/front-end/parser/tests/my_test.c

CMakeFiles/compilers.dir/front-end/parser/tests/my_test.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/compilers.dir/front-end/parser/tests/my_test.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/guy/projects/cooper/2019_spring/compilers/front-end/parser/tests/my_test.c > CMakeFiles/compilers.dir/front-end/parser/tests/my_test.c.i

CMakeFiles/compilers.dir/front-end/parser/tests/my_test.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/compilers.dir/front-end/parser/tests/my_test.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/guy/projects/cooper/2019_spring/compilers/front-end/parser/tests/my_test.c -o CMakeFiles/compilers.dir/front-end/parser/tests/my_test.c.s

CMakeFiles/compilers.dir/front-end/parser/parser.c.o: CMakeFiles/compilers.dir/flags.make
CMakeFiles/compilers.dir/front-end/parser/parser.c.o: ../front-end/parser/parser.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/guy/projects/cooper/2019_spring/compilers/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building C object CMakeFiles/compilers.dir/front-end/parser/parser.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/compilers.dir/front-end/parser/parser.c.o   -c /home/guy/projects/cooper/2019_spring/compilers/front-end/parser/parser.c

CMakeFiles/compilers.dir/front-end/parser/parser.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/compilers.dir/front-end/parser/parser.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/guy/projects/cooper/2019_spring/compilers/front-end/parser/parser.c > CMakeFiles/compilers.dir/front-end/parser/parser.c.i

CMakeFiles/compilers.dir/front-end/parser/parser.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/compilers.dir/front-end/parser/parser.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/guy/projects/cooper/2019_spring/compilers/front-end/parser/parser.c -o CMakeFiles/compilers.dir/front-end/parser/parser.c.s

CMakeFiles/compilers.dir/front-end/parser/parser_tester.c.o: CMakeFiles/compilers.dir/flags.make
CMakeFiles/compilers.dir/front-end/parser/parser_tester.c.o: ../front-end/parser/parser_tester.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/guy/projects/cooper/2019_spring/compilers/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building C object CMakeFiles/compilers.dir/front-end/parser/parser_tester.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/compilers.dir/front-end/parser/parser_tester.c.o   -c /home/guy/projects/cooper/2019_spring/compilers/front-end/parser/parser_tester.c

CMakeFiles/compilers.dir/front-end/parser/parser_tester.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/compilers.dir/front-end/parser/parser_tester.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/guy/projects/cooper/2019_spring/compilers/front-end/parser/parser_tester.c > CMakeFiles/compilers.dir/front-end/parser/parser_tester.c.i

CMakeFiles/compilers.dir/front-end/parser/parser_tester.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/compilers.dir/front-end/parser/parser_tester.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/guy/projects/cooper/2019_spring/compilers/front-end/parser/parser_tester.c -o CMakeFiles/compilers.dir/front-end/parser/parser_tester.c.s

CMakeFiles/compilers.dir/front-end/parser/pheader_ast.c.o: CMakeFiles/compilers.dir/flags.make
CMakeFiles/compilers.dir/front-end/parser/pheader_ast.c.o: ../front-end/parser/pheader_ast.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/guy/projects/cooper/2019_spring/compilers/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building C object CMakeFiles/compilers.dir/front-end/parser/pheader_ast.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/compilers.dir/front-end/parser/pheader_ast.c.o   -c /home/guy/projects/cooper/2019_spring/compilers/front-end/parser/pheader_ast.c

CMakeFiles/compilers.dir/front-end/parser/pheader_ast.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/compilers.dir/front-end/parser/pheader_ast.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/guy/projects/cooper/2019_spring/compilers/front-end/parser/pheader_ast.c > CMakeFiles/compilers.dir/front-end/parser/pheader_ast.c.i

CMakeFiles/compilers.dir/front-end/parser/pheader_ast.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/compilers.dir/front-end/parser/pheader_ast.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/guy/projects/cooper/2019_spring/compilers/front-end/parser/pheader_ast.c -o CMakeFiles/compilers.dir/front-end/parser/pheader_ast.c.s

# Object files for target compilers
compilers_OBJECTS = \
"CMakeFiles/compilers.dir/front-end/lexer/ltests/chars.c.o" \
"CMakeFiles/compilers.dir/front-end/lexer/ltests/kw.c.o" \
"CMakeFiles/compilers.dir/front-end/lexer/ltests/num.c.o" \
"CMakeFiles/compilers.dir/front-end/lexer/ltests/op.c.o" \
"CMakeFiles/compilers.dir/front-end/lexer/my_test/lexer-tester.c.o" \
"CMakeFiles/compilers.dir/front-end/lexer/lexer.c.o" \
"CMakeFiles/compilers.dir/front-end/lexer/lheader2.c.o" \
"CMakeFiles/compilers.dir/front-end/parser/tests/exprtests.c.o" \
"CMakeFiles/compilers.dir/front-end/parser/tests/my_test.c.o" \
"CMakeFiles/compilers.dir/front-end/parser/parser.c.o" \
"CMakeFiles/compilers.dir/front-end/parser/parser_tester.c.o" \
"CMakeFiles/compilers.dir/front-end/parser/pheader_ast.c.o"

# External object files for target compilers
compilers_EXTERNAL_OBJECTS = \
"/home/guy/projects/cooper/2019_spring/compilers/front-end/parser/lheaders.o" \
"/home/guy/projects/cooper/2019_spring/compilers/front-end/parser/parser_tester.o" \
"/home/guy/projects/cooper/2019_spring/compilers/front-end/parser/pheader_ast.o"

compilers: CMakeFiles/compilers.dir/front-end/lexer/ltests/chars.c.o
compilers: CMakeFiles/compilers.dir/front-end/lexer/ltests/kw.c.o
compilers: CMakeFiles/compilers.dir/front-end/lexer/ltests/num.c.o
compilers: CMakeFiles/compilers.dir/front-end/lexer/ltests/op.c.o
compilers: CMakeFiles/compilers.dir/front-end/lexer/my_test/lexer-tester.c.o
compilers: CMakeFiles/compilers.dir/front-end/lexer/lexer.c.o
compilers: CMakeFiles/compilers.dir/front-end/lexer/lheader2.c.o
compilers: CMakeFiles/compilers.dir/front-end/parser/tests/exprtests.c.o
compilers: CMakeFiles/compilers.dir/front-end/parser/tests/my_test.c.o
compilers: CMakeFiles/compilers.dir/front-end/parser/parser.c.o
compilers: CMakeFiles/compilers.dir/front-end/parser/parser_tester.c.o
compilers: CMakeFiles/compilers.dir/front-end/parser/pheader_ast.c.o
compilers: ../front-end/parser/lheaders.o
compilers: ../front-end/parser/parser_tester.o
compilers: ../front-end/parser/pheader_ast.o
compilers: CMakeFiles/compilers.dir/build.make
compilers: CMakeFiles/compilers.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/guy/projects/cooper/2019_spring/compilers/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Linking C executable compilers"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/compilers.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/compilers.dir/build: compilers

.PHONY : CMakeFiles/compilers.dir/build

CMakeFiles/compilers.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/compilers.dir/cmake_clean.cmake
.PHONY : CMakeFiles/compilers.dir/clean

CMakeFiles/compilers.dir/depend:
	cd /home/guy/projects/cooper/2019_spring/compilers/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/guy/projects/cooper/2019_spring/compilers /home/guy/projects/cooper/2019_spring/compilers /home/guy/projects/cooper/2019_spring/compilers/cmake-build-debug /home/guy/projects/cooper/2019_spring/compilers/cmake-build-debug /home/guy/projects/cooper/2019_spring/compilers/cmake-build-debug/CMakeFiles/compilers.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/compilers.dir/depend

