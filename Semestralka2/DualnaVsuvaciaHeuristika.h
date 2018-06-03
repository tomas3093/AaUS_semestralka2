#pragma once
#include "structures/table/UnsortedSequenceTableNoUnique.h"
#include "Obec.h"
#include "structures/priority_queue/heap.h"

// Konstanty
const double MAX_PODIEL_ORNA_PODA = 0.2;
const double MAX_PODIEL_OVOCNE_SADY = 0.1;
const int ROK_PRE_VYPOCET_VYMER = OBEC_UDAJE_ROK_ZACIATOK + OBEC_POCET_ZAZNAMENAVANYCH_ROKOV - 1;

/// <summary> Trieda ktora riesi zadany optimalizacny problem pomocou dualnej vsuvacej heuristiky. </summary>
class DualnaVsuvaciaHeuristika
{
public:
	/// <summary> Konstruktor. </summary>
	DualnaVsuvaciaHeuristika();

	/// <summary> Destruktor. </summary>
	~DualnaVsuvaciaHeuristika();

	/// <summary> Vyriesi optimalizacny problem a vypise optimalne riesenie. </summary>
	void riesProblemPreDaneObce(structures::UnsortedSequenceTableNoUnique<int, Obec*>* obce);

private:
	/// <summary> Celkova vymera ornej pody zadanych obci. </summary>
	long int celkovaVymeraOrnejPody_;

	/// <summary> Celkova vymera ovocnych sadov zadanych obci. </summary>
	long int celkovaVymeraOvocnychSadov_;

	/// <summary> Celkovy pocet obyvatelov zadanych obci. </summary>
	long int celkovyPocetObyvatelov_;
};



inline DualnaVsuvaciaHeuristika::DualnaVsuvaciaHeuristika()
	:celkovaVymeraOrnejPody_(0), celkovaVymeraOvocnychSadov_(0), celkovyPocetObyvatelov_(0)
{
}


inline DualnaVsuvaciaHeuristika::~DualnaVsuvaciaHeuristika()
{
}

inline void DualnaVsuvaciaHeuristika::riesProblemPreDaneObce(
	structures::UnsortedSequenceTableNoUnique<int, Obec*>* obce)
{
	// Zisti celkove vymery ornej pody a ovocnych sadov zadanych obci
	celkovaVymeraOrnejPody_ = 0;
	celkovaVymeraOvocnychSadov_ = 0;
	celkovyPocetObyvatelov_ = 0;

	// Vytvorenie prioritneho frontu s prioritou 'pocet obyvatelov' a zistenie celkovych vymer povodnej mnoziny obci
	structures::Heap<Obec*> obcePodlaPoctuObyvatelov = structures::Heap<Obec*>();
	for (structures::TableItem<int, Obec*>* const item : *obce)
	{
		Obec* obec = item->accessData();

		// Ak su vymery nenulove
		if (obec->dajVymeru(ROK_PRE_VYPOCET_VYMER, ORNA_PODA) != 0 && obec->dajVymeru(ROK_PRE_VYPOCET_VYMER, OVOCNE_SADY))
		{
			obcePodlaPoctuObyvatelov.push(obec->dajObyvatelstvo(SPOLU), obec);

			celkovaVymeraOrnejPody_ += item->accessData()->dajVymeru(ROK_PRE_VYPOCET_VYMER, ORNA_PODA);
			celkovaVymeraOvocnychSadov_ += item->accessData()->dajVymeru(ROK_PRE_VYPOCET_VYMER, OVOCNE_SADY);
			celkovyPocetObyvatelov_ += item->accessData()->dajObyvatelstvo(SPOLU);
		}
	}


	// Heuristika
	long int aktualnaVymeraOrnejPody = celkovaVymeraOrnejPody_;
	long int aktualnaVymeraOvocnychSadov = celkovaVymeraOvocnychSadov_;
	int aktualnyPocetObyvatelov = celkovyPocetObyvatelov_;

	while (	aktualnaVymeraOrnejPody > celkovaVymeraOrnejPody_ * MAX_PODIEL_ORNA_PODA && 
			aktualnaVymeraOvocnychSadov > celkovaVymeraOvocnychSadov_ * MAX_PODIEL_OVOCNE_SADY)
	{
		Obec* odstranenaObec = obcePodlaPoctuObyvatelov.pop();
		aktualnaVymeraOrnejPody -= odstranenaObec->dajVymeru(ROK_PRE_VYPOCET_VYMER, ORNA_PODA);
		aktualnaVymeraOvocnychSadov -= odstranenaObec->dajVymeru(ROK_PRE_VYPOCET_VYMER, OVOCNE_SADY);
		aktualnyPocetObyvatelov -= odstranenaObec->dajObyvatelstvo(SPOLU);
	}

	// Vypis vysledneho riesenia
	std::cout << "\nPocet obyvatelov obci z povodnej mnoziny: " << celkovyPocetObyvatelov_ << "\n";
	std::cout << "Vymera ornej pody povodnej mnoziny: " << celkovaVymeraOrnejPody_ << " m2\n";
	std::cout << MAX_PODIEL_ORNA_PODA * 100 << "% ornej pody povodnej mnoziny: " << celkovaVymeraOrnejPody_ * MAX_PODIEL_ORNA_PODA << " m2\n";
	std::cout << "Vymera ovocnych sadov povodnej mnoziny: " << celkovaVymeraOvocnychSadov_ << " m2\n";
	std::cout << MAX_PODIEL_OVOCNE_SADY * 100 << "% ovocnych sadov povodnej mnoziny: " << celkovaVymeraOvocnychSadov_ * MAX_PODIEL_OVOCNE_SADY << " m2\n";

	std::cout << "\nRiesenie najdene heuristikou:\n";
	std::cout << "Celkovy pocet obyvatelov obci (hodnota UF): " << aktualnyPocetObyvatelov << "\n";
	std::cout << "Celkova vymera ornej pody obci: " << aktualnaVymeraOrnejPody << " m2\n";
	std::cout << "Celkova vymera ovocnych sadov obci: " << aktualnaVymeraOvocnychSadov << " m2\n";

	int i = 1;
	while (!obcePodlaPoctuObyvatelov.isEmpty())
	{
		Obec* obec = obcePodlaPoctuObyvatelov.pop();
		std::cout << "\n\n" << i << ". " << obec->dajNazov();
		std::cout << "\nPocet obyvatelov: " << obec->dajObyvatelstvo(SPOLU);
		std::cout << "\nVymera ornej pody: " << obec->dajVymeru(ROK_PRE_VYPOCET_VYMER, ORNA_PODA) << " m2";
		std::cout << "\nVymera ovocnych sadov: " << obec->dajVymeru(ROK_PRE_VYPOCET_VYMER, OVOCNE_SADY) << " m2";
		i++;
	}
}
