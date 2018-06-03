#include "stdafx.h"
#include "EvidenciaOkresov.h"
#include <iostream>
#include "structures/table/sorting/quick_sort.h"
#include "ComparatorOkresyZmenaPodieluOvocneSady.h"
#include "structures/stack/explicit_stack.h"
#include "ComparatorOkresyZmenaPodieluOrnaPoda.h"
#include "ComparatorOkresyZmenaPodieluLesnePozemky.h"

EvidenciaOkresov::EvidenciaOkresov()
	:okresy_(new structures::UnsortedSequenceTableNoUnique<int, Okres*>()),
okresyPodlaId_(new structures::Treap<int, Okres*>()),
okresyPodlaNazvu_(new structures::Treap<std::string, Okres*>()),
cachePodielOvocnychSadov_(new structures::Array<structures::Array<Okres*>*>(OBEC_POCET_ZAZNAMENAVANYCH_ROKOV)),
cachePodielOrnejPodyOvocnychSadov_(new structures::Array<structures::Array<Okres*>*>(OBEC_POCET_ZAZNAMENAVANYCH_ROKOV)),
cachePodielLesnychPozemkov_(new structures::Array<structures::Array<Okres*>*>(OBEC_POCET_ZAZNAMENAVANYCH_ROKOV))
{
	// Inicializovanie cache na nedefinovane hodnoty
	for (size_t i = 0; i < cachePodielOvocnychSadov_->size(); ++i)
	{
		(*cachePodielOvocnychSadov_)[i] = nullptr;
	}
	for (size_t i = 0; i < cachePodielLesnychPozemkov_->size(); ++i)
	{
		(*cachePodielLesnychPozemkov_)[i] = nullptr;
	}
	for (size_t i = 0; i < cachePodielOrnejPodyOvocnychSadov_->size(); ++i)
	{
		(*cachePodielOrnejPodyOvocnychSadov_)[i] = nullptr;
	}
}


EvidenciaOkresov::~EvidenciaOkresov()
{
	for (structures::TableItem<int, Okres*>* const item : *okresyPodlaId_)
	{
		delete item->accessData();
	}
	delete okresyPodlaId_;

	for (size_t i = 0; i < cachePodielOvocnychSadov_->size(); ++i)
	{
		delete (*cachePodielOvocnychSadov_)[i];
	}
	delete cachePodielOvocnychSadov_;

	for (size_t i = 0; i < cachePodielLesnychPozemkov_->size(); ++i)
	{
		delete (*cachePodielLesnychPozemkov_)[i];
	}
	delete cachePodielLesnychPozemkov_;

	for (size_t i = 0; i < cachePodielOrnejPodyOvocnychSadov_->size(); ++i)
	{
		delete (*cachePodielOrnejPodyOvocnychSadov_)[i];
	}
	delete cachePodielOrnejPodyOvocnychSadov_;

	delete okresyPodlaNazvu_;
	delete okresy_;
}

Okres* EvidenciaOkresov::dajOkresPodlaNazvu(std::string nazov)
{
	if (okresyPodlaNazvu_->containsKey(nazov))
	{
		return (*okresyPodlaNazvu_)[nazov];
	}

	return nullptr;
}

Okres* EvidenciaOkresov::dajOkresPodlaId(int id)
{
	if (okresyPodlaId_->containsKey(id))
	{
		return (*okresyPodlaId_)[id];
	}

	return nullptr;
}

void EvidenciaOkresov::nacitajSa(std::ifstream* infile)
{
	int pocet = 0;
	*infile >> pocet;
	for (int i = 0; i < pocet; ++i)
	{
		Okres* novy = new Okres(infile);
		okresyPodlaId_->insert(novy->dajId(), novy);
		okresyPodlaNazvu_->insert(novy->dajNazov(), novy);
		okresy_->insert(i + 1, novy);
	}
}

