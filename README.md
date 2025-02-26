# Interactive simulation of plume and pyroclastic volcanic ejections

- [Introduction](#Introduction)
- [Instructions](#Instructions)


<a name="Introduction"></a>
## Introduction

This is the source code for the article "Interactive simulation of plume and pyroclastic volcanic ejections" presented at the I3D'22 conference and published in Proceedings of ACM. The author version of the paper is available here: https://hal.archives-ouvertes.fr/hal-03639288.
This project lets the user author a volcanic eruption simulation and visualize it in real time. A replay function is available, and the users can also export the simulation data for offline rendering purposes.
The proposed simulation model consists of two layers: a minimalistic dynamic model computing the trajectory of the plume, and procedural details added on the top of the first layer to get a nice visualization. Both can be visualized: the first one with cylinders and the second one with spheres.
The default parameters give a raising plume that spreads at high altitude. Wind can be added to give more curved plumes, and if the user modify certain initial conditions (eg. decrease the initial velocity or increase the initial density of the plume), the column will fall back and form pyroclastic flows.
The simulation is real time as long as not too many spheres are simulated; it won't be real time any more if there are too many subspheres or if the simulation lasts for too long, especially in the case of pyroclastic flows. The export of data is not real time either.

<a name="Instructions"></a>
## Instructions

The repository contains all the necessary assets to run the project without additional material, using the CMakeLists.txt file or the Makefile.
If the execution of the project does not work after building because of missing shaders, try copy/pasting the "scenes" folder in the build folder.

The core code can be found in "code_plume/scenes/sources/smoke".

Once the code is executed, click on "Start" to start the simulation, "Stop" to pause it and "Stop and Reset" to start a new simulation from scratch.
You can display the spheres and/or the cylinders checking the corresponding boxes.
Several parameters can be changed interactively:
- Initial plume speed
- Initial plume density,
- Vent radius and altitude
The wind parameters can also be changed, choosing the wind velocity and angle at several altitudes, which will be linearly interpolated. Two presets can be chosen and make wind control easier: "no wind" and "linear wind".
If "linear wind" is chosen, its maximum magnitude can then be changed. This maximum magnitude is the wind velocity above the maximum altitude; under this altitude, the wind velocity increases linearly from 0 at ground level to the maximum at the maximum altitude.

The user can also check "Replay" to replay the animation and "Stop Replay" to continue the simulation where it stopped.

If the user wants to export the simulation data, they have to check "Export data" BEFORE starting the simulation. The simulation will be very slow and the replay feature won't be available. The export data can be found in the "code_plume/output" folder.
Some other data logs will be found in the same folder, such as the altitude, density or speed at each frame.
