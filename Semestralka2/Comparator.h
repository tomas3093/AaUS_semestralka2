#pragma once
#include "structures/table/table.h"

/// <summary> Reprezentuje vseobecny komparator, ktory je schopny porovnat dva objekty. </summary>
template <typename K, typename T>
class Comparator
{
public:
	Comparator();
	virtual ~Comparator();

	/// <summary> Porovna dva TableItemy. Vrati true ak nie su v spravnom poradi, inak false. </summary>
	virtual bool compare(structures::TableItem<K, T> tableItem1, structures::TableItem<K, T> tableItem2) = 0;
};

template <typename K, typename T>
inline Comparator<K, T>::Comparator()
{
}

template <typename K, typename T>
inline Comparator<K, T>::~Comparator()
{
}
