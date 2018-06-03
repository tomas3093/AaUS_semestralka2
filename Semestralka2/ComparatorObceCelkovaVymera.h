#pragma once
#include "Comparator.h"
#include "Obec.h"

class ComparatorObceCelkovaVymera :
	public Comparator<int, Obec*>
{
public:
	ComparatorObceCelkovaVymera(int rok);
	~ComparatorObceCelkovaVymera();

	bool compare(structures::TableItem<int, Obec*> tableItem1, structures::TableItem<int, Obec*> tableItem2) override;
	
private:
	int rok_;
};


inline ComparatorObceCelkovaVymera::ComparatorObceCelkovaVymera(int rok) 
:rok_(rok)
{
}

inline ComparatorObceCelkovaVymera::~ComparatorObceCelkovaVymera()
{
}

inline bool ComparatorObceCelkovaVymera::compare(structures::TableItem<int, Obec*> tableItem1,
	structures::TableItem<int, Obec*> tableItem2)
{
	return tableItem1.accessData()->dajVymeru(rok_, CELKOVA) > tableItem2.accessData()->dajVymeru(rok_, CELKOVA);
}
