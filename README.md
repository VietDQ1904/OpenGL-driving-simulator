# Car Driving Project with OpenGL #

This project demonstrates a car driving game using OpenGL, GLM, stb_image, Bullet Physics and Assimp. It has a 
player-controlled car and procedural generated terrains using Perlin Noise.

<a name="dependencies"></a>

### List of dependencies ###

   1. MSYS2 MINGW64: compiler for C++. 
   2. VSCode: main code editor.
   3. CMake: building project.
   4. OpenGL (using GLFW 3.4 and glad): rendering the scene.
   5. Bullet Physics: physics implementation.
   6. GLM: matrix and vector calculations.
   7. stb_image: loading images. 
   8. Assimp: loading 3D models.

<a name = "instructions"></a>

### Instructons ###
   1. Install the respository via the command:
      ```
      git clone https://github.com/VietDQ1904/OpenGL-driving-game.git
      ```
   
   2. Change directory to the project. Create a build folder, change your current directory to /build:
      ```
      cd OpenGL-driving-game
      mkdir build
      cd build
      ```
   
   3. Make sure that all dependencies are installed in MSYS2 MINGW64 (use pacman -Q inside MSYS2 to view installed
   packages). Then build the project using the command: 

      ```
      cmake -G "MinGW Makefiles" ..
      cmake --build .
      ```

      If you already use MINGW64 as your main compiler, you don't need to specify it:

      ```
      cmake -G ..
      cmake --build .
      ```

      Alternatively, if you use VSCode, you can download CMake Tools extensions to automate the build process.
   
   4. After you build, you will have an executable file. Run it to start the game.
   
   






