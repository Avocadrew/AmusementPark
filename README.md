# AmusementPark
#### Jun-Yu (Andrew) Chen
## Introduction
A tiny amusement park with switching scenes and interactive components created with OpenGL and GLSL in C++.

## Building and Running
Can be built directly with CMake with the given CMakeLists text file, preferred building/running platform would be Visual Studio 2019 on Windows 10.

## Scenes & Objectives
### Scene 1: An amusement park with a ferris wheel and a roller coaster (Geometric Manipulations)

<p align="center">
  <img src="https://user-images.githubusercontent.com/64970325/204154769-a98849e5-60bb-418d-b4ab-1b81e12c14de.PNG" />
</p>

- ***Hierarchical Animated Model***: 
  - **Ferris Wheel:** By drawing a C2 curve with control points placed as a vertical circle, we draw the carriages and extending arms while modifying orientation with time-dependent transformations. 
- ***Parametric Instancing:***
  - **Trees:** By giving a scale and color we could define trees in position.
  - **Ferris Wheel:** The number of carriages is defined by the value of the "Carriage" slider in interface.
  - **Fence:** The width of the fence is defined by the value of the "Fence" slider in interface.
- ***Subdivision:***
  - **Pillars for Roller Coaster:** Decides subdivision level with "Subdivision" Parameter.
  - **Sphere Core Roller Coaster:** Decides subdivision level with "Subdivision" Parameter.
- ***Level of Detail:***
  - **Subdivision Objects:** With "Detail" button pressed, subdivision would be determined by zoom-in ratio of world camera.
- ***Sweep Objects:***
  - **Trees:** Cone + Cylinder
  - **Frame of Ferris Wheel:** Sphere + Cylinder + Cube * 2
- ***Skybox:***
  - **Sky**: Generate Cubemap, project to near infinity, and calculate margins.



### Scene 2: Water Surface and On-screen Special Effects (Complex Shaders)
<p align="center">
  <img src="https://user-images.githubusercontent.com/64970325/204155000-ede96924-1a59-4371-9423-cc2da60a4772.PNG" />
</p>

- ***Cool Scene-based Shaders***
  - **Pixelation:** Mosaic Effect, representating the color of a group of pixels as one.
  - **Panic:** Determining color of pixel with time and positioning.
  - **Offset:** (1 - Original Color), to produce the inverse of original color.
  - **Brightness:** Modifying brightness of scene.
  - **Color Mask:** Masking a color onto the scene.
  - **NPR:** Non-Photorealistic Rendering presenting the scene with color-related tweaks.
  - **Grayscale:** Turning the scene into grayscale.
  - **Sobel:** Presenting the edges of objects with filtering techniques.
  - **Toon:** Presenting the scene with cartoonic colors.
- ***Projector Textures***
  - **Offset:** As a sliding window, able to use the Scene-based Shaders partially.
- ***Object Shaders***
  - **Wave:** Created a multiple sine wave shader.
  - **Phong/Gouraud Shader:** Manipulating color with lighting, color, and positioning.
- ***Environment Map***
  - **Environment Box:** By calculating current camera position and the point of skybox reflected by the water surface we would be able to create reflection and refraction on the water surface.
  
  
  
### Scene 3: Color Matching Minigame (Interactive Techniques)
<p align="center">
  <img src="https://user-images.githubusercontent.com/64970325/204154984-9f208ea8-62ee-4a33-b23a-66a1eaea8f9f.PNG" />
</p>

- ***Particle System***
  - **Firework:** Iteratively draw cubes and pass position and color to shader from main program.
- ***Interactivity***
  - **Gameplay:** Click on balls that match colors with the fireworks to score.
- ***Billboard***
  - **Spring Couplets:** Transformation in relation to camera angle and position to maintain orientation.


## Implementation
Done as coursework for CS3026 Introduction to Computer Graphics in NTUST, Fall 2021.

##### © 2022 Andrew (Jun-Yu) Chen
