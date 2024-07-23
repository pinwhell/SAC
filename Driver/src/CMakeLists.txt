file(GLOB SACDRV_SRCS *.cpp)
add_library(SACDrv SHARED ${SACDRV_SRCS})
target_include_directories(SACDrv PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_libraries(SACDrv VBXIDrvSDK SACShared)