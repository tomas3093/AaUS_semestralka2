#pragma once

#include "Kraj.h"
#include "structures/table/UnsortedSequenceTableNoUnique.h"

/// <summary> Trieda reprezentuje evidenciu vsetkych krajov SR. </summary>
class EvidenciaKrajov
{
public:
	/// <summary> Konstruktor. </summary>
	EvidenciaKrajov();

	/// <summary> Destruktor. </summary>
	~EvidenciaKrajov();

	/// <summary> Vrati kraj podla zadaneho nazvu. Ak taky kraj neexistuje, vrati nullptr. </summary>
	Kraj* dajKrajPodlaNazvu(std::string nazov);

	/// <summary> Vrati kraj podla zadaneho id. Ak taky kraj neexistuje, vrati nullptr. </summary>
	Kraj* dajKrajPodlaId(int id);

	/// <summary> Nacita evidenciu zo suboru. </summary>
	void nacitajSa(std::ifstream* infile);

	void vypisUdajeVymeraKraja(int rok, int idKraja);

	/// <summary> Vypise kraje ktore mali podiel vymery lesov ku celkovej vymere kraja aspon zadany pocet percent v zadanom roku. </summary>
	void vypisKrajePodlaPodieluLesov(int rok, double percentHodnota);

	/// <summary> Vypise kraje ktore obsahuju aspon zadane mnozstvo okresov, ktore mali podiel vymery lesov ku celkovej vymere kraja aspon zadany pocet percent v zadanom roku. </summary>
	void vypisKrajePodlaPoctuOkresovPodielLesov(int rok, size_t pocetOkresov, double percentHodnota);

	/// <summary> Vypise kraje ktore obsahuju najviac a najmenej obci, ktorych podiel vymery lesov ku celkovej vymere kraja aspon zadany pocet percent v zadanom roku. </summary>
	void vypisKrajePodlaPoctuObciPodielLesov(int rok, double percentHodnota);

private:
	/// <summary> Neutriedena tabulka vsetkych krajov podla ID pre potreby triedenia. </summary>
	structures::UnsortedSequenceTableNoUnique<int, Kraj*>* kraje_;

	/// <summary> Tabulka vsetkych krajov podla ID. </summary>
	structures::Table<int, Kraj*>* krajePodlaId_;

	/// <summary> Tabulka vsetkych krajov podla nazvu. </summary>
	structures::Table<std::string, Kraj*>* krajePodlaNazvu_;
};

