# Driving simulator #

This project demonstrates a driving simulator using OpenGL, Bullet Physics and other libraries. It has a user-controlled car, procedurally generated road path, terrain and road signs.

<a name="dependencies"></a>

### List of dependencies ###
   - C++17 or newer compiler.
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
   - Change the directory to the project:
      ```
      cd OpenGL-driving-simulator
      ```
   - Create a folder for libraries, add every required packages inside the folder.
   - Change directory to the project, create a build folder and change your current directory to /build:
      ```
      mkdir build
      cd build
      ```
   - Make sure all dependencies are installed before building. Build the project using the command (modify the command to make it works for your system): 
      ```
      cmake -G ..
      cmake --build .
      ```
   - After you build, you will have an executable file (DrivingSimulator.exe). Run it to start the program.

<a name= "license"></a>

### License ###
   This project is licensed under MIT license - see [LICENSE](LICENSE) for more details.
   
   






