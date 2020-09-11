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

#include <fstream>
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

        SG_File(std::string  filename, long seed);
		virtual ~SG_File();
		void writeStarDescription(SG_Star* star);
		void writePlanetDescription(SG_Planet* planet);

    protected:
        std::ofstream    mOutputFile;  ///< The output file

		// This functions must be implemented in the derived classes
        virtual void addValue     (std::string  name, bool value) =0;  ///< This function must be implemented in the derived classes
        virtual void addValue     (std::string  name, char* value,        std::string  comment="") =0; ///< This function must be implemented in the derived classes
        virtual void addValue     (std::string  name, std::string  value, std::string  comment="") =0; ///< This function must be implemented in the derived classes
        virtual void addValue     (std::string  name, long double value,  std::string  comment="") =0; ///< This function must be implemented in the derived classes
        virtual void addIntValue  (std::string  name, long double value,  std::string  comment="") =0; ///< This function must be implemented in the derived classes
        virtual void addPercentage(std::string  name, long double value) =0;   ///< This function must be implemented in the derived classes
        virtual void addSection   (std::string  name) =0;      ///< This function must be implemented in the derived classes
        virtual void closeSection (std::string  name="") =0;   ///< This function must be implemented in the derived classes
		virtual void subSection   () =0;   ///< This function must be implemented in the derived classes
		virtual void addLine      () =0;   ///< This function must be implemented in the derived classes

    private:
        void writeStar(SG_Star* star);
        void writePlanet(SG_Planet* planet);
        void writeSolidPlanet(SG_Planet* planet);
        void writeGasPlanet(SG_Planet* planet);
        void writeClimate(SG_Planet* planet);
        void writeAtmosphere(SG_Planet* planet);
        std::string mPlanet_Phrase[11];
        long        mSeed;

};


#endif




