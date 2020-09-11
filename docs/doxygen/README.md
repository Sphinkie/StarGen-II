# StarGen II

**StarGen** creates datasheets for realistic solar systems.
It is a C++ Solar-System Generator, based on a random seed, or on real astronomical data from the Celestia database.

Starting with a small amount of information about a star (mass, luminosity and age), **StarGen** extrapolates the solar system from its initial dust sphere to its accretion into planets, asteroids and atoms. Then it calculates the composition of the planets ground, oceans, atmosphere and climate.

It is used for developing space simulation games. 

### The Libray

Rewritten in C++, **StarGen-II** offers a set of classes that can be used to define the characteristics of solar systems, during the runtime of your application, or in advance using the *command line* program.

The program generate an XML file (or a plain TXT), describing a solar system.

### Output Format

**StarGen-II** generates *XML files* that can be easily parsed by your software, and *TXT files* that are more *human-friendly* and contain *comments* (especially *units*).

### Interface with Celestia Catalog

You can associate StarGen-II with the [Celestia](https://celestia.fr) catalog (files are included with Stargen-II). In that case, you can generate solar system datasheets based on real astronomical data.

## Contacts

**[StarGen-II website](https://sphinkie.github.io/StarGen-II)**

**[David de Lorenzo](https://sphinkie.github.io/)**

**[GitHub Repository](https://github.com/sphinkie/stargen-ii)**

