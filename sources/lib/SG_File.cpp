/* ------------------------------------------------------------------------- */
// File       : SG_File.cpp
// Project    : Star Gen
// Author C++ : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include <math.h>
#include <time.h>
#include "SG_File.h"
#include "SG_Const.h"

using namespace std;

/*----------------------------------------------------------------------*/
/// Constructor
/*----------------------------------------------------------------------*/
SG_File::SG_File(std::string filename, long seed)
{
using namespace std;

    mPlanet_Phrase[SG_Planet::tUnknown]    = "Unknown";
    mPlanet_Phrase[SG_Planet::tRock]       = "Rock";
    mPlanet_Phrase[SG_Planet::tVenusian]   = "Venusian";
    mPlanet_Phrase[SG_Planet::tTerrestrial]= "Terrestrial";
    mPlanet_Phrase[SG_Planet::tGasGiant]   = "GasGiant";
    mPlanet_Phrase[SG_Planet::tMartian]    = "Martian";
    mPlanet_Phrase[SG_Planet::tWater]      = "Water";
    mPlanet_Phrase[SG_Planet::tIce]        = "Ice";
    mPlanet_Phrase[SG_Planet::tSubGasGiant]= "GasPlanet";
    mPlanet_Phrase[SG_Planet::tSubSubGasGiant] = "GasDwarf";
    mPlanet_Phrase[SG_Planet::tAsteroids] = "Asteroids";

	fstream::openmode mode;

	// On ouvre le fichier en ecriture
	// mode = (fstream::out|fstream::app);
	mode = (fstream::out|fstream::trunc);
	mOutputFile.open(filename.c_str(), mode);

	mSeed = seed;
}

/*----------------------------------------------------------------------*/
/// Destructor. Close the output file.
/*----------------------------------------------------------------------*/
SG_File::~SG_File()
{
	mOutputFile.close();
}

/*----------------------------------------------------------------------*/
/// Write all the data concerning the Star, in the file.
/*----------------------------------------------------------------------*/
void SG_File::writeStarDescription(SG_Star* star)
{
	addSection("system");
	addIntValue("seed", mSeed);
	subSection();

	addSection("star");
    if (!star->mName.empty())
        addValue("name", star->mName);
    addValue("mass",     star->mMass,        "solar mass");
    addValue("lum",      star->mLum,         "solar lum");
	addValue("age",      star->mAge/1e9,     "billion years");
	addValue("life",     star->mLife/1e9,    "billion years left on main sequence");
	addValue("ecosphere",star->mR_ecosphere, "AU");
    addIntValue("planets"  ,star->mPlanetNumber,"planets");
	closeSection();
	mOutputFile << "\n";
}



/*----------------------------------------------------------------------*/
/// Write all the data concerning the Planet, in the file.
/*----------------------------------------------------------------------*/
void SG_File::writePlanetDescription(SG_Planet* planet)
{
	addLine();
	addSection("planet");
    addIntValue("number", planet->mPlanet_no);
	subSection();

	this->writePlanet(planet);
	this->writeAtmosphere(planet);
	if (!planet->mGas_planet) this->writeSolidPlanet(planet);
	if (planet->mGas_planet)  this->writeGasPlanet(planet);
		
	closeSection("planet");
}



/*----------------------------------------------------------------------*/
/// write the Planet description 
/*----------------------------------------------------------------------*/
void SG_File::writePlanet(SG_Planet* planet)
{
	bool S_atm = false;
	if (planet->mSurf_pressure/EARTH_SURF_PRES_IN_MILLIBARS > 0.001) S_atm = true;

	addSection("general");
	addValue("type"      ,mPlanet_Phrase[planet->mType]);
	addValue("giant"     ,planet->mGas_giant);
    addValue("gasPlanet" ,planet->mGas_planet);
	addValue("earthlike" ,planet->mEarthlike);
	addValue("albedo"    ,planet->mAlbedo);
	addValue("atmosphere",S_atm);
	closeSection();


	addSection("orbit");
	addValue("radius"      , planet->getOrbit() , "unit = AU");
	addIntValue("tilt"     , planet->mAxial_tilt, "unit = degree");
	addValue("eccentricity", planet->getEccentricity());
	// Planet is tidally locked with one face to the star.
    addValue("facelocked", planet->mFaceLocked);
	// Planet's rotation is in resonnant spin lock with the star.
	addValue("resonnant" ,  planet->mResonant_period);           
	closeSection();

	addSection("physical");
	addValue("density"      , planet->mDensity,    "unit = grams/cc");
	addValue("radius"       , planet->mRadius,     "unit = km");
    addValue("coreRadius"   , planet->mCore_radius,"unit = km");
	addValue("mass"         , TOEM(planet->mMass), "unit = EM");
//	addValue("DustMass_EM"  , TOEM(planet->mDust_mass), "unit = EM"); // debug
//	addValue("GasMass_EM"   , TOEM(planet->mGas_mass),  "unit = EM");  // debug
	addPercentage("dustMass", planet->mDust_mass/planet->mMass);
	addPercentage("gasMass" , planet->mGas_mass/planet->mMass);
	addValue("escVelocity"  , planet->mEsc_velocity/CM_PER_KM, "unit = km/s");
	closeSection();

	addSection("durations");
	addValue("yearE", planet->mOrbit_period, "unit = earth days");
	long double yearL = (planet->mDay)? ((planet->mOrbit_period/planet->mDay)*24) : 0;
	addValue("yearL", yearL,             "unit = local days");
	addValue("day"  , planet->mDay,      "unit = hours");
	addValue("dayE" , planet->mDay/24.0, "unit = earth days");
	closeSection();

	addSection("composition");
	// Molecular weight retained = minMolec and above.
	addValue("minMolec", planet->mMolec_weight, "and above");
	addValue("metal",    planet->mMetal);
	addValue("rock",    !planet->mGas_planet);

	subSection();
    std::string state_phrase[5] = {"", "solid", "liquid", "", "gas"};
	for (int g=0; g<MAX_GAZ; g++)
	{
		SG_Gas* curr_gas = planet->mAtmosphere->getGas(g,SOLID);
		if ((curr_gas) && (curr_gas->getWeight()>planet->mMolec_weight)) 
		{
			addSection("molecule");
            std::string symbol = planet->mAtmosphere->getGasSymbol(g);
			addValue("symbol", symbol, state_phrase[planet->mAtmosphere->getGasState(g)]);
			closeSection();
		}
	}
	closeSection("composition");
}


