#include "stdafx.h"
#include "EvidenciaObci.h"
#include <iostream>
#include "ComparatorObceCelkovaVymera.h"
#include "structures/table/UnsortedSequenceTableNoUnique.h"
#include "structures/table/sorting/quick_sort.h"
#include "structures/stack/explicit_stack.h"
#include "ComparatorObcePrirastokZastPloch.h"
#include "ComparatorObceZmenaPodieluOrnejPody.h"
#include "ComparatorObceObyvatelstvo.h"


EvidenciaObci::EvidenciaObci()
	:obce_(new structures::UnsortedSequenceTableNoUnique<int, Obec*>), 
obcePodlaId_(new structures::Treap<int, Obec*>),
obcePodlaNazvu_(new structures::Treap<std::string, structures::ArrayList<Obec*>*>())
{
}


EvidenciaObci::~EvidenciaObci()
{
	for (structures::TableItem<int, Obec*>* item : *obcePodlaId_)
	{
		delete item->accessData();
	}
	delete obcePodlaId_;

	for (structures::TableItem<std::string, structures::ArrayList<Obec*>*>* item : *obcePodlaNazvu_)
	{
		delete item->accessData();
	}
	delete obcePodlaNazvu_;

	delete obce_;
}

Obec* EvidenciaObci::dajObecPodlaNazvu(std::string nazovObce, bool &naslaSa, int idOkresu)
{
	Obec* result = nullptr;

	// Ak taka obec existuje
	if (obcePodlaNazvu_->containsKey(nazovObce))
	{
		naslaSa = true;
		structures::ArrayList<Obec*> * obceSDanymNazvom = (*obcePodlaNazvu_)[nazovObce];

		// Je taka obec len 1
		if (obceSDanymNazvom->size() == 1)
		{
			result = (*obceSDanymNazvom)[0];
		}
		else
		{
			// Je takych obci viac
			for (Obec* const obec : *obceSDanymNazvom)
			{
				if (obec->dajIdOkresu() == idOkresu)
				{
					result = obec;
					break;
				}
			}
		}
	}
	else
	{
		naslaSa = false;
	}
	return result;
}

void EvidenciaObci::vypisUdajeVymeraObce(int rok1, int rok2, std::string nazovObce, int idOkresu)
{
	bool naslaSa = false;
	Obec* obec = dajObecPodlaNazvu(nazovObce, naslaSa, idOkresu);
	if (naslaSa && obec != nullptr)
	{
		std::cout << obec->dajNazov() << std::endl;
		for (int aktRok = rok1; aktRok <= rok2; ++aktRok)
		{
			std::cout << "\nUdaje za rok " << aktRok << std::endl;
			std::cout << "Celkova vymera:\t\t" << obec->dajVymeru(aktRok, VymeraPolozka::CELKOVA) << " m2\n";
			std::cout << "Vymera ornej pody:\t" << obec->dajVymeru(aktRok, VymeraPolozka::ORNA_PODA) << " m2\n";
			std::cout << "Vymera ovocnych sadov:\t" << obec->dajVymeru(aktRok, VymeraPolozka::OVOCNE_SADY) << " m2\n";

			double udaj = 0;
			double celkovaVymera = obec->dajVymeru(aktRok, VymeraPolozka::CELKOVA);
			udaj = (double(obec->dajVymeru(aktRok, VymeraPolozka::VODNE_PLOCHY)) / celkovaVymera) * 100;
			std::cout << "Vodne plochy / celkova vymera:\t" << udaj << " %\n";

			udaj = (double(obec->dajVymeru(aktRok, VymeraPolozka::LESNE_POZEMKY)) / celkovaVymera) * 100;
			std::cout << "Lesne pozemky / celkova vymera:\t" << udaj << " %\n";

			udaj = (double(obec->dajVymeru(aktRok, VymeraPolozka::ZASTAVANE_PLOCHY)) / celkovaVymera) * 100;
			std::cout << "Zastavane plochy a nadvoria / celkova vymera: " << udaj << " %\n";
		}
	}
	else if (naslaSa && obec == nullptr)
	{
		std::cout << "Zadana obec sa nenachadza v zadanom okrese!\n\n";
	}
	else
	{
		std::cout << "Obec so zadanym nazvom neexistuje!\n\n";
	}
}

void EvidenciaObci::nacitajSa(std::ifstream* infile)
{
	int pocet = 0;
	*infile >> pocet;
	for (int i = 0; i < pocet; ++i)
	{
		Obec* nova = new Obec(infile);
		obce_->insert(i + 1, nova);
		obcePodlaId_->insert(nova->dajId(), nova);

		// Ak este neexistuje kluc s danym nazvom obce
		if (!obcePodlaNazvu_->containsKey(nova->dajNazov()))
		{
			structures::ArrayList<Obec*>* zoznamObci = new structures::ArrayList<Obec*>();
			zoznamObci->add(nova);
			obcePodlaNazvu_->insert(nova->dajNazov(), zoznamObci);
		}
		else
		{
			// Ak uz zoznam existuje prida novu obec do zoznamu
			((*obcePodlaNazvu_)[nova->dajNazov()])->add(nova);
		}
	}

	// Nacitanie ostatnych dat
	nacitajVymeryObci();
	nacitajObyvatelstvoObci();
}

