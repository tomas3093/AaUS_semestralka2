#pragma once

#include "../unsorted_sequence_table.h"
#include "../../../Comparator.h"

namespace structures
{
	/// <summary> Triedenie UnsortedSequenceTable navrhnute ako funkcny objekt. </summary>
	/// <typeparam name = "K"> Kluc prvkov v tabulke. </typepram>
	/// <typeparam name = "T"> Typ dat ukladanych v tabulke. </typepram>
	template <typename K, typename T>
	class Sort
	{
	public:
		/// <summary> Konstruktor. </summary>
		Sort();

		/// <summary> Destruktor. </summary>
		virtual ~Sort();

		/// <summary> Funkcny operátor utriedi tabulku. </summary>
		/// <param name = "table"> UnsortedSequenceTable, ktoru ma utriedit. </param>
		/// <remarks> Využíva metódu sort. </remarks>
		void operator()(UnsortedSequenceTable<K, T>& table, Comparator<K, T>& comparator);

		/// <summary> Utriedi tabulku. </summary>
		/// <param name = "table"> UnsortedSequenceTable, ktoru ma utriedit. </param>
		virtual void sort(UnsortedSequenceTable<K, T>& table, Comparator<K, T>& comparator) = 0;
	};

	template<typename K, typename T>
	inline Sort<K, T>::Sort()
	{
	}

	template<typename K, typename T>
	inline Sort<K, T>::~Sort()
	{
	}

	template<typename K, typename T>
	inline void Sort<K, T>::operator()(UnsortedSequenceTable<K, T>& table, Comparator<K, T>& comparator)
	{
		sort(table, comparator);
	}
}
