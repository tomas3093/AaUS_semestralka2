#pragma once
#include "Comparator.h"
#include "Okres.h"

class ComparatorOkresyZmenaPodieluOvocneSady :
	public Comparator<int, Okres*>
{
public:
	ComparatorOkresyZmenaPodieluOvocneSady(int rok1, int rok2);
	~ComparatorOkresyZmenaPodieluOvocneSady();

	bool compare(structures::TableItem<int, Okres*> tableItem1, structures::TableItem<int, Okres*> tableItem2) override;

private:
	int rok1_;
	int rok2_;
};



inline ComparatorOkresyZmenaPodieluOvocneSady::ComparatorOkresyZmenaPodieluOvocneSady(int rok1, int rok2)
	:rok1_(rok1), rok2_(rok2)
{
}


inline ComparatorOkresyZmenaPodieluOvocneSady::~ComparatorOkresyZmenaPodieluOvocneSady()
{
}

inline bool ComparatorOkresyZmenaPodieluOvocneSady::compare(structures::TableItem<int, Okres*> tableItem1,
	structures::TableItem<int, Okres*> tableItem2)
{
	double zmenaPodielu1 = tableItem1.accessData()->dajZmenuPodieluOvocnychSadov(rok1_, rok2_);
	double zmenaPodielu2 = tableItem2.accessData()->dajZmenuPodieluOvocnychSadov(rok1_, rok2_);

	return zmenaPodielu1 > zmenaPodielu2;
}