void EvidenciaObci::vypisObcePodlaCelkovejVymery(int rok, bool vzostupne)
{
	ComparatorObceCelkovaVymera cmp = ComparatorObceCelkovaVymera(rok);
	structures::QuickSort<int, Obec*>().sort(*obce_, cmp);

	if (vzostupne)
	{
		int poradie = 1;
		for (structures::TableItem<int, Obec*>* const item : *obce_)
		{
			Obec* obec = item->accessData();
			std::cout << poradie << ". " << obec->dajNazov() << "\nCelkova vymera v roku " << rok << ": " << obec->dajVymeru(rok, CELKOVA) << " m2 \n\n";
			poradie++;
		}
	}
	else
	{
		structures::ExplicitStack<Obec*> stackObce = structures::ExplicitStack<Obec*>();
		// Naplnenie zasobnika
		for (structures::TableItem<int, Obec*>* const item : *obce_)
		{
			Obec* obec = item->accessData();
			stackObce.push(obec);
		}

		// Vypis v opacnom poradi
		int poradie = 1;
		while (!stackObce.isEmpty())
		{
			Obec* obec = stackObce.pop();
			std::cout << poradie << ". " << obec->dajNazov() << "\nCelkova vymera v roku " << rok << ": " << obec->dajVymeru(rok, CELKOVA) << " m2 \n\n";
			poradie++;
		}
	}
}

void EvidenciaObci::vypisObcePodlaPrirastku(int rok, bool vzostupne)
{
	ComparatorObcePrirastokZastPloch cmp = ComparatorObcePrirastokZastPloch(rok);
	structures::QuickSort<int, Obec*>().sort(*obce_, cmp);

	if (vzostupne)
	{
		int poradie = 1;
		for (structures::TableItem<int, Obec*>* const item : *obce_)
		{
			Obec* obec = item->accessData();
			std::cout << poradie << ". " << obec->dajNazov() << "\nPrirastok zastavanych ploch a nadvorii v roku " << rok << ": " << obec->dajPrirastokZastavanychPloch(rok) << " % \n\n";
			poradie++;
		}
	}
	else
	{
		structures::ExplicitStack<Obec*> stackObce = structures::ExplicitStack<Obec*>();
		// Naplnenie zasobnika
		for (structures::TableItem<int, Obec*>* const item : *obce_)
		{
			Obec* obec = item->accessData();
			stackObce.push(obec);
		}

		// Vypis v opacnom poradi
		int poradie = 1;
		while (!stackObce.isEmpty())
		{
			Obec* obec = stackObce.pop();
			std::cout << poradie << ". " << obec->dajNazov() << "\nPrirastok zastavanych ploch a nadvorii v roku " << rok << ": " << obec->dajPrirastokZastavanychPloch(rok) << " % \n\n";
			poradie++;
		}
	}
}

void EvidenciaObci::vypisObcePodlaZmenyPodieluOrnejPody(int rok1, int rok2, bool vzostupne)
{
	ComparatorObceZmenaPodieluOrnejPody cmp = ComparatorObceZmenaPodieluOrnejPody(rok1, rok2);
	structures::QuickSort<int, Obec*>().sort(*obce_, cmp);

	if (vzostupne)
	{
		int poradie = 1;
		for (structures::TableItem<int, Obec*>* const item : *obce_)
		{
			Obec* obec = item->accessData();
			std::cout << poradie << ". " << obec->dajNazov() << "\nZmena podielu ornej pody medzi rokmi " << rok1 << " a " << rok2 << ": " << obec->dajZmenuPodieluOrnejPody(rok1, rok2) << " % \n\n";
			poradie++;
		}
	}
	else
	{
		structures::ExplicitStack<Obec*> stackObce = structures::ExplicitStack<Obec*>();
		// Naplnenie zasobnika
		for (structures::TableItem<int, Obec*>* const item : *obce_)
		{
			Obec* obec = item->accessData();
			stackObce.push(obec);
		}

		// Vypis v opacnom poradi
		int poradie = 1;
		while (!stackObce.isEmpty())
		{
			Obec* obec = stackObce.pop();
			std::cout << poradie << ". " << obec->dajNazov() << "\nZmena podielu ornej pody medzi rokmi " << rok1 << " a " << rok2 << ": " << obec->dajZmenuPodieluOrnejPody(rok1, rok2) << " % \n\n";
			poradie++;
		}
	}
}

