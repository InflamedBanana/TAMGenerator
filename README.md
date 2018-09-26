# TAMGenerator
Little console application used to generate .dds packed tonal art maps, including all mipmaps. It uses the CImage C++ Library 
and directX dds.h header to create the dds files. The files are not compressed, but it can be done in an instant using the 
directX plugin for photoshop.

This generator produces only dot maps, but it could be enhanced to support lines.
The generation uses a simple uniform_random, while it serves the purpose, it can be particularly long at times on large maps (> 1024)

Different variables includes, number & size of the maps, lowest & darkest tones, dot size & size variation.

### Tonal Art Map

Packed maps :

[![Dot TonalArtMap](/imgs/tam_packed.png)]

Result unpacked ( without mip maps ) :

[![Dot TonalArtMap](/imgs/tam.png)]