structures::Array<int>* EvidenciaOkresov::dajIdVsetkychOkresov()
{
	structures::Array<int>* ret = new structures::Array<int>(okresyPodlaId_->size());
	int i = 0;
	for (structures::TableItem<int, Okres*>* const item : *okresyPodlaId_)
	{
		(*ret)[i] = item->getKey();
		i++;
	}

	return ret;
}

void EvidenciaOkresov::vypisOkresyPodlaOvocnychSadov(int rok)
{
	// Najdenie minima a maxima
	long int minHodnota = LONG_MAX;
	long int maxHodnota = 0;
	Okres* okresMin = nullptr;
	Okres* okresMax = nullptr;

	// Ak v cache nie je ulozena pozadovana hodnota
	if ((*cachePodielOvocnychSadov_)[rok - OBEC_UDAJE_ROK_ZACIATOK] == nullptr)
	{
		for (structures::TableItem<int, Okres*>* const item : *okresy_)
		{
			long int hodnota = item->accessData()->dajVymeru(rok, OVOCNE_SADY);
			if (hodnota < minHodnota)
			{
				minHodnota = hodnota;
				okresMin = item->accessData();
			}
			if (hodnota > maxHodnota)
			{
				maxHodnota = hodnota;
				okresMax = item->accessData();
			}
		}

		// Ulozenie najdenych hodnot do cache
		structures::Array<Okres*>* cache = new structures::Array<Okres*>(2);
		(*cache)[0] = okresMin;
		(*cache)[1] = okresMax;
		(*cachePodielOvocnychSadov_)[rok - OBEC_UDAJE_ROK_ZACIATOK] = cache;
	}
	else
	{
		structures::Array<Okres*>* cache = (*cachePodielOvocnychSadov_)[rok - OBEC_UDAJE_ROK_ZACIATOK];
		okresMin = (*cache)[0];
		okresMax = (*cache)[1];
		minHodnota = okresMin->dajVymeru(rok, OVOCNE_SADY);
		maxHodnota = okresMax->dajVymeru(rok, OVOCNE_SADY);
	}

	std::cout << "Okres s najmensou vymerov ovocnych sadov v roku " << rok << ":\t";
	std::cout << okresMin->dajNazov() << "\nVymera ovocnych sadov: " << minHodnota << " m2 \n\n";

	std::cout << "Okres s najvacsou vymerov ovocnych sadov v roku " << rok << ":\t";
	std::cout << okresMax->dajNazov() << "\nVymera ovocnych sadov: " << maxHodnota << " m2 \n\n";
}

void EvidenciaOkresov::vypisOkresyPodlaOrnPodyKuOvocSadom(int rok)
{
	// Najdenie minima a maxima
	double minHodnota = DBL_MAX;
	double maxHodnota = 0;
	Okres* okresMin = nullptr;
	Okres* okresMax = nullptr;

	// Ak v cache nie je ulozena pozadovana hodnota
	if ((*cachePodielOrnejPodyOvocnychSadov_)[rok - OBEC_UDAJE_ROK_ZACIATOK] == nullptr)
	{
		for (structures::TableItem<int, Okres*>* const item : *okresy_)
		{
			double hodnota = item->accessData()->dajPodielOrnejPodyOvocnychSadov(rok);

			if (hodnota < minHodnota)
			{
				minHodnota = hodnota;
				okresMin = item->accessData();
			}
			if (hodnota > maxHodnota)
			{
				maxHodnota = hodnota;
				okresMax = item->accessData();
			}
		}

		// Ulozenie najdenych hodnot do cache
		structures::Array<Okres*>* cache = new structures::Array<Okres*>(2);
		(*cache)[0] = okresMin;
		(*cache)[1] = okresMax;
		(*cachePodielOrnejPodyOvocnychSadov_)[rok - OBEC_UDAJE_ROK_ZACIATOK] = cache;
	}
	else
	{
		structures::Array<Okres*>* cache = (*cachePodielOrnejPodyOvocnychSadov_)[rok - OBEC_UDAJE_ROK_ZACIATOK];
		okresMin = (*cache)[0];
		okresMax = (*cache)[1];
		minHodnota = okresMin->dajPodielOrnejPodyOvocnychSadov(rok);
		maxHodnota = okresMax->dajPodielOrnejPodyOvocnychSadov(rok);
	}

	std::cout << "Okres s najmensim podielom ornej pody voci vymere ovocnych sadov v roku " << rok << ":\t";
	std::cout << okresMin->dajNazov() << "\nPodiel vymery ornej pody voci vymere ovocnych sadov: " << minHodnota << " %\n\n";

	std::cout << "Okres s najvacsim podielom ornej pody voci vymere ovocnych sadov v roku " << rok << ":\t";
	std::cout << okresMax->dajNazov() << "\nPodiel vymery ornej pody voci vymere ovocnych sadov: " << maxHodnota << " %\n\n";
}

