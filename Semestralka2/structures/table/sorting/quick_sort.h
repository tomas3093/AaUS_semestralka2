#pragma once

#include "sort.h"
#include "../unsorted_sequence_table.h"

namespace structures
{
	/// <summary> Triedenie Quick sort. </summary>
	/// <typeparam name = "K"> Kluc prvkov v tabulke. </typepram>
	/// <typeparam name = "T"> Typ dat ukladanych v tabulke. </typepram>
	template <typename K, typename T>
	class QuickSort : public Sort<K, T>
	{
	public:
		/// <summary> Utriedi tabulku triedenim Quick sort. </summary>
		/// <param name = "table"> NonortedSequenceTable, ktoru ma utriedit. </param>
		void sort(UnsortedSequenceTable<K, T>& table, Comparator<K, T>& comparator) override;

		void quick(int min, int max, UnsortedSequenceTable<K, T>& table, Comparator<K, T>& comparator);
	};

	template<typename K, typename T>
	inline void QuickSort<K, T>::sort(UnsortedSequenceTable<K, T>& table, Comparator<K, T>& comparator)
	{
		quick(0, table.size() - 1, table, comparator);
	}

	template <typename K, typename T>
	void QuickSort<K, T>::quick(int min, int max, UnsortedSequenceTable<K, T>& table, Comparator<K, T>& comparator)
	{
		TableItem<K, T> pivot = table.getItemAtIndex((min + max) / 2);

		int lavy = min;
		int pravy = max;

		do
		{
			while (comparator.compare(pivot, table.getItemAtIndex(lavy)))
				lavy++;
			while (comparator.compare(table.getItemAtIndex(pravy), pivot))
				pravy--;
			if (lavy <= pravy)
			{
				table.swap(lavy, pravy);
				lavy++;
				pravy--;
			}
		} while (lavy <= pravy);

		if (min < pravy)
			quick(min, pravy, table, comparator);

		if (lavy < max)
			quick(lavy, max, table, comparator);
	}
}
