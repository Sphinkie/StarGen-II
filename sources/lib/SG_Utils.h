/* ------------------------------------------------------------------------- */
// File       : SG_Utils.h
// Project    : Star Gen
// Author C++ : David de Lorenzo
// Author C   : Jim Burrows
/* ------------------------------------------------------------------------- */

#ifndef _STARGEN_UTILS_H_
#define _STARGEN_UTILS_H_

#include <string>

#if _MSC_VER > 1000
#pragma once
#endif

/* ------------------------------------------------------------------------- */
/// Several utilities functions for StarGen calculation (random numbers, etc).
/* ------------------------------------------------------------------------- */
class SG_Utils
{
	public:
		static long double random_number(long double, long double);
		static long double about(long double, long double);
		static long double random_eccentricity(void);
		static long double min(long double X, long double Y);
		static long double max(long double X, long double Y);
        static void writeLog(std::string data, bool append = true);
		static long generateSeed();

};


#endif
