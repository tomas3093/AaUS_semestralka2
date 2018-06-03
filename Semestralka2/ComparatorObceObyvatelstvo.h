#pragma once
#include "Comparator.h"
#include "Obec.h"
#include "Kraj.h"


class ComparatorObceObyvatelstvo :
	public Comparator<int, Obec*>
{
public:
	ComparatorObceObyvatelstvo(Pohlavie pohlavie);
	~ComparatorObceObyvatelstvo();

	bool compare(structures::TableItem<int, Obec*> tableItem1, structures::TableItem<int, Obec*> tableItem2) override;

private:
	Pohlavie pohlavie_;
};


inline ComparatorObceObyvatelstvo::ComparatorObceObyvatelstvo(Pohlavie pohlavie)
	:pohlavie_(pohlavie)
{
}

inline ComparatorObceObyvatelstvo::~ComparatorObceObyvatelstvo()
{
}

inline bool ComparatorObceObyvatelstvo::compare(structures::TableItem<int, Obec*> tableItem1,
	structures::TableItem<int, Obec*> tableItem2)
{
	switch (pohlavie_)
	{
	case MUZI:
		return tableItem1.accessData()->dajObyvatelstvo(MUZI) > tableItem2.accessData()->dajObyvatelstvo(MUZI);

	case ZENY:
		return tableItem1.accessData()->dajObyvatelstvo(ZENY) > tableItem2.accessData()->dajObyvatelstvo(ZENY);

	case SPOLU:
	default:
		return tableItem1.accessData()->dajObyvatelstvo(SPOLU) > tableItem2.accessData()->dajObyvatelstvo(SPOLU);
	}
}
