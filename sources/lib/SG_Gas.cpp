/* ------------------------------------------------------------------------- */
// File       : SG_Gas.cpp
// Project    : Star Gen
// Author C++ : David de Lorenzo
// Author C   : Jim Burrows (see Credits.txt)
/* ------------------------------------------------------------------------- */

#include	<math.h>	
#include	"SG_Gas.h"
#include	"SG_Const.h"

/* ------------------------------------------------------------------------- */
/// Constructor.
/** 
@param AtomicNumber  The atomic number is the number of protons found in the nucleus of the atom. (ID key)
@param Symbol        The chemical symbol of the gas.
@param AtomicWeight  The atomic weigth is the average atomic mass of the chemical element.
@param MeltPoint     The Melting temperature of the element at 1 bar (unit = Kelvin)
@param BoilingPoint  The Boiling temperature of the element at 1 bar (unit = Kelvin)
@param Density       The gas density of the element (unit = gram/cc)
@param Abunds        The Aboundance of the element on the Solar System (unit = ??).
@param Reactivity    This indicate how fast the gas will disapear with time (0 = no loss)
@param Toxicity      Above this limit, the gas reaches a toxic ratio (unit = ppm).
@param Name          The full name of the gas.
*/
/* ------------------------------------------------------------------------- */
SG_Gas::SG_Gas(	int AtomicNumber, std::string Symbol,
                long double AtomicWeight, long double MeltPoint, long double BoilingPoint, 
                long double	Density,      long double Abunds, 
                long double Reactivity,   long double Toxicity,  std::string Name)
{
	// Gas description
	mNum        = AtomicNumber;
	mSymbol     = Symbol;
	mName       = Name;
	mWeight     = AtomicWeight;
	mMelt       = MeltPoint;
	mBoil       = BoilingPoint;
	mDensity    = Density;
	mAbunds     = Abunds;
	mReactivity = Reactivity;
	mMax_ipp    = Toxicity * PPM_PRESSURE; // converted in IPP (Inspired Partial Pressure)

	// Calculated parameters for this gas (in the atmosphere)
	mAmount = 0;
	mPartialPressure  = 0;
	mPartialPercentage= 0;

	// Gas default colour: transparent white
	mGasColour.r = 1;
	mGasColour.g = 1;
	mGasColour.b = 1;
	mGasColour.a = 0;
}

/* ------------------------------------------------------------------------- */
/// Destructor
/* ------------------------------------------------------------------------- */
SG_Gas::~SG_Gas() 
{
}


/* ------------------------------------------------------------------------- */
/// This function returns the Boiling Temperature of this gas, under a given pressure.
/** 
The Boiling Point is the transition between a liquid state and a gas state.
@param pressure The pressure submitted to the gas. (Units = mB)
@return The Boiling point of the gaz (Units = K)
*/
/* ------------------------------------------------------------------------- */
long double SG_Gas::getBoilingPoint(long double pressure)
{
	long double pressureB = pressure / MILLIBARS_PER_BAR; // unit = bar
	long double bp        = mBoil /(373. * ((log((pressureB) + 0.001) / -5050.5) + (1.0 / 373.)));
	return bp;
}


/* ------------------------------------------------------------------------- */
/// This function returns the Melting Temperature of this gas, under a given pressure.
/** 
The Boiling Point is the transition between a solid state and a liquid state.
@param pressure The pressure submitted to the gas. (Units = mB)
@return The Melting Point of the gaz (Units = K)
*/
/* ------------------------------------------------------------------------- */
long double SG_Gas::getMeltingPoint(long double pressure)
{
	/// Note: we consider here that the Melt Point is the same for all pressure.
	/// A better calculation can be done.
	return mMelt;
}

/* ------------------------------------------------------------------------- */
/// This function return the state of the element at certain conditions.
/**
@param temperature The temperature submitted to the element.
@param pressure    The pressure submitted to the element.
@return The state of the element (solid, liquid, or gas)
*/
/* ------------------------------------------------------------------------- */
int SG_Gas::getState(long double temperature, long double pressure)
{
	long double BP = this->getBoilingPoint(pressure);
	long double MP = this->getMeltingPoint(pressure);

	if (temperature<MP) return SOLID;
	if (temperature<BP) return LIQUID;
	return GAS;
}


/* ------------------------------------------------------------------------- */
/// This function returns the aboundance of the Gas in the Sun.
/** @return the solar aboundance of the Gas. */
/* ------------------------------------------------------------------------- */
long double SG_Gas::getAbound()
{
	return mAbunds; 				
}


