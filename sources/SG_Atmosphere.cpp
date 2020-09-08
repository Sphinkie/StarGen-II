/* ------------------------------------------------------------------------- */
// File       : SG_Atmosphere.cpp
// Project    : Star Gen
// Author C++ : David de Lorenzo
// Author C   : Jim Burrows (see Credits.txt)
/* ------------------------------------------------------------------------- */

#include  "SG_Atmosphere.h"
#include  "SG_Planet.h"
#include  "SG_Utils.h"
#include  "SG_Const.h"


/* ------------------------------------------------------------------------- */
/// Constructor.
/** 
All the gas that can be present in at atmosphere are declared here.
CAUTION: do not exceed MAX_GAZ (or you should increase this constant).
*/
/* ------------------------------------------------------------------------- */
SG_Atmosphere::SG_Atmosphere(void* myPlanet)
{
	// Memorise the planet which own this atmosphere
	mPlanet = myPlanet;

	// Intialization of the gas array
	for (int j=0; j<MAX_GAZ; mGasList[j++]=NULL);
	int i=0;

	/// At-Nb Symbol At-Weight Melt Boil   Density   Abound-S  Rea Max-IPP
	mGasList[i++]= new SG_Gas
		(AN_H, "H",  1.0079, 14.06, 20.40, 8.99e-05, 27925.4000 /*91.00%*/, 1, 0, "Hydrogen");
//		(AN_H, "H2", 2.0158, 14.06, 20.40, 8.99e-05, 27925.4000 /*91.00%*/, 1, 0, "Hydrogen");
	mGasList[i++]= new SG_Gas
		(AN_HE,"He", 4.0026,  3.46,  4.20, 0.000178, 2722.70000 /*7.100%*/, 0, 61000*MMHG_TO_PPM,"Helium");
	mGasList[i++]= new SG_Gas
		(AN_N, "N2", 14.0067,63.34, 77.40, 0.001250, 3.13329000 /*0.015%*/, 0, 2330*MMHG_TO_PPM,"Nitrogen/Azote");
//		(AN_N, "N2",28.0134, 63.34, 77.40, 0.001250, 3.13329000 /*0.015%*/, 0, 2330*MMHG_TO_PPM,"Nitrogen/Azote");
	mGasList[i++]= new SG_Gas
		(AN_O, "O", 16,      54.80, 90.20, 0.001429, 23.8232000 /*0.050%*/, 10, 400*MMHG_TO_PPM,"Oxygen");
//		(AN_O, "O2",32,      54.80, 90.20, 0.001429, 23.8232000 /*0.050%*/, 10, 400*MMHG_TO_PPM,"Oxygen");
	mGasList[i++]= new SG_Gas
		(AN_O3,"O3",48,      80.16,161.16, 0.001000, 0.00000100, 2/*0.001*/, 0.1,"Ozone");
	mGasList[i++]= new SG_Gas
		(AN_NE,"Ne",20.17,   24.53, 27.10, 0.000900, 3.44350e-5, 0, 3900*MMHG_TO_PPM,"Neon");
	mGasList[i++]= new SG_Gas
		(AN_AR,"Ar",39.948,  84.00, 87.30, 0.001782, 0.10092500, 0, 1220*MMHG_TO_PPM,"Argon");
	mGasList[i++]= new SG_Gas
		(AN_KR,"Kr",83.8,   116.60,119.70, 0.003708, 4.4978e-05, 0, 350*MMHG_TO_PPM,"Krypton");
	mGasList[i++]= new SG_Gas
		(AN_XE,"Xe",131.3,  161.30,165.00, 0.005880, 4.6989e-06, 0, 160*MMHG_TO_PPM,"Xenon");
	mGasList[i++]= new SG_Gas
		(AN_NH3,"NH3",17,   195.46,239.66, 0.001000, 0.00010000, 1/*0.001*/, 100,"Ammonia");
	mGasList[i++]= new SG_Gas
		(AN_H2O,"H2O",18,   273.16,373.16, 1.000000, 0.00100000, 0, 0,"Water");
	mGasList[i++]= new SG_Gas
		(AN_CO2,"CO2",44,   194.66,194.66, 0.001000, 0.00400000, 0, 7*MMHG_TO_PPM,"CarbonDiOxide");
	mGasList[i++]= new SG_Gas
		(AN_CH4,"CH4",16,    90.16,109.16, 0.010000, 0.00010000, 1/*0*/, 5000,"Methane");

	// Additional elements
	mGasList[i++]= new SG_Gas
		(AN_F,  "F",  18.99, 53.58, 85.10, 0.001696, 0.00084333, 50, 0.1,"Fluor");
	mGasList[i++]= new SG_Gas
		(AN_CL, "Cl", 35.45,172.22,239.20, 0.003214, 0.00523600, 40, 1,"Chlore");
	mGasList[i++]= new SG_Gas
		(AN_C,  "C",  12.01,3820.0,5100.0, 2.260000, 6.89323800 /*0.033%*/, 0, 0,"Carbon");
	mGasList[i++]= new SG_Gas
		(AN_S,  "S",  32.06, 386.0, 717.8, 2.070000, 0.42000000 /*0.002%*/, 0, 2,"Sulfur");
	mGasList[i++]= new SG_Gas
		(AN_SO2,"SO2",64.06, 197.0, 262.9, 3.049000, 0.00020000, 4, 2,"Sulfur Dioxid"); /* abound uncertain */
	mGasList[i++]= new SG_Gas
		(AN_CO,"CO",  28.01,   0.0,  81.4, 0.968000, 0.00001000, 0, 25, "Carbon Monoxid");

	/// Metals (here or not ?) - TODO
	mGasList[i++]= new SG_Gas
		(AN_FE, "Fe", 55.85,1809.0,3023.0, 7.800000, 0.63000000 /*0.003%*/,0, 0.0,"Iron");
	mGasList[i++]= new SG_Gas
		(AN_CU, "Cu", 63.54,1356.6,2840.0, 8.960000, 0.00001000, 0, 0,"Copper");
	mGasList[i++]= new SG_Gas
		(AN_NI, "Ni", 58.69,1726.0,2647.0, 8.900000, 0.00001000, 0, 0,"Nickel");


	/// Complex gas : after reaction - TODO
//	mGasList[i++]= new SG_Gas
//		(AN_H2SO4,"H2SO4", 98,-15c,330c, 1.82, x, 0, x,"Sulfuric acid"); //letal>1,2 mg/l 

	mTotalAmount     = 0;
	mTotalIPP        = 0;
	mGases           = 0;
	mPoisonedBy      = "";

	// We calculate the pressure due to gases heavier than Nitrogen (Azote)
	for (int index=0; index<MAX_GAZ; index++)
	{
		if (!mGasList[index]) break;
		if ((mGasList[index]->getWeight() >= AN_N) && (mGasList[index]->getMaxIPP() < 1e9))
			mTotalIPP += mGasList[index]->getMaxIPP();
	}
}


