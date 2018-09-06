C Game Engine
=============

<img src="https://lh5.googleusercontent.com/9NEKGaEAUFrSYh-b8khLpgl3zV8FpcN9gW7ucask4Dw6wwa-NEWsYC3n9BjOKNQ2HsoR6q7Qc3NM3Ml8CaXh=w1920-h894-rw" alt="" width="500" height="500"/>

What started out as a mere PhysX engine turned into a tool for creating 2D & 3D games. The Game Engine was created using C++ along with OpenGL and has been one of my biggest projects!

<img src="https://lh6.googleusercontent.com/Wvpy03_1TR8CK2MGhsAF6AsMi4GwpfZYyChH0ElSqnMREVlTR-UfQptQ2cHtOiz1hh9Y3mqMYwmeJpLZ2stO=w1920-h894-rw" alt="" width="500" height="500"/>

It uses a Component Object Model (COM) binary-interface standard, similar to Unity's and Unreal's in the sense that each Game Object in the scene contains a list of components. These components define many characteristics of that specific Game Object. By default, every Game Object created has a Transform component already attached which defines the object's Position, Rotation and Scale in the world.

<img src="https://lh4.googleusercontent.com/5TtqEGB9gv9k8paE4tXfVOewanfy5H0mFhsZ6UChmDqlWqHtjCEdGk72hsCLacET7aJ8iYw6F1s5DVX9tOOP=w1920-h894-rw" alt="" width="500" height="500"/>

In order for a Game Object to actually appear within the scene, it needs to be rendered. The Game Engine is equipped with a subsystem known as the Rendering Engine. In order for this subsystem to work, it needs to have a Game Object with a MeshRenderer / SpriteRenderer component attached to it. This component must also have an appropriate Material with a Texture linked to it and the Game Object will render to the screen.

<img src="https://lh5.googleusercontent.com/V8KpvHGCOj1W_FDndf_s7NaAgsBROa5Ib3yjEHSMEDOHjPkbOBM-7smKRJUoQgbTuyfcQzCsfHAwypmTun-w=w1920-h894-rw" alt="" width="500" height="500"/>

The Rendering Engine doesn't just draw textured meshes / sprites to the screen. It is also in charge of handling complex post-processing effects using Shaders. GLSL is the shader language this engine uses and one of the most significant post-processing effect implemented in the engine is Fast Approximate Anti-aliasing (FXAA) for smoothing jaggered edges on Game Objects.

<img src="https://lh5.googleusercontent.com/ijiQHVBH9v9RvA4l1O9Kv-LOakXTLVP60QmbCywhXnG3rgxFGPyMun44ZTEbrfgAGN10g9sG6H1VRr6LSRo_=w1920-h894-rw" alt="" width="500" height="500"/>

This subsystem is also handling various other rendering techniques. One of which is the Lighting System which handles the rendering of various types of lights in the 3D / 2D environment. The first is Directional Lights. Much like the sun, the directional light acts as a light that comes from one direction. Then there are Point Lights which are lights that radiate in all directions and has a certain position in space. Finally, there are Spot Lights that acts as a strong beam of light which illuminates only a small area where this beam is pointing towards.



