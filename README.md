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
 * The `/gl-matrix` code is originally from https://github.com/coreh/gl-matrix.c,
   but I've made some modifications in my fork: https://github.com/wernsey/gl-matrix.c

[bitmap]: https://github.com/wernsey/bitmap
[tinyrenderer]: https://github.com/ssloy/tinyrenderer/wiki/Lesson-0:-getting-started
[scratchapixel]: https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/perspective-correct-interpolation-vertex-attributes