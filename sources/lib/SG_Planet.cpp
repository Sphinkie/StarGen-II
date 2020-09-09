/* ------------------------------------------------------------------------- */
// File       : SG_Planet.cpp
// Project    : Star Gen
// Author C++ : David de Lorenzo
// Author C   : Jim Burrows (see Credits.txt)
/* ------------------------------------------------------------------------- */
#include <math.h>
#include "SG_Planet.h"
#include "SG_Utils.h"
#include "SG_Const.h"

#define UNDEFINED_TILT 360

/* ------------------------------------------------------------------------- */
/// Constructor
/* ------------------------------------------------------------------------- */
SG_Planet::SG_Planet(SG_Star* sun, int planet_no, long double a, long double e, long double mass)
{
	mPrimaryStar = sun;
	mPlanet_no   = planet_no;
	m_a          = a;
	m_e          = e;
	mMass        = mass;

	mAtmosphere      = new SG_Atmosphere(this);
	mHigh_temp       = 0;
	mLow_temp        = 0;
	mMax_temp        = 0;
	mMin_temp        = 0;
	mGreenhouse_rise = 0;
	mResonant_period = false;
	mFaceLocked      = false;
	mDust_mass       = 0;
	mGas_mass        = 0;
	mGas_giant       = false;
	mGas_planet      = false;
	mMetal           = false;
	mType            = tUnknown;
	mAxial_tilt      = UNDEFINED_TILT;
	mBreathe         = NONE;
};


/* ------------------------------------------------------------------------- */
/// Destructor
/* ------------------------------------------------------------------------- */
SG_Planet::~SG_Planet()
{
	delete mAtmosphere;
}


/*--------------------------------------------------------------------------*/
///	 This function returns the radius of the core of the planet (units = km).
/**	 
This formula is listed as eq.9 in Fogg's article, although some typos
crop up in that eq.  See "The Internal Constitution of Planets", by
Dr. D. S. Kothari, Mon. Not. of the Royal Astronomical Society, vol 96
pp.833-843, 1936 for the derivation.  
Specifically, this is Kothari's eq.23, which appears on page 840.
*/
/*--------------------------------------------------------------------------*/
long double SG_Planet::getKothariRadius(long double mass, bool giant, int zone)
{
	volatile long double temp1;
	long double temp2;
	long double radius;
	long double atomic_weight =1;
	long double atomic_num    =1;

	if (zone==1)
	{
		if (giant)
		{
			atomic_weight= 9.5;
			atomic_num   = 4.5;
		}
		else
		{
			atomic_weight= 15.0;
			atomic_num   = 8.0;
		}
	}
	else if (zone==2)
	{
		if (giant)
		{
			atomic_weight= 2.47;
			atomic_num   = 2.0;
		}
		else
		{
			atomic_weight= 10.0;
			atomic_num   = 5.0;
		}
	}
	else if (zone==3)
	{
		if (giant)
		{
			atomic_weight= 7.0;
			atomic_num   = 4.0;
		}
		else
		{
			atomic_weight= 10.0;
			atomic_num   = 5.0;
		}
	}
	else
	  SG_Utils::writeLog("Error: orbital zone not initialized correctly!");

	temp1 = (BETA_20/A1_20) * (pow1_3(SOLAR_MASS_IN_GRAMS)/pow1_3(atomic_weight*atomic_num));

	temp2  = A2_20 * pow4_3(atomic_weight) * pow2_3(SOLAR_MASS_IN_GRAMS) * pow2_3(mass);
	temp2  = temp2 / (A1_20 * pow2(atomic_num)) + 1.0;

	radius = 2.0 * (temp1 / temp2) * pow1_3(mass) / CM_PER_KM;
	radius /= JIMS_FUDGE;			// Make Earth = actual earth 

	return (radius);
}


/*--------------------------------------------------------------------------*/
/// This function determine the 'empirical' density of a planet.
/**
@param orbit_radius  The orbital radius (Units = AU).
@param r_ecosphere   The radius of the Star ecosphere (Units = AU).
@param gas_giant     TRUE if you want to estimate for a gaz geant planet.
@return The 'empirical' density (Units = grams/cc)
/*--------------------------------------------------------------------------*/
long double SG_Planet::estimateDensity(long double orbit_radius,long double r_ecosphere, bool gas_giant)
{
	long double temp;
	temp = pow1_8(TOEM(mMass)) * pow1_4(r_ecosphere / orbit_radius);

	if (gas_giant)
		return(temp * 1.2);
	else
		return(temp * 5.5);
}


/*--------------------------------------------------------------------------*/
/// This function calculate the length of the day on the planet.
/**	 Fogg's information for this routine came from Dole "Habitable Planets
for Man", Blaisdell Publishing Company, NY, 1964.  From this, he came
up with his eq.12, which is the equation for the 'base_angular_velocity'
below.  He then used an equation for the change in angular velocity per
time (dw/dt) from P. Goldreich and S. Soter's paper "Q in the Solar
System" in Icarus, vol 5, pp.375-389 (1966).	 Using as a comparison the
change in angular velocity for the Earth, Fogg has come up with an
approximation for our new planet (his eq.13) and take that into account.
This is used to find 'change_in_angular_velocity' below.

Used parameters are mass (in solar masses), radius (in Km), orbital
period (in days), orbital radius (in AU), density (in g/cc),
eccentricity, and whether it is a gas giant or not.

Note: OK when applied on the solar system planets, but wrong (16h) when applied
to the Earth. Maybe because the Moon influence is not taken into account.
@return The length of the day (units = hours).
*/
/*--------------------------------------------------------------------------*/
long double SG_Planet::calculateDayLength()
{
	long double planetary_mass_in_grams = mMass * SOLAR_MASS_IN_GRAMS;
	long double equatorial_radius_in_cm = mRadius * CM_PER_KM;
	long double year_in_hours           = mOrbit_period * 24;
	long double k2;
	long double base_angular_velocity;
	long double change_in_angular_velocity;
	long double ang_velocity;               // sec per radian
	long double spin_resonance_factor;
	long double day_in_hours;               // hour per rotation

	int stopped = false;
	mResonant_period = false;	// Warning: Modify the planet

	if (mGas_giant)
		k2 = 0.24;
	else 
		k2 = 0.33;

	base_angular_velocity = sqrt(2.0 * J * (planetary_mass_in_grams) /
	                             (k2 * pow2(equatorial_radius_in_cm)) );

	SG_Utils::writeLog("base_angular_velocity = "+ITOS(base_angular_velocity));

	// This next calculation determines how much the planet's rotation is
	// slowed by the presence of the star.
	change_in_angular_velocity = CHANGE_IN_EARTH_ANG_VEL *
			(mDensity / EARTH_DENSITY) *
			(equatorial_radius_in_cm / EARTH_RADIUS) *
			(EARTH_MASS_IN_GRAMS / planetary_mass_in_grams) *
			pow2(mPrimaryStar->mMass) * (1.0 / pow(m_a, 6.0));

	ang_velocity = base_angular_velocity + (change_in_angular_velocity * mPrimaryStar->mAge);

	SG_Utils::writeLog("ang_velocity = "+ITOS(ang_velocity));

	// Change from rad/sec to hours/rotation.
	if (ang_velocity <= 0.0)
	{
		stopped = true;
		day_in_hours = INCREDIBLY_LARGE_NUMBER ;
	}
	else 
	{
		stopped = false;
		day_in_hours = RADIANS_PER_ROTATION / (SECONDS_PER_HOUR * ang_velocity);
	}

	// Special case for very slow rotations.
	if ((day_in_hours >= year_in_hours) || stopped)
	{
		if (m_e > 0.1)
		{
			spin_resonance_factor = (1.0 - m_e) / (1.0 + m_e);
			mResonant_period = true;
		}
		else 
		{
			spin_resonance_factor = 1;
			mResonant_period = false;
		}
		return(spin_resonance_factor * year_in_hours);
	}

	// General case.
	return(day_in_hours);
}

