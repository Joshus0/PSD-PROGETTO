#ifndef CODA_PRIORITA_H //Guardia di inclusione per evitare inclusioni multiple
#define CODA_PRIORITA_H

#include "entita/richiesta.h"   //Inclusione dell'header per la definizione della struttura Richiesta e delle funzioni correlate, necessaria per gestire le richieste nella coda di priorità
#include "archivioRichieste.h" //Inclusione dell'header per la definizione della struttura ArchivioRichieste e delle funzioni correlate, utile per accedere alle richi

typedef struct CodaPriorita CodaPriorita; //Forward declaration della struttura CodaPriorita presente in codaPriorita.c, utilizzata per implementare la coda di priorità basata su un heap di richieste

//Dichiarazione delle funzioni per la gestione della coda di priorità
CodaPriorita* creaCodaPriorita(int capacitaIniziale);
void distruggiCodaPriorita(CodaPriorita* codaTarget);

//Funzioni per la manipolazione della coda di priorità
void inserisciInCodaPriorita(CodaPriorita* codaTarget, Richiesta* richiestaDaAccodare);
Richiesta* estraiMaxDaCodaPriorita(CodaPriorita* codaTarget);

//Funzioni per accedere alle informazioni sulla coda di priorità
int isCodaPrioritaVuota(const CodaPriorita* codaTarget);
int getDimensioneCodaPriorita(const CodaPriorita* codaTarget);

#endif
//Chiusura di #ifndef CODA_PRIORITA_H