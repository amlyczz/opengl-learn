cmake_minimum_required(VERSION 3.21)
project(opengl_learn)

set(CMAKE_CXX_STANDARD 11)

#add_executable(opengl_learn main.cpp test.cpp test.cpp glad.c pano.cpp 01.cpp stb_image.h 02_triangle.cpp)
add_executable(main main.cpp glad.c stb_image.h)
target_link_libraries(main glfw3)

add_executable(test test.cpp glad.c stb_image.h)
target_link_libraries(test glfw3)

add_executable(pano pano.cpp glad.c stb_image.h)
target_link_libraries(pano glfw3)

add_executable(01 01.cpp glad.c stb_image.h)
target_link_libraries(01 glfw3)

add_executable(02 02_triangle.cpp glad.c stb_image.h)
target_link_libraries(02 glfw3)
