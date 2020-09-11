/* ------------------------------------------------------------------------- */
// File       : SG_Gas.h
// Project    : Star Gen
// Author C++ : David de Lorenzo
// Author C   : Jim Burrows (see Credits.txt)
/* ------------------------------------------------------------------------- */

#ifndef _SG_GAS_H_
#define _SG_GAS_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "SG_Utils.h"

#define SOLID  1
#define LIQUID 2
#define GAS    4
#define ALL    7

/* ------------------------------------------------------------------------- */
/// A Gas from an atmosphere.
/* ------------------------------------------------------------------------- */
class SG_Gas
{
	public:
        struct SG_colour{
			double r;      ///< Red component of the Gas colour
			double g;      ///< Green component of the Gas colour
			double b;      ///< Blue component of the Gas colour
			double a;      ///< Transparency of the Gas
			};

	public:
        SG_Gas(	int AtomicNumber, std::string  Symbol,
		        long double AtomicWeight, long double MeltPoint, long double BoilingPoint, 
		        long double	Density,      long double Abunds, 
                long double Reactivity,   long double Max_ipp, std::string  Name);
		~SG_Gas();

		long double getAbound();
		long double getBoilingPoint(long double pressure);
		long double getMeltingPoint(long double pressure);
		bool        isRadioactive();
		bool        isToxic(long double surf_pressure);
		int         getState(long double temperature, long double pressure);
		long double getReact(long double temperature, long double pressure, long double years);
		long double getPVRMS(long double temperature, long double Escape_velocity, long double years);
		long double getFract(long double Molec_weight);

		void        setAmount(long double amount);
		long double getAmount();
		long double getWeight();
		int         getAtomicNumber();
        std::string    getSymbol();
        std::string    getName();
		long double getMaxIPP();
		long double getInspiredPartialPressure(long double surf_pressure);
		void        setPartialPressure(long double Partial_pressure);
		long double getPartialPressure();
		void        setPartialPercentage(long double pourcentage);
		long double getPartialPercentage();
		void        setGasColour(double r, double g, double b, double a);
		SG_colour   getGasColour();

    private:

		long double getPres2(long double temperature, long double pressure, long double years);
		long double getRMSvelocity(long double temperature);

        std::string    mName;     ///< Name of the gas molecule.
        std::string    mSymbol;   ///< Chemical Symbol of the Gas.
		int         mNum;         ///< Atomic Number (AN).
		long double mWeight;      ///< Atomic Weight.
		long double mMelt;        ///< Fusion Point of the molecule (under 1 bar).
		long double mBoil;        ///< Boiling Point of the molecule (under 1 bar).
		long double mAbunds;      ///< Aboundance of this gas in the Sun
		long double mReactivity;
		long double	mDensity;      ///< Gas density of the gas. (g/cc)
		long double mMax_ipp;      ///< Max inspired partial pressure (millibars)
		SG_colour   mGasColour;    ///< The colour of the gas

		// Calculated values for a specific atmosphere
		long double mAmount;	       ///< Amount of the gas in the atmosphere.
		long double mPartialPressure;  ///< Part of the atmosphere pressure due to the gas.
		long double mPartialPercentage;///< The percentage of this gas in the atmosphere [0..1]

};

#endif
