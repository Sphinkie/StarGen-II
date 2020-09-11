/* ------------------------------------------------------------------------- */
// File       : CE_Catalog.cpp
// Project    : Celestia interface for Stargen 2
// Author     : David de Lorenzo
// Author     : Chris Laurel (Celestia Project)
/* ------------------------------------------------------------------------- */

#include <iostream>
#include "CE_Catalog.h"
#include "CE_Bytes.h"
using namespace std;

/* ------------------------------------------------------------------------- */
/// Contructor
/* ------------------------------------------------------------------------- */
CE_Catalog::CE_Catalog(std::string filename)
{
	mStarsInFile = 0;
    mFilename    = filename;
	// Statistics
	mNormalStarCount = 0;
	mWhiteDwarfCount = 0;
	mNeutronStarCount= 0;
	mBlackHoleCount  = 0;
}

/* ------------------------------------------------------------------------- */
/// Open the Celestia database file, and check the Header
/* ------------------------------------------------------------------------- */
bool CE_Catalog::openFile(std::string filename)
{
    mStarFile.open(filename.c_str(), ios::in | ios::binary);
    if (!mStarFile.good())
	{
        std::cout << "Error opening " << filename << endl;
		return false;
	}

	// Check the file Header
	{
        int   headerLength = 8;
		char* header = new char[headerLength];
        mStarFile.read(header, headerLength);
        if (strncmp(header, "CELSTARS", headerLength)) return false;
		delete[] header;
	}

    // File version: 16 bits: 01 00 = 0x0001
	{
        uint16_t version;
        mStarFile.read((char*) &version, sizeof(version));
		LE_TO_CPU_INT16(version, version);
		if (version != 0x0100) return false;
	}

    // Lit le nombre d'étoiles dans le fichier - 32 bits : 87 B7 01 00 = 0x0001B787 = 112.519
    mStarFile.read((char*) &mStarsInFile, sizeof(mStarsInFile));
	LE_TO_CPU_INT32(mStarsInFile, mStarsInFile);

	return true;
}

/* ------------------------------------------------------------------------- */
/// Close the database file.
/* ------------------------------------------------------------------------- */
void CE_Catalog::closeFile()
{
    mStarFile.close();
}

/* ------------------------------------------------------------------------- */
/// Read the content of the Celestia database file (stars.dat)
/**
@param limit Le nombre d'étoiles à lire (0 or empty for ALL).
@return Le nombre d'étoiles lues.
_Note: La lecture des 112000 étoiles du fichier prend moins d'une seconde._  
Des conversions sont nécessaires, car les valeurs sont stockées en binaire inverse.
*/
/* ------------------------------------------------------------------------- */
long CE_Catalog::readStars(long limit)
{
    if (!this->openFile(mFilename))
    {
        cout << "not a valid .DAT file" << endl;
        this->closeFile();
        return 0;
    }

    if (mStarFile.bad()) return 0;
    long starIndex = 1;
    if (limit==0) limit = mStarsInFile;

    while ((starIndex<mStarsInFile) && // On ne lit pas au dela du fichier
           (starIndex<=limit)       )  // On ne lit pas au dela de ce qui est demandé
	{
        uint32_t  catNo = 0;      // 32 bits
        float     x=0, y=0, z=0;  // 32 bits
        uint16_t  absMag;         // 16 bits
        uint16_t  spectralType;   // 16 bits

		// Read data from the file, and convert to LittleEndian
        mStarFile.read((char*) &catNo, sizeof catNo);        // Catalog Nb - 8 bits
		LE_TO_CPU_INT32(catNo, catNo);
        mStarFile.read((char*) &x, sizeof x);                // X position - 32 bits
		LE_TO_CPU_FLOAT(x, x);
        mStarFile.read((char*) &y, sizeof y);                // Y position - 32 bits
		LE_TO_CPU_FLOAT(y, y);
        mStarFile.read((char*) &z, sizeof z);                // Z position - 32 bits
		LE_TO_CPU_FLOAT(z, z);
        mStarFile.read((char*) &absMag, sizeof absMag);      // Absolute magnitude - 16 bits
		LE_TO_CPU_INT16(absMag, absMag);
        mStarFile.read((char*) &spectralType, sizeof spectralType); // Spectral Type - 16 bits
		LE_TO_CPU_INT16(spectralType, spectralType);

        double AM  = absMag / 256.0;
        short  TY = (spectralType >> 12);
		short  SP = CE_Star::Spectral_Unknown;
		short  SS = 0;
		short  ST = CE_Star::Lum_Unknown;

		switch (TY)
		{
		case 0:      // NormalStar  
			{
			mNormalStarCount++;
            SP = (spectralType>>8) & 0x000F;  // spectrum
            SS = (spectralType>>4) & 0x000F;  // sub-spectrum
			ST = (spectralType)    & 0x000F;  // Yerkes Type
			break;
			}
		case 1:        // WhiteDwarf
			{
			mWhiteDwarfCount++;
            SP = (spectralType>>8) & 0x000F; // spectrum
			// On transforme un octet 0..7 en 16..23 (spectre des naines blanches)
			SP += + 16;
			if (SP > 23) SP=23;
            SS = (spectralType>>4) & 0x000F; // sub-spectrum
			ST = CE_Star::Lum_VII;           // Yerkes Type
 			break;
			}
		case 2:        // NeutronStar:
			{
			mNeutronStarCount++;
			SP = CE_Star::Spectral_NS;      // spectre
			SS = 0;                         // sub-spectre
			ST = CE_Star::Lum_Neutron;      // Type
			break;
			}
		case 3:        // BlackHole:
			{
			mBlackHoleCount++;
			SP = CE_Star::Spectral_BH;      // spectre
			SS = 0;                         // sub-spectre
			ST = CE_Star::Lum_BlackHole;    // Type
			break;
			}
		}
        // Add this Star in our std::vectorCatalog
        CE_Star* newStar = new CE_Star(catNo, AM, SP, SS, ST, x, y, z);
        this->mCatalog[catNo] = newStar;
        starIndex++;
	}

    std::cout << "Catalog: " << mStarsInFile      << " stars in database" << endl;
    std::cout << "Catalog: " << mCatalog.size()   << " stars read" << endl;
    std::cout << "Normal Star Count  " << mNormalStarCount  << endl;
    std::cout << "White Dwarf Count  " << mWhiteDwarfCount  << endl;
    std::cout << "Neutron Star Count " << mNeutronStarCount << endl;
    std::cout << "Black-Hole Count   " << mBlackHoleCount   << endl;

    return (int)mCatalog.size();

    this->closeFile();

}

/* ------------------------------------------------------------------------- */
/// Returns a star from the catalog
/* ------------------------------------------------------------------------- */
CE_Star* CE_Catalog::getStar(unsigned int index)
{
    if (index<this->mCatalog.size())
        return this->mCatalog[index];
    else
        return nullptr;
}


