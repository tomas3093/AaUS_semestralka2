#pragma once

#include "sort.h"
#include "../unsorted_sequence_table.h"

namespace structures
{
	/// <summary> Triedenie Bubble sort. </summary>
	/// <typeparam name = "K"> Kluc prvkov v tabulke. </typepram>
	/// <typeparam name = "T"> Typ dat ukladanych v tabulke. </typepram>
	template <typename K, typename T>
	class BubbleSort : public Sort<K, T>
	{
	public:
		/// <summary> Utriedi tabulku triedenim Bubble sort. </summary>
		/// <param name = "table"> UnsortedSequenceTable, ktoru ma utriedit. </param>
		void sort(UnsortedSequenceTable<K, T>& table, Comparator<K, T>& comparator) override;
	};

	template<typename K, typename T>
	inline void BubbleSort<K, T>::sort(UnsortedSequenceTable<K, T>& table, Comparator<K, T>& comparator)
	{
		bool vymena;
		do {
			vymena = false;
			for (int i = 0; i < table.size() - 1; i++) {
				if (comparator.compare(table.getItemAtIndex(i), table.getItemAtIndex(i + 1))) {
					table.swap(i, i + 1);
					vymena = true;
				}
			}
		} while (vymena);
	}

}