# Cover Generator 

The cover generator is a plugin for Unreal Engine 4. 

![Example Covers](https://cloud.githubusercontent.com/assets/6062062/23833642/d3113832-0748-11e7-96b2-104c767ba6c4.gif)

It analyzes the geometry of a level to find possible cover points. Cover points can then be parsed via the native **Environment Query System (EQS)** to allow AI controlled characters to find spots to hide during a fight or to prepare an ambush.

The system offers different set of information per generated cover such as if it is a crouched or standing cover, if the character can stand or lean on the sides to shoot.

The plugin is a proof of concept only. It is stable but not production ready, a lot could be optimized and a good cleaning is needed.

# Example video of AI using the cover generator
<a href="http://www.youtube.com/watch?feature=player_embedded&v=igwEAeQFwhM
" target="_blank"><img src="http://img.youtube.com/vi/igwEAeQFwhM/0.jpg" 
alt="Dynamic tactical cover generation coupled with a GOAP AI " width="240" height="180" border="10" /></a>

# How to use the cover generator? 
- Add the plugin to your project

- The cover generator is using the native nav mesh bounds volume of a level to generate the covers, so if you do not already have one in the scene, add a nav mesh bounds volume to your level.

![Add a nav mesh bound volume](https://cloud.githubusercontent.com/assets/6062062/23832918/7c1c2ab4-073e-11e7-9bbe-1860f58f1a65.jpg)

- You can see the generated navigation mesh by pressing 'P' on your keyboard.

![Visualize the navigation mesh](https://cloud.githubusercontent.com/assets/6062062/23832943/cd750854-073e-11e7-8ef6-464449bf18a2.jpg)

- Create a CoverGenerator blueprint and drop it in your level

![Create a CoverGenerator blueprint](https://cloud.githubusercontent.com/assets/6062062/23832877/d51f40e8-073d-11e7-9ba2-04e13065fd29.jpg)

- By default, the cover generator will generate the cover points on begin play. To visualize the generated points, click on the cover generator actor in your scene and tick the option "Debug Draw All Points"

![Display generated cover points](https://cloud.githubusercontent.com/assets/6062062/23832977/5e7e1d54-073f-11e7-848d-9fad46882bfa.jpg
)

- Press the "simulate" button. You can see the result on the image below. Blue spheres represent cover points, the lowest ones for crouched covers, the highest ones for standing covers. Arrows are representing from where the character can see or shoot.


![Simulating level](https://cloud.githubusercontent.com/assets/6062062/23833001/c020b030-073f-11e7-832a-a8a5a8696790.jpg
)

- To be able to use the generated covers in your AI logic, first enable the Environment Query System: [UE4 quick start guide](https://docs.unrealengine.com/latest/INT/Engine/AI/EnvironmentQuerySystem/QuickStart/2/)

- Create a new EQS query

![Create a new EQS Query](https://cloud.githubusercontent.com/assets/6062062/23833001/c020b030-073f-11e7-832a-a8a5a8696790.jpg
)

- Select "Cover FMemory" as a source of points

![Cover FMemory](https://cloud.githubusercontent.com/assets/6062062/23833195/8ca307a0-0742-11e7-9fe7-433e4e9016f9.jpg
)

- You can use an EQS Testing pawn to visualize the result

![EQS testing pawn](https://cloud.githubusercontent.com/assets/6062062/23833170/3fce8e72-0742-11e7-8caf-d213aebed291.gif
)



# How to get cover points from a Blueprint?

![Blueprint Example](https://user-images.githubusercontent.com/6062062/51174720-c2a2a980-18b8-11e9-9aad-05d2d7054b61.JPG)