/*----------------------------------------------------------------------*/
/// write the planet description for solid planets 
/*----------------------------------------------------------------------*/
void SG_File::writeSolidPlanet(SG_Planet* planet)
{
	addSection("temperatures");
	addValue   ("exospheric", planet->mExospheric_temp, "unit = Kelvin");
	addIntValue("surfaceAvg", planet->mSurf_temp -FREEZING_POINT_OF_WATER, "unit = Celcius");
	addIntValue("surfaceMax", planet->mMax_temp  -FREEZING_POINT_OF_WATER, "unit = Celcius");
	addIntValue("surfaceMin", planet->mMin_temp  -FREEZING_POINT_OF_WATER, "unit = Celcius");
	addIntValue("nightTemp",  planet->mLow_temp  -FREEZING_POINT_OF_WATER, "unit = Celcius");
	addIntValue("dayTemp",    planet->mHigh_temp -FREEZING_POINT_OF_WATER, "unit = Celcius");
	closeSection();

	addSection("surface");
	addValue("gravity", planet->mSurf_grav,  "unit = Earth G");
	long double pressure = planet->mSurf_pressure/EARTH_SURF_PRES_IN_MILLIBARS;
	addValue   ("pressure",     pressure,               "unit = Earth atm");
    addIntValue("boilingPoint", planet->mBoiling_point, "unit = Kelvin");
    addValue   ("greenhouseEffect"  ,(planet->mGreenhouse_effect && (pressure>0)));
	closeSection();


	addSection("coverage");
	addPercentage("ground", 1-planet->mHydrosphere-planet->mIce_cover);
	addPercentage("water" , planet->mHydrosphere);
	addPercentage("ice"   , planet->mIce_cover);
	addPercentage("cloud" , planet->mCloud_cover);
	closeSection();

	// We analyse some elements
    string S_grav = "gravity OK";
	if (planet->mSurf_grav <0.8) S_grav = "low gravity";
	else if (planet->mSurf_grav > 1.2) S_grav = "high gravity";

    string S_temp;
	if      (planet->mSurf_temp < EARTH_AVERAGE_KELVIN -5) S_temp = "cold";
	else if (planet->mSurf_temp < EARTH_AVERAGE_KELVIN -2) S_temp = "cool";
	else if (planet->mSurf_temp < EARTH_AVERAGE_KELVIN +3) S_temp = "temperature OK";
	else if (planet->mSurf_temp > EARTH_AVERAGE_KELVIN +8) S_temp = "warm";
	else                                                   S_temp = "hot";

    string S_ice = "no";
	if (planet->mIce_cover >= 0.10) S_ice = "yes";  

    string S_atm = "normal atm";
	if      (planet->mSurf_pressure < EARTH_SURF_PRES_IN_MILLIBARS/1000.0) S_atm = "airless";
	else if (planet->mSurf_pressure < MIN_O2_IPP)                          S_atm = "unbreathly thin atm";
	else if (planet->mSurf_pressure < EARTH_SURF_PRES_IN_MILLIBARS/2.0)    S_atm = "thin atm";
	else if (planet->mSurf_pressure > EARTH_SURF_PRES_IN_MILLIBARS*2.0)    S_atm = "thick atm";
	else if (planet->mSurf_pressure > MAX_HABITABLE_PRESSURE)              S_atm = "unbreathly thick atm";

    string S_clouds;
	if      (planet->mCloud_cover < 0.10) S_clouds = "cloudless";
	else if (planet->mCloud_cover < 0.40) S_clouds = "few clouds";
	else if (planet->mCloud_cover < 0.80) S_clouds = "mid-coverage";
	else                                  S_clouds = "cloudy";

	addSection("comments");
	addValue("gravity"     ,S_grav);
	addValue("temperature" ,S_temp);
	addValue("icyPlanet"   ,S_ice);
	addValue("atmosphere"  ,S_atm);
	addValue("clouds"      ,S_clouds);
	closeSection();

    if (S_atm != "airless") writeClimate(planet);
}



