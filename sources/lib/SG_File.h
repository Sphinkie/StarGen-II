/* ------------------------------------------------------------------------- */
// File       : SG_File.h
// Project    : Star Gen
// Author C++ : David de Lorenzo
/* ------------------------------------------------------------------------- */
#ifndef _STARGEN_FILE_H_
#define _STARGEN_FILE_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include <fstream.h>
#include "SG_Star.h"
#include "SG_Planet.h"


/* ------------------------------------------------------------------------- */
/// This is a generic class for creating a file with the solar system description.
/** This class must be instancied into derived classes. For instance derived class 
SG_File_TXT to produce a Text file, SG_File_XML to produce an XML file, and based 
on these examples, others derived classes can be created, and used in SG_SolarSystem.
*/
/* ------------------------------------------------------------------------- */
class SG_File
{
	public:

		SG_File(SG_String filename, long seed);
		virtual ~SG_File();
		void writeStarDescription(SG_Star* star);
		void writePlanetDescription(SG_Planet* planet);

	protected:
		
		void writeStar(SG_Star* star);
		void writePlanet(SG_Planet* planet);
		void writeSolidPlanet(SG_Planet* planet);
		void writeGasPlanet(SG_Planet* planet);
		void writeClimate(SG_Planet* planet);
		void writeAtmosphere(SG_Planet* planet);
		ofstream    mOutputFile;
		char*       mPlanet_Phrase[11];
		long        mSeed;

		// This functions must be implemented in the derived classes
		virtual void addValue     (SG_String name, bool value) =0;
		virtual void addValue     (SG_String name, char* value,       SG_String comment="") =0;
		virtual void addValue     (SG_String name, SG_String value,     SG_String comment="") =0;
		virtual void addValue     (SG_String name, long double value, SG_String comment="") =0;
		virtual void addIntValue  (SG_String name, long double value, SG_String comment="") =0;
		virtual void addPercentage(SG_String name, long double value) =0;
		virtual void addSection   (SG_String name) =0;
		virtual void closeSection (SG_String name="") =0;
		virtual void subSection   () =0;
		virtual void addLine      () =0;

};


#endif