/*--------------------------------------------------------------------------*/
/// This function determine if the same side of the planet is always facing the star.
/** 
@return TRUE if a face of the planet is locked toward the star.
*/
/*--------------------------------------------------------------------------*/
bool SG_Planet::calculateFaceLocking()
{
	int day_length = (int)mDay;                // unit = hour
	int year_length = (int)(mOrbit_period*24); // unit = hour
	return (day_length == year_length);
}


/*--------------------------------------------------------------------------*/
/// This function determine the inclinaison angle of the planet.
/** 
If the Axial tilt is already set, we don't change it.
@param orb_radius The orbital radius (Unit = Astronomical Units AU)
@return The planet inclination (Unit = degrees)
*/
/*--------------------------------------------------------------------------*/
int SG_Planet::calculateInclination(long double orb_radius)
{
	long double Inclination;

	// If mAxial_tilt is already defined, we dont calculate it.
	if (mAxial_tilt != UNDEFINED_TILT) return mAxial_tilt;

	Inclination  = pow(orb_radius,0.2);
	Inclination *= SG_Utils::about(EARTH_AXIAL_TILT,0.4);
	return ((int)Inclination % 360);
}


/*--------------------------------------------------------------------------*/
///	 This function implements the escape velocity calculation.
/** 
In a given gravitational field, the escape velocity is the minimum speed 
an  object needs to have to move away indefinitely from the planet, 
as opposed to falling back or staying in an orbit. 
Examples: earth=11km/s  moon=2km/s sun=617km/s
Note that it appears that Fogg's eq.15 is incorrect.
@return The escape velocity of the planet (unit = cm/sec).
*/
/*--------------------------------------------------------------------------*/
long double SG_Planet::calculateEscapeVelocity()
{
	long double mass_in_grams;	/// The mass is in units of solar mass.
	long double radius_in_cm;	/// The radius is in kilometers.

	mass_in_grams = mMass * SOLAR_MASS_IN_GRAMS;
	radius_in_cm  = mRadius * CM_PER_KM;
	return(sqrt(2.0 * GRAV_CONSTANT * mass_in_grams / radius_in_cm));
}


/*--------------------------------------------------------------------------*/
/// This function returns an estimation of the smallest molecular weight retained by the planet.
/**
This is an approximate value (molecule_limit), which is useful for determining the atmosphere composition.
This function is based on the 'cinetic gas theory'.
- PV = nRT    (perfect gas equation)
- E  = mv²/2  (cinetic energy)
- RT = mv²/3
@return The minimal molecule weight retained by the planet gravity. (unit=gram)
@sa     calculateMinimalMoleculeWeight
*/
/*--------------------------------------------------------------------------*/
long double SG_Planet::estimateMinimalMoleculeWeight()
{
	long double esc_velocity = this->calculateEscapeVelocity() / CM_PER_METER; // unit=m/s
	long double V2 = pow2( esc_velocity/GAS_RETENTION_THRESHOLD );
	long double RT = MOLAR_GAS_CONST * mExospheric_temp;
    // m = 3RT/v²
	return (3.0 * RT / V2);
}


/*--------------------------------------------------------------------------*/
/// This function calculates the gravity of a planet.	
/**	
@param distance The distance from the planet center, where the gravity is calculated.
(units = km) (use mRadius to obtain the surface gravity). 
@return The gravity (Units = Earth gravities)
*/
/*--------------------------------------------------------------------------*/
long double SG_Planet::calculateGravity(long double distance)
{
	long double mass = mMass * SOLAR_MASS_IN_GRAMS;      // inner sphere mass (units=gram).
	if (distance<mRadius) mass=getSphereMass(mDensity, distance);
	/// Acceleration (units = cm/sec2)
	long double acceleration = GRAV_CONSTANT*(mass)/pow2(distance*CM_PER_KM);
	return (acceleration/EARTH_ACCELERATION);
}


/*--------------------------------------------------------------------------*/
/// This function returns the 'inventory' of the gas in the atmosphere.
/** 
This function implements Fogg's equation 17.
The Earth inventory is 1000 (reference).
@param escape_vel        The escape velocity of the planet (unit=cm/sec).
@param stellar_mass      The mass of the star (unit=solar mass)
@param greenhouse_effect TRUE if the planet is under a greenhouse effect.
@param accreted_gas      TRUE if the planet has a certain amount of gas.
@return The inventory (unitless).
@sa calculateSurfaceTemperature generatePlanet
*/
/*--------------------------------------------------------------------------*/
long double SG_Planet::calculateVolatilGasRatio(
			long double escape_vel,
			long double stellar_mass,
			bool greenhouse_effect,
			bool accreted_gas)
{
	long double earth_units;
	long double velocity_ratio;
	long double proportion_const;
	long double N_RMSvelocity;
	long double ratio;

	N_RMSvelocity  = this->getRMSvelocity(MOL_NITROGEN);
	velocity_ratio = escape_vel / N_RMSvelocity;
	if (velocity_ratio >= GAS_RETENTION_THRESHOLD)
	{
		switch (mOrbit_zone) 
		{
			case 1:
				proportion_const = 140000.0;
				break;
			case 2:
				proportion_const = 75000.0;
				break;
			case 3:
				proportion_const = 250.0;
				break;
			default:
				proportion_const = 0;
				SG_Utils::writeLog("Error: orbital zone not initialized correctly!");
				break;
		}
		earth_units = TOEM(mMass);
		ratio = (proportion_const * earth_units) / stellar_mass;
		if (greenhouse_effect || accreted_gas)
			return(ratio);
		else 
			return(ratio/140.0);
	}
	else 
		return(0.0);
}


/*--------------------------------------------------------------------------*/
/// This function determine the pressure on the planet.
/**
This implements Fogg's equation 18.
It use the 'inventory' of the atmosphere (reference value = 1000).
This is more dedicated for solid (non gas) planets.
@param  radius The distance from the planet center where the pressure is calculated.
(unit=km) (use mRadius to get the planet surface pressure).
@return The pressure (Units = millibars)
*/
/*--------------------------------------------------------------------------*/
long double SG_Planet::calculatePressure(long double radius)
{
	long double radius_ER = mRadius/EARTH_RADIUS_IN_KM;         // unit = Earth radius
	long double gravity   = this->calculateGravity(mRadius);    // unit = Earth G
	return (mVolatile_gas_inventory * gravity * pow2(radius_ER) * EARTH_SURF_PRES_IN_BARS);
}


