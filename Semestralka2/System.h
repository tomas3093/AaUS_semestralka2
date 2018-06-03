#pragma once
#include "EvidenciaKrajov.h"
#include "EvidenciaOkresov.h"
#include "EvidenciaObci.h"

/// <summary> Trieda reprezentuje informacny system pre podporu rozhodovania biofarmara. </summary>
class System
{
public:
	/// <summary> Konstruktor. </summary>
	System();

	/// <summary> Destruktor. </summary>
	~System();

	/// <summary> Nacita potrebne udaje a vytvori potrebne struktury. </summary>
	void nacitajSa();

	/// <summary> Spusti konzolove menu pre ovladanie aplikacie. </summary>
	void uzivatelskeRozhranie();

	/// <summary> Zapise udaje o obciach v zadanom roku, specifikovanych zadanym krajom alebo okresom do suboru. </summary>
	void zapisDataDoSuboru(std::string nazovSuboru, int rok, Kraj* kraj, Okres* okres);

private:
	/// <summary> Evidencia krajov SR. </summary>
	EvidenciaKrajov* kraje_;

	/// <summary> Evidencia okresov SR. </summary>
	EvidenciaOkresov* okresy_;

	/// <summary> Evidencia obci SR. </summary>
	EvidenciaObci* obce_;
};

