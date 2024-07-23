file(GLOB SAC_SHARED_SRCS *.cpp)

add_library(SACShared STATIC ${SAC_SHARED_SRCS})
target_include_directories(SACShared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)