ift-demo
========

Demo source code of the Image Foresting Transform with a few examples of image operators

This directory contains the C source code for several image processing
operators based on the Image Foresting Transform.

@article{Falcao04a,
author = "A.X. Falc{\~{a}}o and J. Stolfi and R.A. Lotufo",
title  = "The Image Foresting Transform: {T}heory, Algorithms, and Applications",
journal = "IEEE Trans. on Pattern Analysis and Machine Intelligence",
volume = 26,
number = 1,
pages = "19--29",
year = 2004,
doi = "10.1109/TPAMI.2004.10012"
}

The implementation of the algorithms are following the lectures of the
SP-ASC 2010 in www.ic.unicamp.br/~afalcao/talks

Type make to compile the ift-demo library:

cd ift-demo
make

In demo you will find several subdirectories. Type make followed by
the basename of the program to compile it. Example:

cd demo/watershed
make watershed

By typing the name of the program, you will see its usage. Example:

watershed

In this case, the correct usage is

watershed flower.pgm flower_grad.pgm flower-seeds.txt

All directories come with image files and other informations needed to
execute the programs. 

Enjoy and do not hesitate to ask us for help.

Alexandre Falcão and Thiago Spina.




