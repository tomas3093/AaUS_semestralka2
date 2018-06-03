#pragma once
#include <string>
#include <fstream>
#include "Okres.h"
#include "structures/list/linked_list.h"
#include "structures/priority_queue/heap.h"
#include "Obec.h"
#include "structures/table/UnsortedSequenceTableNoUnique.h"
#include "structures/table/sorting/quick_sort.h"

/// <summary> Trieda reprezentuje kraj SR. </summary>
class Kraj
{
public:
	/// <summary> Konstruktor. </summary>
	Kraj(std::ifstream* infile);

	/// <summary> Destruktor. </summary>
	~Kraj();

	/// <summary> Vrati id kraja. </summary>
	int dajId() { return id_; }

	/// <summary> Vrati nazov kraja. </summary>
	std::string dajNazov() { return nazov_; }

	/// <summary> Prida dane id_okresu do zoznamu okresov kraja a zaroven zvysi vymery kraja o vymeru daneho okresu. </summary>
	void pridajOkres(Okres* okres);

	/// <summary> Vrati pozadovany udaj o vymere kraja v danom roku. </summary>
	long int dajVymeru(int rok, VymeraPolozka polozka);

	/// <summary> Vrati zoznam okresov v kraji, ktore maju aspon zadane percentualne mnozstvo lesov z celkovej vymery. </summary>
	structures::List<Okres*>* dajOkresySVymerouLesa(int rok, double percentHodnota);

	/// <summary> Vrati zoznam obci v kraji, ktore maju aspon zadane percentualne mnozstvo lesov z celkovej vymery. </summary>
	structures::List<Obec*>* dajObceSVymerouLesa(int rok, double percentHodnota);

private:
	/// <summary> Unikatny identifikator kraja. </summary>
	int id_;

	/// <summary> Nazov kraja. </summary>
	std::string nazov_;

	/// <summary> Udaje o vymere kraja. </summary>
	VymeraUdaje* vymeraUdaje_;

	/// <summary> Zoznam okresov ktore sa v kraji nachadzaju. </summary>
	structures::UnsortedSequenceTableNoUnique<int, Okres*>* okresy_;

private:
	/// <summary> Nacita atributy kraja zo suboru. </summary>
	void nacitajSa(std::ifstream* infile);
};



inline Kraj::Kraj(std::ifstream* infile)
	:id_(0), nazov_("null"), vymeraUdaje_(new VymeraUdaje()), okresy_(new structures::UnsortedSequenceTableNoUnique<int, Okres*>())
{
	nacitajSa(infile);
}


inline Kraj::~Kraj()
{
	delete vymeraUdaje_;
	delete okresy_;
}

inline void Kraj::pridajOkres(Okres* okres)
{
	okresy_->insert(okres->dajId(), okres);

	//Zvysi vymeru v kazdom roku a pre kazdu polozku
	for (int rok = OBEC_UDAJE_ROK_ZACIATOK; rok < OBEC_UDAJE_ROK_ZACIATOK + OBEC_POCET_ZAZNAMENAVANYCH_ROKOV; ++rok)
	{
		for (int j = 0; j < OBEC_POCET_ZAZNAMENAVANYCH_UDAJOV; ++j)
		{
			vymeraUdaje_->zvysVymeru(rok, intToPolozka(j), okres->dajVymeru(rok, intToPolozka(j)));
		}
	}
}

inline long int Kraj::dajVymeru(int rok, VymeraPolozka polozka)
{
	return vymeraUdaje_->dajVymeru(rok, polozka);
}

inline structures::List<Okres*>* Kraj::dajOkresySVymerouLesa(int rok, double percentHodnota)
{
	structures::LinkedList<Okres*>* okresySDanouVymerouLesa = new structures::LinkedList<Okres*>();
	for (structures::TableItem<int, Okres*>* const item : *okresy_)
	{
		Okres* okres = item->accessData();
		double podiel = okres->dajPodielLesnychPozemkov(rok);
		if (podiel >= percentHodnota)
			okresySDanouVymerouLesa->add(okres);
	}

	return okresySDanouVymerouLesa;
}

inline structures::List<Obec*>* Kraj::dajObceSVymerouLesa(int rok, double percentHodnota)
{
	structures::LinkedList<Obec*>* obceSDanouVymerouLesa = new structures::LinkedList<Obec*>();
	for (structures::TableItem<int, Okres*>* const item : *okresy_)
	{
		structures::List<Obec*>* obceOkresu = item->accessData()->dajObceSVymerouLesa(rok, percentHodnota);
		for (Obec* const obec : *obceOkresu)
		{
			obceSDanouVymerouLesa->add(obec);
		}

		delete obceOkresu;
	}

	return obceSDanouVymerouLesa;
}

inline void Kraj::nacitajSa(std::ifstream* infile)
{
	*infile >> id_;
	*infile >> nazov_;
}
