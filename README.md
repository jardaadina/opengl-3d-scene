# Interactive 3D Scene (OpenGL)

This project is an interactive 3D scene created using OpenGL. It features a relaxing natural landscape, realistic lighting, and various special effects.

## Overview

I created a 3D environment designed for relaxation, which includes:
* **A small cabin** detailed with a sofa, TV, glass table, and ambient lamps.
* **A pine forest**, lanterns, and flowers to create a natural setting.
* **A lake** populated with ducks and other detailed elements.
* **An animated dog** that walks around the cabin.
* **Night Mode** with specific lighting conditions.

The scene can be explored freely, and the user can toggle various graphics settings to experience different rendering styles.

## Features

* **Advanced Lighting:** Directional lighting (Sun) and point lighting (lanterns, indoor lamps).
* **Special Effects:** Dynamic fog for a realistic atmosphere.
* **Animations:** A moving dog and an automated camera path for a guided tour.
* **Rendering Modes:** Support for Solid, Wireframe, Polygonal, and Smooth shading.
* **Day/Night Cycle:** Simulation of day and night transitions.

## Controls

| Key | Action |
| :--- | :--- |
| **W / S** | Move Forward / Backward |
| **A / D** | Move Left / Right |
| **U / I** | Move Up / Down |
| **Q / E** | Rotate Camera |
| **N** | Toggle Night Mode |
| **F / G** | Increase / Decrease Fog density |
| **1** | Toggle Point Lights |
| **P / O / K / L** | Switch Rendering Mode (Wireframe, Polygonal, Solid, Smooth) |
| **B** | Start Scene Presentation Animation |

## Technical Implementation

* **OpenGL + GLFW:** Used for rendering and handling user interactions.
* **Blender:** Used for modeling the 3D objects.
* **GLM:** Used for mathematical calculations and transformations.
* **Shaders:** Custom shaders implemented for advanced graphic effects.

I utilized directional and point lights to achieve realism and a fog effect to add depth to the scene. The animations are controlled through specific transformation functions.

## What I Learned
* Working with Blender was challenging but highly rewarding.
* OpenGL programming is complex but extremely powerful.
* Manual debugging was difficult, but it significantly improved my problem-solving skills.

## Future Improvements
* Add rain and wind effects for a dynamic atmosphere.
* Implement more realistic shadows for objects.
* Add more animations for increased interaction.
* Introduce new objects to make the scene more complex.
