#include "stdafx.h"
#include "EvidenciaKrajov.h"
#include <iostream>
#include "structures/table/treap.h"
#include "structures/table/sorting/quick_sort.h"
#include "ComparatorKrajeLesyParameter.h"
#include "ComparatorKrajePocetObciSVymerou.h"

EvidenciaKrajov::EvidenciaKrajov()
	:kraje_(new structures::UnsortedSequenceTableNoUnique<int, Kraj*>()),
krajePodlaId_(new structures::Treap<int, Kraj*>),
krajePodlaNazvu_(new structures::Treap<std::string, Kraj*>())
{
}


EvidenciaKrajov::~EvidenciaKrajov()
{
	for (structures::TableItem<int, Kraj*>* const item : *kraje_)
	{
		delete item->accessData();
	}
	delete kraje_;

	delete krajePodlaId_;
	delete krajePodlaNazvu_;
}

Kraj* EvidenciaKrajov::dajKrajPodlaNazvu(std::string nazov)
{
	if (krajePodlaNazvu_->containsKey(nazov))
	{
		return (*krajePodlaNazvu_)[nazov];
	}

	return nullptr;
}

Kraj* EvidenciaKrajov::dajKrajPodlaId(int id)
{
	if (krajePodlaId_->containsKey(id))
	{
		return (*krajePodlaId_)[id];
	}

	return nullptr;
}

void EvidenciaKrajov::nacitajSa(std::ifstream* infile)
{
	int pocet = 0;
	*infile >> pocet;
	for (int i = 0; i < pocet; ++i)
	{
		Kraj* novy = new Kraj(infile);
		kraje_->insert(i, novy);
		krajePodlaId_->insert(novy->dajId(), novy);
		krajePodlaNazvu_->insert(novy->dajNazov(), novy);
	}
}

void EvidenciaKrajov::vypisUdajeVymeraKraja(int rok, int idKraja)
{
	Kraj* kraj = dajKrajPodlaId(idKraja);

	std::cout << kraj->dajNazov() << std::endl;
	std::cout << "Celkova vymera:\t\t" << kraj->dajVymeru(rok, VymeraPolozka::CELKOVA) << " m2\n";
	std::cout << "Vymera ornej pody:\t" << kraj->dajVymeru(rok, VymeraPolozka::ORNA_PODA) << " m2\n";
	std::cout << "Vymera ovocnych sadov:\t" << kraj->dajVymeru(rok, VymeraPolozka::OVOCNE_SADY) << " m2\n";

	double udaj = 0;
	double celkovaVymera = kraj->dajVymeru(rok, VymeraPolozka::CELKOVA);
	udaj = (double(kraj->dajVymeru(rok, VymeraPolozka::VODNE_PLOCHY)) / celkovaVymera) * 100;
	std::cout << "Vodne plochy / celkova vymera:\t" << udaj << " %\n";

	udaj = (double(kraj->dajVymeru(rok, VymeraPolozka::LESNE_POZEMKY)) / celkovaVymera) * 100;
	std::cout << "Lesne pozemky / celkova vymera:\t" << udaj << " %\n";

	udaj = (double(kraj->dajVymeru(rok, VymeraPolozka::ZASTAVANE_PLOCHY)) / celkovaVymera) * 100;
	std::cout << "Zastavane plochy a nadvoria / celkova vymera: " << udaj << " %\n";
}

void EvidenciaKrajov::vypisKrajePodlaPodieluLesov(int rok, double percentHodnota)
{
	ComparatorKrajeLesyParameter cmp = ComparatorKrajeLesyParameter(rok, percentHodnota);
	structures::QuickSort<int, Kraj*>().sort(*kraje_, cmp);

	int poradie = 1;
	for (structures::TableItem<int, Kraj*>* const item : *kraje_)
	{
		Kraj* kraj = item->accessData();
		double podiel = (double(kraj->dajVymeru(rok, LESNE_POZEMKY)) / double(kraj->dajVymeru(rok, CELKOVA))) * 100;
		if (podiel >= percentHodnota)
		{
			std::cout << poradie << ". " << kraj->dajNazov() << "\nPodiel vymery lesov z celkovej vymery kraja v roku " << rok << ": " << podiel << " % \n\n";
			poradie++;
		}
	}
}

