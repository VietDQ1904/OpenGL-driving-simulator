# Driving simulator #

This project demonstrates a driving simulator using OpenGL, Bullet Physics and other libraries. It has a user-controlled car, procedurally generated road path, terrain and road signs.

<a name="dependencies"></a>

### List of dependencies ###

   - MSYS2 MINGW-W64: compiler for C++. 
   - VSCode: main code editor.
   - CMake: building project.
   - GLAD: loading OpenGL specifications.
   - GLFW 3.4: creating window, handle events.
   - Bullet Physics: physics implementation.
   - GLM: matrix and vector calculations.
   - stb_image: loading images. 
   - Assimp: loading 3D models.

<a name = "instructions"></a>

### Instructions ###
   - Install the respository via the command:
      ```
      git clone https://github.com/VietDQ1904/OpenGL-driving-simulator.git
      ```
   
   - Change directory to the project, create a build folder and change your current directory to /build:
      ```
      cd OpenGL-driving-simulator
      mkdir build
      cd build
      ```
   
   - Make sure all dependencies are installed before building (use "pacman -Q" inside MSYS2 terminal to view 
   installed packages). Build the project using the command (for G++ compiler): 

      ```
      cmake -G "MinGW Makefiles" .. 
      cmake --build .
      ```

      If you already have a default compiler, you don't need to specify it:

      ```
      cmake -G ..
      cmake --build .
      ```

      Alternatively, if you use VSCode, you can download CMake Tools extension to automate the build process.
   
   - After you build, you will have an executable file (DrivingSimulator.exe). Run it to start the program.

<a name= "license"></a>

### License ###
   This project is licensed under MIT license - see [LICENSE](LICENSE) for more details.
   
   






