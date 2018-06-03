#pragma once
class EkonomAktObyvatelstvo
{
public:
	EkonomAktObyvatelstvo(int pocetMuzov, int pocetZien);
	~EkonomAktObyvatelstvo();

	int dajObyvatelstvo(Pohlavie pohlavie);

private:
	int pocetMuzov;
	int pocetZien;
};



inline EkonomAktObyvatelstvo::EkonomAktObyvatelstvo(int pocetMuzov, int pocetZien)
	:pocetMuzov(pocetMuzov), pocetZien(pocetZien)
{
}


inline EkonomAktObyvatelstvo::~EkonomAktObyvatelstvo()
{
}

inline int EkonomAktObyvatelstvo::dajObyvatelstvo(Pohlavie pohlavie)
{
	switch (pohlavie)
	{
	case MUZI:
		return pocetMuzov;

	case ZENY:
		return pocetZien;

	case SPOLU:
	default:
		return pocetMuzov + pocetZien;
	}
}
