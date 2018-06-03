#pragma once

#include "table.h"
#include "../list/list.h"
#include <stdexcept>

namespace structures 
{

	/// <summary> Sekvencna tabulka. </summary>
	/// <typeparam name = "K"> Kluc prvkov v tabulke. </typepram>
	/// <typeparam name = "T"> Typ dat ukladanych v tabulke. </typepram>
	template <typename K, typename T>
	class SequenceTable : public Table<K, T>
	{
	public:
		/// <summary> Destruktor. </sumary>
		~SequenceTable();

		/// <summary> Operacia klonovania. Vytvori a vrati duplikat udajovej struktury. </summary>
		/// <returns> Ukazovatel na klon struktury. </returns>
		virtual Structure* clone() const = 0;

		/// <summary> Vrati pocet prvkov v tabulke. </summary>
		/// <returns> Pocet prvkov v tabulke. </returns>
		size_t size() const override;

		/// <summary> Operator priradenia. </summary>
		/// <param name = "other"> Tabulka, z ktorej ma prebrat vlastnosti. </param>
		/// <returns> Adresa, na ktorej sa tato tabulka nachadza po priradeni. </returns>
		Table<K, T>& operator=(const Table<K, T>& other) override;

		/// <summary> Operator priradenia. </summary>
		/// <param name = "other"> Sekvencna tabulka, z ktorej ma prebrat vlastnosti. </param>
		/// <returns> Adresa, na ktorej sa tato tabulka nachadza po priradeni. </returns>
		SequenceTable<K, T>& operator=(const SequenceTable<K, T>& other);

		/// <summary> Vrati adresou data s danym klucom. </summary>
		/// <param name = "key"> Kluc dat. </param>
		/// <returns> Adresa dat s danym klucom. </returns>
		/// <exception cref="std::out_of_range"> Vyhodena, ak kluc nepatri do tabulky. </exception>  
		T& operator[](const K key) override;

		/// <summary> Vrati hodnotou data s daynm klucom. </summary>
		/// <param name = "key"> Kluc dat. </param>
		/// <returns> Hodnota dat s danym klucom. </returns>
		/// <exception cref="std::out_of_range"> Vyhodena, ak kluc nepatri do zoznamu. </exception>  
		const T operator[](const K key) const override;

		/// <summary> Vlozi data s danym klucom do tabulky. </summary>
		/// <param name = "key"> Kluc vkladanych dat. </param>
		/// <param name = "data"> Vkladane data. </param>
		/// <exception cref="std::logic_error"> Vyhodena, ak tabulka uz obsahuje data s takymto klucom. </exception>  
		void insert(const K& key, const T& data) override;

		/// <summary> Odstrani z tabulky prvok s danym klucom. </summary>
		/// <param name = "key"> Kluc prvku. </param>
		/// <returns> Odstranene data. </returns>
		/// <exception cref="std::logic_error"> Vyhodena, ak tabulka neobsahuje data s takymto klucom. </exception>  
		T remove(const K& key) override;

		/// <summary> Bezpecne ziska data s danym klucom. </summary>
		/// <param name = "key"> Kluc dat. </param>
		/// <param name = "data"> Najdene data (vystupny parameter). </param>
		/// <returns> true, ak sa podarilo najst a naplnit data s danym klucom, false inak. </returns>
		bool tryFind(const K& key, T& data) override;

		/// <summary> Zisti, ci tabulka obsahuje data s danym klucom. </summary>
		/// <param name = "key"> Kluc dat. </param>
		/// <returns> true, tabulka obsahuje dany kluc, false inak. </returns>
		bool containsKey(const K& key) override;

		/// <summary> Vymaze tabulku. </summary>
		void clear() override;

		/// <summary> Vrati skutocny iterator na zaciatok struktury </summary>
		/// <returns> Iterator na zaciatok struktury. </returns>
		/// <remarks> Zabezpecuje polymorfizmus. </remarks>
		Iterator<TableItem<K, T>*>* getBeginIterator() const override;

		/// <summary> Vrati skutocny iterator na koniec struktury </summary>
		/// <returns> Iterator na koniec struktury. </returns>
		/// <remarks> Zabezpecuje polymorfizmus. </remarks>
		Iterator<TableItem<K, T>*>* getEndIterator() const override;
	protected:
		/// <summary> Konstruktor. </summary>
		/// <param name = "list"> Zoznam, do ktoreho sa budu ukladat prvky tabulky. </param>
		/// <remarks> 
		/// Potomkovia ako list vlozia instanciu implicitne alebo explicitne implementovaneho zoznamu. 
		/// List dealokuje tento objekt.
		/// </remarks>
		SequenceTable(List<TableItem<K, T>*>* list);