void EvidenciaOkresov::vypisOkresyPodlaPodieluLesov(int rok)
{
	// Najdenie minima a maxima
	double minHodnota = DBL_MAX;
	double maxHodnota = 0;
	Okres* okresMin = nullptr;
	Okres* okresMax = nullptr;

	// Ak v cache nie je ulozena pozadovana hodnota
	if ((*cachePodielLesnychPozemkov_)[rok - OBEC_UDAJE_ROK_ZACIATOK] == nullptr)
	{
		for (structures::TableItem<int, Okres*>* const item : *okresy_)
		{
			double hodnota = item->accessData()->dajPodielLesnychPozemkov(rok);
			if (hodnota < minHodnota)
			{
				minHodnota = hodnota;
				okresMin = item->accessData();
			}
			if (hodnota > maxHodnota)
			{
				maxHodnota = hodnota;
				okresMax = item->accessData();
			}
		}

		// Ulozenie najdenych hodnot do cache
		structures::Array<Okres*>* cache = new structures::Array<Okres*>(2);
		(*cache)[0] = okresMin;
		(*cache)[1] = okresMax;
		(*cachePodielLesnychPozemkov_)[rok - OBEC_UDAJE_ROK_ZACIATOK] = cache;
	}
	else
	{
		structures::Array<Okres*>* cache = (*cachePodielLesnychPozemkov_)[rok - OBEC_UDAJE_ROK_ZACIATOK];
		okresMin = (*cache)[0];
		okresMax = (*cache)[1];
		minHodnota = okresMin->dajPodielLesnychPozemkov(rok);
		maxHodnota = okresMax->dajPodielLesnychPozemkov(rok);
	}

	std::cout << "Okres s najmensim podielom lesnych pozemkov voci celkovej vymere okresu v roku " << rok << ":\t";
	std::cout << okresMin->dajNazov() << "\nPodiel vymery lesnych pozemkov voci celkovej vymere okresu: " << minHodnota << " %\n\n";

	std::cout << "Okres s najvacsim podielom lesnych pozemkov voci celkovej vymere okresu v roku " << rok << ":\t";
	std::cout << okresMax->dajNazov() << "\nPodiel vymery lesnych pozemkov voci celkovej vymere okresu: " << maxHodnota << " %\n\n";
}

void EvidenciaOkresov::vypisOkresyPodlaZmenyPodieluOvocnychSadov(int rok1, int rok2, bool vzostupne)
{
	ComparatorOkresyZmenaPodieluOvocneSady cmp = ComparatorOkresyZmenaPodieluOvocneSady(rok1, rok2);
	structures::QuickSort<int, Okres*>().sort(*okresy_, cmp);

	if (vzostupne)
	{
		int poradie = 1;
		for (structures::TableItem<int, Okres*>* const item : *okresy_)
		{
			Okres* okres = item->accessData();
			std::cout << poradie << ". " << okres->dajNazov() << "\nZmena podielu vymery ovocnych sadov medzi rokmi " << rok1 << " a " << rok2 << ": " << okres->dajZmenuPodieluOvocnychSadov(rok1, rok2) << " % \n\n";
			poradie++;
		}
	}
	else
	{
		structures::ExplicitStack<Okres*> stackOkresy = structures::ExplicitStack<Okres*>();
		// Naplnenie zasobnika
		for (structures::TableItem<int, Okres*>* const item : *okresy_)
		{
			Okres* okres = item->accessData();
			stackOkresy.push(okres);
		}

		// Vypis v opacnom poradi
		int poradie = 1;
		while (!stackOkresy.isEmpty())
		{
			Okres* okres = stackOkresy.pop();
			std::cout << poradie << ". " << okres->dajNazov() << "\nZmena podielu vymery ovocnych sadov medzi rokmi " << rok1 << " a " << rok2 << ": " << okres->dajZmenuPodieluOvocnychSadov(rok1, rok2) << " % \n\n";
			poradie++;
		}
	}
}

