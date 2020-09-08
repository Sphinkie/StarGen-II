/* ------------------------------------------------------------------------- */
// File       : SG_Stardust.cpp
// Project    : Star Gen
// Author C++ : David de Lorenzo
// Author C   : Jim Burrows (see Credits.txt)
/* ------------------------------------------------------------------------- */

#include "SG_Stardust.h"
#include "SG_Utils.h"
#include "SG_const.h"

/* ------------------------------------------------------------------------- */
/// Constructor.
/** 
@param star       A reference to the star of the system.
@param max_planet The max number of planets authorized for this system (default=10).
/* ------------------------------------------------------------------------- */
SG_Stardust::SG_Stardust(SG_Star* star, int max_planet)
{
	mSun        = star;
	mDust_left  = true;
	mPlanetIndex= 0;
	mMaxPlanet  = max_planet;
	mCloudEccentricity = 0.2; 
	mDustDensityCoeff  = DUST_DENSITY_COEFF;

	mDust_inner_limit   = 0; 
	mDust_outer_limit   = mSun->getStellarDustLimit();
	mPlanet_inner_bound = mSun->getNearestPlanetOrbit();
	mPlanet_outer_bound = mSun->getFarthestPlanetOrbit();

	SG_Utils::writeLog("Dust inner limit  = "+ ITOS(mDust_inner_limit));
	SG_Utils::writeLog("Dust outer limit  = "+ ITOS(mDust_outer_limit));
	SG_Utils::writeLog("Planet inner limit= "+ ITOS(mPlanet_inner_bound));
	SG_Utils::writeLog("Planet outer limit= "+ ITOS(mPlanet_outer_bound));
	SG_Utils::writeLog(" ");

	// Initialisation of the planet array.
	mPlanetList = new SG_Planet*[mMaxPlanet];
	for (int index=0; index<mMaxPlanet; index++) mPlanetList[index]=NULL;

	// Initialisation of a large dustband surrounding the star.
	mDust_head = new dust_band;
	mDust_head->inner_edge  = mDust_inner_limit;
	mDust_head->outer_edge  = mDust_outer_limit;
	mDust_head->dust_present= true;
	mDust_head->gas_present = true;
	mDust_head->next_band   = NULL;

}


/* ------------------------------------------------------------------------- */
/// Destructor
/* ------------------------------------------------------------------------- */
SG_Stardust::~SG_Stardust()
{
	// Delete all stargen-planets
	for (int index=0; index<mMaxPlanet; index++) 
	{
		if (mPlanetList[index]) delete(mPlanetList[index]);
	}

	// Delete all stargen-dustbands
	dust_band* next;
	for(dust_band* band=mDust_head; band; band=next)
	{
		next = band->next_band;
		delete(band);
	}

	// Delete the  planet's array
	delete (mPlanetList);
}



/* ------------------------------------------------------------------------- */
/// This function returns the inner orbit affected by this proto planet.
/** 
@param protoplanet The protoplanet.
@return The inner effect orbit for this protoplanet (unit = AU).
*/
/* ------------------------------------------------------------------------- */
long double SG_Stardust::getInnerEffectLimit(protoPlanet* protoplanet)
{
	long double a = protoplanet->orbit;
	long double e = protoplanet->eccnt;
	long double m = protoplanet->reducedmass;

	long double R = a * (1 - e) * (1 - m) / (1.0 + mCloudEccentricity);
	if (R<0) R=0;
	return (R);
}

/* ------------------------------------------------------------------------- */
/// This function returns the outer orbit affected by this proto planet.
/** 
@param protoplanet The protoplanet.
@return The outer effect orbit for this protoplanet (unit = AU).
*/
/* ------------------------------------------------------------------------- */
long double SG_Stardust::getOuterEffectLimit(protoPlanet* protoplanet)
{
	long double a = protoplanet->orbit;
	long double e = protoplanet->eccnt;
	long double m = protoplanet->reducedmass;

	long double R = a * (1 + e) * (1 + m) / (1.0 - mCloudEccentricity);
	if (R<0) R=0;
	return (R);
}


