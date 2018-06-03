#pragma once
#include <string>
#include <fstream>
#include "VymeraUdaje.h"
#include "structures/table/UnsortedSequenceTableNoUnique.h"
#include "Obec.h"
#include "structures/list/linked_list.h"

/// <summary> Trieda reprezentuje okres v SR. </summary>
class Okres
{
public:
	/// <summary> Konstruktor. </summary>
	Okres(std::ifstream* infile);

	/// <summary> Destruktor. </summary>
	~Okres();

	int dajId() { return id_; }
	int dajIdKraja() { return id_ - (id_ % 100); }
	std::string dajNazov() { return nazov_; }
	VymeraUdaje* dajVymeraUdaje() { return vymeraUdaje_; }

	/// <summary> Prida dane id_obce do zoznamu obci okresu a zaroven zvysi vymery okresu o vymeru danej obce. </summary>
	void pridajObec(Obec* obec);

	/// <summary> Vrati pozadovany udaj o vymere okresu v danom roku. </summary>
	long int dajVymeru(int rok, VymeraPolozka polozka);

	/// <summary> Vrati podiel vymery ornej pody ku vymere ovocnych sadov okresu v zadanom roku. Ak je vymera ovoc. sadov nulova, vrati NaN. </summary>
	double dajPodielOrnejPodyOvocnychSadov(int rok);

	/// <summary> Vrati podiel vymery lesnych pozemkov voci celkovej vymere okresu v zadanom roku. </summary>
	double dajPodielLesnychPozemkov(int rok);

	/// <summary> Vrati zmenu podielu vymery ovocnych sadov voci celkovej vymere okresu medzi dvoma rokmi. </summary>
	double dajZmenuPodieluOvocnychSadov(int rok1, int rok2);

	/// <summary> Vrati zmenu podielu vymery ornej pody voci celkovej vymere okresu medzi dvoma rokmi. </summary>
	double dajZmenuPodieluOrnejPody(int rok1, int rok2);

	/// <summary> Vrati zmenu podielu vymery lesnych pozemkov voci celkovej vymere okresu medzi dvoma rokmi. </summary>
	double dajZmenuPodieluLesnychPozemkov(int rok1, int rok2);

	/// <summary> Vrati zoznam obci v okrese, ktore maju aspon zadane percentualne mnozstvo lesov z celkovej vymery. </summary>
	structures::List<Obec*>* dajObceSVymerouLesa(int rok, double percentHodnota);

private:
	/// <summary> Unikatny identifikator okresu. </summary>
	int id_;

	/// <summary> Nazov okresu. </summary>
	std::string nazov_;

	/// <summary> Udaje o vymere obce. </summary>
	VymeraUdaje* vymeraUdaje_;

	/// <summary> Id obci ktore sa v okrese nachadzaju. </summary>
	structures::UnsortedSequenceTableNoUnique<int, Obec*>* obce_;

private:
	/// <summary> Nacita atributy (id, id_kraja a nazov) okresu zo suboru. </summary>
	void nacitajSa(std::ifstream* infile);
};


inline Okres::Okres(std::ifstream* infile)
	:id_(0), nazov_("null"), vymeraUdaje_(new VymeraUdaje()), obce_(new structures::UnsortedSequenceTableNoUnique<int, Obec*>())
{
	nacitajSa(infile);
}


inline Okres::~Okres()
{
	delete vymeraUdaje_;
	delete obce_;
}

inline void Okres::pridajObec(Obec* obec)
{
	obce_->insert(obec->dajId(), obec);
	
	//Zvysi vymeru v kazdom roku a pre kazdu polozku
	for (int rok = OBEC_UDAJE_ROK_ZACIATOK; rok < OBEC_UDAJE_ROK_ZACIATOK + OBEC_POCET_ZAZNAMENAVANYCH_ROKOV; ++rok)
	{
		for (int j = 0; j < OBEC_POCET_ZAZNAMENAVANYCH_UDAJOV; ++j)
		{
			vymeraUdaje_->zvysVymeru(rok, intToPolozka(j), obec->dajVymeru(rok, intToPolozka(j)));
		}
	}
}

