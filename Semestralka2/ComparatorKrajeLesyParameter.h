#pragma once
#include "Comparator.h"
#include "Kraj.h"

class ComparatorKrajeLesyParameter :
	public Comparator<int, Kraj*>
{
public:
	ComparatorKrajeLesyParameter(int rok, double percentHodnota);
	~ComparatorKrajeLesyParameter();

	bool compare(structures::TableItem<int, Kraj*> tableItem1, structures::TableItem<int, Kraj*> tableItem2) override;

private:
	int rok_;
	double percentHodnota_;
};



inline ComparatorKrajeLesyParameter::ComparatorKrajeLesyParameter(int rok, double percentHodnota)
	:rok_(rok), percentHodnota_(percentHodnota)
{
}


inline ComparatorKrajeLesyParameter::~ComparatorKrajeLesyParameter()
{
}

inline bool ComparatorKrajeLesyParameter::compare(structures::TableItem<int, Kraj*> tableItem1,
	structures::TableItem<int, Kraj*> tableItem2)
{
	double podiel1 = double(tableItem1.accessData()->dajVymeru(rok_, LESNE_POZEMKY)) / double(tableItem1.accessData()->dajVymeru(rok_, CELKOVA));
	double podiel2 = double(tableItem2.accessData()->dajVymeru(rok_, LESNE_POZEMKY)) / double(tableItem2.accessData()->dajVymeru(rok_, CELKOVA));

	return podiel1 > podiel2;
}
