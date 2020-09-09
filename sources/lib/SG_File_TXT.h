/* ------------------------------------------------------------------------- */
// File       : SG_File_TXT.h
// Project    : Star Gen
// Author C++ : David de Lorenzo
/* ------------------------------------------------------------------------- */
#ifndef _STARGEN_FILE_TXT_H_
#define _STARGEN_FILE_TXT_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "SG_File.h"

/* ------------------------------------------------------------------------- */
/// This is a file writer for the solar system description. 
/** The output format is an explicit plain text file. */
/* ------------------------------------------------------------------------- */
class SG_File_TXT : public SG_File
{
	public:

		SG_File_TXT(SG_String filename, long seed);
		~SG_File_TXT();

	protected:

		void addValue     (SG_String name, bool value);
		void addValue     (SG_String name, char* value,       SG_String comment);
		void addValue     (SG_String name, SG_String value,     SG_String comment);
		void addValue     (SG_String name, long double value, SG_String comment);
		void addIntValue  (SG_String name, long double value, SG_String comment);
		void addPercentage(SG_String name, long double value);
		void addSection   (SG_String name);
		void closeSection (SG_String name);
		void subSection   ();
		void addLine      ();
};


#endif



