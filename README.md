# Interactive Simulation of Taal Volcano’s Eruption Plumes in a Customized 3D Scene Editor
- [Introduction](#Introduction)
- [Instructions](#Instructions)


<a name="Introduction"></a>
## Introduction

Contained here is the source code for AnitoPlume which is a real time simulation software for Taal Volcano's eruption plumes. Using this software will allow users to simulate and/or recreate volcanic eruptions of Taal by setting the values that are modifiable before and during the simulation sequence. 
Taal volcano is a volcano located in the southwestern part of Luzon in the Philippines and is considered one of the most active volcanoes in the country. knowing this and the fact that its toxic volcanic plumes cause major risk toward the volcano's immediate surroundings, with a potential of affecting areas miles away from it. knowing this and the lack of tools to help us deal with these eruptions the group made AnitoPlume as while there are other simulations that can give users hazard maps and 3d simulations none exist specifically for Taal.
The Simulation software was built up from the work  of Maud Lastic et. al (2022) specifically the plume movement and mathematics this program improves upon the former by both improving the user experience and adding other functionalities such as a plume direction tracker for seeing the towns that will be hit by the plume, a texture picker for seeing taal in different time periods, an improved camera system that has two camera modes and finally tooltips and landmarks that handle the trivia information on Taal's vents and the scene visualization of the areas that will be hit by the plume.  


<a name="Instructions"></a>
## Instructions

The repository contains all the necessary assets to run the project without additional material, using the CMakeLists.txt file or the Makefile.
If the execution of the project does not work after building because of missing shaders, try copy/pasting the "scenes" folder in the build folder and in the "x64-Debug" folder inside of the build folder.

The core code can be found in "code_plume/scenes/sources/smoke".

Once the program is run you may click on the "Play Button" to begin the Simulation, while the simulation is running you may press the "Pause" and "Restart" buttons to either pause the simulation or restart the simulation to the very beginning while also pausing it.
You may check the box of the display type you want to use from cylindres to billboards in the display settings tab of the gui along with the option to toggle the visibility of the tooltips.
Camera Setting press the radio button of the camera mode you wish to use between aerial view and orbital view with the zoom of orbital view being editted  via the distance slider or through usage of the Mouse Wheel.
For Aerial Camera Controls:
Right Mouse Button: assume control of the camera, mouse movement moves the rotation of the camera in said direction.
WASD: Forward, Backward and sideward translation of the camera.
Q and E: Ascension and Descension moveement of the camera.
In the Program you are able to modify the eruption parameters in the eruption parameters tab of the simulator imput window with the parameters being:
- Initial plume speed
- Initial plume density,
- Vent radius and altitude
For the Wind Settings it is setup in a way that one will use the horizontal slider below the plot points to set the altitude of the wind settings while using the 2 sliders to the left to set the wind intensity and wind direction. The three settings above the slider for linear wind speed are for setting different values for the wind:
No Wind: 0 wind intensity on all altitudes
Linear Wind: Linear increasing wind intensity based on the slider.
Max intensity: Maximum wind intensity on all altitudes.
The checkbox for All Angles just activates away to set the angle for all altitudes to the same one as the angle you are modifying.
Lastly the button for the 2020 eruption sets the wind settings to the parameters for the Taal 2020 eruption.