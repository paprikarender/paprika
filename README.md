What is Paprika Render?
=======================
Paprika Render is an open source physically based renderer based on partly PBRT and partly (good old) NVIDIA Gelato. It uses Open Shading Language (OSL) for advanced shading and Embree for high-performance rendering.

Materials and Open Shading Language
===================================
Paprika Render supports many common scattering models like Lambertian, Oren-Nayar, dielectrics, ideal reflection/refraction and anisotropic Ward BRDFs.

Open Shading Language (OSL) is a robust language for programmable shading in advanced renderers. The scattering models supported by Paprika Render can be used and mixed through OSL.

Workflow
========
Paprika Render has a simple C++ scene description API close to Gelato and a Lua binding. Paprika Render is independent of any scene format. By using Generator plugin interface, it's possible to introduce new scene format plug-ins like XML, Collada or any other binary mesh format.

Building
========
Paprika Render depends on OSL and Embree libraries. Although it's not easy to build these, it's possible to get Embree binaries from here and OSL binaries from Blender's SVN repo. After getting these dependencies, you can build Paprika Render by using Visual Studio 2013.
