#pragma once
#include "Obec.h"
#include "structures/table/treap.h"
#include "structures/list/array_list.h"
#include "structures/table/UnsortedSequenceTableNoUnique.h"
#include "Kraj.h"

/// <summary> Trieda reprezentuje evidenciu vsetkych obci SR. </summary>
class EvidenciaObci
{
public:
	/// <summary> Konstruktor. </summary>
	EvidenciaObci();

	/// <summary> Destruktor. </summary>
	~EvidenciaObci();

	/// <summary> Vrati obec podla zadaneho nazvu a id okresu. Ak sa ziadna obec so zadanym nazvom nenasla, 
	///vrati nullptr a v parametri naslaSa false, inak true. Ak sa nasla ale nenachadza sa v zadanom okrese, vrati nullptr. </summary>
	Obec* dajObecPodlaNazvu(std::string nazovObce, bool& naslaSa, int idOkresu = 0);

	/// <summary> Vypise pozadovane udaje o zadanej obci v jednotlivych rokoch. </summary>
	void vypisUdajeVymeraObce(int rok1, int rok2, std::string nazovObce, int idOkresu = 0);

	/// <summary> Nacita vsetky obce a ich udaje zo suboru. </summary>
	void nacitajSa(std::ifstream* infile);

	structures::Table<int, Obec*>* dajVsetkyObcePodlaId() { return obcePodlaId_; }

	/// <summary> Vypise zoradene obce podla celkovej vymery v danom roku zostupne alebo vzostupne podla parametra. </summary>
	void vypisObcePodlaCelkovejVymery(int rok, bool vzostupne);

	/// <summary> Vypise zoradene obce podla prirastku zastavanych ploch voci celkovej vymere obce v danom roku. </summary>
	void vypisObcePodlaPrirastku(int rok, bool vzostupne);

	/// <summary> Vypise zoradene obce podla zmeny podielu ornej pody voci celkovej vymere obce medzi dvoma rokmi. </summary>
	void vypisObcePodlaZmenyPodieluOrnejPody(int rok1, int rok2, bool vzostupne);

	/// <summary> Vypise zoradene obce na zaklade obyvatelstva podla zadanych parametrov. </summary>
	void vypisObcePodlaObyvatelstva(Pohlavie pohlavie, Kraj* kraj, Okres* okres, bool vzostupne);

	/// <summary> Vrati obce ktore patria do daneho kraja alebo okresu (podla toho co je zadane). </summary>
	structures::UnsortedSequenceTableNoUnique<int, Obec*>* dajObcePodlaKriterii(Kraj* kraj, Okres* okres);

private:
	/// <summary> Neutriedena tabulka vsetkych obci podla ID pre potreby triedenia. </summary>
	structures::UnsortedSequenceTableNoUnique<int, Obec*>* obce_;

	/// <summary> Tabulka vsetkych obci podla ID. </summary>
	structures::Table<int, Obec*>* obcePodlaId_;

	/// <summary> Tabulka vsetkych obci podla nazvu. </summary>
	structures::Table<std::string, structures::ArrayList<Obec*>*>* obcePodlaNazvu_;

private:
	/// <summary> Nacita udaje o vymere kazdej obce zo suboru. </summary>
	void nacitajVymeryObci();

	/// <summary> Nacita udaje o obyvatelstve kazdej obce zo suboru. </summary>
	void nacitajObyvatelstvoObci();
};

