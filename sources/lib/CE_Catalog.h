/* ------------------------------------------------------------------------- */
// File       : CE_Catalog.h
// Project    : Celestia interface for Stargen 2
// Author     : David de Lorenzo
// Author     : Chris Laurel (Celestia Project)
/* ------------------------------------------------------------------------- */

#ifndef _CATALOG_H_
#define _CATALOG_H_

#if _MSC_VER > 1000
#pragma once
#endif 

#include <fstream>
#include <map>
#include "CE_Star.h"

/* ------------------------------------------------------------------------- */
/// Interface with the Celestia database file.
/** 
- header     = CELSTARS
- version    = 0x0001        
- starcount  = 0x87B70100	=> 0001B787 : Number of stars in the file (112519)
- catalog No = 0x01000000	=> 00000001 : Catalog index of the star
- position X = 0x654D6644	=> float : Star Position X (unit = AL) origin = Sun
- position Y = 0x0F7C8041	=> float : Star Position Y
- position Z = 0xB754DFC0	=> float : Star Position Z
- Abs magn   = 0xD701	=> 0x01D7 : 256 x Absolute Magnitude (ex: 471 = 1.84 * 256)
- Spect. Type= 0x5803	=> 0x0358 : bit field

Spectrum example: 0x0358 = 0000 0011 0101 1000 = tttt ssss cccc llll
- tttt = global type of the star (normal, naine blanche, neutron, trou noir)
- ssss = star spectrum (O, B, A, etc)
- cccc = star sub-spectrum (0..9)
- llll = star luminosity (classification Yerkes)
*/
/* ------------------------------------------------------------------------- */
class CE_Catalog
{
public:
    CE_Catalog(std::string filename);
    long       readStars(long limit = 0);
    CE_Star*   getStar(unsigned int index);

protected:

    bool     openFile(std::string filename);
    void     closeFile();

    std::string   mFilename;             ///< Celestia DAT filename (usually stars.dat)
    std::ifstream mStarFile;             ///< Celestia DAT file
    int           mStarsInFile;          ///< Number of stars in the file
    std::map<const long, CE_Star*> mCatalog; ///< Catalog of the stars found in the .dat file


	int      mNormalStarCount;   ///< statistics counter
	int      mWhiteDwarfCount;   ///< statistics counter
	int      mNeutronStarCount;  ///< statistics counter
	int      mBlackHoleCount;    ///< statistics counter
};

#endif 
