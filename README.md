# FXAPI

A simple software 3D renderer in C.

It uses my [bitmap][] module as a framebuffer and for texture maps.

The renderer uses Barycentric coordinates for texture mapping as in
[tinyrenderer][], which is a bit slow. I was working on a scanline
rasterizer at some point, but I had some trouble with the perspective
correction, and didn't get around to finishing it.

## Links

 * Dmitry V. Sokolov's [tinyrenderer][] tutorial
 * The [www.scratchapixel.com][scratchapixel] tutorials
 * The official home of `bmp.c` is here: <https://github.com/wernsey/bitmap>
 * The `/gl-matrix` code is originally from <https://github.com/coreh/gl-matrix.c>,
   but I've made some modifications in my fork: <https://github.com/wernsey/gl-matrix.c>

Other references

 1. <https://www.davrous.com/2013/06/13/tutorial-series-learning-how-to-write-a-3d-soft-engine-from-scratch-in-c-typescript-or-javascript/>
 1. <http://www.3dcpptutorials.sk/index.php?id=60>
 1. <https://www.opengl.org/discussion_boards/showthread.php/156767-Clipping-triangles-in-clip-space>
 1. <https://github.com/bulletphysics/bullet3/blob/master/examples/TinyRenderer/TinyRenderer.cpp>
 1. <http://www.gamasutra.com/view/news/168577/Indepth_Software_rasterizer_and_triangle_clipping.php>
 1. <http://myweb.lmu.edu/dondi/share/cg/hsr.pdf>
 1. [Object files](https://en.wikipedia.org/wiki/Wavefront_.obj_file)
    * <http://www.martinreddy.net/gfx/3d/OBJ.spec>
    * <http://paulbourke.net/dataformats/obj/>
 1. [MD2 files](http://en.wikipedia.org/wiki/MD2_(file_format))
    * <http://tfc.duke.free.fr/coding/md2-specs-en.html>
    * <http://tfc.duke.free.fr/old/models/md2.htm>
    * <http://openglsamples.sourceforge.net/md2.html>
 1. MD5 files
    * <http://tfc.duke.free.fr/coding/md5-specs-en.html>
    * <https://www.3dgep.com/loading-and-animating-md5-models-with-opengl/>
    * <https://www.braynzarsoft.net/viewtutorial/q16390-27-loading-an-md5-model>
    * <https://lwjglgamedev.gitbooks.io/3d-game-development-with-lwjgl/content/chapter19/chapter19.html>

In the future I might consider adding support for these file formats:

* [STL](https://en.wikipedia.org/wiki/STL_(file_format))
* [PLY](http://paulbourke.net/dataformats/ply/)
* [OFF](https://en.wikipedia.org/wiki/OFF_(file_format))


This particular branch now supports model-space normal maps; I actually want to support tangent space
normal maps, but it might take a while before I can get there. Here are some links I'll save for later

* https://learnopengl.com/Advanced-Lighting/Normal-Mapping
* http://ogldev.atspace.co.uk/www/tutorial26/tutorial26.html
* [Messing with Tangent Space](https://www.gamasutra.com/view/feature/129939/messing_with_tangent_space.php?print=1) gamasutra article
* https://www.reddit.com/r/opengl/comments/3o3wn6/tangent_space_is_confusing/cvujbr7/
* http://www.3dkingdoms.com/weekly/weekly.php?a=37

[bitmap]: https://github.com/wernsey/bitmap
[tinyrenderer]: https://github.com/ssloy/tinyrenderer/wiki/Lesson-0-getting-started
[scratchapixel]: https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/perspective-correct-interpolation-vertex-attributes
