#pragma once
#include "structures/table/unsorted_sequence_table.h"

namespace structures
{

	/// <summary> Sekvencna neutriedena tabulka, ktora nekontroluje unikatnost klucov. </summary>
	/// <typeparam name = "K"> Kluc prvkov v tabulke. </typepram>
	/// <typeparam name = "T"> Typ dat ukladanych v tabulke. </typepram>
	template <typename K, typename T>
	class UnsortedSequenceTableNoUnique
		: public structures::UnsortedSequenceTable<K, T>
	{
	public:
		UnsortedSequenceTableNoUnique();
		~UnsortedSequenceTableNoUnique();

		/// <summary> Vlozi data s danym klucom do tabulky. Nekontroluje unikatnost kluca! Zlozitost O(1). </summary>
		/// <param name = "key"> Kluc vkladanych dat. </param>
		/// <param name = "data"> Vkladane data. </param>  
		void insert(const K& key, const T& data) override;
	};


	template <typename K, typename T>
	UnsortedSequenceTableNoUnique<K, T>::UnsortedSequenceTableNoUnique()
	{
	}

	template <typename K, typename T>
	UnsortedSequenceTableNoUnique<K, T>::~UnsortedSequenceTableNoUnique()
	{
	}

	template <typename K, typename T>
	void UnsortedSequenceTableNoUnique<K, T>::insert(const K& key, const T& data)
	{
		structures::TableItem<K, T>* tableItem = new structures::TableItem<K, T>(key, data);
		this->list_->add(tableItem);
	}

}