# Change History

----

## 2.0

Translation of all the code in C++.  
Creation of classes: `SG_SolarSystem`, `SG_Planet`, `SG_Star`, `SG_Atmosphere`, `SG_Gas`, etc.  
Logfile is `stargen.log`  
Supported file formats are now: plain TXT and XML.  
HTML format can easily be added, but not present in this version.  
CELESTIA format is not present in this version.  
Version 2 does not contain a executable: It is a library. Next version will contain an executable.  
The String type used is not standard (sorry) so you may have problem to compile the library.  
The solution is that you edit `SG_File.cpp` and remove the <i>#include "ogre.h"</i> and define the `CString` according to your libraries.

----

## 1.0b5 → 1.0b6

Changed the "Deluge: ..." log message from -v to -v -v
Correctly calculate the Greenhouse rise.
Change the range of allowable masses from .2-1.3 → .2->1.5
Change the range of generated masses from .6-1.3 → .7->1.4

----

## 1.0b6 → 1.0b7

Add the -2 option to only output systems with multiple habitable planets
Update data from [solstation.com](http://www.solstation.com)

  - New numbers for:
      - Alpha Century A+B
      - 61 Cygni A
      - 70 Ophiuchi A
      - HR 7703
      - 82 Eridani
      - HR 753
      - Xi Bootis A
      - 107 Piscium
      - Chara
      - Xi Ursae Majoris Ba
      - Chi Orionis A
      - 41 Arae A
      - Gamma Leporis A
      - HR 1614) A
      - HR 7722
      - HR 4523
  - Add new systems:
      - HR 5568
      - HR 222
      - HR 4458
      - 12 Ophiuchi
      - HR 511
      - Zeta 1 Reticuli
      - Zeta 2 Reticuli
      - 47 Ursae Majoris
      - Iota Horologii
  - Move uninhabitable systems to the end:
      - Epsilon Indi
      - 36 Ophiuchi A+B
      - 70 Ophiuchi B (New)
      - MLO 4 (HR 6426) A (New)
      - BD-05 1844 A (New)
Changed the default increment (-i) from 9->11

----