/* ------------------------------------------------------------------------- */
/// Destructor
/* ------------------------------------------------------------------------- */
SG_Atmosphere::~SG_Atmosphere()
{
	for (int i=0; i<MAX_GAZ; i++)
	{
		if (mGasList[i]) delete (mGasList[i]);
	}
}


/* ------------------------------------------------------------------------- */
/// This function calculate a first approximation of the atmosphere composition.
/**
this function also updates the total quantity of gases in the atmosphere.
@return The number of gases present in the atmosphere.
@sa calculateAtmosphere
*/
/* ------------------------------------------------------------------------- */
int SG_Atmosphere::estimateComposition()
{
	long double partial_amount = 0;
	int number_of_gas = 0;
	int gas_index     = 0;

	while (mGasList[gas_index])
	{
		partial_amount = this->getGasAmount(mGasList[gas_index]);
		if (partial_amount>0) number_of_gas++;
		mTotalAmount += partial_amount;
		gas_index++;
	}

	mGases = number_of_gas;
	return number_of_gas;
}


/* ------------------------------------------------------------------------- */
/// Calculate the amount of a given gas in the atmosphere.
/** 
@param gaz The gas to analyse.
@return The amount of this gas in the atmosphere.
@sa getGasPressure
*/
/* ------------------------------------------------------------------------- */
long double SG_Atmosphere::getGasAmount(SG_Gas* gaz)
{
	SG_Planet* P = static_cast<SG_Planet*>(mPlanet);

	long double gas_amount = 0;
	long double bp = gaz->getBoilingPoint(P->mSurf_pressure);

	if ((bp >= 0) && (bp < P->mLow_temp) && (gaz->getWeight() >= P->mMolec_weight))
	{
		// The initial aboundance of this gas 
		long double abund = gaz->getAbound();
		// The amount of gas kept depending on temperature and pressure, and time
		long double react = gaz->getReact(P->mSurf_temp, P->mSurf_pressure, P->mPrimaryStar->mAge);
		// The amount of gas kept depending on the planet gravity
		long double pvrms = gaz->getPVRMS(P->mSurf_temp, P->mEsc_velocity, P->mPrimaryStar->mAge);
//		long double vrms  = this->getRMSvelocity(temperature); 
		// The amount of gas kept depending its atomic mass
		long double fract = gaz->getFract(P->mMolec_weight);

		gas_amount = abund * pvrms * react * fract;
		gaz->setAmount(gas_amount);

		SG_Utils::writeLog("gas ="+gaz->getSymbol());
		SG_Utils::writeLog("  abund :"+ITOS(abund));
		SG_Utils::writeLog(" *pvrms :"+ITOS(pvrms));
		SG_Utils::writeLog(" *react :"+ITOS(react));
		SG_Utils::writeLog(" *fract :"+ITOS(fract));
		SG_Utils::writeLog(" =amount:"+ITOS(gas_amount));
	}
	return gas_amount;
}