void EvidenciaKrajov::vypisKrajePodlaPoctuOkresovPodielLesov(int rok, size_t pocetOkresov, double percentHodnota)
{
	bool nasielSa = false;

	for (structures::TableItem<int, Kraj*>* const item : *kraje_)
	{
		structures::List<Okres*>* okresy = item->accessData()->dajOkresySVymerouLesa(rok, percentHodnota);
		if (okresy->size() >= pocetOkresov)
		{
			std::cout << item->accessData()->dajNazov() << " kraj\nOkresy s podielom lesov minimalne " << percentHodnota << "% :\n";
			for (Okres* const okres : *okresy)
			{
				double podiel = okres->dajPodielLesnychPozemkov(rok);
				std::cout << okres->dajNazov() << "\nPodiel lesnych pozemkov: " << podiel << "%\n\n";
			}

			nasielSa = true;
		}
		delete okresy;
	}

	if (!nasielSa)
	{
		std::cout << "\n\nNenasiel sa ziadny kraj splnujuci dane kriteria!\n\n";
	}
}

void EvidenciaKrajov::vypisKrajePodlaPoctuObciPodielLesov(int rok, double percentHodnota)
{
	structures::UnsortedSequenceTableNoUnique<Kraj*, structures::List<Obec*>*> krajePodlaPoctuObci = structures::UnsortedSequenceTableNoUnique<Kraj*, structures::List<Obec*>*>();
	for (structures::TableItem<int, Kraj*>* const item : *kraje_)
	{
		Kraj* kraj = item->accessData();
		structures::List<Obec*>* obce = kraj->dajObceSVymerouLesa(rok, percentHodnota);
		krajePodlaPoctuObci.insert(kraj, obce);
	}

	// Sort
	ComparatorKrajePocetObciSVymerou cmp = ComparatorKrajePocetObciSVymerou();
	structures::QuickSort<Kraj*, structures::List<Obec*>*>().sort(krajePodlaPoctuObci, cmp);

	// Vypis
	if (krajePodlaPoctuObci.size() > 0)
	{
		// Kraj s minimalnym poctom vyhovujucich obci
		structures::TableItem<Kraj*, structures::List<Obec*>*> item = krajePodlaPoctuObci.getItemAtIndex(0);
		std::cout << "Kraj s minimalnym poctom vyhovujucich obci (" << item.accessData()->size() << ") : " << item.getKey()->dajNazov() << " kraj\nVyhovujuce obce:\n";
		for (Obec* const obec : *item.accessData())
		{
			double podiel = (double(obec->dajVymeru(rok, LESNE_POZEMKY) / double(obec->dajVymeru(rok, CELKOVA)))) * 100;
			std::cout << obec->dajNazov() << "\nPodiel lesov z celkovej vymery obce: " << podiel << "%\n\n";
		}

		// Kraj s maximalnym poctom vyhovujucich obci
		item = krajePodlaPoctuObci.getItemAtIndex(krajePodlaPoctuObci.size() - 1);
		std::cout << "Kraj s maximalnym poctom vyhovujucich obci (" << item.accessData()->size() << ") : " << item.getKey()->dajNazov() << " kraj\nVyhovujuce obce:\n";
		for (Obec* const obec : *item.accessData())
		{
			double podiel = (double(obec->dajVymeru(rok, LESNE_POZEMKY) / double(obec->dajVymeru(rok, CELKOVA)))) * 100;
			std::cout << obec->dajNazov() << "\nPodiel lesov z celkovej vymery obce: " << podiel << "%\n\n";
		}
	}
	else
	{
		std::cout << "\nZadanym parametrom nevyhovuje ziadny kraj\n\n";
	}

	// Dealokacia
	for (structures::TableItem<Kraj*, structures::List<Obec*>*>* const item : krajePodlaPoctuObci)
	{
		delete item->accessData();
	}
}
