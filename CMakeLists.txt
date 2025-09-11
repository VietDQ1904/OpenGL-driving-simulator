cmake_minimum_required(VERSION 3.15)
project(DrivingSimulator)
set(CMAKE_CXX_STANDARD 17)

set (SOURCES src/main.cpp
             src/model.cpp
             src/shader.cpp
             src/cubemap.cpp
             src/physics.cpp
             src/car.cpp
             src/texture.cpp
             src/camera.cpp
             src/perlinNoise.cpp
             src/worleyNoise.cpp
             src/spline.cpp
             src/roadPath.cpp
             src/terrainPath.cpp
             src/barrierPath.cpp
             src/resourceManagement.cpp
             src/roadSigns.cpp
             src/grass.cpp
             src/tree.cpp
             src/soundTrack.cpp
    )

set (IMGUI  lib/imgui/imgui.cpp
            lib/imgui/imgui_draw.cpp
            lib/imgui/imgui_tables.cpp
            lib/imgui/imgui_widgets.cpp
            lib/imgui/backends/imgui_impl_glfw.cpp
            lib/imgui/backends/imgui_impl_opengl3.cpp
    )

set (IMPLOT lib/implot/implot.cpp
            lib/implot/implot_items.cpp
    )

# ========= FIND LIBRARIES ==========
find_package(OpenGL REQUIRED)

# Package OpenGL
if (OpenGL_FOUND)
    message(STATUS "OpenGL package found.")
else()
    message(FATAL_ERROR "OpenGL package not found.")
endif()

# Package Assimp
find_package(Assimp REQUIRED)

if (Assimp_FOUND)
    message(STATUS "Assimp package found.")
else()
    message(FATAL_ERROR "Assimp package not found.")
endif()

find_package(OpenAL REQUIRED)
if (OpenAL_FOUND)
    message(STATUS "OpenAL package found.")
else()
    message(FATAL_ERROR "OpenAL package not found.")
endif()

find_package(Bullet REQUIRED)
if (Bullet_FOUND)
    message(STATUS "Bullet Physics package found.")
else()
    message(FATAL_ERROR "Bullet Physics package not found.")
endif()

# ========= INCLUDE DIRECTORIES ==========
include_directories(
    ./lib/glad/include
    ./lib/glm/glmlib/include
    ./lib/imgui
    ./lib/imgui/backends 
    ./lib/implot
    ${OPENAL_INCLUDE_DIRS}
    ./lib/freetype/include
    ./lib/bullet/include
)

# ========= ADD glad STATIC LIBRARY ==========
add_library(glad STATIC ./lib/glad/src/glad.c)

# ========= EXECUTABLE ==========
add_executable(DrivingSimulator ${SOURCES} ${IMGUI} ${IMPLOT})
# ========= LINK LIBRARIES ==========
target_link_libraries(DrivingSimulator PRIVATE
    glad
    glfw3
    OpenGL::GL
    assimp::assimp
    OpenAL::OpenAL
    BulletDynamics BulletCollision LinearMath
)
