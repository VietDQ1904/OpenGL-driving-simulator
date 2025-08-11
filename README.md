# Car Driving Project with OpenGL #

This project demonstrates a driving game using OpenGL, GLM, stb_image, Bullet Physics and Assimp. It has a 
player-controlled car and procedural generated road and terrain.

<a name="dependencies"></a>

### List of dependencies ###

   - MSYS2 MINGW64: compiler for C++. 
   - VSCode: main code editor.
   - CMake: building project.
   - OpenGL (using GLFW 3.4 and glad): rendering the scene.
   - Bullet Physics: physics implementation.
   - GLM: matrix and vector calculations.
   - stb_image: loading images. 
   - Assimp: loading 3D models.

<a name = "instructions"></a>

### Instructions ###
   - Install the respository via the command:
      ```
      git clone https://github.com/VietDQ1904/OpenGL-driving-game.git
      ```
   
   - Change directory to the project, create a build folder and change your current directory to /build:
      ```
      cd OpenGL-driving-game
      mkdir build
      cd build
      ```
   
   - Make sure that all dependencies are installed in MSYS2 MINGW64 before building (use pacman -Q inside MSYS2 to view 
   installed packages). Build the project using the command: 

      ```
      cmake -G "MinGW Makefiles" .. 
      cmake --build .
      ```

      If you already used MINGW64 as your main compiler, you don't need to specify it:

      ```
      cmake -G ..
      cmake --build .
      ```

      Alternatively, if you use VSCode, you can download CMake Tools extension to automate the build process.
   
   - After you build, you will have an executable file. Run it to start the game.

<a name= "license"></a>

### License ###
   This project is licensed under MIT license - see [LICENSE](LICENSE) for more details.
   
   






