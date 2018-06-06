cmake_minimum_required(VERSION 3.5)

project(scene_library)
add_definitions("-std=c++11")

file(GLOB SOURCES "lodepng/*.cpp")
add_library(lodepng_library STATIC ${SOURCES})
target_include_directories(lodepng_library
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/lodepng
)

file(GLOB SOURCES "scene/*.cpp"  "scene/*.h")
add_library(scene_library STATIC ${SOURCES})
target_include_directories(scene_library
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/scene
)

file(GLOB SOURCES "application/*.cpp")
add_executable(app ${SOURCES})
target_include_directories(app 
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/application
)
target_link_libraries(app PRIVATE scene_library lodepng_library)