#pragma once
#include "Comparator.h"
#include "Okres.h"

class ComparatorOkresyZmenaPodieluOrnaPoda :
	public Comparator<int, Okres*>
{
public:
	ComparatorOkresyZmenaPodieluOrnaPoda(int rok1, int rok2);
	~ComparatorOkresyZmenaPodieluOrnaPoda();

	bool compare(structures::TableItem<int, Okres*> tableItem1, structures::TableItem<int, Okres*> tableItem2) override;

private:
	int rok1_;
	int rok2_;
};



inline ComparatorOkresyZmenaPodieluOrnaPoda::ComparatorOkresyZmenaPodieluOrnaPoda(int rok1, int rok2)
	:rok1_(rok1), rok2_(rok2)
{
}


inline ComparatorOkresyZmenaPodieluOrnaPoda::~ComparatorOkresyZmenaPodieluOrnaPoda()
{
}

inline bool ComparatorOkresyZmenaPodieluOrnaPoda::compare(structures::TableItem<int, Okres*> tableItem1,
	structures::TableItem<int, Okres*> tableItem2)
{
	double zmenaPodielu1 = tableItem1.accessData()->dajZmenuPodieluOrnejPody(rok1_, rok2_);
	double zmenaPodielu2 = tableItem2.accessData()->dajZmenuPodieluOrnejPody(rok1_, rok2_);

	return zmenaPodielu1 > zmenaPodielu2;
}