/* ------------------------------------------------------------------------- */
/// This function indicates if there is still some stellar dust in the specified orbital range.
/** 
@param inside_range   The inner orbit of the area (unit=UA)
@param outside_range  The outer orbit of the area (unit=UA)
@return TRUE if there is still some dust in this area.
*/
/* ------------------------------------------------------------------------- */
bool SG_Stardust::isDustAvailable(long double inside_range, long double outside_range)
{
	dust_band* current_dust_band;
	bool dust_here;

	current_dust_band = mDust_head;
	while ((current_dust_band != NULL) && (current_dust_band->outer_edge < inside_range))
		current_dust_band = current_dust_band->next_band;

	if (current_dust_band == NULL)
		dust_here = false;
	else 
		dust_here = current_dust_band->dust_present;

	while ((current_dust_band != NULL) && (current_dust_band->inner_edge < outside_range)) 
	{
		dust_here         = dust_here || current_dust_band->dust_present;
		current_dust_band = current_dust_band->next_band;
	}
	return(dust_here);
}


/* ------------------------------------------------------------------------- */
/// This function updates the dustbands surrounding the star.
/** 
When a protoplanet passes thru the stellar dust, it creates a new band and takes dust and gas from it.
@param protoplanet The protoplanet. The protoplanet has a band limited by its innerEffect and outerEffect, called the "protoplanet'band".
*/
/* ------------------------------------------------------------------------- */
void SG_Stardust::updateDustLanes(protoPlanet* protoplanet)
{
	bool       gasLeft;
	dust_band* node1;
	dust_band* node2;
	dust_band* node3;

	// if the protoplanet mass is higher than the critical mass
	// the gas are collected too. (gasLeft = false)
	gasLeft= (protoplanet->mass < protoplanet->criticalmass);	
	mDust_left = false;

	// we check all the existing dustbands
	node1 = mDust_head;
	while ((node1 != NULL))
	{
//	 SG_Utils::writeLog(" before "+ITOS(node1->inner_edge)+" to "+ITOS(node1->outer_edge));
		// if the protoplanet'band is inside a band, we cut it in 3 bands
		if ((node1->inner_edge < protoplanet->innereffect) && 
			(node1->outer_edge > protoplanet->outereffect)  )
		{
			// Creation on a new dustband on the protoplanet path
			node2 = new dust_band;
			node2->inner_edge = protoplanet->innereffect;
			node2->outer_edge = protoplanet->outereffect;
			if (node1->gas_present)
				node2->gas_present = gasLeft;
			else
				node2->gas_present = false;
			node2->dust_present = false;

			// Creation on a new dustband after the protoplanet path
			node3 = new dust_band;
			node3->inner_edge  = protoplanet->outereffect;
			node3->outer_edge  = node1->outer_edge;
			node3->gas_present = node1->gas_present;
			node3->dust_present= node1->dust_present;

			// Adjust the existing band (before the protoplanet path)
			node1->outer_edge = protoplanet->innereffect;

//	SG_Utils::writeLog(" node1 "+ITOS(node1->inner_edge)+" to "+ITOS(node1->outer_edge));
//	SG_Utils::writeLog(" node2 "+ITOS(node2->inner_edge)+" to "+ITOS(node2->outer_edge));
//	SG_Utils::writeLog(" node3 "+ITOS(node3->inner_edge)+" to "+ITOS(node3->outer_edge));
			// we link the 3 dustbands together
			node3->next_band  = node1->next_band;
			node1->next_band  = node2;
			node2->next_band  = node3;

			// Next dustband to analyse
			node1 = node3->next_band;
		}

		// if the protoplanet's band is the innerpart of the band, we cut it in 2 bands.
		else if ((node1->inner_edge < protoplanet->outereffect) && 
			     (node1->outer_edge > protoplanet->outereffect)  )
		{
			// Creation on a new dustband after the protoplanet path
			node2 = new dust_band;
			node2->next_band   = node1->next_band;
			node2->dust_present= node1->dust_present;
			node2->gas_present = node1->gas_present;
			node2->outer_edge  = node1->outer_edge;
			node2->inner_edge  = protoplanet->outereffect;

			// Adjust the existing band (on the protoplanet path)
			node1->outer_edge   = protoplanet->outereffect;
			node1->dust_present = false;
			if (node1->gas_present)
				node1->gas_present = gasLeft;
			else 
				node1->gas_present = false;

			// we link the 2 dustbands together
			node1->next_band = node2;
			// Next dustband to analyse
			node1 = node2->next_band;
		}

		// if the protoplanet's band is the outerpart of the band, we cut it in 2 bands.
		else  if ((node1->inner_edge < protoplanet->innereffect) && 
			      (node1->outer_edge > protoplanet->innereffect)  )
		{
			// Creation on a new dustband on the protoplanet path
			node2 = new dust_band;
			node2->next_band    = node1->next_band;
			node2->dust_present = false;
			if (node1->gas_present)
				node2->gas_present = gasLeft;
			else 
				node2->gas_present = false;
			node2->outer_edge = node1->outer_edge;
			node2->inner_edge = protoplanet->innereffect;

			// Adjust the existing band (before the protoplanet path)
			node1->outer_edge = protoplanet->innereffect;

			// we link the 2 dustbands together
			node1->next_band  = node2;
			// Next dustband to analyse
			node1 = node2->next_band;
		}

		// If the protoplanet'band is larger than the band: we dont add any band
		else if ((node1->inner_edge >= protoplanet->innereffect) && 
			     (node1->outer_edge <= protoplanet->outereffect)  )
		{
			// Adjust the existing band (on the protoplanet path)
			if (node1->gas_present)
				node1->gas_present = gasLeft;
			node1->dust_present = false;
			// Next dustband to analyse
			node1 = node1->next_band;
		}

		// If the protoplanet'band doesnt cross the band: we do nothing
		else if ((node1->outer_edge < protoplanet->innereffect) || 
			     (node1->inner_edge > protoplanet->outereffect)  )
			// Next dustband to analyse
			node1 = node1->next_band;
	}

	
	// We merge the bands that have the same characteristics (presence of gas and/or dust)
	dust_band* currBand = mDust_head;
	dust_band* nextBand = NULL;
	while ((currBand != NULL))
	{
//	SG_Utils::writeLog(" planetbound  "+ITOS(mPlanet_inner_bound)+" to "+ITOS(mPlanet_outer_bound));
//	SG_Utils::writeLog(" current band "+ITOS(currBand->inner_edge)+" to "+ITOS(currBand->outer_edge));
//	SG_Utils::writeLog(" current band dust_present="+ITOA(currBand->dust_present));
		if ((currBand->dust_present) && 
		    (currBand->outer_edge >= mPlanet_inner_bound) && 
		    (currBand->inner_edge <= mPlanet_outer_bound))
 			mDust_left = true;
		nextBand = currBand->next_band;
		if (nextBand)
		{
			if ((currBand->dust_present == nextBand->dust_present) &&
			    (currBand->gas_present  == nextBand->gas_present )  )
			{
				currBand->outer_edge = nextBand->outer_edge;
				currBand->next_band  = nextBand->next_band;
				delete(nextBand);
			}
		}
		currBand = currBand->next_band;
	}
}

