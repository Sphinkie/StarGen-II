/* ------------------------------------------------------------------------- */
// File       : SG_Utils.cpp
// Project    : Star Gen
// Author C++ : David de Lorenzo
// Author C   : Jim Burrows (see Credits.txt)
/* ------------------------------------------------------------------------- */

#include	<math.h>
#include	<fstream.h>
#include	<time.h>

#include	"SG_Utils.h"
#include	"SG_Const.h"


/*----------------------------------------------------------------------*/
/// This function returns a random real number between the specified inner and outer bounds.												*/
/*----------------------------------------------------------------------*/
long double SG_Utils::random_number(long double inner, long double outer)
{
	long double range;

	range = outer - inner;
	return((((long double)rand()) / (long double)(RAND_MAX)) * range + inner);
}


/*----------------------------------------------------------------------*/
/// This function returns a value within a certain variation of the	exact value given it in 'value'.									*/
/*----------------------------------------------------------------------*/
long double SG_Utils::about(long double value, long double variation)
{
	return(value + (value * random_number(-variation,variation)));
}

/*----------------------------------------------------------------------*/
///
/*----------------------------------------------------------------------*/
long double SG_Utils::random_eccentricity()
{
	long double	e;

	e = 1.0 - pow(random_number(0.0, 1.0), ECCENTRICITY_COEFF);

	if (e > .99)	// Note that this coresponds to a random
		e = .99;	// number less than 10E-26
					// It happens with GNU C for -S254 -W27
	return(e);
}


/*----------------------------------------------------------------------*/
/// This function write in a logFile nammed "stargen.log"
/*----------------------------------------------------------------------*/
void SG_Utils::writeLog(SG_String data, bool append)
{
using namespace std;

	fstream::openmode mode;

	// On ouvre le fichier en ecriture
	if (append)  mode = (fstream::out|fstream::app);
	else         mode = (fstream::out|fstream::trunc);

	ofstream outputfile("Stargen.log", mode);
	data += "\n";
	outputfile << data;	// On ecrit les données
	outputfile.close();
}


/*----------------------------------------------------------------------*/
/// Return the lower value
/*----------------------------------------------------------------------*/
long double SG_Utils::min(long double X, long double Y)
{
	if (X>Y) return Y; else return X;
}


/*----------------------------------------------------------------------*/
/// Return the higher value.
/*----------------------------------------------------------------------*/
long double SG_Utils::max(long double X, long double Y)
{
	if (X>Y) return X; else return Y;
}



/* ------------------------------------------------------------------------- */
/// Generation d'une seed aléatoire pour les nombres aléatoires
/* ------------------------------------------------------------------------- */
long SG_Utils::generateSeed()
{
	time_t  temp_time;
	unsigned seed = (unsigned)(time(&temp_time));
	srand(seed);
	return rand();
}