/*--------------------------------------------------------------------------*/
/// This function returns the boiling point of water on a planet, under a given atmosphere.
/**
This implements Fogg's equation 21.
@param pressure The pressure submiitted to the water (Units = millibars).
(Use the surface pressure to obtain the boiling temperature on the planet surface).
@return The boiling point (Units = Kelvin)
*/
/*--------------------------------------------------------------------------*/
long double SG_Planet::calculateBoilingPoint(long double pressure)
{
	if (pressure==0) return 0;
	
	long double pressure_in_bars;
	pressure_in_bars = pressure / MILLIBARS_PER_BAR;
	long double delta_press  = log10(pressure_in_bars) / 5050.5;
	return (1.0 / ((1.0/BOILING_POINT_OF_WATER) - delta_press));
}


/*--------------------------------------------------------------------------*/
/// This function calculates the fraction of the planet which is covered with water.
/**	 This function is Fogg's eq.22.	 Given the volatile gas inventory and
planetary radius of a planet (in Km), this function returns the
fraction of the planet covered with water. Note: the fraction of Earth's
surface covered by water is 71%, not 75% as Fogg used.
@sa calculateSurfaceTemperature
*/
/*--------------------------------------------------------------------------*/
long double SG_Planet::calculateHydroFraction(long double volatile_gas_inventory)
{
	long double fraction = (0.71 * volatile_gas_inventory/1000.0) * pow2(EARTH_RADIUS_IN_KM/mRadius);

	if (fraction >= 1.0)
		return (1.0);
	else
		return (fraction);
}


/*--------------------------------------------------------------------------*/
/// This function return the fraction of the planet wich is covered with clouds.
/**
Given the surface temperature of a planet (in Kelvin), this function
returns the fraction of cloud cover available.	 This is Fogg's eq.23.
See Hart in "Icarus" (vol 33, pp23 - 39, 1978) for an explanation.
This equation is Hart's eq.3.
Slightly modified by using constants and relationships from
Glass's book "Introduction to Planetary Geology", p.46.
The 'CLOUD_COVERAGE_FACTOR' is the amount of surface area on Earth
covered by one Kg. of cloud.
@sa calculateSurfaceTemperature
*/
/*--------------------------------------------------------------------------*/
long double SG_Planet::calculateCloudFraction(long double smallest_MW_retained, long double hydro_fraction)
{
	long double water_vapor_in_kg, fraction, surf_area, hydro_mass;

	if (smallest_MW_retained > WATER_VAPOR)
		return(0.0);
	else
	{
		surf_area  = 4.0 * PI * pow2(mRadius);
		hydro_mass = hydro_fraction * surf_area * EARTH_WATER_MASS_PER_AREA;
		water_vapor_in_kg  = (0.00000001 * hydro_mass);
		water_vapor_in_kg *= exp(Q2_36 * (mSurf_temp - EARTH_AVERAGE_KELVIN));
		fraction = CLOUD_COVERAGE_FACTOR * water_vapor_in_kg / surf_area;
		if (fraction >= 1.0)
			return(1.0);
		else
			return(fraction);
	}
}


/*--------------------------------------------------------------------------*/
/// This function returns the fraction of the planet wich is covered with ice.
/**
Given the surface temperature of a planet (in Kelvin), this function
returns the fraction of the planet's surface covered by ice.  
- This is Fogg's eq.24. See Hart[24] in Icarus vol.33, p.28 for an explanation.
- Modify constant from 70 to 90 in order to bring it more in line with the 
fraction of the Earth's surface covered with ice, which is approximatly 1.6%.
@sa calculateSurfaceTemperature
*/
/*--------------------------------------------------------------------------*/
long double SG_Planet::calculateIceFraction(long double hydro_fraction)
{
	long double surf_temp;
	long double fraction;

	surf_temp = SG_Utils::min(328, mSurf_temp);

	fraction = pow(((328.0 - surf_temp) / 90.0), 5.0);

	if (fraction > (1.5 * hydro_fraction)) fraction = (1.5 * hydro_fraction);

	if (fraction >= 1.0)
		return(1.0);
	else
		return(fraction);
}


/*--------------------------------------------------------------------------*/
/// This function determine if the planet suffers from the Greenhouse effect.
/**	
The new definition is based on the inital surface temperature and what
state water is in. If it's too hot, the water will never condense out
of the atmosphere, rain down and form an ocean. The albedo used here
was chosen so that the boundary is about the same as the old method
Neither zone, nor r_greenhouse are used in this version
Note: If the orbital radius of the planet is too big, 99% of it's volatiles
are assumed to have been deposited in surface reservoirs,
(otherwise, it suffers from the greenhouse effect).	*/
/*--------------------------------------------------------------------------*/
bool SG_Planet::calculateGreenhouseEffect()
{
	long double	temp = mPrimaryStar->getEffectiveTemperature(m_a, GREENHOUSE_TRIGGER_ALBEDO);

	if (temp > FREEZING_POINT_OF_WATER)
		return(true);
	else
		return(false);
}


/*--------------------------------------------------------------------------*/
/// This function returns the rise in temperature produced by the greenhouse effect.
/**	
The Greenhouse effect is the warming trend on the surface, and on the lower
atmosphere of a planet, that occcurs when solar radiation is trapped, as emissions 
from the planet. 
Like in a greehouse, the atmosphere may keep the solar radiation, and rise the surface temperature.
- This is Fogg's eq.20, and is also Hart's eq.20 in his "Evolution of Earth's Atmosphere" article.
- Note: pow(x,0.25) was changed to pow(x,0.4) to match Venus.
- Examples: Earth = +30°   Venus = +500°

@return The temperature rise, due to the GreeHouse effect (units = Kelvin).
*/
/*--------------------------------------------------------------------------*/
long double SG_Planet::calculateGreehouseRiseTemp(long double optical_depth, long double effective_temp)
{
	long double convection_factor;
	long double temperature;

	convection_factor  = EARTH_CONVECTION_FACTOR;
	convection_factor *= pow(mSurf_pressure/EARTH_SURF_PRES_IN_MILLIBARS, 0.4);
	temperature        = effective_temp * convection_factor;
	temperature       *= (pow1_4(1.0 + 0.75*optical_depth) - 1.0);
	return (temperature);
}


