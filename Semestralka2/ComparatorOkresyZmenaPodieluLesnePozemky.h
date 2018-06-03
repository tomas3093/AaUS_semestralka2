#pragma once
#include "Comparator.h"
#include "Okres.h"

class ComparatorOkresyZmenaPodieluLesnePozemky :
	public Comparator<int, Okres*>
{
public:
	ComparatorOkresyZmenaPodieluLesnePozemky(int rok1, int rok2);
	~ComparatorOkresyZmenaPodieluLesnePozemky();

	bool compare(structures::TableItem<int, Okres*> tableItem1, structures::TableItem<int, Okres*> tableItem2) override;

private:
	int rok1_;
	int rok2_;
};



inline ComparatorOkresyZmenaPodieluLesnePozemky::ComparatorOkresyZmenaPodieluLesnePozemky(int rok1, int rok2)
	:rok1_(rok1), rok2_(rok2)
{
}


inline ComparatorOkresyZmenaPodieluLesnePozemky::~ComparatorOkresyZmenaPodieluLesnePozemky()
{
}

inline bool ComparatorOkresyZmenaPodieluLesnePozemky::compare(structures::TableItem<int, Okres*> tableItem1,
	structures::TableItem<int, Okres*> tableItem2)
{
	double zmenaPodielu1 = tableItem1.accessData()->dajZmenuPodieluLesnychPozemkov(rok1_, rok2_);
	double zmenaPodielu2 = tableItem2.accessData()->dajZmenuPodieluLesnychPozemkov(rok1_, rok2_);

	return zmenaPodielu1 > zmenaPodielu2;
}
