/* ------------------------------------------------------------------------- */
// File       : SG_Atmosphere.h
// Project    : Star Gen 2
// Author C++ : David de Lorenzo
// Author C   : Jim Burrows (see Credits.txt)
/* ------------------------------------------------------------------------- */

#ifndef _SG_ATMOSPHERE_H_
#define _SG_ATMOSPHERE_H_

#include "SG_Gas.h"

#if _MSC_VER > 1000
#pragma once
#endif

#define	NONE          0
#define	BREATHABLE    1
#define	UNBREATHABLE  2
#define	STINKING      3
#define	POISONOUS     4

#define MAX_GAZ      30		 ///< Max number of Gases (can be changed)

/* ------------------------------------------------------------------------- */
/// A planet's Atmosphere. 
/* ------------------------------------------------------------------------- */
class SG_Atmosphere
{
	public:
		SG_Atmosphere(void* myPlanet);
		~SG_Atmosphere();
		int         calculateAtmosphere();
		int         getBreathability();
        std::string getToxicGasList();
        std::string getGasName(int rank, int stateMask = ALL);
        std::string getGasSymbol(int rank, int stateMask = ALL);
		SG_Gas*     getGas(int rank, int stateMask = ALL);
		long double getGasRatio(int rank);
		int         getGasState(int rank);

    private:

		int   estimateComposition();
		long  calculateGasRepartition();
		void  sortGasTable();
		void  swapGas(int index1, int index2);
		long double getGasAmount(SG_Gas* gaz);
		long double getGasPressure(SG_Gas* gaz);

		void*       mPlanet;            ///< The planet which this atmosphere belongs.
		long double mTotalAmount;       ///< Total amount of gases in the atmosphere.
		long double mTotalIPP;          ///< Total pressure for the gases heavier than Nitrogen.
		int         mGases;             ///< The number of gases in the atmosphere.
		SG_Gas*     mGasList[MAX_GAZ];  ///< The list of the gas of the atmosphere.
        std::string mPoisonedBy;        ///< List of the toxic gas of the atmosphere.
};



#endif
