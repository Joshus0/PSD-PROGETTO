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

/*
 * Funzione: isCodaPrioritaVuota
 * -----------------------------------------------
 * Verifica se la coda di priorità non contiene alcuna richiesta attiva
 * (escludendo le richieste marcate come non valide a causa della lazy deletion).
 *
 * Parametri:
 *   codaTarget - Puntatore alla coda da verificare (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso codaTarget == NULL.
 *
 * Post-condizione:
 *   Nessuna modifica allo stato del sistema.
 *
 * Ritorna:
 *   1 se la coda e' vuota, 0 altrimenti (anche se codaTarget e' NULL).
 */
int isCodaPrioritaVuota(const CodaPriorita* codaTarget);

/*
 * Funzione: getDimensioneCodaPriorita
 * ------------------------------------
 * Restituisce il numero totale di richieste attualmente contenute nella coda,
 * comprese quelle marcate come non valide dalla lazy deletion.
 *
 * Parametri:
 *   codaTarget - Puntatore alla coda (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso codaTarget == NULL.
 *
 * Post-condizione:
 *   Nessuna modifica allo stato del sistema.
 *
 * Ritorna:
 *   Numero intero di richieste, oppure 0 se la coda e' vuota
 *   o codaTarget e' NULL.
 */
int getDimensioneCodaPriorita(const CodaPriorita* codaTarget);

/*
 * Funzione: stampaCodaPriorita
 * ----------------------------
 * Stampa su stdout il contenuto completo della coda di priorita', mostrando
 * tutte le richieste in ordine di priorita' (urgenza decrescente). Ogni
 * richiesta e' visualizzata con una riga formattata a tabella contenente
 * i dati essenziali.
 *
 * Parametri:
 *   codaTarget - Puntatore alla coda da stampare (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso codaTarget == NULL.
 *
 * Post-condizione:
 *   L'output viene scritto su stdout. La coda non viene modificata.
 *
 * Ritorna:
 *   Niente (void).
 */
void stampaCodaPriorita(const CodaPriorita* codaTarget);

/* 
 * STAMPA FILTRATA
 */

/*
 * Funzione: stampaRichiestePerStato
 * ----------------------------------
 * Scorre la coda e stampa su stdout solo le richieste il cui stato
 * corrisponde a quello specificato.
 *
 * Parametri:
 *   codaTarget - Coda da scorrere (puo' essere NULL)
 *   stato      - Stato da usare come filtro (APERTA, PIANIFICATA,
 *                IN_LAVORAZIONE, CONCLUSA, ANNULLATA)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce
 *   internamente il caso codaTarget == NULL o coda vuota.
 *   stato deve essere un valore valido dell'enumerazione StatoRichiesta.
 *
 * Post-condizione:
 *   L'output viene scritto su stdout. La coda non viene modificata.
 *
 * Ritorna:
 *   Niente (void).
 */
void stampaRichiestePerStato(const CodaPriorita* codaTarget, StatoRichiesta stato);

/*
 * Funzione: stampaRichiestePerUrgenza
 * -------------------------------------
 * Scorre la coda e stampa su stdout solo le richieste il cui livello
 * di urgenza corrisponde a quello specificato.
 *
 * Parametri:
 *   codaTarget - Coda da scorrere (puo' essere NULL)
 *   urgenza    - Livello di urgenza da usare come filtro (0-4)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce
 *   internamente il caso codaTarget == NULL o coda vuota.
 *   urgenza dovrebbe essere nel range 0-4 per produrre risultati
 *   significativi.
 *
 * Post-condizione:
 *   L'output viene scritto su stdout. La coda non viene modificata.
 *
 * Ritorna:
 *   Niente (void).
 */
void stampaRichiestePerUrgenza(const CodaPriorita* codaTarget, int urgenza);

/*
 * Funzione: stampaRichiestePerTipologia
 * ---------------------------------------
 * Scorre la coda e stampa su stdout solo le richieste la cui tipologia
 * di problema corrisponde esattamente alla stringa specificata.
 *
 * Parametri:
 *   codaTarget - Coda da scorrere (puo' essere NULL)
 *   tipologia  - Stringa con la tipologia da usare come filtro
 *                (es. "Idraulico", "Elettrico") (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce
 *   internamente il caso codaTarget == NULL o coda vuota.
 *
 * Post-condizione:
 *   L'output viene scritto su stdout. La coda non viene modificata.
 *
 * Ritorna:
 *   Niente (void).
 */
void stampaRichiestePerTipologia(const CodaPriorita* codaTarget, const char* tipologia);

/*
 * Funzione: stampaRichiestePerAppartamento
 * -----------------------------------------
 * Scorre la coda e stampa su stdout solo le richieste il cui
 * appartamento o area di provenienza corrisponde esattamente
 * alla stringa specificata.
 *
 * Parametri:
 *   codaTarget   - Coda da scorrere (puo' essere NULL)
 *   appartamento - Stringa con il codice appartamento o area da usare
 *                  come filtro (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce
 *   internamente il caso codaTarget == NULL o coda vuota.
 *
 * Post-condizione:
 *   L'output viene scritto su stdout. La coda non viene modificata.
 *
 * Ritorna:
 *   Niente (void).
 */
void stampaRichiestePerAppartamento(const CodaPriorita* codaTarget, const char* appartamento);

/*
 * Funzione: stampaRichiestePerTecnico
 * -------------------------------------
 * Scorre la coda e stampa su stdout solo le richieste assegnate
 * al tecnico il cui codice corrisponde esattamente alla stringa
 * specificata.
 *
 * Parametri:
 *   codaTarget    - Coda da scorrere (puo' essere NULL)
 *   codiceTecnico - Codice identificativo del tecnico da usare
 *                   come filtro (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce
 *   internamente il caso codaTarget == NULL o coda vuota.
 *
 * Post-condizione:
 *   L'output viene scritto su stdout. La coda non viene modificata.
 *
 * Ritorna:
 *   Niente (void).
 */
void stampaRichiestePerTecnico(const CodaPriorita* codaTarget, const char* codiceTecnico);

#endif
//Chiusura di #ifndef CODA_PRIORITA_H