/*--------------------------------------------------------------------------*/
/// This function returns the albedo of the planet.
/**
The Albedo is a mesure of the light and radiation reflected by a planet. 
- Examples : snow=90%   ocean=10%   planet Earth=30%  Moon=12%
- The cloud adjustment is the fraction of cloud cover obscuring each
of the three major components of albedo that lie below the clouds.
@param water_fraction The fraction of the surface covered with oceans [0..1]
@param cloud_fraction The fraction of the surface covered with clouds [0..1]
@param ice_fraction   The fraction of the surface covered with ice [0..1]
@return The albedo value is between 0..1 (representing 0%..100%)
@sa calculateSurfaceTemperature
*/
/*--------------------------------------------------------------------------*/
long double SG_Planet::calculateAlbedo(long double water_fraction, long double cloud_fraction, long double ice_fraction)
{
	long double rock_fraction, cloud_adjustment, components;
	long double cloud_part,	rock_part, water_part, ice_part;

	rock_fraction = 1.0 - water_fraction - ice_fraction;
	components = 0.0;
	if (water_fraction > 0.0)
		components = components + 1.0;
	if (ice_fraction > 0.0)
		components = components + 1.0;
	if (rock_fraction > 0.0)
		components = components + 1.0;

	cloud_adjustment = cloud_fraction / components;

	if (rock_fraction >= cloud_adjustment)
		rock_fraction = rock_fraction - cloud_adjustment;
	else
		rock_fraction = 0.0;

	if (water_fraction > cloud_adjustment)
		water_fraction = water_fraction - cloud_adjustment;
	else
		water_fraction = 0.0;

	if (ice_fraction > cloud_adjustment)
		ice_fraction = ice_fraction - cloud_adjustment;
	else
		ice_fraction = 0.0;

	cloud_part = cloud_fraction * CLOUD_ALBEDO;		/* about(...,0.2); */

	if (mSurf_pressure == 0.0)
	{
		rock_part = rock_fraction * ROCKY_AIRLESS_ALBEDO; /* about(...,0.3); */
		ice_part  = ice_fraction  * AIRLESS_ICE_ALBEDO;   /* about(...,0.4); */
		water_part = 0;
	}
	else 
	{
		rock_part = rock_fraction  * ROCKY_ALBEDO; /* about(...,0.1); */
		water_part= water_fraction * WATER_ALBEDO; /* about(...,0.2); */
		ice_part  = ice_fraction   * ICE_ALBEDO;   /* about(...,0.1); */
	}

	return (cloud_part + rock_part + water_part + ice_part);
}


/*--------------------------------------------------------------------------*/
///	 This function returns the optical depth of the atmosphere.
/** 
The optical depth increase when the atmosphere is composed with light elements
and when the atmosphere is thin (low pressure).
The optical depth is useful in determining the amount of greenhouse effect 
on a planet.
@param molecular_weight The atmosphere melecule weight (unit = gram)
@param pressure   The atmosphere pressure at the surface of the planet (unit=mB)
@return The Optical Depth (unitless)
*/
/*--------------------------------------------------------------------------*/
long double SG_Planet::calculateOpacity(long double molecular_weight, long double pressure)
{
	long double optical_depth   = 0;
	long double pressure_in_atm = pressure/EARTH_SURF_PRES_IN_MILLIBARS;

	if (molecular_weight >= 45) 
		optical_depth = optical_depth + 0.05;
	else if (molecular_weight >= 30)
		optical_depth = optical_depth + 0.15;
	else if (molecular_weight >= 20)
		optical_depth = optical_depth + 1.00;
	else if (molecular_weight >= 10)
		optical_depth = optical_depth + 2.34;
	else if (molecular_weight >  0)
		optical_depth = optical_depth + 3.00;

	if (pressure_in_atm >= 70)
		optical_depth = optical_depth * 8.333;
	else if (pressure_in_atm >= 50)
		optical_depth = optical_depth * 6.666;
	else if (pressure_in_atm >= 30)
		optical_depth = optical_depth * 3.333;
	else if (pressure_in_atm >= 10)
		optical_depth = optical_depth * 2.0;
	else if (pressure_in_atm >= 5)
		optical_depth = optical_depth * 1.5;

	return(optical_depth);
}


/*--------------------------------------------------------------------------*/
/// This function calculates the time it takes for 1/e of a gas to escape from a planet's atmosphere.
/**	
Taken from Dole p.34. He cites Jeans (1916) & Jones (1923)
@param molecular_weight
@return Gas life time (unit = years)
*/
/*--------------------------------------------------------------------------*/
long double SG_Planet::getGasLife(long double molecular_weight)
{
	long double v    = this->getRMSvelocity(molecular_weight);
	long double g    = mSurf_grav * EARTH_ACCELERATION;
	long double r    = mRadius * CM_PER_KM;
	long double t    = (pow3(v) / (2.0 * pow2(g) * r)) * exp((3.0 * g * r) / pow2(v));
	long double years= t / (SECONDS_PER_HOUR * 24.0 * EARTH_DAYS_IN_A_YEAR);

//	long double ve = mEsc_velocity;
//	long double k = 2;
//	long double t2 = ((k * pow3(v) * r) / pow4(ve)) * exp((3.0 * pow2(ve)) / (2.0 * pow2(v)));
//	long double years2 = t2 / (SECONDS_PER_HOUR * 24.0 * DAYS_IN_A_YEAR);
//	fprintf (stderr, "gas_life: %LGs, V ratio: %Lf\n", years, ve / v);

	if (years > 2.0E10) years = INCREDIBLY_LARGE_NUMBER;

	return years;
}


/*--------------------------------------------------------------------------*/
/// This function returns the weight of the lightest molecule kept by the planet.
/** 
@return The exact minimal molecule weight, on this planet. 
@sa estimateMinimalMoleculeWeight
*/
/*--------------------------------------------------------------------------*/
long double SG_Planet::calculateMinimalMoleculeWeight()
{
	long double target  = 5.0e9;
	long double guess_1 = this->estimateMinimalMoleculeWeight();
	long double guess_2 = guess_1;

	long double life_1 = this->getGasLife(guess_1);
	long double life_2 = life_1;

	// We initialize the age of the planetary system.
	if (mPrimaryStar) target = mPrimaryStar->mAge;

	// We estimate the weight of the molecule those lifetime is the closest 
	// of the age of the planetary system.
	int	loops = 0;
	// Convergence with a dychotomic method.
	if (life_1 > target)
	{
		// If this molecule's lifetime is longer that the age of the planet,
		// we try with a lighter molecule.
		// (light molecules live less)
		while ((life_1 > target) && (loops++ < 25))
		{
			guess_1 = guess_1 / 2.0;
			life_1  = this->getGasLife(guess_1);
		}
	}
	else
	{
		// If this molecule's lifetime is shorter that the age of the planet,
		// we try with a heavier molecule.
		// (heavy molecules live longer)
		while ((life_2 < target) && (loops++ < 25))
		{
			guess_2 = guess_2 * 2.0;
			life_2  = this->getGasLife(guess_2);
		}
	}

	// Interpolation between guess_1 and guess_2
	loops = 0;
	while (((guess_2 - guess_1) > 0.1) && (loops++ < 25))
	{
		long double guess_3 = (guess_1 + guess_2) / 2.0;
		long double life_3  = this->getGasLife(guess_3);

		if (life_3 < target)
			guess_1 = guess_3;
		else
			guess_2 = guess_3;
	}

	// We retrun the weitgh found.
	// life = this->getGasLife(guess_2);
	return (guess_2);
}


