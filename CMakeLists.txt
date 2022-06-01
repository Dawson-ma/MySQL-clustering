PROJECT(clustering)

# Path for MySQL include directory
INCLUDE_DIRECTORIES("C:\Program Files\MySQL\MySQL Server 8.0\include")

ADD_DEFINITIONS("-DHAVE_DLOPEN")
ADD_LIBRARY(clustering MODULE clustering.cc clustering.def)
add_executable(clustering clustering.cc algorithm/fastcluster.cpp algorithm/kmeans_util.cpp)
TARGET_LINK_LIBRARIES(clustering wsock32)