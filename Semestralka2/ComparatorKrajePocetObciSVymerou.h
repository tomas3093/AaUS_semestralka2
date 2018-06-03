#pragma once
#include "Comparator.h"
#include "structures/list/list.h"
#include "Obec.h"
#include "Kraj.h"

class ComparatorKrajePocetObciSVymerou :
	public Comparator<Kraj*, structures::List<Obec*>*>
{
public:
	ComparatorKrajePocetObciSVymerou();
	~ComparatorKrajePocetObciSVymerou();

	bool compare(structures::TableItem<Kraj*, structures::List<Obec*>*> tableItem1,
		structures::TableItem<Kraj*, structures::List<Obec*>*> tableItem2) override;
};



inline ComparatorKrajePocetObciSVymerou::ComparatorKrajePocetObciSVymerou()
{
}


inline ComparatorKrajePocetObciSVymerou::~ComparatorKrajePocetObciSVymerou()
{
}

inline bool ComparatorKrajePocetObciSVymerou::compare(structures::TableItem<Kraj*, structures::List<Obec*>*> tableItem1,
	structures::TableItem<Kraj*, structures::List<Obec*>*> tableItem2)
{
	return tableItem1.accessData()->size() > tableItem2.accessData()->size();
}