/* ------------------------------------------------------------------------- */
/// This function collects Dust from all the existing dustbands, starting at the given dustband.
/** 
@param protoplanet The protoplanet that passes thru the dustbands, and grows.
@param dustband    The first dustband of the list.
*/
/* ------------------------------------------------------------------------- */
void SG_Stardust::collectDust(protoPlanet* protoplanet, dust_band*  dustband)
{
	long double	mass_density= 0.0;
	long double	gas_density = 0.0;
	long double	dust_density= 0.0;
	long double	temp1;
	long double	temp2;
	long double	bandwidth;
	long double	width;
	long double	volume;
			
	
	if (!dustband) 
	{
		// We have reached the last dustband
		// We integrate the mass collected in all the bands.
		protoplanet->mass = protoplanet->collectedmass;
		protoplanet->reducedmass = pow1_4(protoplanet->mass / (1 + protoplanet->mass));
		return;
	}

	long double Inner_effect = this->getInnerEffectLimit(protoplanet);
	long double Outer_effect = this->getOuterEffectLimit(protoplanet);
	
	SG_Utils::writeLog(" Inner_effect ="+ITOS(Inner_effect) + " AU");
	SG_Utils::writeLog(" Outer_effect ="+ITOS(Outer_effect) + " AU");
	
	if ((Inner_effect<dustband->outer_edge) && (dustband->inner_edge<Outer_effect))
	{
		// We calculate the density of gas and dust in this band
		if (dustband->dust_present)   dust_density = mDustDensity;
				
		if ((protoplanet->mass < protoplanet->criticalmass) || (!dustband->gas_present))
		{
			mass_density = dust_density;
			gas_density  = 0;
		}
		else
		{
			long double mass_ratio = sqrt(protoplanet->criticalmass / protoplanet->mass);
			mass_density = K * dust_density / (1 + mass_ratio * (K - 1));
			gas_density  = mass_density - dust_density;
		}

		// We evaluate the volume of dust and gas in this band.
		// if the protoplanet is inside this dustband
		bandwidth = Outer_effect - Inner_effect;
		temp1     = Outer_effect - dustband->outer_edge;
		temp2     = dustband->inner_edge - Inner_effect;
		if (temp1<0) temp1=0;	
		if (temp2<0) temp2=0;
		width  = bandwidth - temp1 - temp2;
		volume = 4 * PI * pow2(protoplanet->orbit) * (1.0 - protoplanet->eccnt*(temp1-temp2)/bandwidth);
		volume *= protoplanet->reducedmass * width;
		SG_Utils::writeLog("  mass_density ="+ITOS(mass_density));
		SG_Utils::writeLog("  gas_density  ="+ITOS(gas_density));
		SG_Utils::writeLog("  dust_density ="+ITOS(dust_density));
		SG_Utils::writeLog(" volume        ="+ITOS(volume));
		SG_Utils::writeLog(" collected new_mass ="+ITOS(volume * mass_density));

		// The protoplanet collects the dust and gas of this band
		protoplanet->collectedmass += volume * mass_density;
		protoplanet->gasmass       += volume * gas_density;
		protoplanet->dustmass      += volume * dust_density;
	}

	// Then, try to collect dust in the next dustband.
	this->collectDust(protoplanet, dustband->next_band);
}


