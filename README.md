C Game Engine
=============

What started out as a mere [PhysX](https://en.wikipedia.org/wiki/PhysX) engine turned into a tool for creating 2D & 3D games. The Game Engine was created using C++ along with OpenGL and has been one of my biggest projects!

<img src="https://raw.githubusercontent.com/Mannilie/c_gameengine/master/img/img1.jpg" alt="" width="80%" height="80%"/>

It uses a [Component Object Model (COM)](https://en.wikipedia.org/wiki/Component_Object_Model) binary-interface standard, similar to [Unity](https://en.wikipedia.org/wiki/Unity_(game_engine))'s and [Unreal](https://en.wikipedia.org/wiki/Unreal_Engine)'s in the sense that each Game Object in the scene contains a list of components. These components define many characteristics of that specific Game Object. By default, every Game Object created has a Transform component already attached which defines the object's Position, Rotation and Scale in the world.

<img src="https://raw.githubusercontent.com/Mannilie/c_gameengine/master/img/img2.jpg" alt="" width="80%" height="80%"/>

In order for a Game Object to actually appear within the scene, it needs to be rendered. The Game Engine is equipped with a subsystem known as the Rendering Engine. In order for this subsystem to work, it needs to have a Game Object with a MeshRenderer / SpriteRenderer component attached to it. This component must also have an appropriate Material with a Texture linked to it and the Game Object will render to the screen.

<img src="https://raw.githubusercontent.com/Mannilie/c_gameengine/master/img/img3.jpg" alt="" width="80%" height="80%"/>

The Rendering Engine doesn't just draw textured meshes / sprites to the screen. It is also in charge of handling complex post-processing effects using [Shaders](https://en.wikipedia.org/wiki/Shader). GLSL is the shader language this engine uses and one of the most significant post-processing effect implemented in the engine is [Fast Approximate Anti-aliasing (FXAA)](https://en.wikipedia.org/wiki/Fast_approximate_anti-aliasing) for smoothing jaggered edges on Game Objects.

<img src="https://raw.githubusercontent.com/Mannilie/c_gameengine/master/img/img4.jpg" alt="" width="80%" height="80%"/>

This subsystem is also handling various other rendering techniques. One of which is the Lighting System which handles the rendering of various types of lights in the 3D / 2D environment. The first is Directional Lights. Much like the sun, the directional light acts as a light that comes from one direction. Then there are Point Lights which are lights that radiate in all directions and has a certain position in space. Finally, there are Spot Lights that acts as a strong beam of light which illuminates only a small area where this beam is pointing towards.

<img src="https://raw.githubusercontent.com/Mannilie/c_gameengine/master/img/img5.jpg" alt="" width="80%" height="80%"/>
