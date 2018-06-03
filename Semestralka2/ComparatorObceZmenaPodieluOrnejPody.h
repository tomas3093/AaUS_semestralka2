#pragma once
#include "Comparator.h"
#include "Obec.h"

class ComparatorObceZmenaPodieluOrnejPody :
	public Comparator<int, Obec*>
{
public:
	ComparatorObceZmenaPodieluOrnejPody(int rok1, int rok2);
	~ComparatorObceZmenaPodieluOrnejPody();

	bool compare(structures::TableItem<int, Obec*> tableItem1, structures::TableItem<int, Obec*> tableItem2) override;

private:
	int rok1_;
	int rok2_;
};



inline ComparatorObceZmenaPodieluOrnejPody::ComparatorObceZmenaPodieluOrnejPody(int rok1, int rok2)
	:rok1_(rok1), rok2_(rok2)
{

}


inline ComparatorObceZmenaPodieluOrnejPody::~ComparatorObceZmenaPodieluOrnejPody()
{
}

inline bool ComparatorObceZmenaPodieluOrnejPody::compare(structures::TableItem<int, Obec*> tableItem1,
	structures::TableItem<int, Obec*> tableItem2)
{
	double zmenaPodielu1 = tableItem1.accessData()->dajZmenuPodieluOrnejPody(rok1_, rok2_);
	double zmenaPodielu2 = tableItem2.accessData()->dajZmenuPodieluOrnejPody(rok1_, rok2_);

	return zmenaPodielu1 > zmenaPodielu2;
}