/*--------------------------------------------------------------------------*/
/// This function returns the mass at which a planet at this orbit will begin to accrete gas and dust.
/**	 
@param protoplanet The protoplanet (orbit and eccentricity)
@return The critical mass of the protoplanet (unit = solar masses).
*/
/*--------------------------------------------------------------------------*/
long double SG_Stardust::getCriticalMass(protoPlanet* protoplanet)
{
	long double	perihelion_dist = protoplanet->orbit * (1 - protoplanet->eccnt);
	long double	temp            = perihelion_dist * sqrt(mSun->mLum);
	return (B * pow(temp,-0.75));
}



/* ------------------------------------------------------------------------- */
/// This function accrete the dust located on the protoplanet orbital path.
/** @param protoplanet The protoplanet. */
/* ------------------------------------------------------------------------- */
void SG_Stardust::accreteDust(protoPlanet* protoplanet)
{
	long double	previous_collectedmass;

	// We collect dust from the dustbands, as long as the mass of the protoplanet 
	// increase (more of 0.01%)
	do
	{
		// We do severall pass, because the protoplanet grows, 
		// and its attraction field grows too.
		previous_collectedmass     = protoplanet->collectedmass;  
		protoplanet->collectedmass = 0;  
		protoplanet->dustmass = 0;  
		protoplanet->gasmass  = 0;  
//		SG_Utils::writeLog("   accreteDust: starting a pass with mass "+ITOS(protoplanet->mass));
		this->collectDust(protoplanet, mDust_head);
	}
	while ( protoplanet->collectedmass > (1.0001 * previous_collectedmass) );

	// We updates the dustlanes crossed by the protoplanet
	this->updateDustLanes(protoplanet);
	
}



