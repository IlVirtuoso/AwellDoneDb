cmake_minimum_required(VERSION 3.0.0)

project(AWellDoneDB VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(AWellDoneDB
src/XmlParser.cpp
src/Table.iml.cpp
src/Types.iml.cpp
src/Database.iml.cpp
main.cpp 
src/SQLParser.iml.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
