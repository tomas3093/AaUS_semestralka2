#pragma once
#include "Okres.h"
#include "structures/table/treap.h"
#include "structures/table/UnsortedSequenceTableNoUnique.h"

/// <summary> Trieda reprezentuje evidenciu vsetkych okresov SR. </summary>
class EvidenciaOkresov
{
public:
	/// <summary> Konstruktor. </summary>
	EvidenciaOkresov();

	/// <summary> Destruktor. </summary>
	~EvidenciaOkresov();

	/// <summary> Vrati okres podla zadaneho nazvu. Ak taky okres neexistuje, vrati nullptr. </summary>
	Okres* dajOkresPodlaNazvu(std::string nazov);

	/// <summary> Vrati okres podla zadaneho id. Ak taky okres neexistuje, vrati nullptr. </summary>
	Okres* dajOkresPodlaId(int id);

	/// <summary> Nacita atributy (id, id_krajov a nazvy) vsetkych okresov zo suboru. </summary>
	void nacitajSa(std::ifstream* infile);

	/// <summary> Vrati pole ID vsetkych okresov. </summary>
	structures::Array<int>* dajIdVsetkychOkresov();

	/// <summary> Vypise okresy ktore mali najvacsiu a najmensiu vymeru ovocnych sadov v zadanom roku. </summary>
	void vypisOkresyPodlaOvocnychSadov(int rok);

	/// <summary> Vypise okresy ktore mali najvacsi a najmensi podiel vymery ornej pody voci vymere ovocnych sadov v zadanom roku. </summary>
	void vypisOkresyPodlaOrnPodyKuOvocSadom(int rok);

	/// <summary> Vypise okresy ktore mali najvacsi a najmensi podiel vymery lesov voci celkovej vymere okresu v zadanom roku. </summary>
	void vypisOkresyPodlaPodieluLesov(int rok);

	/// <summary> Vypise zoradene okresy podla zmeny podielu vymery ovocnych sadov voci celkovej vymere okresu medzi dvoma rokmi. </summary>
	void vypisOkresyPodlaZmenyPodieluOvocnychSadov(int rok1, int rok2, bool vzostupne);

	/// <summary> Vypise zoradene okresy podla zmeny podielu vymery ornej pody voci celkovej vymere okresu medzi dvoma rokmi. </summary>
	void vypisOkresyPodlaZmenyPodieluOrnejPody(int rok1, int rok2, bool vzostupne);

	/// <summary> Vypise zoradene okresy podla zmeny podielu vymery lesnych pozemkov voci celkovej vymere okresu medzi dvoma rokmi. </summary>
	void vypisOkresyPodlaZmenyPodieluLesnychPozemkov(int rok1, int rok2, bool vzostupne);

private:
	/// <summary> Neutriedena tabulka vsetkych okresov podla ID pre potreby triedenia. </summary>
	structures::UnsortedSequenceTableNoUnique<int, Okres*>* okresy_;

	/// <summary> Tabulka vsetkych okresov podla ID. </summary>
	structures::Table<int, Okres*>* okresyPodlaId_;

	/// <summary> Tabulka vsetkych okresov podla nazvu. </summary>
	structures::Table<std::string, Okres*>* okresyPodlaNazvu_;

	/// <summary> Pole, ktore uklada vypocitane hodnoty pre neskorsie pouzitie. Na kazdom indexe obsahuje 
	///dvojprvkove pole, v ktorom je ulozena polozka s maximalnou a minimalnou hodnotou. </summary>
	structures::Array<structures::Array<Okres*>*>* cachePodielOvocnychSadov_;

	/// <summary> Pole, ktore uklada vypocitane hodnoty pre neskorsie pouzitie. Na kazdom indexe obsahuje 
	///dvojprvkove pole, v ktorom je ulozena polozka s maximalnou a minimalnou hodnotou. </summary>
	structures::Array<structures::Array<Okres*>*>* cachePodielOrnejPodyOvocnychSadov_;

	/// <summary> Pole, ktore uklada vypocitane hodnoty pre neskorsie pouzitie. Na kazdom indexe obsahuje 
	///dvojprvkove pole, v ktorom je ulozena polozka s maximalnou a minimalnou hodnotou. </summary>
	structures::Array<structures::Array<Okres*>*>* cachePodielLesnychPozemkov_;

};

