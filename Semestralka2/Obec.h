#pragma once

#include <fstream>

#include "structures/array/array.h"
#include "constants.h"
#include "EkonomAktObyvatelstvo.h"
#include "VymeraUdaje.h"

/// <summary> Trieda reprezentuje obec a vsetky udaje o nej za dane roky. </summary>
class Obec
{
public:
	/// <summary> Konstruktor. </summary>
	Obec(std::ifstream * infile);

	/// <summary> Destruktor. </summary>
	~Obec();

	int dajId() { return id_; }
	std::string dajNazov() { return nazov_; }
	int dajIdOkresu() { return idOkresu_; }
	int dajIdKraja() { return idOkresu_ - (idOkresu_ % 100); }
	VymeraUdaje* dajVymeraUdaje() { return vymeraUdaje_; }

	/// <summary> Vrati prirastok zastavanych ploch voci celkovej vymere obce v danom roku. </summary>
	double dajPrirastokZastavanychPloch(int rok);

	/// <summary> Vrati zmenu podielu vymery ornej pody voci celkovej vymere obce medzi dvoma rokmi. </summary>
	double dajZmenuPodieluOrnejPody(int rok1, int rok2);

	/// <summary> Nacita zakladne udaje obce - id, nazov, idOkresu. </summary>
	void nacitajSa(std::ifstream* infile);

	/// <summary> Nacita udaje o vymere obce zo suboru. </summary>
	void nacitajVymeryObce(std::ifstream* infile);

	/// <summary> Nacita udaje o obyvatelstve obce zo suboru. </summary>
	void nacitajObyvatelstvoObce(std::ifstream* infile);

	/// <summary> Vrati pozadovany udaj o vymere v danom roku. </summary>
	long int dajVymeru(int rok, VymeraPolozka polozka);
	
	/// <summary> Vrati udaj o obyvatelstve daneho pohlavia. </summary>
	int dajObyvatelstvo(Pohlavie pohlavie);

private:
	/// <summary> Ciselne ID obce. </summary>
	int id_;

	/// <summary> Nazov obce. </summary>
	std::string nazov_;

	/// <summary> Id okresu v ktorom sa obec nachadza. </summary>
	int idOkresu_;

	/// <summary> Udaje o vymere obce. </summary>
	VymeraUdaje* vymeraUdaje_;

	/// <summary> Udaje o obyvatelstve obce. </summary>
	EkonomAktObyvatelstvo* obyvatelstvoUdaje_;
};

inline Obec::Obec(std::ifstream * infile)
	:id_(0), nazov_(""), idOkresu_(0),
vymeraUdaje_(new VymeraUdaje()),
obyvatelstvoUdaje_(new EkonomAktObyvatelstvo(0, 0))
{
	nacitajSa(infile);
}

inline Obec::~Obec()
{
	delete vymeraUdaje_;
	delete obyvatelstvoUdaje_;
}

inline double Obec::dajPrirastokZastavanychPloch(int rok)
{
	long int celkovaVymera = vymeraUdaje_->dajVymeru(rok, CELKOVA);
	long int zastavanePlochyVymera = vymeraUdaje_->dajVymeru(rok, ZASTAVANE_PLOCHY);
	double pomer = 0;

	if (celkovaVymera != 0)
		pomer = double(zastavanePlochyVymera) / double(celkovaVymera);

	if (rok > OBEC_UDAJE_ROK_ZACIATOK)
	{
		long int celkovaVymeraMinula = vymeraUdaje_->dajVymeru(rok - 1, CELKOVA);
		long int zastavanePlochyVymeraMinula = vymeraUdaje_->dajVymeru(rok - 1, ZASTAVANE_PLOCHY);
		double pomerMinuly = 0;

		if (celkovaVymeraMinula != 0)
			pomerMinuly = double(zastavanePlochyVymeraMinula) / double(celkovaVymeraMinula);

		// 100% ubytok
		if (pomer == 0 && pomerMinuly != 0)
			return -100;

		// 100% prirastok
		if (pomer != 0 && pomerMinuly == 0)
			return 100;

		// 0% prirastok
		if (pomer == 0 && pomerMinuly == 0)
			return 0;

		return ((pomer - pomerMinuly) / pomerMinuly) * 100;
	}
	else
		return 0;
}

inline double Obec::dajZmenuPodieluOrnejPody(int rok1, int rok2)
{
	double vymera1 = 0;
	if (vymeraUdaje_->dajVymeru(rok1, CELKOVA) != 0)
		vymera1 = double(vymeraUdaje_->dajVymeru(rok1, ORNA_PODA)) / double(vymeraUdaje_->dajVymeru(rok1, CELKOVA));
	
	double vymera2 = 0;
	if (vymeraUdaje_->dajVymeru(rok2, CELKOVA) != 0)
		vymera2 = double(vymeraUdaje_->dajVymeru(rok2, ORNA_PODA)) / double(vymeraUdaje_->dajVymeru(rok2, CELKOVA));
	
	return (vymera1 - vymera2) * 100;
}

inline void Obec::nacitajSa(std::ifstream* infile)
{
	*infile >> id_;

	std::string a;
	std::getline(*infile, a);
	std::getline(*infile, nazov_);

	*infile >> idOkresu_;
}

inline void Obec::nacitajVymeryObce(std::ifstream* infile)
{
	vymeraUdaje_->nacitajSa(infile);
}

inline void Obec::nacitajObyvatelstvoObce(std::ifstream* infile)
{
	int muzi = 0;
	int zeny = 0;

	for (int i = 0; i < OBEC_UDAJE_OBYVATELSTVO_POCET_VEK_KATEGORII; ++i)
	{
		int hodnota = 0;
		*infile >> hodnota;
		muzi += hodnota;

		char a;
		*infile >> a;

		*infile >> hodnota;
		zeny += hodnota;

		if (i < OBEC_UDAJE_OBYVATELSTVO_POCET_VEK_KATEGORII - 1)
		{
			*infile >> a;
		}
	}

	delete obyvatelstvoUdaje_;
	obyvatelstvoUdaje_ = new EkonomAktObyvatelstvo(muzi, zeny);
}

inline long int Obec::dajVymeru(int rok, VymeraPolozka polozka)
{
	return vymeraUdaje_->dajVymeru(rok, polozka);
}

inline int Obec::dajObyvatelstvo(Pohlavie pohlavie)
{
	return obyvatelstvoUdaje_->dajObyvatelstvo(pohlavie);
}
