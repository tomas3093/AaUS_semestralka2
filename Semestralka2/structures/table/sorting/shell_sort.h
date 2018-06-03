#pragma once

#include "sort.h"
#include "../unsorted_sequence_table.h"

namespace structures
{

	/// <summary> Triedenie Shell sort. </summary>
	/// <typeparam name = "K"> Kluc prvkov v tabulke. </typepram>
	/// <typeparam name = "T"> Typ dat ukladanych v tabulke. </typepram>
	template <typename K, typename T>
	class ShellSort : public Sort<K, T>
	{
	public:
		/// <summary> Utriedi tabulku triedenim Shell sort. </summary>
		/// <param name = "table"> NonortedSequenceTable, ktoru ma utriedit. </param>
		void sort(UnsortedSequenceTable<K, T>& table, Comparator<K, T>& comparator) override;

		void shell(int krok, UnsortedSequenceTable<K, T>& table, Comparator<K, T>& comparator);
	};

	template<typename K, typename T>
	inline void ShellSort<K, T>::sort(UnsortedSequenceTable<K, T>& table, Comparator<K, T>& comparator)
	{
		shell(log10(table.size()), table, comparator);
	}

	template <typename K, typename T>
	void ShellSort<K, T>::shell(int krok, UnsortedSequenceTable<K, T>& table, Comparator<K, T>& comparator)
	{
		for (int delta = 0; delta < krok - 1; ++delta)
		{
			for (int i = delta; i < table.size() - 1; i += krok)
			{
				int j = i;
				while ((j - krok >= delta) && !comparator.compare(table.getItemAtIndex(j), table.getItemAtIndex(j - krok)))
				{
					table.swap(j, j - krok);
					j -= krok;
				}
			}
		}
		if (krok > 1)
		{
			shell(krok - 1, table, comparator);
		}
	}
}
