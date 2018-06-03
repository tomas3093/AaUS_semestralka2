#pragma once
#include "Comparator.h"
#include "Obec.h"

class ComparatorObcePrirastokZastPloch :
	public Comparator<int, Obec*>
{
public:
	ComparatorObcePrirastokZastPloch(int rok);
	~ComparatorObcePrirastokZastPloch();

	bool compare(structures::TableItem<int, Obec*> tableItem1, structures::TableItem<int, Obec*> tableItem2) override;

private:
	int rok_;
};



inline ComparatorObcePrirastokZastPloch::ComparatorObcePrirastokZastPloch(int rok)
	:rok_(rok)
{
}


inline ComparatorObcePrirastokZastPloch::~ComparatorObcePrirastokZastPloch()
{
}

inline bool ComparatorObcePrirastokZastPloch::compare(structures::TableItem<int, Obec*> tableItem1,
	structures::TableItem<int, Obec*> tableItem2)
{
	double prirastok1 = tableItem1.accessData()->dajPrirastokZastavanychPloch(rok_);
	double prirastok2 = tableItem2.accessData()->dajPrirastokZastavanychPloch(rok_);

	return prirastok1 > prirastok2;
}