/*--------------------------------------------------------------------------*/
/// This function calculates the climatic conditions at the surface of the planet. 
/** This function estimate the ice, water, cloud fractions at the surface of the planet
and the minimal, average and maximal temperatures.
*/
/*--------------------------------------------------------------------------*/
void SG_Planet::calculateSurfaceClimate(
		long double last_water,
		long double last_clouds,
		long double last_ice,
		long double last_temp,
		long double last_albedo)
{
	long double effective_temp;
	long double water_raw;
	long double clouds_raw;
	long double opacity;
	long double greenhouse_temp;
	bool        boil_off = true;

	if (mGreenhouse_effect && (mMax_temp < mBoiling_point))
	{
		SG_Utils::writeLog("Deluge ");
		SG_Utils::writeLog(" Max Temp ("+ITOS(mMax_temp)+") < Boiling Point" );
		SG_Utils::writeLog(" Boiling Point ="+ITOS(mBoiling_point));
		mGreenhouse_effect = 0;
		mVolatile_gas_inventory = this->calculateVolatilGasRatio(
				mEsc_velocity, mPrimaryStar->mMass,
				mGreenhouse_effect, (mGas_mass/mMass) > 0.000001);

		mSurf_pressure  = this->calculatePressure(mRadius);
		mBoiling_point  = this->calculateBoilingPoint(mSurf_pressure);
	}

	mHydrosphere	= this->calculateHydroFraction(mVolatile_gas_inventory);
	mCloud_cover 	= this->calculateCloudFraction(mMolec_weight, mHydrosphere);
	mIce_cover   	= this->calculateIceFraction(mHydrosphere);
	water_raw     	= mHydrosphere;
	clouds_raw     	= mCloud_cover;

	if ((mGreenhouse_effect) && (mSurf_pressure > 0.0))
		mCloud_cover	= 1.0;

	if (mMax_temp >= mBoiling_point)
	{
		mHydrosphere	= 0;
		boil_off = true;
		if (mMolec_weight > WATER_VAPOR)
			mCloud_cover = 0.0;
		else
			mCloud_cover = 1.0;
	}

	if (mSurf_temp < (FREEZING_POINT_OF_WATER - 3.0))   mHydrosphere = 0.0;
	mAlbedo	        = this->calculateAlbedo(mHydrosphere, mCloud_cover, mIce_cover);
	effective_temp 	= mPrimaryStar->getEffectiveTemperature(m_a, mAlbedo);
	opacity         = this->calculateOpacity(mMolec_weight, mSurf_pressure);
	greenhouse_temp = this->calculateGreehouseRiseTemp(opacity, effective_temp);
	mSurf_temp   	= effective_temp + greenhouse_temp;

	if (!boil_off) mHydrosphere	= (mHydrosphere + (last_water * 2)) / 3;
	mCloud_cover    = (mCloud_cover + (last_clouds * 2)) / 3;
	mIce_cover      = (mIce_cover   + (last_ice * 2))    / 3;
	mAlbedo	        = (mAlbedo      + (last_albedo * 2)) / 3;
	mSurf_temp      = (mSurf_temp   + (last_temp * 2))   / 3;

	this->calculateTemperatureRange();
}



/*--------------------------------------------------------------------------*/
/// This function estimates the climate at the surface of the planet.
/*--------------------------------------------------------------------------*/
void SG_Planet::estimateSurfaceClimate()
{
	long double effective_temp;
	long double water_raw;
	long double clouds_raw;
	long double opacity;
	long double greenhouse_temp;
	bool        boil_off = true;

	mAlbedo         = EARTH_ALBEDO;
	effective_temp  = mPrimaryStar->getEffectiveTemperature(m_a, mAlbedo);
	opacity         = this->calculateOpacity(mMolec_weight, mSurf_pressure);
	greenhouse_temp = this->calculateGreehouseRiseTemp(opacity, effective_temp);
	mSurf_temp      = effective_temp + greenhouse_temp;
	this->calculateTemperatureRange();

	if (mGreenhouse_effect && (mMax_temp < mBoiling_point))
	{
		SG_Utils::writeLog("Deluge ");
		mGreenhouse_effect = 0;
		mVolatile_gas_inventory = this->calculateVolatilGasRatio(
				mEsc_velocity, mPrimaryStar->mMass,
				mGreenhouse_effect, (mGas_mass/mMass) > 0.000001);

		mSurf_pressure = this->calculatePressure(mRadius);
		mBoiling_point = this->calculateBoilingPoint(mSurf_pressure);
	}

	mHydrosphere = this->calculateHydroFraction(mVolatile_gas_inventory);
	mCloud_cover = this->calculateCloudFraction(mMolec_weight, mHydrosphere);
	mIce_cover   = this->calculateIceFraction(mHydrosphere);
	water_raw    = mHydrosphere;
	clouds_raw   = mCloud_cover;

	if ((mGreenhouse_effect) && (mSurf_pressure > 0.0))
		mCloud_cover = 1.0;

	if (mSurf_temp < (FREEZING_POINT_OF_WATER - 3.0))   mHydrosphere = 0.0;
	mAlbedo	        = this->calculateAlbedo(mHydrosphere, mCloud_cover, mIce_cover);
	effective_temp 	= mPrimaryStar->getEffectiveTemperature(m_a, mAlbedo);
	opacity         = this->calculateOpacity(mMolec_weight,mSurf_pressure);
	greenhouse_temp = this->calculateGreehouseRiseTemp(opacity, effective_temp);
	mSurf_temp   	= effective_temp + greenhouse_temp;
	this->calculateTemperatureRange();
}


/*--------------------------------------------------------------------------*/
/// This function calculates all the climatic conditions on the planet.
/**
This function calls calculateSurfaceClimate until converging to 1/4 degree.
@sa calculateSolidPlanet calculateSurfaceClimate
*/
/*--------------------------------------------------------------------------*/
void SG_Planet::calculateGlobalClimate()
{
	long double initial_temp = mPrimaryStar->getBodyTemperature(m_a, mAlbedo);

	/*
	long double h2_life  = this->getGasLife(MOL_HYDROGEN);
	long double h2o_life = this->getGasLife(WATER_VAPOR);
	long double n2_life  = this->getGasLife(MOL_NITROGEN);
	long double n_life   = this->getGasLife(ATOMIC_NITROGEN);

	SG_Utils::writeLog("Gas lifetime H2  = "+ITOS(h2_life )+" years");
	SG_Utils::writeLog("Gas lifetime H2O = "+ITOS(h2o_life)+" years");
	SG_Utils::writeLog("Gas lifetime N   = "+ITOS(n_life  )+" years");
	SG_Utils::writeLog("Gas lifetime N2  = "+ITOS(n2_life )+" years");
	*/

	this->estimateSurfaceClimate();

	for (int count = 0; count <= 25; count++)
	{
		long double last_water  = mHydrosphere;
		long double last_clouds = mCloud_cover;
		long double last_ice    = mIce_cover;
		long double last_temp   = mSurf_temp;
		long double last_albedo = mAlbedo;

		this->calculateSurfaceClimate(last_water, last_clouds, last_ice,last_temp, last_albedo);

		if (fabs(mSurf_temp - last_temp) < 0.25)
			break;
	}
	mGreenhouse_rise = mSurf_temp - initial_temp;

	SG_Utils::writeLog("Surface temperature = "+ITOS(mSurf_temp)); 
	SG_Utils::writeLog("GreenHouse rise temperature = "+ITOS(mGreenhouse_rise)); 
	if (mGreenhouse_effect) SG_Utils::writeLog("GreenHouse Effect = running");  
}


