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

/*
 * Funzione: stampaRichiesteArchivioPerStato
 * ------------------------------------------
 * Scorre linearmente la lista doppia dell'archivio e stampa su stdout
 * solo le richieste il cui stato corrisponde a quello specificato.
 * Se nessuna richiesta corrisponde al filtro, stampa un messaggio
 * di avviso.
 *
 * Parametri:
 *   archivio - Puntatore all'archivio da scorrere (puo' essere NULL)
 *   stato    - Stato da usare come filtro (APERTA, PIANIFICATA,
 *              IN_LAVORAZIONE, CONCLUSA, ANNULLATA)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce
 *   internamente il caso archivio == NULL.
 *   stato deve essere un valore valido dell'enumerazione StatoRichiesta.
 *
 * Post-condizione:
 *   L'output viene scritto su stdout. L'archivio non viene modificato.
 *
 * Ritorna:
 *   Niente (void).
 */
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

/*
 * Funzione: stampaRichiesteArchivioPerUrgenza
 * --------------------------------------------
 * Scorre linearmente la lista e stampa su stdout solo le richieste
 * il cui livello di urgenza corrisponde a quello specificato.
 * Se nessuna richiesta corrisponde, stampa un messaggio di avviso.
 *
 * Parametri:
 *   archivio - Puntatore all'archivio da scorrere (puo' essere NULL)
 *   urgenza  - Livello di urgenza da usare come filtro (0-4)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce
 *   internamente il caso archivio == NULL.
 *   urgenza dovrebbe essere nel range 0-4 per risultati significativi.
 *
 * Post-condizione:
 *   L'output viene scritto su stdout. L'archivio non viene modificato.
 *
 * Ritorna:
 *   Niente (void).
 */
void stampaRichiesteArchivioPerUrgenza(const ArchivioRichieste* archivio, int urgenza);

/*
 * Funzione: stampaRichiesteArchivioPerTipologia
 * -----------------------------------------------
 * Scorre linearmente la lista e stampa su stdout solo le richieste
 * la cui tipologia di problema corrisponde esattamente alla stringa
 * specificata. Se nessuna richiesta corrisponde, stampa un messaggio
 * di avviso.
 *
 * Parametri:
 *   archivio  - Puntatore all'archivio da scorrere (puo' essere NULL)
 *   tipologia - Stringa con la tipologia da usare come filtro
 *               (es. "Idraulico", "Elettrico") (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce
 *   internamente il caso archivio == NULL.
 *
 * Post-condizione:
 *   L'output viene scritto su stdout. L'archivio non viene modificato.
 *
 * Ritorna:
 *   Niente (void).
 */
void stampaRichiesteArchivioPerTipologia(const ArchivioRichieste* archivio, const char* tipologia);

/*
 * Funzione: stampaRichiesteArchivioPerAppartamento
 * -------------------------------------------------
 * Scorre linearmente la lista e stampa su stdout solo le richieste
 * il cui appartamento o area di provenienza corrisponde esattamente
 * alla stringa specificata. Se nessuna richiesta corrisponde, stampa
 * un messaggio di avviso.
 *
 * Parametri:
 *   archivio     - Puntatore all'archivio da scorrere (puo' essere NULL)
 *   appartamento - Stringa con il codice appartamento o area da usare
 *                  come filtro (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce
 *   internamente il caso archivio == NULL.
 *
 * Post-condizione:
 *   L'output viene scritto su stdout. L'archivio non viene modificato.
 *
 * Ritorna:
 *   Niente (void).
 */
void stampaRichiesteArchivioPerAppartamento(const ArchivioRichieste* archivio, const char* appartamento);

/*
 * Funzione: stampaRichiesteArchivioPerTecnico
 * --------------------------------------------
 * Scorre linearmente la lista e stampa su stdout solo le richieste
 * assegnate al tecnico il cui codice corrisponde esattamente alla
 * stringa specificata. Salta le richieste senza tecnico assegnato.
 * Se nessuna richiesta corrisponde, stampa un messaggio di avviso.
 *
 * Parametri:
 *   archivio      - Puntatore all'archivio da scorrere (puo' essere NULL)
 *   codiceTecnico - Codice identificativo del tecnico da usare come
 *                   filtro (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce
 *   internamente il caso archivio == NULL.
 *
 * Post-condizione:
 *   L'output viene scritto su stdout. L'archivio non viene modificato.
 *
 * Ritorna:
 *   Niente (void).
 */
void stampaRichiesteArchivioPerTecnico(const ArchivioRichieste* archivio, const char* codiceTecnico);


#endif
//Chiusura di #ifndef ARCHIVIO_RICHIESTE_H
