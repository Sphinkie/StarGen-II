/* ------------------------------------------------------------------------- */
// File       : SG_Utils.h
// Project    : Star Gen
// Author C++ : David de Lorenzo
// Author C   : Jim Burrows (see Credits.txt)
/* ------------------------------------------------------------------------- */
#ifndef _STARGEN_UTILS_H_
#define _STARGEN_UTILS_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "ogre.h"		// Pour les Strings
#define SG_String     Ogre::String


#define ITOA(X)   Ogre::StringConverter::toString(X)
#define ITOS(X)   Ogre::StringConverter::toString((Ogre::Real)(X))

/* ------------------------------------------------------------------------- */
/// Several utilities functions for StarGen calculations (random numbers, etc).
/* ------------------------------------------------------------------------- */
class SG_Utils
{
	public:
		static long double random_number(long double, long double);
		static long double about(long double, long double);
		static long double random_eccentricity(void);
		static long double min(long double X, long double Y);
		static long double max(long double X, long double Y);
		static void writeLog(SG_String data, bool append = true);
		static long generateSeed();

};


#endif