/* ------------------------------------------------------------------------- */
/// This function determines the proportion of a given gas in this atmosphere.
/** 
Note: The function getGasAmount() should be called once before.
@param gaz The gas to evaluate.
@return The pressure due to this gas in the atmosphere (Unit = mB)
@sa getGasAmount
*/
/* ------------------------------------------------------------------------- */
long double SG_Atmosphere::getGasPressure(SG_Gas* gaz)
{
	SG_Planet* myPlanet = static_cast<SG_Planet*>(mPlanet);

	long double partial_pressure = 0;  // unit = mB
	long double partial_amount   = 0;
	long double pourcentage = 0;

	if (mTotalAmount==0) return 0;
	if (myPlanet->mSurf_pressure==0) return 0;

	partial_amount = gaz->getAmount();
		
	if (partial_amount>0) 
	{
		partial_pressure = myPlanet->mSurf_pressure * (partial_amount / mTotalAmount);
		pourcentage = partial_pressure / myPlanet->mSurf_pressure;
		SG_Utils::writeLog(" gas :"+gaz->getSymbol());
		SG_Utils::writeLog(" partial_pressure :"+ITOS(partial_pressure));
		SG_Utils::writeLog(" pourcentage :"     +ITOS(pourcentage));
	}

	gaz->setPartialPressure(partial_pressure);
	gaz->setPartialPercentage(pourcentage);
	
	return partial_pressure;
}


/* ------------------------------------------------------------------------- */
/// This function determines the precise composition of the atmosphere, and the proportion of the different gases in it.
/**
Note: The function estimateAtmosphere() should be called once before.
*/
/* ------------------------------------------------------------------------- */
int SG_Atmosphere::calculateAtmosphere()
{
	int number_of_gas = this->estimateComposition();
	SG_Utils::writeLog(" number_of_gas :"+ITOS(number_of_gas));

	if (number_of_gas>0) 
	{
		int gas_index = 0;
		while (mGasList[gas_index])
		{
			this->getGasPressure(mGasList[gas_index]);
			gas_index++;
		}
		// We sort the gas table, by decreasing percentage.
		this->sortGasTable();
	}

	return number_of_gas;
}


/*--------------------------------------------------------------------------*/
/// This function determines if the atmosphere can be breathed or not.
/**
This function uses figures on the maximum inspired partial pressures
of Oxygen, other atmospheric and traces gases.
From pages 15,16,18 of Dole's "Habitable Planets for Man".
@return The value returned is one of the value defined in the .h file 
(NONE, BREATHABLE, UNBREATHABLE, STINKING, POISONOUS)
*/
/*--------------------------------------------------------------------------*/
int SG_Atmosphere::getBreathability()
{
	SG_Planet* myPlanet = static_cast<SG_Planet*>(mPlanet);

	if (mGases == 0) return NONE;

	for (int index=0; index < mGases; index++)
	{
		long double ipp = mGasList[index]->getInspiredPartialPressure(myPlanet->mSurf_pressure);

		// If the IPP of the gas is higher than the Max IPP, it reaches a toxic level.
		if (ipp > mGasList[index]->getMaxIPP())
		{
			// we store the name of the first toxic gas found (the one with the highest ratio)
			mPoisonedBy = mGasList[index]->getName(); 
			return POISONOUS;
		}

		// If the IPP of the gas is close to the Max IPP, the air smells really bad.
		if (ipp > 0.8*mGasList[index]->getMaxIPP())
		{
			return STINKING;
		}

		// Air is unbreatheable if not enough Oxygen in the atmosphere.
		if (mGasList[index]->getAtomicNumber() == AN_O)
		{
			if (ipp < MIN_O2_IPP) return UNBREATHABLE;
		}
	}

	return BREATHABLE;
}


/*--------------------------------------------------------------------------*/
/// This function returns the full name of a gas from the atmosphere.
/** 
@param rank      The rank of the gas in the atmosphere (0= the gas with the highest ratio)
@param stateMask The gas can be filtered according to its status (GAS, LIQUID, SOLID or ALL)
It is also possible to use expressions, ie: SOLID|LIQUID.
@return The full name of the gas. (or a blank string if there is no gas at this index).
*/
/*--------------------------------------------------------------------------*/
SG_String SG_Atmosphere::getGasName(int rank, int stateMask)
{
	SG_Gas* gas  = mGasList[rank];
	if (!gas) return "";

	SG_Planet* myPlanet = static_cast<SG_Planet*>(mPlanet);
	int state = gas->getState(myPlanet->mSurf_temp, myPlanet->mSurf_pressure);
	if ((state&stateMask) == 0x00) return "";

	return gas->getName();
}


