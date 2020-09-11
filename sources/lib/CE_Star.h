/* ------------------------------------------------------------------------- */
// File       : CE_Star.h
// Project    : Celestia interface for Stargen 2
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _CATALOG_STAR_H_
#define _CATALOG_STAR_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include <string>

#define UNDEFINED -1

/* ------------------------------------------------------------------------- */
/// Une étoile du catalogue Celestia (données minimales connues).
/** 
Il faut stocker un minimum de données dans cette classe, car elle est 
reproduite à des milliers d'exemplaires. Les données sont donc condensées et 
on déduit les autres données par calcul.
@sa http://en.wikipedia.org/wiki/Stellar_classification
@sa http://en.wikipedia.org/wiki/White_dwarf
*/
/* ------------------------------------------------------------------------- */
class CE_Star
{
public:
    CE_Star(long Number, double Magn, short spectrum, short SubSpectrum, short YerkesType, float x=0, float y=0, float z=0);
	~CE_Star();
    void    setAbsoluteMagnitude(float Magnitude);
	void    setCatalogNumber(int Number);
	void    setSpectralType(short spectralType);
	int     getCatalogNumber();
    double  getAbsoluteMagnitude();
    double  getBolometricMagnitude();
    double  getMass();
    double  getTemperature();
    double  getAge();
    std::string  getSpectralType();
    std::string  getName();

	/// Spectral class of the star
    enum SpectralClass    
    {
		Spectral_O     = 0,  ///< very hot and luminous - violet-bleu
		Spectral_B     = 1,  ///< very luminous - bleu-blanc
		Spectral_A     = 2,  ///< common star - blanc-vert
		Spectral_F     = 3,  ///< main sequence star - vert-jaune
		Spectral_G     = 4,  ///< jaune (soleil)
		Spectral_K     = 5,  ///< cool star - jaune-orange
		Spectral_M     = 6,  ///< red dwarfs - rouge
		Spectral_R     = 7,  ///< Carbon star (now classified in class C)
		Spectral_S     = 8,  ///< Cooler Carbon star (carbon-monoxyd)
		Spectral_N     = 9,  ///< Carbon star (now classified in class C)
		Spectral_WC    = 10, ///< superluminous Wolf-Rayet stars
		Spectral_WN    = 11, ///< superluminous Wolf-Rayet stars
		Spectral_Unknown=12, ///< Undefined / Unknown
		Spectral_L     = 13, ///< Lithium core stars
		Spectral_T     = 14, ///< Cooler brown dwarf star (Methane)
		Spectral_C     = 15, ///< Carbon Star
		Spectral_DA    = 16, ///< white dwarf A (Hydrogen rich)
		Spectral_DB    = 17, ///< white dwarf B (Helium rich)
		Spectral_DO    = 19, ///< white dwarf O (Helium rich)
		Spectral_DC    = 18, ///< white dwarf C (continious spectrum)
		Spectral_DQ    = 20, ///< white dwarf Q (carbon rich)
		Spectral_DZ    = 21, ///< white dwarf Z (metal rich)
		Spectral_D     = 22, ///< white dwarf (generic)
		Spectral_DX    = 23, ///< white dwarf (unknow)
		Spectral_NS    = 24, ///< Neutrom Star (pulsar)
		Spectral_BH    = 25, ///< black hole
		Spectral_P     = 26, ///< Planetary Nebulae
		Spectral_Q     = 27, ///< Nova
    };

	/// Luminosity class of the star
	enum LuminosityClass
    { 
		Lum_Ia0      = 0,     ///< Ia-0 (hyper giant - extreme luminous)
	 	Lum_Ia       = 1,     ///< Ia   (super giant - most luminous)
		Lum_Ib       = 2,     ///< Ib   (super giant - less luminous)
		Lum_II       = 3,     ///< II   (bright giant)
		Lum_III      = 4,     ///< III  (normal giant)  
		Lum_IV       = 5,     ///< IV   (sub giant) 
		Lum_V        = 6,     ///< V    (main sequence star, dwarf)
		Lum_VI       = 7,     ///< VI   (sub dwarf) 
		Lum_Unknown  = 8,     ///< (Undefined / Unknown)
		Lum_VII      = 9,     ///< VII  (white dwarfs)
		Lum_Neutron  = 10,    ///< (neutron star)
		Lum_BlackHole= 11,    ///< (Black Hole)
    };


private:

    double      calculateAge();
    std::string readNameFromFile(long index);

    unsigned int    mCatalogNumber; ///< No de l'étoile dans le catalogue Celestia
    double          mAbsMag;        ///< Magnitude absolue
	SpectralClass   mSpectrum;      ///< Spectre de l'étoile O,B,A,F,G etc
    short           mSubSpectrum;   ///< Sous-spectre de l'étoile : 0..9
	LuminosityClass mType;          ///< Type de l'étoile (classification Yerkes)
    double          mAge;           ///< Age of the star (random, but following the rules)
    std::string     mName;          ///< Name of the star (from the starnames.dat file)
    float			mX,mY,mZ;       ///< Coordinates of the star (unit=al / Origin=Sun)

};

#endif 