/*--------------------------------------------------------------------------*/
/// This function determine the min and max temperatures on the surface of the planet.
/*--------------------------------------------------------------------------*/
void SG_Planet::calculateTemperatureRange()
{
	long double pressmod = 1.0 / sqrt(1 + 20 * mSurf_pressure/1000.0);
	long double ppmod    = 1.0 / sqrt(10 + 5 * mSurf_pressure/1000.0);
	long double tiltmod  = fabs(cos(mAxial_tilt * PI/180) * pow2(1 + m_e));
	long double daymod   = 1.0 / (200.0/mDay + 1);
	long double mh       = pow(1 + daymod, pressmod);
	long double ml       = pow(1 - daymod, pressmod);
	long double hi       = mh * mSurf_temp;
	long double lo       = ml * mSurf_temp;
	long double sh       = hi + pow((100+hi) * tiltmod, sqrt(ppmod));
	long double wl       = lo - pow((150+lo) * tiltmod, sqrt(ppmod));
	long double max      = mSurf_temp + sqrt(mSurf_temp) * 10;
	long double min      = mSurf_temp / sqrt(mDay + 24);

	if (lo < min) lo = min;
	if (wl < 0)   wl = 0;

	mHigh_temp = soft(hi, max, min);
	mLow_temp  = soft(lo, max, min);
	mMax_temp  = soft(sh, max, min);
	mMin_temp  = soft(wl, max, min);
}


/*--------------------------------------------------------------------------*/
/// Function for 'soft limiting' the temperatures
/*--------------------------------------------------------------------------*/
long double SG_Planet::soft(long double v, long double max, long double min)
{
	long double dv  = v - min;
	long double dm  = max - min;
	long double x   = 2*(dv/dm)-1;
	long double lim = x / pow1_4(1 + pow4(x));
	long double soft= (lim+1)/2 * dm + min;
	return soft;
}


/*--------------------------------------------------------------------------*/
/// This function returns the orbital zone of the planet. 
/**
Zone 1: Zone near of the sun, luminous and warm (ie: Earth).
Zone 2: Medium zone.
Zone 3: Zone far from the sun. The sun looks like a star from there.
@param luminosity The lumionosity of the sun (ie: the primary star).
@param orbit_radius The radius of the orbit of the planet (units = UA)
@return The 'orbital zone' of the planet (1-2-3).
*/
/*--------------------------------------------------------------------------*/
int SG_Planet::getOrbitalZone(long double luminosity, long double orbit_radius)
{
	if (orbit_radius < (4.0 * sqrt(luminosity)))
		return(1);
	else if (orbit_radius < (15.0 * sqrt(luminosity)))
		return(2);
	else
		return(3);
}

/*--------------------------------------------------------------------------*/
/// This function returns the exospheric temperature of the planet.
/**
The exosphere is the outermost, least dense portion of the atmosphere.
In this area, the gas molecule velocity increase (especially H and He), and the 
molecules may escape from the atmosphere. For this reason, the temperature
also increase in the exosphere. For the Earth, the exospheric temperature
is about 1000 C.
@return The exospheric temperature of the planet (unit = Kelvin).
*/
/*--------------------------------------------------------------------------*/
long double SG_Planet::calculateExoTemperature()
{
	return (EARTH_EXOSPHERE_TEMP / pow2(m_a / mPrimaryStar->mR_ecosphere));
}


/*--------------------------------------------------------------------------*/
/// This function returns the period of the planet orbit (Kepler Law).
/**
@param star_mass    Mass of the primary star (unit = Solar Mass)
@return the orbit period of the planet (unit = Earth days)
*/
/*--------------------------------------------------------------------------*/
long double SG_Planet::calculateOrbitPeriod(long double star_mass)
{
	long double period_in_years;

	period_in_years = sqrt (pow3(m_a) / (mMass + star_mass));
	return (period_in_years * EARTH_DAYS_IN_A_YEAR);
}


/*--------------------------------------------------------------------------*/
/// This function returns the density of a sphere.
/**	
@param mass   The mass of the sphere (unit = solar masses).
@param radius The equatorial radius of the sphere (unit = km).  
@return The density of the sphere (unit = gram/cm3).
*/
/*--------------------------------------------------------------------------*/
long double SG_Planet::getSphereDensity(long double mass, long double radius)
{
	long double volume;

	mass   = mass * SOLAR_MASS_IN_GRAMS;
	radius = radius * CM_PER_KM;
	volume = (4.0/3.0) * PI * pow3(radius);
	return (mass / volume);
}


/*--------------------------------------------------------------------------*/
/// This function returns the radius of a sphere.
/**
@param mass    The mass of the sphere (units = solar masses)
@param density The density of the sphere (units = gram/cm3)
@return The radius of the sphere (units = km)
*/
/*--------------------------------------------------------------------------*/
long double SG_Planet::getSphereRadius(long double mass, long double density)
{
	long double volume, radius;

	mass   = mass * SOLAR_MASS_IN_GRAMS;
	volume = mass / density;
	radius = pow1_3( (3*volume) / (4*PI) ) / CM_PER_KM;
	return (radius);
}


/*--------------------------------------------------------------------------*/
/// This function returns the mass of a sphere.
/**
@param density The density of the sphere (units = gram/cm3)
@param radius  The radius of the sphere (units = km)
@return The mass of the sphere (units = solar masses)
*/
/*--------------------------------------------------------------------------*/
long double SG_Planet::getSphereMass(long double density, long double radius)
{
	long double mass, volume;

	radius = radius * CM_PER_KM;
	volume = (4.0/3.0) * PI * pow3(radius);
	mass   = volume * density / SOLAR_MASS_IN_GRAMS;
	return (mass);
}


/*--------------------------------------------------------------------------*/
/// This function returns the RMS velocity of a certain gas, under the planet high atmosphere temperature.
/**	
This is Fogg's eq.16.  
The molecular weight is used as the basis of the Root Mean Square (RMS) velocity 
of the molecule or atom.
@param molecular_weight The molecular weight of the gas (unit=gram)
@return The RMS velocity of the gas molecules (unit=cm/sec)
*/
/*--------------------------------------------------------------------------*/
long double SG_Planet::getRMSvelocity(long double molecular_weight)
{
	long double RT = 3.0 * MOLAR_GAS_CONST * mExospheric_temp;
	long double V = sqrt(RT / molecular_weight);
	return(V * CM_PER_METER);
}


/*--------------------------------------------------------------------------*/
/// This function returns the semi-major-axis of the orbit (unit = AU).
/*--------------------------------------------------------------------------*/
long double SG_Planet::getOrbit()
{
	return m_a;
}


/*--------------------------------------------------------------------------*/
/// This function returns the eccentricity of the orbit.
/*--------------------------------------------------------------------------*/
long double SG_Planet::getEccentricity()
{
	return m_e;
}


