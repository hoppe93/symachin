include_directories("${PROJECT_SOURCE_DIR}/include")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}/Modules/" CACHE PATH "CMake module path" FORCE)

option(DEBUG "Compile with debug symbols and no optimizations" OFF)

set(main
	"${PROJECT_SOURCE_DIR}/interpreter/Main.cpp"
	"${PROJECT_SOURCE_DIR}/interpreter/Lexer.cpp"
	"${PROJECT_SOURCE_DIR}/interpreter/Parser.cpp"
	"${PROJECT_SOURCE_DIR}/interpreter/Parser.cmd.cpp"
)

if (DEBUG)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-implicit-fallthrough -O0 -g -pg")
else (DEBUG)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-implicit-fallthrough -O3 -march=native")
endif (DEBUG)

set(target ${main})
add_executable(isymachin ${target})
target_link_libraries(isymachin symachin)

