include_directories("${PROJECT_SOURCE_DIR}/include")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}/Modules/" CACHE PATH "CMake module path" FORCE)

#option(BUILD_TESTS "Build unit tests" OFF)
#option(COLOR_TERMINAL "Allow colorful SOFT output" ON)
option(DEBUG "Compile with debug symbols and no optimizations" OFF)

set(main
	"${PROJECT_SOURCE_DIR}/lib/Expression.cpp"
	"${PROJECT_SOURCE_DIR}/lib/ExpressionParser.cpp"
	"${PROJECT_SOURCE_DIR}/lib/Factor.cpp"
	"${PROJECT_SOURCE_DIR}/lib/SymachinException.cpp"
	"${PROJECT_SOURCE_DIR}/lib/Term.cpp"
)
set(operators
	"${PROJECT_SOURCE_DIR}/lib/Operators/Replace.cpp"
)

if (DEBUG)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-implicit-fallthrough -O0 -g -pg")
else (DEBUG)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-implicit-fallthrough -O3 -march=native")
endif (DEBUG)

set(target ${main} ${operators})
add_library(symachin ${target})

#if (BUILD_TESTS)
#	set(core_tests ${main_tests})
#	add_executable(soft_tests ${core_tests})
#endif (BUILD_TESTS)

configure_file(
	"${PROJECT_SOURCE_DIR}/include/symachin/config.h.in"
	"${PROJECT_SOURCE_DIR}/include/symachin/config.h"
)

