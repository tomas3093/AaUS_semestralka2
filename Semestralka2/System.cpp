#include "stdafx.h"

#include <iostream>
#include "System.h"
#include "constants.h"
#include "DualnaVsuvaciaHeuristika.h"


System::System()
	:kraje_(new EvidenciaKrajov()), okresy_(new EvidenciaOkresov()), obce_(new EvidenciaObci())
{
	nacitajSa();
	uzivatelskeRozhranie();
}


System::~System()
{
	delete kraje_;
	delete okresy_;
	delete obce_;
}

void System::nacitajSa()
{
	// Kraje (id a nazvy)
	std::ifstream* infile = new std::ifstream();
	infile->open(KRAJE_VSTUP_SUBOR);
	kraje_->nacitajSa(infile);
	infile->close();

	// Okresy (id, id_krajov a nazvy)
	infile->open(OKRESY_VSTUP_SUBOR);
	okresy_->nacitajSa(infile);
	infile->close();

	// Obce (id, id_okresov, nazvy, vymera a obyvatelstvo)
	infile->open(OBCE_VSTUP_SUBOR);
	obce_->nacitajSa(infile);

	// Prida obce jednotlivym okresom
	structures::Table<int, Obec*>* id_obci = obce_->dajVsetkyObcePodlaId();
	for (structures::TableItem<int, Obec*>* const item : *id_obci)
	{
		Obec* obec = item->accessData();
		Okres* okres = okresy_->dajOkresPodlaId(obec->dajIdOkresu());

		okres->pridajObec(obec);
	}

	// Prida okresy jednotlivym krajom
	structures::Array<int>* id_okresov = okresy_->dajIdVsetkychOkresov();
	for (size_t i = 0; i < id_okresov->size(); ++i)
	{
		Okres* okres = okresy_->dajOkresPodlaId((*id_okresov)[i]);
		Kraj* kraj = kraje_->dajKrajPodlaId(okres->dajIdKraja());
		kraj->pridajOkres(okres);
	}
	delete id_okresov;

	delete infile;
}

void System::zapisDataDoSuboru(std::string nazovSuboru, int rok, Kraj* kraj, Okres* okres)
{
	structures::UnsortedSequenceTableNoUnique<int, Obec*>* obcePodlaKriterii = obce_->dajObcePodlaKriterii(kraj, okres);
	std::ofstream outFile = std::ofstream(nazovSuboru);

	if (obcePodlaKriterii->size() > 0)
	{
		for (structures::TableItem<int, Obec*>* const item : *obcePodlaKriterii)
		{
			Obec* obec = item->accessData();
			outFile << obec->dajId() << ";";
			outFile << obec->dajIdOkresu() << ";";
			outFile << obec->dajIdKraja() << ";";
			outFile << obec->dajObyvatelstvo(SPOLU) << ";";
			outFile << obec->dajNazov() << ";";
			outFile << obec->dajVymeru(rok, ORNA_PODA) << ";";
			outFile << obec->dajVymeru(rok, OVOCNE_SADY) << "\n";
		}
	}
	delete obcePodlaKriterii;
}

