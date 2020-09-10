/* ------------------------------------------------------------------------- */
// File       : StarDatabase.h
// Project    : StarGen 2
// Author     : David de Lorenzo
// Author     : Chris Laurel (Celestia Project)
/* ------------------------------------------------------------------------- */

#ifndef _STAR_DATABASE_H_
#define _STAR_DATABASE_H_

#if _MSC_VER > 1000
#pragma once
#endif 

#include <fstream>
#include <vector>
#include "CatalogStar.h"

/* ------------------------------------------------------------------------- */
/// Interface avec les fichiers Celestia.
/** 
-header     = CELSTARS
-version    = 0x0001
-starcount  = 0x87B70100.....0001B787.....=112519 = Nombre d'étoiles dans le fichier.
-catalog No = 0x01000000.....00000001.....= No de catalog de l'étoile
-position X = 0x654D6644.....float........= Position de l'étoile
-position Y = 0x0F7C8041.....float........  unit = AL
-position Z = 0xB754DFC0.....float........  origin = Sun
-Abs magn   = 0xD701.........0x01D7........=471 = 1.84 * 256
-Spect. Type= 0x5803.........0x0358........champ de bits
Exemple 0x0358 = 0000 0011 0101 1000
soit           = tttt ssss cccc llll
- tttt est le type global de l'étoile (normal, naine blanche, neutron, trou noir)
- ssss est le spectre de l'étoile (O, B, A, etc)
- cccc est la sous classe du spectre (0..9)
- llll est la luminosité de l'étoile (classification Yerkes)
*/
/* ------------------------------------------------------------------------- */
class StarDatabase  
{
public:
    StarDatabase(std::string filename);
    int          readStars(int starNumber = 0);
    CatalogStar* getStar(unsigned int index);

protected:

    bool     openFile(std::string filename);
    void     closeFile();

    std::string   mFilename;            ///< Celestia DAT filename (usually stars.dat)
    std::ifstream starFile;             ///< Celestia DAT file
    int      mStarsInFile;              ///< Nombre d'étoiles dans le fichier
    std::vector<CatalogStar> mCatalog;  ///< Catalog of the stars found in the .dat file


	int      mNormalStarCount;   ///< Compteur pour statistiques
	int      mWhiteDwarfCount;   ///< Compteur pour statistiques
	int      mNeutronStarCount;  ///< Compteur pour statistiques
	int      mBlackHoleCount;    ///< Compteur pour statistiques
};

#endif 