/* ------------------------------------------------------------------------- */
/// This function aggregates the protoplanet with the other bodies from the close orbits, and eventually create a planet.
/* ------------------------------------------------------------------------- */
void SG_Stardust::coalesceProtoplanets(protoPlanet* protoplanet)
{
	long double  diff;
	long double  dist1;
	long double  dist2;
	bool         finished   = false;
	SG_Planet*   the_planet;

	SG_Utils::writeLog(">> SG_Stardust::coalesceProtoplanets");

	// First, we try to find an existing planet with an over-lapping orbit.
	// We check all the existing planets.
	for (int index=0; index<mMaxPlanet; index++)
	{
		the_planet = mPlanetList[index];
		if (!the_planet) break;
		SG_Utils::writeLog("   checking collision with planet "+ITOA(index));
		
		long double PlanetMass  = the_planet->getMass();  // Unit = solar mass
		long double PlanetOrbit = the_planet->getOrbit(); // Major semi-axis
		long double PlanetEccnt = the_planet->getEccentricity();
		long double PlanetReducedMass = pow1_4(PlanetMass / (1+PlanetMass));

		// Just to make the equations easier to read:
		long double ProtoMass  = protoplanet->mass;
		long double ProtoOrbit = protoplanet->orbit;
		long double ProtoEccnt = protoplanet->eccnt;
		long double ProtoReducedMass = protoplanet->reducedmass;

		SG_Utils::writeLog(" PlanetReducedMass ="+ITOS(PlanetReducedMass));
		SG_Utils::writeLog(" Proto ReducedMass ="+ITOS(ProtoReducedMass));

		diff = PlanetOrbit - ProtoOrbit;

		if (diff > 0)
		{	// x aphelion
			dist1 =   ProtoOrbit  * ((1+ProtoEccnt)  * (1+ProtoReducedMass)  -1);
			dist2 = - PlanetOrbit * ((1-PlanetEccnt) * (1-PlanetReducedMass) -1);
		}
		else
		{	// x perihelion 
			dist1 = -ProtoOrbit * ((1-ProtoEccnt)  * (1-ProtoReducedMass)  -1);
			dist2 = PlanetOrbit * ((1+PlanetEccnt) * (1+PlanetReducedMass) -1);
		}
	
		// If the planet crosses this orbit, we have a collision
		if ((fabs(diff) <= fabs(dist1)) || (fabs(diff) <= fabs(dist2)))
		{
			// Calculation of the new orbit of the planet
			long double new_a = (PlanetMass+ProtoMass) / ((PlanetMass/PlanetOrbit) + (ProtoMass/ProtoOrbit));

			// Calculation of the new eccentricity of the planet orbit.
			long double new_e = PlanetMass * sqrt(PlanetOrbit) * sqrt(1 - pow2(PlanetEccnt));
			new_e = new_e + (ProtoMass * sqrt(ProtoOrbit) * sqrt(sqrt(1 - pow2(ProtoEccnt))));
			new_e = new_e / ((PlanetMass + ProtoMass) * sqrt(new_a));
			new_e = 1 - pow2(new_e);
			if ((new_e<0) || (new_e>=1)) new_e = 0;
			new_e = sqrt(new_e);

			// Calculation of the planet new mass.
			long double new_mass = PlanetMass + protoplanet->mass;

			SG_Utils::writeLog("Collision between two protoplanets! ");
			SG_Utils::writeLog(" orbit proto ="+ITOS(ProtoOrbit)+" AU");
			SG_Utils::writeLog(" mass  proto ="+ITOS(TOEM(ProtoMass))+" EM");
			SG_Utils::writeLog(" orbit Planet="+ITOS(PlanetOrbit)+" AU");
			SG_Utils::writeLog(" mass  Planet="+ITOS(TOEM(PlanetMass))+" EM");
			SG_Utils::writeLog(" new orbit   ="+ITOS(new_a)+" AU");
			SG_Utils::writeLog(" new mass    ="+ITOS(TOEM(new_mass))+" EM");

			// Update the protoplanet
			protoplanet->orbit   = new_a;
			protoplanet->eccnt   = new_e;
			protoplanet->mass    = new_mass;
			// protoplanet dustmass and gasmass unchanged
			protoplanet->reducedmass = pow1_4(protoplanet->mass / (1 + protoplanet->mass));
			protoplanet->criticalmass= this->getCriticalMass(protoplanet);
			protoplanet->innereffect = this->getInnerEffectLimit(protoplanet);
			protoplanet->outereffect = this->getOuterEffectLimit(protoplanet);

			// We collect the dust another time for this new body 
			// (the aggregated planet+protoplanet)
			this->accreteDust(protoplanet);	

			// Update the planet
			the_planet->setOrbit(new_a);
			the_planet->setEccentricity(new_e);
			the_planet->setMass(protoplanet->mass);
			the_planet->addDustMass(protoplanet->dustmass);
			the_planet->addGasMass(protoplanet->gasmass);
			if (protoplanet->mass >= protoplanet->criticalmass) the_planet->setGasGiant(true);

			// And all is done for this planet.
			finished = true;
			break;
		}
	}

	// Planetesimals didn't collide. Make it a planet.
	if (!finished)
	{
		if (mPlanetIndex<mMaxPlanet)
		{
			// we create a planet and put it in the array.
			the_planet = new SG_Planet (mSun, mPlanetIndex, protoplanet->orbit, protoplanet->eccnt, protoplanet->mass);
			the_planet->setGasGiant(protoplanet->mass >= protoplanet->criticalmass);
			the_planet->addDustMass(protoplanet->dustmass);
			the_planet->addGasMass(protoplanet->gasmass);
			mPlanetList[mPlanetIndex] = the_planet;

			SG_Utils::writeLog("No collision found. Creating planet "+ITOA(mPlanetIndex));
			SG_Utils::writeLog(" orbit    ="+ITOS(protoplanet->orbit)+" AU");
			SG_Utils::writeLog(" mass     ="+ITOS(TOEM(protoplanet->mass))+" EM");
			SG_Utils::writeLog(" dust mass="+ITOS(TOEM(protoplanet->dustmass))+" EM");
			SG_Utils::writeLog(" gas mass ="+ITOS(TOEM(protoplanet->gasmass))+" EM");

		}
	}
	SG_Utils::writeLog("<< SG_Stardust::coalesceProtoplanets");
}


