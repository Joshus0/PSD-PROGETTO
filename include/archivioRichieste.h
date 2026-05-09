#ifndef ARCHIVIO_RICHIESTE_H    //Guardia di inclusione per evitare inclusioni multiple
#define ARCHIVIO_RICHIESTE_H
#include <stdio.h>
#include "main/utilita.h"
#include "entita/richiesta.h" //Inclusione dell'header per la definizione della struttura Richiesta e delle funzioni correlate

typedef struct ArchivioRichieste ArchivioRichieste; //Forward declaration della struttura ArchivioRichieste presente in archivioRichieste.c
typedef struct NodoLista NodoLista;                 //Forward declaration della struttura NodoLista presente in archivioRichieste.c, utilizzata per implementare la lista di richieste



//Dichiarazione delle funzioni per la gestione dell'archivio delle richieste
ArchivioRichieste* creaArchivioRichieste();
void distruggiArchivioRichieste(ArchivioRichieste* archivioTarget);

//Funzioni per la manipolazione dell'archivio delle richieste
void inserisciInCodaArchivio(ArchivioRichieste* archivioTarget, Richiesta* nuovaRichiesta);
void rimuoviNodoDaArchivio(ArchivioRichieste* archivioTarget, NodoLista* nodoDaRimuovere);
void stampaRichiesteArchivioPerStato(const ArchivioRichieste* archivio, StatoRichiesta stato);

//Funzioni per accedere ai nodi e alle richieste nell'archivio
NodoLista* getTestaArchivio(const ArchivioRichieste* archivioTarget);
NodoLista* getCodaArchivio(const ArchivioRichieste* archivioTarget);
NodoLista* getNextNodoLista(const NodoLista* nodoCorrente);
NodoLista* getPrevNodoLista(const NodoLista* nodoCorrente);
Richiesta* getRichiestaDalNodoLista(const NodoLista* nodoCorrente);
int getDimensioneArchivio(const ArchivioRichieste* archivioTarget);

// Funzione per cercare una richiesta specifica tramite il suo codice
Richiesta* cercaRichiestaPerCodice(const ArchivioRichieste* archivioTarget, const char* codiceDaCercare);

#endif
//Chiusura di #ifndef ARCHIVIO_RICHIESTE_H
