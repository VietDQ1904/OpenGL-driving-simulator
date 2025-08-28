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
             src/spline.cpp
             src/roadPath.cpp
             src/terrainPath.cpp
             src/barrierPath.cpp
             src/resourceManagement.cpp
             src/roadSigns.cpp
             src/grass.cpp
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

# Package Freetype
find_package(Freetype REQUIRED)

if (Freetype_FOUND)
    message(STATUS "Freetype package found.")
else()
    message(FATAL_ERROR "Freetype package not found.")
endif()

# Package BZip2
find_package(BZip2 REQUIRED)

if (BZip2_FOUND)
    message(STATUS "BZip2 package found.")
else()
    message(FATAL_ERROR "BZip2 package not found.")
endif()

# Package ZLIB
find_package(ZLIB REQUIRED)

if (ZLIB_FOUND)
    message(STATUS "ZLIB package found.")
else()
    message(FATAL_ERROR "ZLIB package not found.")
endif()

# Binds harfbuzz statically.
set(HARFBUZZ_LIBRARY "${CMAKE_SOURCE_DIR}/lib/harfbuzz/libharfbuzz.a")

if (NOT EXISTS "${HARFBUZZ_LIBRARY}")
    message(FATAL_ERROR "Harfbuzz library not found at ${HARFBUZZ_LIBRARY}")
endif()

set(HARFBUZZ_INCLUDE_DIR "./lib/harfbuzz/include")


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
    ${FREETYPE_INCLUDE_DIRS}
    ${BZIP2_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
    ${HARFBUZZ_INCLUDE_DIRS}
    ${OPENAL_INCLUDE_DIRS}
    # D:/Installs/MSYS/mingw64/include/freetype2
    ./lib/freetype/include
    ./lib/bullet/include
)

# ========= ADD glad STATIC LIBRARY ==========
add_library(glad STATIC ./lib/glad/src/glad.c)

# ========= EXECUTABLE ==========
add_executable(DrivingSimulator ${SOURCES})
# ========= LINK LIBRARIES ==========
target_link_libraries(DrivingSimulator PRIVATE
    glad
    glfw3
    OpenGL::GL
    assimp::assimp

    # Add these before Freetype.
    ${HARFBUZZ_LIBRARY}        
    ${BZIP2_LIBRARIES}
    ${ZLIB_LIBRARIES}
    OpenAL::OpenAL
    Freetype::Freetype
    BulletDynamics BulletCollision LinearMath
)