/*--------------------------------------------------------------------------*/
/// This function returns the chemical symbol of a gas from the atmosphere.
/** 
@param rank      The rank of the gas in the atmosphere (0= the gas with the highest ratio)
@param stateMask The gas can be filtered according to its status (GAS, LIQUID, SOLID or ALL)
It is also possible to use expressions, ie: SOLID|LIQUID.
@return The symbol of the gas. (or a blank string if there is no gas at this index).
*/
/*--------------------------------------------------------------------------*/
SG_String SG_Atmosphere::getGasSymbol(int rank, int stateMask)
{
	SG_Gas* gas  = mGasList[rank];
	if (!gas) return "";

	SG_Planet* myPlanet = static_cast<SG_Planet*>(mPlanet);
	int state = gas->getState(myPlanet->mSurf_temp, myPlanet->mSurf_pressure);
	if ((state&stateMask) == 0x00) return "";

	return gas->getSymbol();
}


/*--------------------------------------------------------------------------*/
/// This function returns the state of a gas from the atmosphere.
/** 
@param rank      The rank of the gas in the atmosphere (0= the gas with the highest ratio)
@return The state of the gas. Returned values are: GAS, LIQUID, SOLID (or 0 if there is no gas at this index).
*/
/*--------------------------------------------------------------------------*/
int SG_Atmosphere::getGasState(int rank)
{
	SG_Planet* myPlanet = static_cast<SG_Planet*>(mPlanet);

	SG_Gas* gas  = mGasList[rank];
	if (!gas) return 0;

	return gas->getState(myPlanet->mSurf_temp, myPlanet->mSurf_pressure);
}


/*--------------------------------------------------------------------------*/
/// This function returns the percentage of a gas in the atmosphere.
/** 
@param rank The rank of the gas in the atmosphere (0= the gas with the highest ratio)
@return The percentage of this gas in the atmosphere [0..1].
*/
/*--------------------------------------------------------------------------*/
long double SG_Atmosphere::getGasRatio(int rank)
{
	SG_Gas* gas  = mGasList[rank];
	if (!gas) return 0;
	else      return (gas->getPartialPercentage());
}


/*--------------------------------------------------------------------------*/
/// This function returns a gas in the atmosphere.
/** 
@param rank The rank of the gas in the atmosphere (0= the gas with the highest ratio)
@param stateMask The gas can be filtered according to its status (GAS, LIQUID, SOLID or ALL)
It is also possible to use expressions, ie: SOLID|LIQUID.
@return This gas (or NULL if not found).
*/
/*--------------------------------------------------------------------------*/
SG_Gas* SG_Atmosphere::getGas(int rank, int stateMask)
{
	SG_Gas* gas  = mGasList[rank];
	if (!gas) return NULL;

	SG_Planet* myPlanet = static_cast<SG_Planet*>(mPlanet);
	int state = gas->getState(myPlanet->mSurf_temp, myPlanet->mSurf_pressure);
	if ((state&stateMask) == 0x00) return NULL;
	
	return mGasList[rank];
}


/*--------------------------------------------------------------------------*/
/// This function sorts the array of gas, following the decreasing percentage.
/*--------------------------------------------------------------------------*/
void SG_Atmosphere::sortGasTable()
{
	long double pc;

	for (int i=1; i<MAX_GAZ-1; i++) 
	{
		if (!mGasList[i]) break;
		pc = mGasList[i]->getPartialPercentage();
		for (int j=i; j>0; j--)   //Move bigger values up one position
		{
			if (!mGasList[j])   break;
			if (!mGasList[j-1]) break;
			if (mGasList[j]->getPartialPercentage() < mGasList[j-1]->getPartialPercentage() ) break;
			swapGas(j-1,j);
		}
	}
}


/*--------------------------------------------------------------------------*/
/// This function swap two gases in the gas array.
/*--------------------------------------------------------------------------*/
void SG_Atmosphere::swapGas(int index1, int index2)
{
	SG_Gas* gas1 = mGasList[index1];
	SG_Gas* gas2 = mGasList[index2];
	mGasList[index1] = gas2;
	mGasList[index2] = gas1;
}

/*--------------------------------------------------------------------------*/
/// This function returns the list of the toxic gas of the atmosphere.
/** 
@return The return value is empty if the atmosphere is not poisonous.
@sa getBreathability
*/
/*--------------------------------------------------------------------------*/
SG_String SG_Atmosphere::getToxicGasList()
{
	return mPoisonedBy;
}