void EvidenciaOkresov::vypisOkresyPodlaZmenyPodieluOrnejPody(int rok1, int rok2, bool vzostupne)
{
	ComparatorOkresyZmenaPodieluOrnaPoda cmp = ComparatorOkresyZmenaPodieluOrnaPoda(rok1, rok2);
	structures::QuickSort<int, Okres*>().sort(*okresy_, cmp);

	if (vzostupne)
	{
		int poradie = 1;
		for (structures::TableItem<int, Okres*>* const item : *okresy_)
		{
			Okres* okres = item->accessData();
			std::cout << poradie << ". " << okres->dajNazov() << "\nZmena podielu vymery ornej pody medzi rokmi " << rok1 << " a " << rok2 << ": " << okres->dajZmenuPodieluOrnejPody(rok1, rok2) << " % \n\n";
			poradie++;
		}
	}
	else
	{
		structures::ExplicitStack<Okres*> stackOkresy = structures::ExplicitStack<Okres*>();
		// Naplnenie zasobnika
		for (structures::TableItem<int, Okres*>* const item : *okresy_)
		{
			Okres* okres = item->accessData();
			stackOkresy.push(okres);
		}

		// Vypis v opacnom poradi
		int poradie = 1;
		while (!stackOkresy.isEmpty())
		{
			Okres* okres = stackOkresy.pop();
			std::cout << poradie << ". " << okres->dajNazov() << "\nZmena podielu vymery ornej pody medzi rokmi " << rok1 << " a " << rok2 << ": " << okres->dajZmenuPodieluOrnejPody(rok1, rok2) << " % \n\n";
			poradie++;
		}
	}
}

void EvidenciaOkresov::vypisOkresyPodlaZmenyPodieluLesnychPozemkov(int rok1, int rok2, bool vzostupne)
{
	ComparatorOkresyZmenaPodieluLesnePozemky cmp = ComparatorOkresyZmenaPodieluLesnePozemky(rok1, rok2);
	structures::QuickSort<int, Okres*>().sort(*okresy_, cmp);

	if (vzostupne)
	{
		int poradie = 1;
		for (structures::TableItem<int, Okres*>* const item : *okresy_)
		{
			Okres* okres = item->accessData();
			std::cout << poradie << ". " << okres->dajNazov() << "\nZmena podielu vymery lesnych pozemkov medzi rokmi " << rok1 << " a " << rok2 << ": " << okres->dajZmenuPodieluLesnychPozemkov(rok1, rok2) << " % \n\n";
			poradie++;
		}
	}
	else
	{
		structures::ExplicitStack<Okres*> stackOkresy = structures::ExplicitStack<Okres*>();
		// Naplnenie zasobnika
		for (structures::TableItem<int, Okres*>* const item : *okresy_)
		{
			Okres* okres = item->accessData();
			stackOkresy.push(okres);
		}

		// Vypis v opacnom poradi
		int poradie = 1;
		while (!stackOkresy.isEmpty())
		{
			Okres* okres = stackOkresy.pop();
			std::cout << poradie << ". " << okres->dajNazov() << "\nZmena podielu vymery lesnych pozemkov medzi rokmi " << rok1 << " a " << rok2 << ": " << okres->dajZmenuPodieluLesnychPozemkov(rok1, rok2) << " % \n\n";
			poradie++;
		}
	}
}