/* ------------------------------------------------------------------------- */
/// This function calculate how the gas will slowly diseapear under some atmospheric conditions.
/** 
@param temperature The temperature submitted to the gas
@param pressure    The pressure submitted to the gas (unit = mB)
@param years       The elapsed time (unit = years)
@return The remaining fraction of the gas.
@sa getPres2
*/
/* ------------------------------------------------------------------------- */
long double SG_Gas::getReact(long double temperature, long double pressure, long double years)
{
	long double bil_years = years*1e-9;
	long double pres2 = this->getPres2(temperature, pressure, years);

	// Argon
	if (mSymbol ==  "Ar") 
	{
		return (0.19 * bil_years/4.0);
	}

	// Oxygen
    else if (((mSymbol == "O") || (mSymbol=="O2")) && (bil_years>2) && (temperature>270) && (temperature<400))
	{
		return pow(1 / (1 + mReactivity), pow1_4(bil_years/2.0) * pres2);
	}

	// Carbon Dioxid
    else if ((mSymbol == "CO2") && (bil_years>2) && (temperature>270) && (temperature<400))
	{
		return (1.5 * pow(1 / (1 + mReactivity), sqrt(bil_years/2.0) * pres2));
	}

	// Other gases
	else 
	{
		return pow(1 / (1 + mReactivity), bil_years/2.0 * pres2);
	}
}


/* ------------------------------------------------------------------------- */
/// Calculations for getReac.
/** 
@param temperature The temperature submitted to the gas
@param pressure    The pressure submitted to the gas (unit = mB)
@param years       The elapsed time (unit = years)
@return ????
@sa getReact
*/
/* ------------------------------------------------------------------------- */
long double SG_Gas::getPres2(long double temperature, long double pressure, long double years)
{
	long double bil_years  = years*1e-9;
	long double pressureB  = pressure / MILLIBARS_PER_BAR;  // unit = bar
	long double pres2;

	// Argon
	if (mSymbol ==  "Ar") 
	{
		return (1.0);
	}

	// Oxygen
	else if (((mSymbol=="O") || (mSymbol=="O2")) && (bil_years>2) && (temperature>270) && (temperature<400))
	{
//		/*	pres2 = (0.65 + pressureB/2).0;   //  Breathable - M: 0.55-1.4 	*/
		pres2 = 0.89 + (pressureB/4.0);		//	Breathable - M: 0.6-1.8 	
		return (pres2);
	}

	// Carbon Dioxid
    else if ((mSymbol == "CO2") && (bil_years>2) && (temperature>270) && (temperature<400))
	{
		pres2 = 0.75 + pressureB;
		return (pres2);
	}

	// Other gases
	else 
	{
		pres2 = (0.75 + pressureB);
        return pow(1 / (1 + mReactivity), bil_years/2.0 * pres2);
	}
}


/*--------------------------------------------------------------------------*/
/// This function return the RMS velocity of a gas, under a certain temperature.
/**	This is Fogg's eq.16.  The molecular weight (usually assumed to be N2)
	is used as the basis of the Root Mean Square (RMS) velocity of the
	molecule or atom.
	@param temperature The Temperature submitted to the gas.
	@return The RMS velocity of the gas molecules (Units = cm/sec)
*/
/*--------------------------------------------------------------------------*/
long double SG_Gas::getRMSvelocity(long double temperature)
{
	long double RT = 3.0 * MOLAR_GAS_CONST * temperature;
	long double V = sqrt(RT / mWeight);
	return(V * CM_PER_METER);
}


/* ------------------------------------------------------------------------- */
/// This function returns the ratio of the gas kept by the planet gravity and temperature.
/** This function calculate the ratio of gas escaped from the planet, due to a RMSvelocity
exceeding the Escape Velocity of the planet.
@param temperature     The temperature submitted to the gas (unit = Kelvin)
@param Escape_velocity The Escape velocity of the planet (unit = cm/s)
@param years           The elapsed time (unit = years)
@return The ratio of gas remaining.
*/
/* ------------------------------------------------------------------------- */
long double SG_Gas::getPVRMS(long double temperature,long double Escape_velocity, long double years)
{
	long double bil_years = years*1e-9;
	long double vrms = this->getRMSvelocity(temperature);
	return pow(1 / (1 + vrms / Escape_velocity), bil_years);
}


/* ------------------------------------------------------------------------- */
/// Faction of the gas depending on the atomic mass.
/** The more the atomic mass of the gas is close of the minimum atomic mass 
retained on the planet, the more the fraction will be close to 0.
On the other side, the more the gas is heavy, the more the fraction will 
be close to 1, and the more the gas will be kept on the planet.
*/
/* ------------------------------------------------------------------------- */
long double SG_Gas::getFract(long double Molec_weight)
{
	return (1 - (Molec_weight / mWeight));
}



/* ------------------------------------------------------------------------- */
/// This function returns the atomic weight for the gas molecule.
/* ------------------------------------------------------------------------- */
long double SG_Gas::getWeight()
{
	return (mWeight);
}


