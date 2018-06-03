#pragma once
#include "structures/array/array.h"
#include "constants.h"

/// <summary> Trieda reprezentuje udaje o vymere obce v jednotlivych rokoch. </summary>
class VymeraUdaje
{
public:
	VymeraUdaje();
	~VymeraUdaje();

	/// <summary> Vrati polozku vymery v danom roku. </summary>
	long int dajVymeru(int rok, VymeraPolozka polozka);

	/// <summary> Zvysi udaj o vymere o zadanu hodnotu. </summary>
	void zvysVymeru(int rok, VymeraPolozka polozka, long int hodnota);

	/// <summary> Nacita udaje o vymere zo suboru. </summary>
	void nacitajSa(std::ifstream* infile);

private:

	/// <summary> Udaje o vymere obce. </summary>
	structures::Array<structures::Array<long int> *>* udaje_;
};



inline VymeraUdaje::VymeraUdaje()
	:udaje_(new structures::Array<structures::Array<long int>*>(OBEC_POCET_ZAZNAMENAVANYCH_UDAJOV))
{
	for (int i = 0; i < OBEC_POCET_ZAZNAMENAVANYCH_UDAJOV; ++i)
	{
		structures::Array<long int>* pole = new structures::Array<long int>(OBEC_POCET_ZAZNAMENAVANYCH_ROKOV);
		(*udaje_)[i] = pole;
	}
}


inline VymeraUdaje::~VymeraUdaje()
{
	for (int i = 0; i < OBEC_POCET_ZAZNAMENAVANYCH_UDAJOV; ++i)
	{
		delete (*udaje_)[i];
	}
	delete udaje_;
}

inline long int VymeraUdaje::dajVymeru(int rok, VymeraPolozka polozka)
{
	return (*(*udaje_)[polozka])[rok - OBEC_UDAJE_ROK_ZACIATOK];
}

inline void VymeraUdaje::zvysVymeru(int rok, VymeraPolozka polozka, long int hodnota)
{
	(*(*udaje_)[polozka])[rok - OBEC_UDAJE_ROK_ZACIATOK] += hodnota;
}

inline void VymeraUdaje::nacitajSa(std::ifstream* infile)
{
	for (int i = 0; i < OBEC_POCET_ZAZNAMENAVANYCH_UDAJOV; ++i)
	{
		structures::Array<long int>* pole = (*udaje_)[i];
		for (int j = 0; j < OBEC_POCET_ZAZNAMENAVANYCH_ROKOV; ++j)
		{
			long int hodnota = 0;
			*infile >> hodnota;
			(*pole)[j] = hodnota;
		}
	}
}
