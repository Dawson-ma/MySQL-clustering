PROJECT(clustering)

# Path for MySQL include directory
INCLUDE_DIRECTORIES("C:\\Program Files\\MySQL\\MySQL Server 8.0\\include")

ADD_DEFINITIONS("-DHAVE_DLOPEN")
ADD_LIBRARY(clustering MODULE clustering.cc clustering.def)
add_executable(clustering clustering.cc algorithm/fastcluster.cpp "algorithm/fastcluster_dm.h" "algorithm/fastcluster_R_dm.h" algorithm/kmeans_util.cpp algorithm/dbscan.h algorithm/kdtree.cpp)
TARGET_LINK_LIBRARIES(clustering wsock32)