/* ------------------------------------------------------------------------- */
/// This function returns the atomic number (AN) of the gas molecule.
/* ------------------------------------------------------------------------- */
int SG_Gas::getAtomicNumber()
{
	return mNum;
}

/* ------------------------------------------------------------------------- */
/// Set the amount of the gas in the atmosphere.
/* ------------------------------------------------------------------------- */
void SG_Gas::setAmount(long double amount)
{
	mAmount = amount;
}

/* ------------------------------------------------------------------------- */
/// Get the amount of the gas in the atmosphere.
/* ------------------------------------------------------------------------- */
long double SG_Gas::getAmount()
{
	return mAmount;
}

/* ------------------------------------------------------------------------- */
/// This function return the chemical symbol of the gas.
/* ------------------------------------------------------------------------- */
std::string SG_Gas::getSymbol()
{
	return mSymbol;
}

/* ------------------------------------------------------------------------- */
/// This function indicates if the element is radioactive or not.
/* ------------------------------------------------------------------------- */
bool SG_Gas::isRadioactive()
{
	return (mWeight >= 84);
}

/* ------------------------------------------------------------------------- */
/// This function indicates if the gas a reached a toxical concentration in the atmosphere.
/* ------------------------------------------------------------------------- */
bool SG_Gas::isToxic(long double surf_pressure)
{
	bool toxicity;
	// If the IPP of the gas is higher than the Max IPP, it reaches a toxic level.
	toxicity = (this->getInspiredPartialPressure(surf_pressure) > mMax_ipp);
	if (mMax_ipp==0) toxicity=false;
	return toxicity;
}

/* ------------------------------------------------------------------------- */
/// This function returns the name of the gas.
/* ------------------------------------------------------------------------- */
std::string SG_Gas::getName()
{
	return mName;
}

/* ------------------------------------------------------------------------- */
/// This function returns the Maximal Inspired Partial Pressure of the gas.
/** Above this partial pressure, the gas reaches a toxic level.
*/
/* ------------------------------------------------------------------------- */
long double SG_Gas::getMaxIPP()
{
	return mMax_ipp;
}


/*--------------------------------------------------------------------------*/
///	This function determine if the gas can be breathed by a human.
/** 
	Taking into account humidification of the air in the nasal passage and throat.
	This formula is on Dole's p. 14
	@param surf_pressure The pressure at the surface of the planet
	@return The inspired partial pressure.
*/
/*--------------------------------------------------------------------------*/
long double SG_Gas::getInspiredPartialPressure(long double surf_pressure)
{
	long double fraction = mPartialPressure / surf_pressure;
	
	return	(surf_pressure - H20_ASSUMED_PRESSURE) * fraction;
}

/*--------------------------------------------------------------------------*/
/// Set the Partial Pressure of the gas in the atmosphere.
/*--------------------------------------------------------------------------*/
void SG_Gas::setPartialPressure(long double Partial_pressure)
{
	mPartialPressure = Partial_pressure;
}

/*--------------------------------------------------------------------------*/
/// This function returns the Partial Pressure of the gas in the atmosphere.
/*--------------------------------------------------------------------------*/
long double SG_Gas::getPartialPressure()
{
	return mPartialPressure;
}

/*--------------------------------------------------------------------------*/
/// This function set the percentage of the gas in the atmosphere.
/** @param pourcentage The percentage should be a value in [0..1].          */
/*--------------------------------------------------------------------------*/
void SG_Gas::setPartialPercentage(long double pourcentage)
{
	mPartialPercentage = pourcentage;
}

/*--------------------------------------------------------------------------*/
/// This function returns the percentage of the gas in the atmosphere.
/** @return The percentage is a value in [0..1].                            */
/*--------------------------------------------------------------------------*/
long double SG_Gas::getPartialPercentage()
{
	return mPartialPercentage;
}

/*--------------------------------------------------------------------------*/
/// This function memorize the colour of this gas.
/** 
@param r The RED value of the color (0..1)
@param g The GREEN value of the color (0..1)
@param b The BLUE value of the color (0..1)
@param a The ALPHA value (transparency) of the color (0..1). 0=transparent.
*/
/*--------------------------------------------------------------------------*/
void SG_Gas::setGasColour(double r, double g, double b, double a)
{
	mGasColour.r = r;
	mGasColour.g = g;
	mGasColour.b = b;
	mGasColour.a = a;
}

/*--------------------------------------------------------------------------*/
/// This function returns the colour of this gas.
/** 
@return The colour and transparency of the gas.
  --------------------------------------------------------------------------*/
SG_Gas::SG_colour SG_Gas::getGasColour()
{
	return mGasColour;
}