/*--------------------------------------------------------------------------*/
/// This function returns the mass of the planet (unit = solar masses)
/*--------------------------------------------------------------------------*/
long double SG_Planet::getMass()
{
	return mMass;
}


/*--------------------------------------------------------------------------*/
/// This function sets the semi-major-axis of the planet's orbit (unit=AU).
/*--------------------------------------------------------------------------*/
void SG_Planet::setOrbit(long double a)
{
	m_a = a;
}


/*--------------------------------------------------------------------------*/
/// This function sets the eccentricity of the planet's orbit.
/*--------------------------------------------------------------------------*/
void SG_Planet::setEccentricity(long double e)
{
	m_e = e;
}


/*--------------------------------------------------------------------------*/
///	 This function sets the axial inclination of the planet.
/** @param tilt Planet inclination (unit = degree) */
/*--------------------------------------------------------------------------*/
void SG_Planet::setAxialTilt(long double tilt)
{
	mAxial_tilt = tilt;
}

/*--------------------------------------------------------------------------*/
/// This function sets the total mass of the planet (dust+gas).
/** @param mass Planet mass (unit = solar mass) */
/*--------------------------------------------------------------------------*/
void SG_Planet::setMass(long double mass)
{
	mMass = mass;
}

/*--------------------------------------------------------------------------*/
/// This function increases the dust mass of the planet (dust, rock, and all solid elements).
/** @param mass Dust mass in the planet (unit = solar mass) */
/*--------------------------------------------------------------------------*/
void SG_Planet::addDustMass(long double mass)
{
	mDust_mass += mass;
}

/*--------------------------------------------------------------------------*/
/// This function increases the gas mass of the planet.
/** @param mass Gas mass in the planet (unit = solar mass) */
/*--------------------------------------------------------------------------*/
void SG_Planet::addGasMass(long double mass)
{
	mGas_mass += mass;
}

/*--------------------------------------------------------------------------*/
/// Set the planet as a gas giant planet.
/** @param giant TRUE if the planet is a gas giant planet */
/*--------------------------------------------------------------------------*/
void SG_Planet::setGasGiant(bool giant)
{
	mGas_giant = giant;
}


/*--------------------------------------------------------------------------*/
/// Calculate the presence of metal in the planet.
/** 
This function is based on the planet density and the molecules retained.
It can be improved.
 */
/*--------------------------------------------------------------------------*/
bool SG_Planet::calculateMetal()
{
	bool metal = false;
	if ((mDensity>4) && (mMolec_weight<250)) metal=true;
	return metal;
}

/*--------------------------------------------------------------------------*/
/// This function calculates the type of the planet.
/** 
@param GasPlanet Set to TRUE, if you want to indicate that the planet is a gas planet.
@return The type of the planet (see planet_type)
*/
/*--------------------------------------------------------------------------*/
SG_Planet::planet_type SG_Planet::calculateType(bool GasPlanet)
{

	// Sub-types of gas-planets
	if (GasPlanet && (mGas_mass/mMass)<0.20)  return tSubSubGasGiant;
	if (GasPlanet && (TOEM(mMass)<20))        return tSubGasGiant;
	if (GasPlanet) return tGasGiant; 

	bool has_atmosphere = (mSurf_pressure>1.0); // 1 mB

	// Sub-types of solid planets without atmosphere.
	if (!has_atmosphere && (TOEM(mMass)<ASTEROID_MASS_LIMIT)) return tAsteroids;
	if (!has_atmosphere ) return tRock; // Planets without atmosphere

	// Sub-types of solid planets with atmosphere.
	if (mHydrosphere > 0.95) return tWater;       // >95% water
	if (mIce_cover   > 0.95) return tIce;         // >95% ice
	if (mHydrosphere > 0.05) return tTerrestrial; // Terrestrial

	// Sub-types of solid planets with atmosphere and <5% water
	if (mSurf_temp > mBoiling_point)  return tVenusian;  // Hot = Venusian
	if ((mGas_mass/mMass) > 0.0001)   return tIce;       // Accreted gas, and no water = Ice World
	return tMartian;                                     // Dry = Martian

}


/*--------------------------------------------------------------------------*/
/// This function generate the planet.
/*--------------------------------------------------------------------------*/
void SG_Planet::calculatePlanet()
{
	mEarthlike = false;

	SG_Utils::writeLog("Planet No "+ITOA(mPlanet_no));
	SG_Utils::writeLog("Orbit    = "+ITOS(m_a) + "AU");

	if (mGas_giant)  this->calculateGasPlanet();
	if (!mGas_giant) this->calculateSolidPlanet();


	/************************************************************************/
	// Is this planet like Earth ?
	if ((mSurf_grav     >= 0.8  ) &&    // between 0.8 and 1.2 g
	    (mSurf_grav     <= 1.2  ) &&    // (earth = 1g)
	    (mSurf_temp     >= 285  ) &&    // between 12°C and 17°C
	    (mSurf_temp     <= 290  ) &&    // (earth = 14°C)
	    (mSurf_pressure >= 700  ) &&    // between 700mB and 1600 mB
	    (mSurf_pressure <= 1600 ) &&    // (earth = 1013mB)
	    (mIce_cover     <= 0.10 ) &&    // less than 10% ice
	    (mCloud_cover   >= 0.10 ) &&    // more than 10% clouds
	    (mHydrosphere   >= 0.25 ) &&    // more than 25% oceans
	    (mHydrosphere   <= 0.95 ) &&    // less than 95% oceans
	    (!mFaceLocked           )  &&    
	    (!mResonant_period      )  &&    
	    (mBreathe        == BREATHABLE)) // and, of course, breathable
	{
		mEarthlike = true ;
	}
	SG_Utils::writeLog("Earthlike  = "+ITOA(mEarthlike));
	SG_Utils::writeLog("===============================================");
}


