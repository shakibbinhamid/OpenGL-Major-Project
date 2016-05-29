OpenGL Major Project : COMP3214

This is a major graphics project in 3rd year Computer Science in University of Southampton. It's a visualisation of
a little neighbourhood in a big city.

BUILD:

on OSX type 'make' at the root directory, then run ./coursework3

on windows VS2015 is required. Not tested on earlier versions. Open
'VS2015/coursework3/coursework3.sln' and build the 'release'
version. 'debug' version will be markedly slow. Then run coursework3-WIN32.exe

KEY BINDINGS:

No use of mouse.

**********************************************************
| H     | Print HELP
----------------------------------------------------------
| T     |
|       | Play a tour from start or
|       | Resume a tour after a pause or
|       | Pause a running tour.
|       | Automatically restarts after end.
----------------------------------------------------------
| E     |
|       | Stop the tour.
|       | Leaves camera at current direction and position.
|       | Pressing T will start the tour.
----------------------------------------------------------
| SPACE |
|       | Pauses the tour.
|       | Same as pressing T during tour
|       | Press T to start the tour from the last tour position
----------------------------------------------------------
| R     |
|       | Restart the tour if tour ongoing.
|       | If no ongoing tour, then just resets the camera.
----------------------------------------------------------
| W     | Move forward, Disabled during tour or auto pilot
| A     | Move left
| S     | Move backwards
| D     | Move right
----------------------------------------------------------
| LEFT  | Look left, Disabled during tour
| RIGHT | Look right
| J     | Look up
| K     | Look down
----------------------------------------------------------
| G     | God Mode Toggle. Fly enabled. Move anywhere.
----------------------------------------------------------
| PG_UP | Elevate up. God mode gets turned on.
| PG_DN | Elevate down. God mode gets turned on.
----------------------------------------------------------
| UP    | Accelarate forward.
| DOWN  | Deccelarate. Accelaration 0 is stop.
----------------------------------------------------------
| P     | Move to predefined viewpoint.
| M     | Return to previous position and direction.
**********************************************************

List of Other Files :

tourInit.txt
  |___________ the initial position for the tour

tourRoute.txt
  |____________ the movements during the tour.

List of Source Files :

main.cpp
  |_______ start of the program. loads all the models, meshes and runs the
  game loop by setting all the uniforms properly etc.

gl_util.cpp
  |__________ see below for gl_util.hpp

include/camera.hpp
  |_________________ Controls the camera. implemented using Euler angles. FPS
  camera (-90 to 90 pitch cutoff, 360 yaw).

include/CircularStepper.h
  |______________________ helper class to step for common circular steps.
  -pi to pi for example. Used to calculate offline steps.

include/gl_util.hpp
  |___________________ helper class for setting up GLEW and GLFW, set key
  bindings and callbacks, texture loading from file

include/mesh.hpp
  |________________ Abstraction class for a Mesh. Defines a Vertex struct,
  creates VBO, VAO, EBO given vertex data. Also calls glDrawTriangles in a
  draw method

include/mesh_generator.hpp
  |_________________________ helper class for generating sphere, cube, plane
  mesh.

include/model.hpp
  |________________ Abstraction for a model. A model consists of one or more
  meshes. Draw command on a model calls draw on meshes. Uses assimp to read
  obj and mtl files and creates meshes. Uses gl_util to load textures. Only
  loads a texture once, even if it's used multiple times.

include/PhysicsWorld.hpp
  |________________________ Abstraction for a bullet dynamics world. Creates
  and adds rigidbodies for spheres, planes and arbritrary models. Simulate
  will step the simulation once. Can return the position of any object given
  name after a step.

include/shader.hpp
  |__________________ Abstraction for a shader program. Can read fragment and
  vertex shader, compile and check them and bind them as well. Use will use
  the shader program.

include/skybox.hpp
  |__________________ helper class for creating and binding a cube texture.

include/tour.hpp
  |________________ Abstraction for a tour. Tour can be paused, stepped,
  stopped, resumed. Can load a tour path from file.

include/world.h
  |_______________ bullet world variables

How it works briefly:
  It loads models using assimp, loads textures for the models using stb_image.
  Then builds shader programs. Then sets up the physics. Finally in game loop
  the key commands are performed. Based on the inputs, a tour may be done etc.
  Rendering includes placing the models in right places, setting some models in
  motion. It simulates day and night by simple trigometric cycle. Street lamps
  are turned on or off by uniform variables.

  Techniques used are -
    1. phong-blinn specular
    2. phong illumination model
    3. Directional light from the sun position
    4. Spotlight near the debris and depot area.
    5. Texturing and model loading, placing and animation.
    6. Attenuation for all light.
    7. Several point lights using street lamps.
    8. Cube mapping and skyboxing.
    9. Environment day-light simulation.

Inspiration:
Bits of code and techniques were taken from these places.

  www.learnopengl.com
  www.opengl-tutorial.org
  www.antongerdelan.net/opengl/