/* ------------------------------------------------------------------------- */
/// This function accrete the stellardust into several planets.
/* ------------------------------------------------------------------------- */
void SG_Stardust::generatePlanets()
{
	SG_Utils::writeLog(">> SG_Stardust::generatePlanets()");
	int BodeIndex = -1; // index start at -1 to have 1 random planet.

	// While there is still some stellar dust, and we don't have reached the max number of planets
	// we keep creating new planets.
	while ( (mDust_left) && (mPlanetIndex<mMaxPlanet))
	{
		SG_Utils::writeLog("mDust_left = "+ ITOA(mDust_left));
		protoPlanet* proto   = new protoPlanet;
		proto->orbit         = mSun->getBodePlanetOrbit(BodeIndex++);
		proto->eccnt         = SG_Utils::random_eccentricity();
		proto->mass          = PROTOPLANET_MASS;
		proto->dustmass      = PROTOPLANET_MASS;
		proto->gasmass       = 0;
		proto->collectedmass = 0;
		proto->reducedmass   = pow1_4(proto->mass / (1.0 + proto->mass));
		proto->criticalmass  = this->getCriticalMass(proto);
		proto->innereffect   = this->getInnerEffectLimit(proto);
		proto->outereffect   = this->getOuterEffectLimit(proto);
		SG_Utils::writeLog("Injecting protoplanet at "+ITOS(proto->orbit)+" AU");

		if (this->isDustAvailable(proto->innereffect,proto->outereffect))
		{
	
			mDustDensity = mDustDensityCoeff * sqrt(mSun->mMass);
			mDustDensity*= exp(-ALPHA * pow(proto->orbit,(1.0/N)));

			this->accreteDust(proto);

			if ((proto->mass != 0) && (proto->mass != PROTOPLANET_MASS))
			{
				this->coalesceProtoplanets(proto);
				mPlanetIndex++;
				SG_Utils::writeLog("...protoplanet coalesced.");
			}
			else
				SG_Utils::writeLog("...failed due to large neighbor.");
		}
		else
			SG_Utils::writeLog("...failed due to not enough dust available.");
		delete (proto);
		SG_Utils::writeLog("\n");
	}
	this->sortPlanetTable();
}



