/* ------------------------------------------------------------------------- */
// File       : StarDatabase.cpp
// Project    : StarGen 2
// Author     : David de Lorenzo
// Author     : Chris Laurel (Celestia Project)
/* ------------------------------------------------------------------------- */

#include <iostream>
#include "StarDatabase.h"
#include "Bytes.h"
using namespace std;

/* ------------------------------------------------------------------------- */
/// Contructeur
/** 
@param StarArray est un pointeur sur un tableau de pointeurs.
@param sizeArray est la taille de ce tableau, à ne pas dépasser.
*/
/* ------------------------------------------------------------------------- */
StarDatabase::StarDatabase(std::string filename)
{
	mStarsInFile = 0;
    mFilename    = filename;
	
	mNormalStarCount = 0;
	mWhiteDwarfCount = 0;
	mNeutronStarCount= 0;
	mBlackHoleCount  = 0;
}

/* ------------------------------------------------------------------------- */
/// Ouvre en lecture un fichier de type CELESTIA, et vérifie son entête.
/* ------------------------------------------------------------------------- */
bool StarDatabase::openFile(std::string filename)
{
	starFile.open(filename.c_str(), ios::in | ios::binary);
	if (!starFile.good())
	{
        std::cout << "Error opening " << filename;
		return false;
	}

	// Verifie l'entete du fichier
	{
        int   headerLength = 8;
		char* header = new char[headerLength];
		starFile.read(header, headerLength);
        if (strncmp(header, "CELSTARS", headerLength)) return false;
		delete[] header;
	}

	// Verifie la version du format de fichier
	{
        uint16_t version;
		starFile.read((char*) &version, sizeof version);
		LE_TO_CPU_INT16(version, version);
		if (version != 0x0100) return false;
	}

	// Lit le nombre d'étoiles dans le fichier
	starFile.read((char*) &mStarsInFile, sizeof mStarsInFile);
	LE_TO_CPU_INT32(mStarsInFile, mStarsInFile);

	return true;
}

/* ------------------------------------------------------------------------- */
/// Close the DAT file.
/* ------------------------------------------------------------------------- */
void StarDatabase::closeFile()
{
    starFile.close();
}

/* ------------------------------------------------------------------------- */
/// Charge le contenu du fichier des étoiles stars.dat au format CELESTIA
/**
@param starNumber Le nombre d'étoiles à lire (0 for ALL).
@return Le nombre d'étoiles lues.
Note: La lecture des 112000 étoiles du fichier prend moins d'une seconde.
Des conversions sont nécesaires, car les valeurs sont stockées en binaire inverse.
*/
/* ------------------------------------------------------------------------- */
int StarDatabase::readStars(int starNumber)
{
    if (!this->openFile(mFilename))
    {
        cout << "not a valid .DAT file" << endl;
        this->closeFile();
        return 0;
    }

    if (!starFile.good()) return 0;

    int starIndex = 0;
    if (starNumber==0) starNumber = mStarsInFile;
    while ((starIndex<mStarsInFile) && // On ne lit pas au dela du fichier
           (starIndex<starNumber)   )  // On ne lit pas au dela de ce qui est demandé
	{
        uint32_t  catNo = 0;      // 32 bits
        float     x=0, y=0, z=0;  // 32 bits
        uint16_t  absMag;         // 16 bits
        uint16_t  spectralType;   // 16 bits

        starFile.read((char*) &catNo, sizeof catNo);        // Catalog Nb - 8 bits
		LE_TO_CPU_INT32(catNo, catNo);
        starFile.read((char*) &x, sizeof x);                // X position - 32 bits
		LE_TO_CPU_FLOAT(x, x);
        starFile.read((char*) &y, sizeof y);                // Y position - 32 bits
		LE_TO_CPU_FLOAT(y, y);
        starFile.read((char*) &z, sizeof z);                // Z position - 32 bits
		LE_TO_CPU_FLOAT(z, z);
        starFile.read((char*) &absMag, sizeof absMag);      // Absolute magnitude - 16 bits
		LE_TO_CPU_INT16(absMag, absMag);
        starFile.read((char*) &spectralType, sizeof spectralType); // Spectral Type - 16 bits
		LE_TO_CPU_INT16(spectralType, spectralType);

        double M  = absMag / 256.0;
        short  TY = (spectralType >> 12);
		short  SP = CatalogStar::Spectral_Unknown;
		short  SS = 0;
		short  ST = CatalogStar::Lum_Unknown;

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
			ST = CatalogStar::Lum_VII;       // Yerkes Type
 			break;
			}
		case 2:        // NeutronStar:
			{
			mNeutronStarCount++;
			SP = CatalogStar::Spectral_NS;      // spectre
			SS = 0;                             // sub-spectre
			ST = CatalogStar::Lum_Neutron;      // Type
			break;
			}
		case 3:        // BlackHole:
			{
			mBlackHoleCount++;
			SP = CatalogStar::Spectral_BH;      // spectre
			SS = 0;                             // sub-spectre
			ST = CatalogStar::Lum_BlackHole;    // Type
			break;
			}
		}
        // Add this Star in our std::vectorCatalog
        CatalogStar newStar = CatalogStar(catNo, M, SP, SS, ST);
        cout << catNo<< endl;
        this->mCatalog.push_back(newStar);
        starIndex++;
	}

    std::cout << "StarDatabase "       << mStarsInFile      << " stars in binary database" << endl;
    std::cout << "StarDatabase "       << mCatalog.size()   << " stars read" << endl;
    std::cout << "Normal Star Count  " << mNormalStarCount  << endl;
    std::cout << "White Dwarf Count  " << mWhiteDwarfCount  << endl;
    std::cout << "Neutron Star Count " << mNeutronStarCount << endl;
    std::cout << "Black-Hole Count   " << mBlackHoleCount   << endl;

    return (mCatalog.size());

    this->closeFile();

}

/* ------------------------------------------------------------------------- */
/// Returns a star from the catalog (0 is Sol)
/* ------------------------------------------------------------------------- */
CatalogStar* StarDatabase::getStar(unsigned int index)
{
    if (index<this->mCatalog.size())
        return &this->mCatalog[index];
    else
        return nullptr;
}