/*----------------------------------------------------------------------*/
/// Write the planet description for solid planets 
/*----------------------------------------------------------------------*/
void SG_File::writeClimate(SG_Planet* planet)
{
    string S_hum = "balanced";
	if      (planet->mHydrosphere < 0.25) S_hum = "arid";
	else if (planet->mHydrosphere < 0.50) S_hum = "dry";
	else if (planet->mHydrosphere > 0.80) S_hum = "wet";
	else if (planet->mHydrosphere > 0.95) S_hum = "water world";


	addSection("ocean");
	subSection();
	for (int i=0; i<MAX_GAZ; i++)
	{
        SG_Gas* liquid = planet->mAtmosphere->getGas(i, LIQUID);
		if (liquid)
		{
			switch(liquid->getAtomicNumber()) 
			{
			case AN_S:
				// Sulfur: there is no sulfur ocean
				break;
			default:
				addSection("liquid");
				addValue("liquid", planet->mAtmosphere->getGasName(i));
				addValue("symbol", planet->mAtmosphere->getGasSymbol(i));
				closeSection();
			}
		}
	}
	closeSection("ocean");


	addSection("rain");
	subSection();
    for (int i=0; i<MAX_GAZ; i++)
	{
		SG_Gas* liquid = planet->mAtmosphere->getGas(i,LIQUID);
		if (liquid)
		{
			addSection("liquid");
			switch(liquid->getAtomicNumber()) 
			{
			case AN_S:
				// Sulfur: it can be SO2 rain or H2SO4 rain
				addValue("liquid", "Sulfuric acid");
				addValue("symbol", "H2SO4");
				break;
			default:
				addValue("liquid", planet->mAtmosphere->getGasName(i));
				addValue("symbol", planet->mAtmosphere->getGasSymbol(i));
			}
			closeSection();
		}
	}
	closeSection("rain");

	addSection("climate");
    addValue("humidityLevel",S_hum);
	closeSection();
}



/*----------------------------------------------------------------------*/
/// Write the atmosphere data. 
/*----------------------------------------------------------------------*/
void SG_File::writeAtmosphere(SG_Planet* planet)
{
	int g=0;
    string breathability_phrase[5];
	breathability_phrase[NONE]="none";
	breathability_phrase[BREATHABLE]="non-toxic";
	breathability_phrase[UNBREATHABLE]="not enough oxygen";
	breathability_phrase[STINKING]="stinking";
	breathability_phrase[POISONOUS]="poisonous";

	addSection("atmosphere");
    addIntValue("inventory", planet->mVolatile_gas_inventory, "unitless. Ref: Earth=1000" );
	addValue("breathe", breathability_phrase[planet->mBreathe]);
	if (planet->mBreathe==POISONOUS) addValue("toxicGas", planet->mAtmosphere->getToxicGasList());
	subSection();
	// we list only the gas that are more than 0.1%
	while (planet->mAtmosphere->getGasRatio(g) >= 0.001) 
	{
		SG_Gas* gaz = planet->mAtmosphere->getGas(g);
		addSection("gas");
		addValue     ("symbol",     gaz->getSymbol());
		addPercentage("proportion", gaz->getPartialPercentage());
		addValue     ("name",       gaz->getName());
		addValue     ("toxic",      gaz->isToxic(planet->mSurf_pressure));
		closeSection();
		g++;
	}
	closeSection("atmosphere");
}


/*----------------------------------------------------------------------*/
/// Write data for gas planets.
/** 
Note: I use Planet::calculatePressure to determine the pressure inside the 
gas planet. This is a not a very good approximation.
*/
/*----------------------------------------------------------------------*/
void SG_File::writeGasPlanet(SG_Planet* planet)
{
	addSection("limit");
	addValue("gravity",  planet->mSurf_grav,  "unit = Earth G");
	addValue("pressure", 100/EARTH_SURF_PRES_IN_MILLIBARS, "unit = Earth atm");
	closeSection();

	if (planet->mCore_radius>0)
	{
		addSection("core");
		addValue("gravity",  planet->calculateGravity(planet->mCore_radius)  ,"unit = Earth G");
		addValue("pressure", planet->calculatePressure(planet->mCore_radius/EARTH_SURF_PRES_IN_MILLIBARS),"unit = mB");
		closeSection();
	}
	
	addSection("center");
	addValue("gravity",  0.0,  "unit = Earth G");
	addValue("pressure", planet->mSurf_pressure/EARTH_SURF_PRES_IN_MILLIBARS,"unit = Earth atm");
	closeSection();
}

