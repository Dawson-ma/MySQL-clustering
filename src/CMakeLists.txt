PROJECT(clustering_hierar)

# Path for MySQL include directory
INCLUDE_DIRECTORIES("C:\Program Files\MySQL\MySQL Server 8.0\include")

ADD_DEFINITIONS("-DHAVE_DLOPEN")
ADD_LIBRARY(clustering_hierar MODULE clustering_hierar.cc clustering_hierar.def)
add_executable(clustering_hierar clustering_hierar.cc algorithm/fastcluster.cpp)
TARGET_LINK_LIBRARIES(clustering_hierar wsock32)