void System::uzivatelskeRozhranie()
{
	int volba = 0;

	do
	{
		std::cout << "\n\n*** System pre rozhodovanie biofarmara ***\n\n";
		std::cout << "MENU\n";
		std::cout << "1. Udaje vymery o obci v danom roku\n";
		std::cout << "2. Zoradenie obci na zaklade kriterii\n";
		std::cout << "3. Vyhladanie okresu na zaklade kriterii\n";
		std::cout << "4. Zoradenie okresov na zaklade kriterii\n";
		std::cout << "5. Vyhladanie krajov na zaklade kriterii\n";
		std::cout << "6. Zoradenie obci na zaklade EA obyvatelstva\n";
		std::cout << "7. Zapisanie udajov o obciach do suboru\n";
		std::cout << "8. Riesenie optimalizacneho problemu\n\n";

		std::cout << "0. Koniec\n\n";

		std::cin >> volba;
		
		// Premenne do switchu
		std::string value1 = "";
		std::string value2 = "";
		std::string value3 = "";
		int cislo1 = 0;
		int cislo2 = 0;
		int cislo3 = 0;
		double desCislo = 0;
		Okres* okres = nullptr;
		Kraj* kraj = nullptr;
		Pohlavie pohlavie = SPOLU;
		structures::UnsortedSequenceTableNoUnique<int, Obec*>* obce = nullptr;
		DualnaVsuvaciaHeuristika heuristika = DualnaVsuvaciaHeuristika();

		switch (volba)
		{
		case 1:
			std::cout << "Volba: Udaje vymery o obci v danom roku\n\n";
			std::cout << "Zadajte prvy rok (" << OBEC_UDAJE_ROK_ZACIATOK << " - " << OBEC_UDAJE_ROK_ZACIATOK + OBEC_POCET_ZAZNAMENAVANYCH_ROKOV - 1 << "): ";
			std::cin >> cislo1;
			if (!checkYear(cislo1))
			{
				showMsg("Zadany rok nie je z pozadovaneho rozsahu!");
				break;
			}

			std::cout << "Zadajte druhy rok (" << OBEC_UDAJE_ROK_ZACIATOK << " - " << OBEC_UDAJE_ROK_ZACIATOK + OBEC_POCET_ZAZNAMENAVANYCH_ROKOV - 1 << "): ";
			std::cin >> cislo2;
			if (!checkYear(cislo2))
			{
				showMsg("Zadany rok nie je z pozadovaneho rozsahu!");
				break;
			}

			std::cout << "\nZadajte nazov obce: ";
			std::getline(std::cin, value1);
			std::getline(std::cin, value1);
			value1.erase(std::remove(value1.begin(), value1.end(), '\n'), value1.end());	// odstrani '\n'

			std::cout << "\nZadajte nazov okresu (kvoli duplicitam - default 0): ";
			std::getline(std::cin, value2);
			value2.erase(std::remove(value2.begin(), value2.end(), '\n'), value2.end());	// odstrani '\n'
		
			okres = okresy_->dajOkresPodlaNazvu(value2);
			if (okres != nullptr)
				obce_->vypisUdajeVymeraObce(cislo1, cislo2, value1, okres->dajId());
			else
				obce_->vypisUdajeVymeraObce(cislo1, cislo2, value1, 0);
			break;

		case 2:
			std::cout << "Volba: Zoradenie obci na zaklade\n\n";
			std::cout << "1. Celkovej vymery obci v zadanom roku\n";
			std::cout << "2. Prirastku zastavanych ploch voci celkovej vymere obce v zadanom roku\n";
			std::cout << "3. Zmeny podielu vymery ornej pody voci celkovej vymere obce medzi dvoma rokmi\n";
			std::cin >> cislo1;

			switch (cislo1)
			{
			case 1:
				std::cout << "Volba: Zoradenie obci na zaklade celkovej vymery v zadanom roku\n\n";
				std::cout << "Zadajte rok (" << OBEC_UDAJE_ROK_ZACIATOK << " - " << OBEC_UDAJE_ROK_ZACIATOK + OBEC_POCET_ZAZNAMENAVANYCH_ROKOV - 1 << "): ";
				std::cin >> cislo1;
				if (!checkYear(cislo1))
				{
					showMsg("Zadany rok nie je z pozadovaneho rozsahu!");
					break;
				}

				std::cout << "\nZoradenie\n1. vzostupne\n2. zostupne\n\n";
				std::cin >> cislo2;

				if (cislo2 == 1)
					obce_->vypisObcePodlaCelkovejVymery(cislo1, true);
				else
					obce_->vypisObcePodlaCelkovejVymery(cislo1, false);
				break;

			case 2:
				std::cout << "Volba: Zoradenie obci na zaklade prirastku zastavanych ploch voci celkovej vymere obce v zadanom roku\n\n";
				std::cout << "Zadajte rok (" << OBEC_UDAJE_ROK_ZACIATOK << " - " << OBEC_UDAJE_ROK_ZACIATOK + OBEC_POCET_ZAZNAMENAVANYCH_ROKOV - 1 << "): ";
				std::cin >> cislo1;
				if (!checkYear(cislo1))
				{
					showMsg("Zadany rok nie je z pozadovaneho rozsahu!");
					break;
				}

				std::cout << "\nZoradenie\n1. vzostupne\n2. zostupne\n\n";
				std::cin >> cislo2;

				if (cislo2 == 1)
					obce_->vypisObcePodlaPrirastku(cislo1, true);
				else
					obce_->vypisObcePodlaPrirastku(cislo1, false);
				break;

			case 3:
				std::cout << "Volba: Zoradenie obci na zaklade zmeny podielu vymery ornej pody voci celkovej vymere obce medzi dvoma rokmi\n\n";
				std::cout << "Zadajte prvy rok (" << OBEC_UDAJE_ROK_ZACIATOK << " - " << OBEC_UDAJE_ROK_ZACIATOK + OBEC_POCET_ZAZNAMENAVANYCH_ROKOV - 1 << "): ";
				std::cin >> cislo1;
				if (!checkYear(cislo1))
				{
					showMsg("Zadany rok nie je z pozadovaneho rozsahu!");
					break;
				}

				std::cout << "Zadajte druhy rok (" << OBEC_UDAJE_ROK_ZACIATOK << " - " << OBEC_UDAJE_ROK_ZACIATOK + OBEC_POCET_ZAZNAMENAVANYCH_ROKOV - 1 << "): ";
				std::cin >> cislo2;
				if (!checkYear(cislo2))
				{
					showMsg("Zadany rok nie je z pozadovaneho rozsahu!");
					break;
				}

				std::cout << "\nZoradenie\n1. vzostupne\n2. zostupne\n\n";
				std::cin >> cislo3;

				if (cislo3 == 1)
					obce_->vypisObcePodlaZmenyPodieluOrnejPody(cislo1, cislo2, true);
				else
					obce_->vypisObcePodlaZmenyPodieluOrnejPody(cislo1, cislo2, false);
				break;

			default:
				break;
			}
			break;
		case 3:
			std::cout << "Volba: Vyhladanie okresu, ktory mal v zadanom roku:\n\n";
			std::cout << "1. Najvacsiu a najmensiu celkovu vymeru ovocnych sadov\n";
			std::cout << "2. Najvacsi a najmensi podiel vymery ornej pody voci vymere ovocnych sadov\n";
			std::cout << "3. Najvacsi a najmensi podiel vymery lesov voci celkovej vymere okresu\n";
			std::cin >> cislo1;

			switch (cislo1)
			{
			case 1:
				std::cout << "Volba: Vyhladanie okresu, ktory mal v zadanom roku najvacsiu a najmensiu celkovu vymeru ovocnych sadov\n\n";
				std::cout << "Zadajte rok (" << OBEC_UDAJE_ROK_ZACIATOK << " - " << OBEC_UDAJE_ROK_ZACIATOK + OBEC_POCET_ZAZNAMENAVANYCH_ROKOV - 1 << "): ";
				std::cin >> cislo1;
				if (!checkYear(cislo1))
				{
					showMsg("Zadany rok nie je z pozadovaneho rozsahu!");
					break;
				}

				okresy_->vypisOkresyPodlaOvocnychSadov(cislo1);
				break;

			case 2:
				std::cout << "Volba: Vyhladanie okresu, ktory mal v zadanom roku najvacsi a najmensi podiel vymery ornej pody voci vymere ovocnych sadov\n\n";
				std::cout << "Zadajte rok (" << OBEC_UDAJE_ROK_ZACIATOK << " - " << OBEC_UDAJE_ROK_ZACIATOK + OBEC_POCET_ZAZNAMENAVANYCH_ROKOV - 1 << "): ";
				std::cin >> cislo1;
				if (!checkYear(cislo1))
				{
					showMsg("Zadany rok nie je z pozadovaneho rozsahu!");
					break;
				}

				okresy_->vypisOkresyPodlaOrnPodyKuOvocSadom(cislo1);
				break;

			case 3:
				std::cout << "Volba: Vyhladanie okresu, ktory mal v zadanom roku najvacsi a najmensi podiel vymery lesov voci celkovej vymere okresu\n\n";
				std::cout << "Zadajte rok (" << OBEC_UDAJE_ROK_ZACIATOK << " - " << OBEC_UDAJE_ROK_ZACIATOK + OBEC_POCET_ZAZNAMENAVANYCH_ROKOV - 1 << "): ";
				std::cin >> cislo1;
				if (!checkYear(cislo1))
				{
					showMsg("Zadany rok nie je z pozadovaneho rozsahu!");
					break;
				}

				okresy_->vypisOkresyPodlaPodieluLesov(cislo1);
				break;

			default:
				break;
			}
			break;

		case 4:
			std::cout << "Volba: Zoradenie okresov na zaklade:\n\n";
			std::cout << "1. Zmeny podielu vymery ovocnych sadov voci celkovej vymere okresu medzi dvomi rokmi\n";
			std::cout << "2. Zmeny podielu vymery ornej pody voci celkovej vymere okresu medzi dvomi rokmi\n";
			std::cout << "3. Zmeny podielu vymery lesnych pozemkov voci celkovej vymere okresu medzi dvomi rokmi\n";
			std::cin >> cislo1;

			switch (cislo1)
			{
			case 1:
				std::cout << "Volba: Zoradenie okresov na zaklade zmeny podielu vymery ovocnych sadov voci celkovej vymere okresu medzi dvomi rokmi\n\n";
				std::cout << "Zadajte prvy rok (" << OBEC_UDAJE_ROK_ZACIATOK << " - " << OBEC_UDAJE_ROK_ZACIATOK + OBEC_POCET_ZAZNAMENAVANYCH_ROKOV - 1 << "): ";
				std::cin >> cislo1;
				if (!checkYear(cislo1))
				{
					showMsg("Zadany rok nie je z pozadovaneho rozsahu!");
					break;
				}

				std::cout << "Zadajte druhy rok (" << OBEC_UDAJE_ROK_ZACIATOK << " - " << OBEC_UDAJE_ROK_ZACIATOK + OBEC_POCET_ZAZNAMENAVANYCH_ROKOV - 1 << "): ";
				std::cin >> cislo2;
				if (!checkYear(cislo2))
				{
					showMsg("Zadany rok nie je z pozadovaneho rozsahu!");
					break;
				}

				std::cout << "\nZoradenie\n1. vzostupne\n2. zostupne\n\n";
				std::cin >> cislo3;

				if (cislo3 == 1)
					okresy_->vypisOkresyPodlaZmenyPodieluOvocnychSadov(cislo1, cislo2, true);
				else
					okresy_->vypisOkresyPodlaZmenyPodieluOvocnychSadov(cislo1, cislo2, false);
				break;

			case 2:
				std::cout << "Volba: Zoradenie okresov na zaklade zmeny podielu vymery ornej pody voci celkovej vymere okresu medzi dvomi rokmi\n\n";
				std::cout << "Zadajte prvy rok (" << OBEC_UDAJE_ROK_ZACIATOK << " - " << OBEC_UDAJE_ROK_ZACIATOK + OBEC_POCET_ZAZNAMENAVANYCH_ROKOV - 1 << "): ";
				std::cin >> cislo1;
				if (!checkYear(cislo1))
				{
					showMsg("Zadany rok nie je z pozadovaneho rozsahu!");
					break;
				}

				std::cout << "Zadajte druhy rok (" << OBEC_UDAJE_ROK_ZACIATOK << " - " << OBEC_UDAJE_ROK_ZACIATOK + OBEC_POCET_ZAZNAMENAVANYCH_ROKOV - 1 << "): ";
				std::cin >> cislo2;
				if (!checkYear(cislo2))
				{
					showMsg("Zadany rok nie je z pozadovaneho rozsahu!");
					break;
				}

				std::cout << "\nZoradenie\n1. vzostupne\n2. zostupne\n\n";
				std::cin >> cislo3;

				if (cislo3 == 1)
					okresy_->vypisOkresyPodlaZmenyPodieluOrnejPody(cislo1, cislo2, true);
				else
					okresy_->vypisOkresyPodlaZmenyPodieluOrnejPody(cislo1, cislo2, false);
				break;

			case 3:
				std::cout << "Volba: Zoradenie okresov na zaklade zmeny podielu vymery lesnych pozemkov voci celkovej vymere okresu medzi dvomi rokmi\n\n";
				std::cout << "Zadajte prvy rok (" << OBEC_UDAJE_ROK_ZACIATOK << " - " << OBEC_UDAJE_ROK_ZACIATOK + OBEC_POCET_ZAZNAMENAVANYCH_ROKOV - 1 << "): ";
				std::cin >> cislo1;
				if (!checkYear(cislo1))
				{
					showMsg("Zadany rok nie je z pozadovaneho rozsahu!");
					break;
				}

				std::cout << "Zadajte druhy rok (" << OBEC_UDAJE_ROK_ZACIATOK << " - " << OBEC_UDAJE_ROK_ZACIATOK + OBEC_POCET_ZAZNAMENAVANYCH_ROKOV - 1 << "): ";
				std::cin >> cislo2;
				if (!checkYear(cislo2))
				{
					showMsg("Zadany rok nie je z pozadovaneho rozsahu!");
					break;
				}

				std::cout << "\nZoradenie\n1. vzostupne\n2. zostupne\n\n";
				std::cin >> cislo3;

				if (cislo3 == 1)
					okresy_->vypisOkresyPodlaZmenyPodieluLesnychPozemkov(cislo1, cislo2, true);
				else
					okresy_->vypisOkresyPodlaZmenyPodieluLesnychPozemkov(cislo1, cislo2, false);
				break;

			default:
				break;
			}
			break;

		case 5:
			std::cout << "Volba: Vyhladanie krajov na zaklade:\n\n";
			std::cout << "1. Obsahuju najviac a najmenej obci, ktorych podiel lesov je aspon X % v zadanom roku\n";
			std::cout << "2. Obsahuju aspon X okresov, ktorych podiel lesov z celkovej vymery je aspon Y % v zadanom roku\n";
			std::cout << "3. Obsahuju aspon X % lesov z celkovej vymery kraja v zadanom roku\n";
			std::cin >> cislo1;

			switch (cislo1)
			{
			case 1:
				std::cout << "Volba: Vyhladanie krajov, ktore obsahuju najviac a najmenej obci, ktorych podiel lesov je aspon X % \n\n";
				std::cout << "Zadajte rok (" << OBEC_UDAJE_ROK_ZACIATOK << " - " << OBEC_UDAJE_ROK_ZACIATOK + OBEC_POCET_ZAZNAMENAVANYCH_ROKOV - 1 << "): ";
				std::cin >> cislo1;
				if (!checkYear(cislo1))
				{
					showMsg("Zadany rok nie je z pozadovaneho rozsahu!");
					break;
				}

				std::cout << "Zadajte percentualnu hodnotu vymery lesov: ";
				std::cin >> desCislo;

				kraje_->vypisKrajePodlaPoctuObciPodielLesov(cislo1, desCislo);
				break;

			case 2:
				std::cout << "Volba: Vyhladanie krajov, ktore obsahuju aspon X okresov, ktorych podiel lesov z celkovej vymery je aspon Y % v zadanom roku\n\n";
				std::cout << "Zadajte rok (" << OBEC_UDAJE_ROK_ZACIATOK << " - " << OBEC_UDAJE_ROK_ZACIATOK + OBEC_POCET_ZAZNAMENAVANYCH_ROKOV - 1 << "): ";
				std::cin >> cislo1;
				if (!checkYear(cislo1))
				{
					showMsg("Zadany rok nie je z pozadovaneho rozsahu!");
					break;
				}

				std::cout << "Zadajte pocet okresov: ";
				std::cin >> cislo2;
				std::cout << "Zadajte percentualnu hodnotu vymery lesov: ";
				std::cin >> desCislo;

				kraje_->vypisKrajePodlaPoctuOkresovPodielLesov(cislo1, cislo2, desCislo);
				break;

			case 3:
				std::cout << "Volba: Vyhladanie krajov, ktore obsahuju aspon X % lesov z celkovej vymery kraja v zadanom roku\n\n";
				std::cout << "Zadajte rok (" << OBEC_UDAJE_ROK_ZACIATOK << " - " << OBEC_UDAJE_ROK_ZACIATOK + OBEC_POCET_ZAZNAMENAVANYCH_ROKOV - 1 << "): ";
				std::cin >> cislo1;
				if (!checkYear(cislo1))
				{
					showMsg("Zadany rok nie je z pozadovaneho rozsahu!");
					break;
				}

				std::cout << "Zadajte percentualnu hodnotu vymery lesov: ";
				std::cin >> desCislo;

				kraje_->vypisKrajePodlaPodieluLesov(cislo1, desCislo);
				break;

			default:
				break;
			}
			break;

		case 6:
			std::cout << "Volba: Zoradenie obci na zaklade EA obyvatelstva:\n\n";
			std::cout << "Zadajte pohlavie:\n";
			std::cout << "1. muzi\n";
			std::cout << "2. zeny\n";
			std::cout << "0. nespecifikovane\n";
			std::cin >> cislo1;

			std::cout << "\n\nZadajte nazov kraja:\n";
			std::cin >> value1;
			value1.erase(std::remove(value1.begin(), value1.end(), '\n'), value1.end());	// odstrani '\n'
			kraj = kraje_->dajKrajPodlaNazvu(value1);

			std::cout << "\n\nZadajte nazov okresu:\n";
			std::cin >> value2;
			value2.erase(std::remove(value2.begin(), value2.end(), '\n'), value2.end());	// odstrani '\n'
			okres = okresy_->dajOkresPodlaNazvu(value2);

			switch (cislo1)
			{
			case 1:
				pohlavie = MUZI;
				break;

			case 2:
				pohlavie = ZENY;
				break;

			default:
				pohlavie = SPOLU;
				break;
			}

			std::cout << "\nZoradenie\n1. vzostupne\n2. zostupne\n\n";
			std::cin >> cislo2;

			if (cislo2 == 1)
				obce_->vypisObcePodlaObyvatelstva(pohlavie, kraj, okres, true);
			else
				obce_->vypisObcePodlaObyvatelstva(pohlavie, kraj, okres, false);
			break;

		case 7:
			std::cout << "Volba: Zapisanie udajov o obciach do suboru\n\n";
			std::cout << "Zadajte nazov suboru:\n";
			std::cin >> value1;
			std::getline(std::cin, value2);

			std::cout << "Zadajte rok (" << OBEC_UDAJE_ROK_ZACIATOK << " - " << OBEC_UDAJE_ROK_ZACIATOK + OBEC_POCET_ZAZNAMENAVANYCH_ROKOV - 1 << "): ";
			std::cin >> cislo1;
			if (!checkYear(cislo1))
			{
				showMsg("Zadany rok nie je z pozadovaneho rozsahu!");
				break;
			}

			std::cout << "\n\nZadajte nazov kraja z ktoreho maju byt obce:\n";
			std::cin >> value2;
			value2.erase(std::remove(value2.begin(), value2.end(), '\n'), value2.end());	// odstrani '\n'
			kraj = kraje_->dajKrajPodlaNazvu(value2);

			std::cout << "\n\nZadajte nazov okresu:\n";
			std::getline(std::cin, value3);
			std::getline(std::cin, value3);
			value3.erase(std::remove(value3.begin(), value3.end(), '\n'), value3.end());	// odstrani '\n'
			okres = okresy_->dajOkresPodlaNazvu(value3);

			zapisDataDoSuboru(value1, cislo1, kraj, okres);
			std::cout << "\nData uspesne zapisane do suboru " << value1 << "\n\n";
			break;

		case 8:
			std::cout << "Volba: Riesenie optimalizacneho problemu\n\n";

			std::cout << "\n\nZadajte nazov kraja z ktoreho maju byt obce:\n";
			std::cin >> value2;
			value2.erase(std::remove(value2.begin(), value2.end(), '\n'), value2.end());	// odstrani '\n'
			kraj = kraje_->dajKrajPodlaNazvu(value2);

			std::cout << "\n\nZadajte nazov okresu:\n";
			std::getline(std::cin, value3);
			std::getline(std::cin, value3);
			value3.erase(std::remove(value3.begin(), value3.end(), '\n'), value3.end());	// odstrani '\n'
			okres = okresy_->dajOkresPodlaNazvu(value3);

			obce = obce_->dajObcePodlaKriterii(kraj, okres);
			heuristika.riesProblemPreDaneObce(obce);
			delete obce;
			break;

		default:
			break;
		}
	}
	while (volba != 0);
}
