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
      ```bash
      git clone https://github.com/VietDQ1904/OpenGL-driving-simulator.git
      ```

   - Change the directory to the project:
      ```bash
      cd OpenGL-driving-simulator
      ```

   - Create a folder for libraries, add every required packages inside the folder. The folder must match the expected structure of CMakeLists.txt.

   - Change directory to the project, create a build folder and change your current directory to /build:
      ```bash
      mkdir build
      cd build
      ```

   - Make sure all dependencies are installed before building. Use the command below to configure a CMake generator (specify the generator to make it works for your system):
      + For Windows: 
      ```bash
      cmake -G "MinGW Makefiles" ..
      ```
      
      + For Unix:
      ```bash
      cmake -G "Unix Makefiles" ..  
      ```

      + For Ninja: 
      ```bash
      cmake -G "Ninja" ..
      ```
      
      You can also specify your compiler:
      ```bash
      cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ ..
      ```

   - Build the project with the command: 
      ```bash
      cmake --build .
      ```

   - After you build, you will have an executable file (DrivingSimulator.exe). Run it to start the program.

<a name= "license"></a>

### License ###
   This project is licensed under MIT license - see [LICENSE](LICENSE) for more details.
   
   