void EvidenciaObci::vypisObcePodlaObyvatelstva(Pohlavie pohlavie, Kraj* kraj, Okres* okres, bool vzostupne)
{
	structures::UnsortedSequenceTableNoUnique<int, Obec*>* obcePodlaKriterii = dajObcePodlaKriterii(kraj, okres);
	
	// Sort
	ComparatorObceObyvatelstvo cmp = ComparatorObceObyvatelstvo(pohlavie);
	structures::QuickSort<int, Obec*>().sort(*obcePodlaKriterii, cmp);

	if (vzostupne)
	{
		int poradie = 1;
		for (structures::TableItem<int, Obec*>* const item : *obcePodlaKriterii)
		{
			Obec* obec = item->accessData();

			switch (pohlavie)
			{
			case MUZI:
				std::cout << poradie << ". " << obec->dajNazov() << "\nPocet ekonomicky aktivnych muzov: " << obec->dajObyvatelstvo(MUZI) << "\n\n";
				break;

			case ZENY:
				std::cout << poradie << ". " << obec->dajNazov() << "\nPocet ekonomicky aktivnych zien: " << obec->dajObyvatelstvo(ZENY) << "\n\n";
				break;

			case SPOLU:
			default:
				std::cout << poradie << ". " << obec->dajNazov() << "\nPocet ekonomicky aktivnych obyvatelov: " << obec->dajObyvatelstvo(SPOLU) << "\n\n";
				break;
			}
			
			poradie++;
		}
	}
	else
	{
		structures::ExplicitStack<Obec*> stackObce = structures::ExplicitStack<Obec*>();
		// Naplnenie zasobnika
		for (structures::TableItem<int, Obec*>* const item : *obcePodlaKriterii)
		{
			Obec* obec = item->accessData();
			stackObce.push(obec);
		}

		// Vypis v opacnom poradi
		int poradie = 1;
		while (!stackObce.isEmpty())
		{
			Obec* obec = stackObce.pop();
			switch (pohlavie)
			{
			case MUZI:
				std::cout << poradie << ". " << obec->dajNazov() << "\nPocet ekonomicky aktivnych muzov: " << obec->dajObyvatelstvo(MUZI) << "\n\n";
				break;

			case ZENY:
				std::cout << poradie << ". " << obec->dajNazov() << "\nPocet ekonomicky aktivnych zien: " << obec->dajObyvatelstvo(ZENY) << "\n\n";
				break;

			case SPOLU:
			default:
				std::cout << poradie << ". " << obec->dajNazov() << "\nPocet ekonomicky aktivnych obyvatelov: " << obec->dajObyvatelstvo(SPOLU) << "\n\n";
				break;
			}

			poradie++;
		}
	}

	delete obcePodlaKriterii;
}

void EvidenciaObci::nacitajVymeryObci()
{
	std::ifstream infile = std::ifstream();
	infile.open(OBCE_VYMERY_VSTUP_SUBOR);

	int pocet = 0;
	infile >> pocet;
	for (int i = 0; i < pocet; ++i)
	{
		int idObce = 0;
		infile >> idObce;

		//preskocenie nazvu obce
		std::string a;
		std::getline(infile, a);
		std::getline(infile, a);

		if (obcePodlaId_->containsKey(idObce))
		{
			Obec* obec = (*obcePodlaId_)[idObce];
			obec->nacitajVymeryObce(&infile);
		}
	}
}

void EvidenciaObci::nacitajObyvatelstvoObci()
{
	std::ifstream infile = std::ifstream();
	infile.open(OBCE_OBYVATELSTVO_VSTUP_SUBOR);

	int pocet = 0;
	infile >> pocet;
	for (int i = 0; i < pocet; ++i)
	{
		int idObce = 0;
		infile >> idObce;

		if (obcePodlaId_->containsKey(idObce))
		{
			Obec* obec = (*obcePodlaId_)[idObce];
			obec->nacitajObyvatelstvoObce(&infile);
		}
		else
		{
			std::string a;
			std::getline(infile, a);
			std::getline(infile, a);
		}
	}
}

structures::UnsortedSequenceTableNoUnique<int, Obec*>* EvidenciaObci::dajObcePodlaKriterii(Kraj* kraj, Okres* okres)
{
	structures::UnsortedSequenceTableNoUnique<int, Obec*>* obcePodlaKriterii = new structures::UnsortedSequenceTableNoUnique<int, Obec*>();
	
	// Je zadany okres
	if (okres != nullptr)
	{
		int idOkresu = okres->dajId();
		int i = 1;
		for (structures::TableItem<int, Obec*>* const item : *obce_)
		{
			if (item->accessData()->dajIdOkresu() == idOkresu)
			{
				obcePodlaKriterii->insert(i, item->accessData());
				i++;
			}
		}

		return obcePodlaKriterii;
	}

	// Je zadany iba kraj
	if (kraj != nullptr)
	{
		int idKraja = kraj->dajId();
		int i = 1;
		for (structures::TableItem<int, Obec*>* const item : *obce_)
		{
			if (item->accessData()->dajIdKraja() == idKraja)
			{
				obcePodlaKriterii->insert(i, item->accessData());
				i++;
			}
		}

		return obcePodlaKriterii;
	}

	// Nie je zadany kraj ani okres
	delete obcePodlaKriterii;
	return new structures::UnsortedSequenceTableNoUnique<int, Obec*>(*obce_);
}
