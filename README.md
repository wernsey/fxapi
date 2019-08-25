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

[bitmap]: https://github.com/wernsey/bitmap
[tinyrenderer]: https://github.com/ssloy/tinyrenderer/wiki/Lesson-0:-getting-started
[scratchapixel]: https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/perspective-correct-interpolation-vertex-attributes