/*--------------------------------------------------------------------------*/
/// Calculations for a gas planet.
/** 
Note that the calculation function must be called in this order, because 
they are using values calculated by the calls of previous functions.
- Gas planet have no accessible surface, or no surface at all. 
- We call 'surface temperature' the temperature at 1 bar.
- (Jupiter=165K - Saturn=135K - Neptune=75K - Uranus=70K)
- We call 'radius' the limit where the gas pressure is 0.1 bar.
*/
/*--------------------------------------------------------------------------*/
void SG_Planet::calculateGasPlanet()
{
	mVolatile_gas_inventory = INCREDIBLY_LARGE_NUMBER;
	mSurf_pressure          = INCREDIBLY_LARGE_NUMBER;
	mBoiling_point          = INCREDIBLY_LARGE_NUMBER;
	mSurf_temp              = INCREDIBLY_LARGE_NUMBER;
	mHydrosphere            = INCREDIBLY_LARGE_NUMBER;
	mSurf_grav              = INCREDIBLY_LARGE_NUMBER;
	mGreenhouse_effect      = false;
	mGreenhouse_rise        = 0;
	mAlbedo                 = SG_Utils::about(GAS_GIANT_ALBEDO,0.1);

	/************************************************************************/
	/* Calculation of physical characteristics                              */
	/************************************************************************/
	mOrbit_zone      = this->getOrbitalZone(mPrimaryStar->mLum, m_a);
	mOrbit_period    = this->calculateOrbitPeriod(mPrimaryStar->mMass);
	mAxial_tilt      = this->calculateInclination(m_a);
	mExospheric_temp = this->calculateExoTemperature();
	mCore_radius     = this->getKothariRadius(mDust_mass,true,mOrbit_zone);
	mDensity         = this->estimateDensity(m_a, mPrimaryStar->mR_ecosphere, true);
	mRadius          = this->getSphereRadius(mMass,mDensity);
	mSurf_grav       = this->calculateGravity(mRadius);
	mEsc_velocity    = this->calculateEscapeVelocity();
	mMolec_weight    = this->calculateMinimalMoleculeWeight();

	mType            = this->calculateType(true);
	mDay             = this->calculateDayLength();	
	mFaceLocked      = this->calculateFaceLocking();

	// The gravity of a gas planet is zero at its center, then increase until we reach
	// the limit of the planet (mRadius), and then decrease when we go away from the planet.
	mSurf_grav       = this->calculateGravity(mRadius);  // max gravity at the limit of the gas planet

	/************************************************************************/
	// Verify if the planet will really retain gas:
	// If mass>Earth, and contains more than 5% gas, and retains He, it is a gas giant.
	/************************************************************************/
	if (  (TOEM(mMass)   > 1         ) &&
	      (mGas_mass     > 0.05*mMass) &&
	      (mMolec_weight <= HELIUM)  )
	{
		mGas_planet = true;
	}
	else
	{
		// Finaly, the first idea was wrong: it is not a gas planet. We stop here.
		mGas_planet = false;
		return;
	}

	/********************************************************************/
	/* Calculation of the atmosphere's composition                      */
	/********************************************************************/
	mVolatile_gas_inventory = this->calculateVolatilGasRatio(mEsc_velocity,mPrimaryStar->mMass,false,true);
	mSurf_pressure          = this->calculatePressure(mRadius);

	// Estimation of the surface temperature of an earthlike planet located on this orbit.
	long double temp = mPrimaryStar->getBodyTemperature(m_a,  EARTH_ALBEDO); 
}



/*--------------------------------------------------------------------------*/
/// Calculations for a solid (non-gas) planet.
/** 
Note that the calculation function must be called in this order, because 
they are using values calculated by the calls of previous functions.
*/
/*--------------------------------------------------------------------------*/
void SG_Planet::calculateSolidPlanet()
{
	/************************************************************************/
	/* Calculation of physical characteristics                              */
	/************************************************************************/
	mOrbit_zone      = this->getOrbitalZone(mPrimaryStar->mLum, m_a);
	mOrbit_period    = this->calculateOrbitPeriod(mPrimaryStar->mMass);
	mAxial_tilt      = this->calculateInclination(m_a);
	mExospheric_temp = this->calculateExoTemperature();
	mCore_radius     = this->getKothariRadius(mDust_mass,false,mOrbit_zone);
	mDensity         = this->estimateDensity(m_a, mPrimaryStar->mR_ecosphere, true);

	mMolec_weight    = this->calculateMinimalMoleculeWeight(); // TODO: here?

	mRadius          = this->getKothariRadius(mMass,false,mOrbit_zone);
	mDensity         = this->getSphereDensity(mMass,mRadius);
	mSurf_grav       = this->calculateGravity(mRadius);

	// If there wher some gas at the birth of the planet,
	// we calculate the mass of gas remaining today.
	if ((mGas_mass / mMass) > 0.000001)
	{
		long double h2_mass = mGas_mass * 0.85;
		long double he_mass = (mGas_mass - h2_mass) * 0.999;
		long double h2_loss = 0.0;
		long double he_loss = 0.0;

		long double h2_life = this->getGasLife(MOL_HYDROGEN);
		long double he_life = this->getGasLife(HELIUM);

		// We remove from the planet mass, all the gases that have escaped
		// since the birth of the planet.
		if (h2_life < mPrimaryStar->mAge)
		{
			h2_loss     = ((1.0 - (1.0 / exp(mPrimaryStar->mAge/h2_life))) * h2_mass);
			mGas_mass  -= h2_loss;
			mMass      -= h2_loss;
			mSurf_grav  = this->calculateGravity(mRadius);
		}

		if (he_life < mPrimaryStar->mAge)
		{
			he_loss     = ((1.0 - (1.0 / exp(mPrimaryStar->mAge/he_life))) * he_mass);
			mGas_mass  -= he_loss;
			mMass      -= he_loss;
			mSurf_grav  = this->calculateGravity(mRadius);
		}

		SG_Utils::writeLog("Planet is losing gas:");
		SG_Utils::writeLog(" H2: "+ITOS(TOEM(h2_loss))+" EM lost");
		SG_Utils::writeLog(" He: "+ITOS(TOEM(he_loss))+" EM lost");
	}


	/************************************************************************/
	/************************************************************************/
	mDay                    = this->calculateDayLength();
	mEsc_velocity           = this->calculateEscapeVelocity();
	mFaceLocked             = this->calculateFaceLocking();

	mSurf_grav              = this->calculateGravity(mRadius);         // adjustment
	mMolec_weight           = this->calculateMinimalMoleculeWeight();  
	mGreenhouse_effect      = this->calculateGreenhouseEffect();       // adjustment
	mVolatile_gas_inventory = this->calculateVolatilGasRatio(
				mEsc_velocity,
				mPrimaryStar->mMass,
				mGreenhouse_effect,
				(mGas_mass/mMass) > 0.000001);
	mSurf_pressure = this->calculatePressure(mRadius);
	mBoiling_point = this->calculateBoilingPoint(mSurf_pressure);

	mMetal = this->calculateMetal();
	
	/********************************************************************/
	/* Calculation of the planet's climate                              */
	/********************************************************************/
	// This sets the following data:
	// Surface Temperature, Albedo, Hydrosphere, clouds, ice, greenhouse effect.
	this->calculateGlobalClimate();

	/********************************************************************/
	/* Calculation of the atmosphere's composition                      */
	/********************************************************************/
//removed by DDL to calculate all atmospheres: if ((mMax_temp >= FREEZING_POINT_OF_WATER) && (mMin_temp <= mBoiling_point))
		mAtmosphere->calculateAtmosphere();
	mBreathe   = mAtmosphere->getBreathability();
		
	/************************************************************************/
	/* Calculation of the planet's type                                     */
	/************************************************************************/
	mType = this->calculateType(mGas_planet);
	if (mType==tIce) mIce_cover = 1.0;

	SG_Utils::writeLog("Gravity    = "+ITOS(mSurf_grav)     + "g");
	SG_Utils::writeLog("Pressure   = "+ITOS(mSurf_pressure) + "mB");
}


/* ------------------------------------------------------------------------- */
/// This function changes the number of the planet.
/** At the begining of the process, the number is the creation order of the planets,
but during the sorting process, the number is set to the orbital order of the planet.
@param number The number is in the range [0..N]
*/
/* ------------------------------------------------------------------------- */
void SG_Planet::setPlanetNumber(int number)
{
	mPlanet_no = number;
}
