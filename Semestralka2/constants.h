#pragma once

#include <string>
#include <iostream>

const size_t OBEC_POCET_ZAZNAMENAVANYCH_ROKOV = 22;
const size_t OBEC_POCET_ZAZNAMENAVANYCH_UDAJOV = 6;
const size_t OBEC_UDAJE_ROK_ZACIATOK = 1996;
const size_t OBEC_UDAJE_OBYVATELSTVO_POCET_VEK_KATEGORII = 48;

// SUBORY
const std::string KRAJE_VSTUP_SUBOR = "data/kraje_id";
const std::string OKRESY_VSTUP_SUBOR = "data/okresy_id";
const std::string OBCE_VSTUP_SUBOR = "data/obce_okresy_id";
const std::string OBCE_VYMERY_VSTUP_SUBOR = "data/vymera_raw";
const std::string OBCE_OBYVATELSTVO_VSTUP_SUBOR = "data/obyvatelstvo_raw";


enum VymeraPolozka
{
	CELKOVA = 0,
	ORNA_PODA = 1,
	OVOCNE_SADY = 2,
	VODNE_PLOCHY = 3,
	LESNE_POZEMKY = 4,
	ZASTAVANE_PLOCHY = 5
};

inline VymeraPolozka intToPolozka(int polozka)
{
	switch (polozka)
	{
	case 0:
		return CELKOVA;
	case 1:
		return ORNA_PODA;
	case 2:
		return OVOCNE_SADY;
	case 3:
		return VODNE_PLOCHY;
	case 4:
		return LESNE_POZEMKY;
	case 5:
		return ZASTAVANE_PLOCHY;
	default:
		return CELKOVA;
	}
}

enum Pohlavie
{
	MUZI = 0,
	ZENY = 1,
	SPOLU = 2
};

inline void showMsg(std::string message)
{
	std::cout << "\n\n" << message << "\n\n";
}

// Vrati bool na zaklade toho ci zadane cislo x patri do rozsahu rokov pre ktore mame udaje
inline bool checkYear(int x)
{
	return x >= OBEC_UDAJE_ROK_ZACIATOK && x <= OBEC_UDAJE_ROK_ZACIATOK + OBEC_POCET_ZAZNAMENAVANYCH_ROKOV - 1;
}