		/// <summary> Najde prvok tabulky s danym klucom. </summary>
		/// <param name = "key"> Hladany kluc. </param>
		/// <returns> Prvok tabulky s danym klucom, ak neexistuje, vrati nullptr. </returns>
		virtual TableItem<K, T>* findTableItem(const K& key) const;
	protected:
		List<TableItem<K, T>*>* list_;
	};

	template<typename K, typename T>
	SequenceTable<K, T>::~SequenceTable()
	{
		this->clear();
		delete list_;
		list_ = nullptr;
	}

	template<typename K, typename T>
	size_t SequenceTable<K, T>::size() const
	{
		return list_->size();
	}

	template<typename K, typename T>
	Table<K, T>& SequenceTable<K, T>::operator=(const Table<K, T>& other)
	{
		if (this != &other)
		{
			*this = dynamic_cast<const SequenceTable<K, T>&>(other);
		}
		return *this;
	}

	template<typename K, typename T>
	SequenceTable<K, T>& SequenceTable<K, T>::operator=(const SequenceTable<K, T>& other)
	{
		if (this != &other)
		{
			clear();
			for (TableItem<K, T>* item : *other.list_)
			{
				list_->add(new TableItem<K, T>(*item));
			}
		}
		return *this;
	}

	template<typename K, typename T>
	T & SequenceTable<K, T>::operator[](const K key)
	{
		TableItem<K, T>* tableItem = findTableItem(key);
		if (tableItem != nullptr)
		{
			return tableItem->accessData();
		}
		else
		{
			throw std::out_of_range("SequenceTable<K, T>::operator[]: Data not found!");
		}
	}

	template<typename K, typename T>
	const T SequenceTable<K, T>::operator[](const K key) const
	{
		TableItem<K, T>* tableItem = findTableItem(key);
		if (tableItem != nullptr)
		{
			T result = tableItem->accessData();
			return result;
		}
		else
		{
			throw std::out_of_range("SequenceTable<K, T>::operator[]: Data not found!");
		}
	}

	template<typename K, typename T>
	void SequenceTable<K, T>::insert(const K & key, const T & data)
	{
		if (!containsKey(key))
		{
			TableItem<K, T>* tableItem = new TableItem<K, T>(key, data);
			list_->add(tableItem);
		}
		else
		{
			throw std::logic_error("SequenceTable<K, T>::insert: Key already present in the table!");
		}
	}

	template<typename K, typename T>
	T SequenceTable<K, T>::remove(const K & key)
	{
		TableItem<K, T>* tableItem = findTableItem(key);
		if (tableItem != nullptr)
		{
			list_->tryRemove(tableItem);
			T result = tableItem->accessData();
			delete tableItem;
			return result;
		}
		else
		{
			throw std::logic_error("SequenceTable<K, T>::remove: Key not found!");
		}
	}

	template<typename K, typename T>
	bool SequenceTable<K, T>::tryFind(const K & key, T & data)
	{
		TableItem<K, T>* tableItem = findTableItem(key);
		if (tableItem != nullptr)
		{
			data = tableItem->accessData();
			return true;
		}
		else
		{
			return false;
		}
	}

	template<typename K, typename T>
	bool SequenceTable<K, T>::containsKey(const K & key)
	{
		T data;
		return tryFind(key, data);
	}

	template<typename K, typename T>
	void SequenceTable<K, T>::clear()
	{
		for (auto tableItem : *this)
		{
			delete tableItem;
		}
		list_->clear();
	}

	template<typename K, typename T>
	Iterator<TableItem<K, T>*>* SequenceTable<K, T>::getBeginIterator() const
	{
		return list_->getBeginIterator();
	}

	template<typename K, typename T>
	Iterator<TableItem<K, T>*>* SequenceTable<K, T>::getEndIterator() const
	{
		return list_->getEndIterator();
	}

	template<typename K, typename T>
	SequenceTable<K, T>::SequenceTable(List<TableItem<K, T>*>* list) :
		Table<K, T>(),
		list_(list)
	{
	}

	template<typename K, typename T>
	TableItem<K, T>* SequenceTable<K, T>::findTableItem(const K & key) const
	{
		for (TableItem<K, T>* tableItem : *this)
		{
			if (tableItem->getKey() == key)
			{
				return tableItem;
			}
		}

		return nullptr;
	}

}