/* ------------------------------------------------------------------------- */
/// This function returns a planet (sorted by orbit radius).
/** 
@param index The index of the planet in the Planet Array (0..N).
@return The planet, or NULL if there is no planet with this index.
*/
/* ------------------------------------------------------------------------- */
SG_Planet* SG_Stardust::getPlanet(int index)
{
	if (index<mMaxPlanet) return (mPlanetList[index]);
	else return NULL;
}


/* ------------------------------------------------------------------------- */
/// This function returns the coefficient of density of the dust in the cloud.
/* ------------------------------------------------------------------------- */
long double SG_Stardust::getDustDensityCoeff()
{
	return mDustDensityCoeff;
}

/* ------------------------------------------------------------------------- */
/// This function changes the coefficient of density of the dust inthe cloud.
/** 
@param ratio The coefficient of density of dust is multiplied by this ratio.
*/
/* ------------------------------------------------------------------------- */
void SG_Stardust::setDustDensityRatio(long double ratio)
{
	mDustDensityCoeff *= ratio;
}

/*--------------------------------------------------------------------------*/
/// This function sorts the array of planets, following the increasing orbit radius.
/*--------------------------------------------------------------------------*/
void SG_Stardust::sortPlanetTable()
{
	long double pc;

	for (int i=1; i<mMaxPlanet; i++) 
	{
		if (!mPlanetList[i]) break;
		pc = mPlanetList[i]->getOrbit();
		for (int j=i; j>0; j--)   //Move lower values up one position
		{
			if (!mPlanetList[j])   break;
			if (!mPlanetList[j-1]) break;
			if (mPlanetList[j]->getOrbit() > mPlanetList[j-1]->getOrbit() ) break;
			this->swapPlanet(j-1,j);
		}
	}

}
/*--------------------------------------------------------------------------*/
/// This function swaps two planets in the planet array.
/*--------------------------------------------------------------------------*/
void SG_Stardust::swapPlanet(int index1, int index2)
{
	SG_Planet* planet1 = mPlanetList[index1];
	SG_Planet* planet2 = mPlanetList[index2];
	mPlanetList[index1] = planet2;
	mPlanetList[index2] = planet1;
	// Adjust the planet number:
	// First, the number is the creation order of the planet,but during the 
	// sorting process, it is set to the orbital order of the planet.
	planet2->setPlanetNumber(index1);
	planet1->setPlanetNumber(index2);
}
