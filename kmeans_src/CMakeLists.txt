PROJECT(kmeans)

# Path for MySQL include directory
INCLUDE_DIRECTORIES("C:\Program Files\MySQL\MySQL Server 8.0\include")

ADD_DEFINITIONS("-DHAVE_DLOPEN")
ADD_LIBRARY(kmeans MODULE kmeans.cc kmeans.def)
add_executable(kmeans kmeans.cc kmeans_util.cpp)
TARGET_LINK_LIBRARIES(kmeans wsock32)
