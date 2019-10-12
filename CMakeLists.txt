cmake_minimum_required( VERSION 3.0 )

# project name
project( genetic_regression )

# flags

# files

# include
include_directories( ./include ./src ./src/engine ) 
# add include directories of any other libs above
# e.g.: ./libs/Randomize/include

# target
add_executable( gp_regression ./src/main.cpp ./src/engine/Expression.cpp ./src/engine/Constant.cpp ./src/engine/Minus.cpp ./src/engine/Plus.cpp ./src/engine/Sin.cpp)
# add all other cpp files here ?

#external libs
# add_subdirectory( ./libs/Logger ) only for static 
#target