inline long int Okres::dajVymeru(int rok, VymeraPolozka polozka)
{
	return vymeraUdaje_->dajVymeru(rok, polozka);
}

inline double Okres::dajPodielOrnejPodyOvocnychSadov(int rok)
{
	double ornaPoda = double(vymeraUdaje_->dajVymeru(rok, ORNA_PODA));
	double ovocneSady = double(vymeraUdaje_->dajVymeru(rok, OVOCNE_SADY));

	if (ovocneSady != 0)
		return (ornaPoda / ovocneSady) * 100;
	else
		return 0;
}

inline double Okres::dajPodielLesnychPozemkov(int rok)
{
	if (vymeraUdaje_->dajVymeru(rok, CELKOVA) != 0)
		return (double(vymeraUdaje_->dajVymeru(rok, LESNE_POZEMKY)) / double(vymeraUdaje_->dajVymeru(rok, CELKOVA))) * 100;
	else
		return 0;
}

inline double Okres::dajZmenuPodieluOvocnychSadov(int rok1, int rok2)
{
	double vymera1 = 0;
	if (vymeraUdaje_->dajVymeru(rok1, CELKOVA) != 0)
		vymera1 = double(vymeraUdaje_->dajVymeru(rok1, OVOCNE_SADY)) / double(vymeraUdaje_->dajVymeru(rok1, CELKOVA));

	double vymera2 = 0;
	if (vymeraUdaje_->dajVymeru(rok2, CELKOVA) != 0)
		vymera2 = double(vymeraUdaje_->dajVymeru(rok2, OVOCNE_SADY)) / double(vymeraUdaje_->dajVymeru(rok2, CELKOVA));

	return (vymera1 - vymera2) * 100;
}

inline double Okres::dajZmenuPodieluOrnejPody(int rok1, int rok2)
{
	double vymera1 = 0;
	if (vymeraUdaje_->dajVymeru(rok1, CELKOVA) != 0)
		vymera1 = double(vymeraUdaje_->dajVymeru(rok1, ORNA_PODA)) / double(vymeraUdaje_->dajVymeru(rok1, CELKOVA));

	double vymera2 = 0;
	if (vymeraUdaje_->dajVymeru(rok2, CELKOVA) != 0)
		vymera2 = double(vymeraUdaje_->dajVymeru(rok2, ORNA_PODA)) / double(vymeraUdaje_->dajVymeru(rok2, CELKOVA));

	return (vymera1 - vymera2) * 100;
}

inline double Okres::dajZmenuPodieluLesnychPozemkov(int rok1, int rok2)
{
	double vymera1 = 0;
	if (vymeraUdaje_->dajVymeru(rok1, CELKOVA) != 0)
		vymera1 = double(vymeraUdaje_->dajVymeru(rok1, LESNE_POZEMKY)) / double(vymeraUdaje_->dajVymeru(rok1, CELKOVA));

	double vymera2 = 0;
	if (vymeraUdaje_->dajVymeru(rok2, CELKOVA) != 0)
		vymera2 = double(vymeraUdaje_->dajVymeru(rok2, LESNE_POZEMKY)) / double(vymeraUdaje_->dajVymeru(rok2, CELKOVA));

	return (vymera1 - vymera2) * 100;
}

inline structures::List<Obec*>* Okres::dajObceSVymerouLesa(int rok, double percentHodnota)
{
	structures::LinkedList<Obec*>* obceSDanouVymerouLesa = new structures::LinkedList<Obec*>();
	for (structures::TableItem<int, Obec*>* const item : *obce_)
	{
		Obec* obec = item->accessData();
		double podiel = (double(obec->dajVymeru(rok, LESNE_POZEMKY)) / double(obec->dajVymeru(rok, CELKOVA))) * 100;
		if (podiel >= percentHodnota)
			obceSDanouVymerouLesa->add(obec);
	}

	return obceSDanouVymerouLesa;
}

inline void Okres::nacitajSa(std::ifstream* infile)
{
	*infile >> id_;
	std::string a;
	std::getline(*infile, a);
	std::getline(*infile, nazov_);
}
