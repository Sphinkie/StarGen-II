/* ------------------------------------------------------------------------- */
// File       : SG_FileWriter.cpp
// Project    : Star Gen
// Author C++ : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "SG_FileWriter.h"
#include "SG_Const.h"
#include "SG_File_TXT.h"
#include "SG_File_XML.h"

/*----------------------------------------------------------------------*/
/// Constructor
/*----------------------------------------------------------------------*/
SG_FileWriter::SG_FileWriter(std::string filename, long seed, int format)
{
	mFormat = format;

	switch(mFormat) 
	{
	case SG_TXT:
		mFile = new SG_File_TXT(filename,seed);
		break;
	case SG_XML:
		mFile = new SG_File_XML(filename,seed);
		break;
	}
}

/*----------------------------------------------------------------------*/
/// Destructor. 
/*----------------------------------------------------------------------*/
SG_FileWriter::~SG_FileWriter()
{
	delete (mFile);
}

/*----------------------------------------------------------------------*/
/// Write all the data concerning the Star, in the file.
/*----------------------------------------------------------------------*/
void SG_FileWriter::writeStarDescription(SG_Star* star)
{
	mFile->writeStarDescription(star);
}


/*----------------------------------------------------------------------*/
/// Write all the data concerning the Planet, in the file.
/*----------------------------------------------------------------------*/
void SG_FileWriter::writePlanetDescription(SG_Planet* planet)
{
	mFile->writePlanetDescription(planet);
}

