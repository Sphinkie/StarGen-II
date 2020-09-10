/* ------------------------------------------------------------------------- */
// File       : SG_FileWriter.h
// Project    : Star Gen
// Author C++ : David de Lorenzo
/* ------------------------------------------------------------------------- */
#ifndef _STARGEN_FILEWRITER_H_
#define _STARGEN_FILEWRITER_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include <fstream>
#include "SG_Star.h"
#include "SG_Planet.h"
#include "SG_File.h"

/* ------------------------------------------------------------------------- */
/// This is a wrapper class for writing the solar system data in a file.
/** 
This class allows to handle several file formats: xml, plain text, etc.
New formats can be added: 
- add a define in SG_FileWriter.h (ie: #define SG_TXT 0)
- Add an include in SG_FileWriter.cpp (ie: #include "SG_File_TXT.h")

Also take example on the SG_File_TXT class.
*/
/* ------------------------------------------------------------------------- */
#define SG_TXT 0
#define SG_XML 1

class SG_FileWriter
{
	public:

        SG_FileWriter(std::string  filename, long seed, int format);
		virtual ~SG_FileWriter();
		void writeStarDescription(SG_Star* star);
		void writePlanetDescription(SG_Planet* planet);

    private:
		int      mFormat;
		SG_File* mFile;
};


#endif




