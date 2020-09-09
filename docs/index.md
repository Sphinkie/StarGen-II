# StarGen-II

*StarGen* creates datasheets for realistic solar systems. It is used for developing space simulation games.

## Original StarGen

The original *StarGen* code is developed by Jim Burrows in *Visual C* and can be found at [eldacur.com](https://eldacur.com/~brons/NerdCorner/StarGen/StarGen.html).

## StarGen 2

Rewritten in C++, *StarGen-II* offers a set of classes that can be used to define the characteristics of a solar system, during the runtime of your application, or before with the *command line* program.

The program generates an XML file (and a plain TXT file) describing a **random solar system** (*random seed*), and the **Sol** solar system (*seed 0*). So if the model is *not too bad*, this *0-datasheet* should roughly looks like our solar system.

When using *StarGen* in your program, you generate and then parse the xml file. You can keep the file, so when you need later the same solar system again, you can re-parse the same xml file.

## Output Format

*To be completed*


## Doxygen Documentation

The C++ classes are explained in the [Doxygen documentation, here.](doxygen/html/index.html)
