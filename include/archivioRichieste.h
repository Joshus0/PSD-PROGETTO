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

/*
 * Funzione: getTestaArchivio
 * --------------------------
 * Restituisce il puntatore al primo nodo della lista doppiamente concatenata,
 * permettendo di iniziare l'iterazione lineare attraverso le richieste archiviate.
 *
 * Parametri:
 *   archivioTarget - Puntatore all'archivio (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso archivioTarget == NULL.
 *
 * Post-condizione:
 *   Nessuna modifica allo stato del sistema.
 *
 * Ritorna:
 *   Puntatore al primo nodo della lista, oppure NULL se l'archivio
 *   e' vuoto o archivioTarget e' NULL.
 */
NodoLista* getTestaArchivio(const ArchivioRichieste* archivioTarget);

/*
 * Funzione: getCodaArchivio
 * -------------------------
 * Restituisce il puntatore all'ultimo nodo della lista doppiamente concatenata,
 * permettendo di accedere efficientemente alla fine della lista.
 *
 * Parametri:
 *   archivioTarget - Puntatore all'archivio (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso archivioTarget == NULL.
 *
 * Post-condizione:
 *   Nessuna modifica allo stato del sistema.
 *
 * Ritorna:
 *   Puntatore all'ultimo nodo della lista, oppure NULL se l'archivio
 *   e' vuoto o archivioTarget e' NULL.
 */
NodoLista* getCodaArchivio(const ArchivioRichieste* archivioTarget);

/*
 * Funzione: getNextNodoLista
 * ---------------------------
 * Restituisce il nodo successivo nella sequenza della lista doppiamente concatenata.
 *
 * Parametri:
 *   nodoCorrente - Puntatore al nodo corrente (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso nodoCorrente == NULL.
 *
 * Post-condizione:
 *   Nessuna modifica allo stato del sistema.
 *
 * Ritorna:
 *   Puntatore al nodo successivo, oppure NULL se nodoCorrente e' l'ultimo
 *   nodo della lista o se nodoCorrente e' NULL.
 */
NodoLista* getNextNodoLista(const NodoLista* nodoCorrente);

/*
 * Funzione: getPrevNodoLista
 * ---------------------------
 * Restituisce il nodo precedente nella sequenza della lista doppiamente concatenata.
 *
 * Parametri:
 *   nodoCorrente - Puntatore al nodo corrente (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso nodoCorrente == NULL.
 *
 * Post-condizione:
 *   Nessuna modifica allo stato del sistema.
 *
 * Ritorna:
 *   Puntatore al nodo precedente, oppure NULL se nodoCorrente e' il primo
 *   nodo della lista o se nodoCorrente e' NULL.
 */
NodoLista* getPrevNodoLista(const NodoLista* nodoCorrente);

/*
 * Funzione: getRichiestaDalNodoLista
 * -----------------------------------
 * Estrae il puntatore alla Richiesta contenuta nel nodo della lista.
 *
 * Parametri:
 *   nodoCorrente - Puntatore al nodo dalla quale estrarre la richiesta (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso nodoCorrente == NULL.
 *
 * Post-condizione:
 *   Nessuna modifica allo stato del sistema.
 *
 * Ritorna:
 *   Puntatore alla Richiesta ospitata dal nodo, oppure NULL se nodoCorrente e' NULL.
 */
Richiesta* getRichiestaDalNodoLista(const NodoLista* nodoCorrente);

/*
 * Funzione: getDimensioneArchivio
 * --------------------------------
 * Restituisce il numero totale di richieste attualmente salvate nell'archivio.
 *
 * Parametri:
 *   archivioTarget - Puntatore all'archivio (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso archivioTarget == NULL.
 *
 * Post-condizione:
 *   Nessuna modifica allo stato del sistema.
 *
 * Ritorna:
 *   Numero intero di richieste, oppure 0 se l'archivio e' vuoto
 *   o archivioTarget e' NULL.
 */
int getDimensioneArchivio(const ArchivioRichieste* archivioTarget);

/*
 * Funzione: cercaRichiestaPerCodice
 * ----------------------------------
 * Effettua una ricerca sequenziale lineare attraverso l'archivio
 * per individuare la richiesta che possiede il codice identificativo
 * specificato. Utile per recuperare una richiesta conoscendone il codice univoco.
 *
 * Parametri:
 *   archivioTarget  - Puntatore all'archivio dove cercare (puo' essere NULL)
 *   codiceDaCercare - Codice identificativo da cercare (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   i casi archivioTarget == NULL o codiceDaCercare == NULL.
 *
 * Post-condizione:
 *   Nessuna modifica allo stato dell'archivio.
 *
 * Ritorna:
 *   Puntatore alla Richiesta trovata, oppure NULL se nessuna richiesta
 *   corrisponde al codice o se l'archivio e' vuoto.
 */
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
