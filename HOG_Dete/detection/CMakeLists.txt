cmake_minimum_required(VERSION 2.8)
project( HOG )
find_package( OpenCV REQUIRED )
set(folder_source main.cpp HOG.cpp)
set(folder_header HOG.h)

SOURCE_GROUP("Source Files" FILES ${folder_source})
SOURCE_GROUP("Header Files" FILES ${folder_header})

add_executable( HOG_DisplayImage ${folder_source} ${folder_header} )
target_link_libraries( HOG_DisplayImage ${OpenCV_LIBS} )
