/* ------------------------------------------------------------------------- */
// File       : SG_Stardust.h
// Project    : Star Gen
// Author C++ : David de Lorenzo
// Author C   : Jim Burrows (see Credits.txt)
/* ------------------------------------------------------------------------- */

#ifndef _STARGEN_STARDUST_H_
#define _STARGEN_STARDUST_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "SG_Star.h"
#include "SG_Planet.h"

/* ------------------------------------------------------------------------- */
/// The cloud of dusts and particles, which is around the star. (Accrete)
/**
The dust and particles which are surrounding the star at its origine, will accrete to form planets
(and moons). This cloud of dust is first composed by one DustLane, which is quicky sliced
by the planets into several dustlanes. 

The schematic shows a star with its stellar-dust cloud. Inside the cloud, there is an area where 
the planets can appear (<i>inner</i> to <i>outer planet bound</i>).
A proto-planet is also represented, with its effect area, ie the area where dust will accrete 
(by gravitation law) to the proto-planet and make it grow.

<b>Bibliography:</b> Dole, Stephen H.  "Formation of Planetary Systems by Aggregation:
a Computer Simulation"	October 1969,  Rand Corporation Paper P-4226.
*/
/* ------------------------------------------------------------------------- */
class SG_Stardust
{
	public:
		SG_Stardust(SG_Star* star, int max_planet=10);
		~SG_Stardust();
		void setDustDensityRatio(long double ratio = 1);
		void generatePlanets();
        long double getDustDensityCoeff();
        SG_Planet* getPlanet(int index);

    private:

        struct dust_band
		{
			long double  inner_edge;
			long double  outer_edge;
			bool         dust_present;
			bool         gas_present;
			dust_band*   next_band;
		};
 

        struct protoPlanet
		{
			long double  orbit;         ///< Unit = AU
			long double  eccnt;         ///< Unit = 0..1
			long double  mass;          ///< Unit = Solar Mass
			long double  gasmass;       ///< Unit = Solar Mass
			long double  dustmass;      ///< Unit = Solar Mass
			long double  reducedmass;   ///< Unit = 0..1
			long double  criticalmass;  ///< Unit = Solar Mass
			long double  collectedmass; ///< Unit = Solar Mass
			long double  innereffect;   ///< Unit = AU
			long double  outereffect;   ///< Unit = AU
		};

		bool isDustAvailable(long double inside_range, long double outside_range);
		long double getInnerEffectLimit(protoPlanet* protoplanet);
		long double getOuterEffectLimit(protoPlanet* protoplanet);
		long double getCriticalMass(protoPlanet* protoplanet);

		void accreteDust(protoPlanet* protoplanet);
		void updateDustLanes(protoPlanet* protoplanet);
		void coalesceProtoplanets(protoPlanet* protoplanet);
		void collectDust(protoPlanet* protoplanet, dust_band* dustband);
		void sortPlanetTable();
		void swapPlanet(int index1, int index2);

		bool         mDust_left;            ///< TRUE if there is still dust in the cloud.
		long double  mDust_inner_limit;     ///< Inner limit of the stardust cloud.
		long double  mDust_outer_limit;     ///< Outer limit of the stardust cloud.
		long double  mPlanet_inner_bound;   ///< Inner limit of the planet formation area.
		long double  mPlanet_outer_bound;   ///< Outer limit of the planet formation area.
		long double  mDustDensity;          ///< Stellardust density in the cloud.
		long double  mDustDensityCoeff;     ///< Density multiplcator factor.
		long double  mCloudEccentricity;    ///< Stardust cloud eccentricity.
		dust_band*   mDust_head;            ///< List of dustlanes.
		SG_Star*     mSun;                  ///< The main star of this system.
		int          mMaxPlanet;            ///< The max number of planet in this system.
		SG_Planet**  mPlanetList;           ///< An Array of planet pointers.
		int          mPlanetIndex;          ///< Index for the planet array.
};

#endif
