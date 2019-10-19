cmake_minimum_required( VERSION 3.0 )

# project name
project( genetic_regression )

# flags

# files
file(GLOB symreg_SRC
    "src/engine/*.cpp"
    "src/expression/*.cpp"
    "src/genetic-programming/*.cpp"
    "src/genetic-programming/reproducers/*.cpp"
    "src/genetic-programming/selectors/*.cpp"
    "src/genetic-programming/mutators/*.cpp"
    "src/*.cpp"
)
# include
include_directories( ./include ./src ./src/engine ) 
# add include directories of any other libs above
# e.g.: ./libs/Randomize/include

# target
add_executable( gp_regression ${symreg_SRC})
set_property(TARGET gp_regression PROPERTY CXX_STANDARD 17)

#external libs
# add_subdirectory( ./libs/Logger ) only for static 
#target
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE RelWithDebInfo)
