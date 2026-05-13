/*
 * Implementazione dell'entita' Richiesta di manutenzione.
 *
 * Gestisce il ciclo di vita di una richiesta: dalla creazione,
 * all'assegnazione a un tecnico, fino alla chiusura o all'annullamento.
 * Ogni richiesta e' identificata da un codice univoco e contiene
 * informazioni sull'appartamento, la tipologia del problema, il livello
 * di urgenza e lo stato corrente nel flusso di lavorazione.
 * 
 * 5 maggio
 * Sabato Pio,
 */
#include "entita/richiesta.h"
#include "main/utilita.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Struttura interna di una Richiesta.
 *
 * I campi codiceTecnicoAssegnato, dataInizioLavorazione, dataChiusura
 * e fasciaOraria sono NULL alla creazione: vengono valorizzati solo
 * quando la richiesta avanza nel proprio ciclo di vita.
 * Il flag isValidaInHeap serve a implementare la "lazy deletion"
 * nella coda di priorita': anziche' rimuovere fisicamente l'elemento
 * dallo heap, lo si marca come non valido e lo si ignora durante
 * le operazioni di estrazione.
 */
struct Richiesta {
    char* codice;
    char* appartamento;
    char* tipologia;
    char* descrizione;
    char* data;
    int urgenza;
    StatoRichiesta stato;
    char* codiceTecnicoAssegnato;
    char* dataInizioLavorazione;
    char* dataChiusura;
    int isValidaInHeap;
    char* fasciaOraria;
};
 
/*
 * creaRichiesta - Alloca e inizializza una nuova richiesta di manutenzione.
 *
 * Tutti i campi stringa vengono duplicati in memoria heap, cosi' da rendere
 * la struttura indipendente dal ciclo di vita dei buffer passati dal chiamante.
 * Lo stato viene impostato ad APERTA e il flag isValidaInHeap a 1 (attiva).
 *
 * Parametri:
 *   codice       - Identificatore univoco della richiesta (non NULL)
 *   appartamento - Appartamento di provenienza (non NULL)
 *   tipologia    - Categoria del problema (non NULL)
 *   descrizione  - Descrizione estesa del problema (non NULL)
 *   data         - Data di apertura nel formato "gg/mm/aaaa" (non NULL)
 *   urgenza      - Livello numerico di urgenza (valore piu' alto = priorita' maggiore)
 *
 * Ritorna:
 *   Puntatore alla nuova Richiesta allocata, oppure NULL in caso di
 *   fallimento della malloc.
 */
Richiesta* creaRichiesta(const char* codice, const char* appartamento, const char* tipologia, 
    const char* descrizione, const char* data, int urgenza) {
    Richiesta* nuova = (Richiesta*)malloc(sizeof(Richiesta));
    if (nuova == NULL) return NULL;
    nuova->codice      = duplicaStringa(codice);
    nuova->appartamento = duplicaStringa(appartamento);
    nuova->tipologia   = duplicaStringa(tipologia);
    nuova->descrizione = duplicaStringa(descrizione);
    nuova->data        = duplicaStringa(data);
    nuova->urgenza     = urgenza;
    nuova->stato       = APERTA; 
    nuova->codiceTecnicoAssegnato  = NULL;
    nuova->dataInizioLavorazione   = NULL;
    nuova->dataChiusura            = NULL;
    nuova->fasciaOraria = NULL;
    nuova->isValidaInHeap = 1;
    return nuova;
}

/*
 * distruggiRichiesta - Libera tutta la memoria associata a una Richiesta.
 *
 * Dealloca ogni campo stringa prima di liberare la struttura stessa.
 * I campi opzionali (codiceTecnicoAssegnato, dataInizioLavorazione, ecc.)
 * possono essere NULL.
 *
 * Parametri:
 *   richiestaTarget - Puntatore alla richiesta da deallocare (ignorato se NULL)
 */
void distruggiRichiesta(Richiesta* richiestaTarget) {
    if (richiestaTarget == NULL) return;
 
    free(richiestaTarget->codice);
    free(richiestaTarget->appartamento);
    free(richiestaTarget->tipologia);
    free(richiestaTarget->descrizione);
    free(richiestaTarget->data);
    free(richiestaTarget->codiceTecnicoAssegnato);
    free(richiestaTarget->dataInizioLavorazione);
    free(richiestaTarget->dataChiusura);
    free(richiestaTarget->fasciaOraria);
 
    free(richiestaTarget);
}

/*
 * GETTER — lettura dei campi della richiesta
 *
 * Tutte le funzioni getter restituiscono NULL (o -1 per i tipi interi)
 * se il puntatore ricevuto e' NULL, cosi' da evitare dereferenziazioni
 * non sicure nel codice chiamante.
 */

/*
 * getCodiceRichiesta - Restituisce il codice univoco della richiesta.
 *
 * Parametri:
 *   richiestaTarget - Puntatore alla richiesta (puo' essere NULL)
 *
 * Ritorna:
 *   Stringa con il codice, oppure NULL se richiestaTarget e' NULL.
 */
const char* getCodiceRichiesta(const Richiesta* richiestaTarget){
    if(richiestaTarget == NULL) return NULL;
    return richiestaTarget->codice;
}

/*
 * getAppartamentoRichiesta - Restituisce l'appartamento di provenienza.
 *
 * Parametri:
 *   richiestaTarget - Puntatore alla richiesta (puo' essere NULL)
 *
 * Ritorna:
 *   Stringa con il codice appartamento, oppure NULL.
 */
const char* getAppartamentoRichiesta(const Richiesta* richiestaTarget){
    if(richiestaTarget == NULL) return NULL;
    return richiestaTarget->appartamento;
}

/*
 * getTipologiaProblemaRichiesta - Restituisce la categoria del problema.
 *
 * Parametri:
 *   richiestaTarget - Puntatore alla richiesta (puo' essere NULL)
 *
 * Ritorna:
 *   Stringa con la tipologia (es. "idraulico"), oppure NULL.
 */
const char* getTipologiaProblemaRichiesta(const Richiesta* richiestaTarget){
    if(richiestaTarget == NULL) return NULL;
    return richiestaTarget->tipologia;
}

/*
 * getDescrizioneRichiesta - Restituisce la descrizione estesa del problema.
 *
 * Parametri:
 *   richiestaTarget - Puntatore alla richiesta (puo' essere NULL)
 *
 * Ritorna:
 *   Stringa con la descrizione, oppure NULL.
 */
const char* getDescrizioneRichiesta(const Richiesta* richiestaTarget){
    if(richiestaTarget == NULL) return NULL;
    return richiestaTarget->descrizione;
}

/*
 * getDataRichiesta - Restituisce la data di apertura della richiesta.
 *
 * Parametri:
 *   richiestaTarget - Puntatore alla richiesta (puo' essere NULL)
 *
 * Ritorna:
 *   Stringa con la data in formato "gg/mm/aaaa", oppure NULL.
 */
const char* getDataRichiesta(const Richiesta* richiestaTarget){
    if(richiestaTarget == NULL) return NULL;
    return richiestaTarget->data;
}

/*
 * getLivelloUrgenzaRichiesta - Restituisce il livello di urgenza.
 *
 * Il valore e' usato come chiave nella coda di priorita': un valore
 * piu' alto corrisponde a una priorita' maggiore nello scheduling
 * degli interventi.
 *
 * Parametri:
 *   richiestaTarget - Puntatore alla richiesta (puo' essere NULL)
 *
 * Ritorna:
 *   Intero con il livello di urgenza, oppure -1 se richiestaTarget e' NULL.
 */
int getLivelloUrgenzaRichiesta(const Richiesta* richiestaTarget) {
    if (richiestaTarget == NULL) return -1;
    return richiestaTarget->urgenza;
}
/*
 * getStatoRichiesta - Restituisce lo stato corrente della richiesta.
 *
 * Parametri:
 *   richiestaTarget - Puntatore alla richiesta (puo' essere NULL)
 *
 * Ritorna:
 *   Valore dell'enumerazione StatoRichiesta, oppure ANNULLATA se
 *   richiestaTarget e' NULL (valore di fallback sicuro).
 */
StatoRichiesta getStatoRichiesta(const Richiesta* richiestaTarget){
    if(richiestaTarget == NULL) return ANNULLATA;
    return richiestaTarget->stato;
}

/*
 * getCodiceTecnicoAssegnatoRichiesta - Restituisce il codice del tecnico assegnato.
 *
 * Parametri:
 *   richiestaTarget - Puntatore alla richiesta (puo' essere NULL)
 *
 * Ritorna:
 *   Stringa con il codice tecnico, oppure NULL se non ancora assegnata
 *   o se richiestaTarget e' NULL.
 */
const char* getCodiceTecnicoAssegnatoRichiesta(const Richiesta* richiestaTarget){
    if(richiestaTarget == NULL) return NULL;
    return richiestaTarget->codiceTecnicoAssegnato;
}

/*
 * getDataInizioLavorazioneRichiesta - Restituisce la data di inizio lavorazione.
 *
 * Parametri:
 *   richiestaTarget - Puntatore alla richiesta (puo' essere NULL)
 *
 * Ritorna:
 *   Stringa con la data, oppure NULL se la lavorazione non e' ancora iniziata.
 */
const char* getDataInizioLavorazioneRichiesta(const Richiesta* richiestaTarget){
    if(richiestaTarget == NULL) return NULL;
    return richiestaTarget->dataInizioLavorazione;
}

/*
 * getDataChiusuraRichiesta - Restituisce la data di chiusura della richiesta.
 *
 * Parametri:
 *   richiestaTarget - Puntatore alla richiesta (puo' essere NULL)
 *
 * Ritorna:
 *   Stringa con la data di chiusura, oppure NULL se la richiesta
 *   e' ancora aperta o in lavorazione.
 */
const char* getDataChiusuraRichiesta(const Richiesta* richiestaTarget){
    if(richiestaTarget == NULL) return NULL;
    return richiestaTarget->dataChiusura;
}
/*
 * isValidaInHeapRichiesta - Indica se la richiesta e' ancora attiva nella
 *                           coda di priorita'.
 *
 * Usata per implementare la "lazy deletion": quando una richiesta viene
 * annullata o completata, il flag viene abbassato a 0 senza rimuoverla
 * fisicamente dallo heap. Le funzioni di estrazione ignorano i nodi
 * con flag a 0.
 *
 * Parametri:
 *   richiestaTarget - Puntatore alla richiesta (puo' essere NULL)
 *
 * Ritorna:
 *   1 se la richiesta e' valida nello heap, 0 altrimenti (anche se NULL).
 */
int isValidaInHeapRichiesta(const Richiesta* richiestaTarget) {
    if (richiestaTarget == NULL) return 0;
    return richiestaTarget->isValidaInHeap;
}

/*
 * getFasciaOrariaRichiesta - Restituisce la fascia oraria dell'intervento.
 *
 * Parametri:
 *   r - Puntatore alla richiesta (puo' essere NULL)
 *
 * Ritorna:
 *   Stringa con la fascia oraria pianificata (es. "09:00-11:00"),
 *   oppure NULL se non ancora pianificata.
 */
const char* getFasciaOrariaRichiesta(const Richiesta* r) {
    if (r == NULL) return NULL;
    return r->fasciaOraria;
}
/*
 * SETTER — modifica dei campi della richiesta
 *
 * I setter che aggiornano campi stringa liberano prima la memoria del
 * valore precedente per evitare memory leak, poi duplicano il nuovo valore.
 */

/*
 * setStatoRichiesta - Aggiorna lo stato della richiesta.
 *
 * Parametri:
 *   richiestaTarget - Puntatore alla richiesta da aggiornare (ignorato se NULL)
 *   nuovoStato      - Nuovo valore di StatoRichiesta da assegnare
 */
void setStatoRichiesta(Richiesta* richiestaTarget, StatoRichiesta nuovoStato) {
    if (richiestaTarget == NULL) return;
    richiestaTarget->stato = nuovoStato;
}

/*
 * setCodiceTecnicoAssegnatoRichiesta - Assegna un tecnico alla richiesta.
 *
 * Libera l'eventuale codice precedente prima di duplicare il nuovo,
 * cosi' da supportare la riassegnazione senza memory leak.
 *
 * Parametri:
 *   richiestaTarget - Puntatore alla richiesta da aggiornare (ignorato se NULL)
 *   nuovoCodice     - Codice del tecnico da assegnare
 */
void setCodiceTecnicoAssegnatoRichiesta(Richiesta* richiestaTarget, const char* nuovoCodice) {
    if (richiestaTarget == NULL) return;
    free(richiestaTarget->codiceTecnicoAssegnato);
    richiestaTarget->codiceTecnicoAssegnato = duplicaStringa(nuovoCodice);
}

/*
 * setDataInizioLavorazioneRichiesta - Registra la data di presa in carico.
 *
 * Parametri:
 *   richiestaTarget - Puntatore alla richiesta da aggiornare (ignorato se NULL)
 *   nuovaData       - Data di inizio lavorazione nel formato "gg/mm/aaaa"
 */
void setDataInizioLavorazioneRichiesta(Richiesta* richiestaTarget, const char* nuovaData) {
    if (richiestaTarget == NULL) return;
    free(richiestaTarget->dataInizioLavorazione);
    richiestaTarget->dataInizioLavorazione = duplicaStringa(nuovaData);
}

/*
 * setDataChiusuraRichiesta - Registra la data di chiusura della richiesta.
 *
 * Parametri:
 *   richiestaTarget - Puntatore alla richiesta da aggiornare (ignorato se NULL)
 *   nuovaData       - Data di chiusura nel formato "gg/mm/aaaa"
 */
void setDataChiusuraRichiesta(Richiesta* richiestaTarget, const char* nuovaData) {
    if (richiestaTarget == NULL) return;
    free(richiestaTarget->dataChiusura);
    richiestaTarget->dataChiusura = duplicaStringa(nuovaData);
}

/*
 * setValidaInHeapRichiesta - Aggiorna il flag di validita' nella coda di priorita'.
 *
 * Impostare il flag a 0 equivale a una rimozione logica dalla coda: la
 * richiesta rimane in memoria ma verra' ignorata durante le estrazioni.
 *
 * Parametri:
 *   richiestaTarget - Puntatore alla richiesta da aggiornare (ignorato se NULL)
 *   flagValidita    - 1 per marcare come valida, 0 per marcare come rimossa
 */
void setValidaInHeapRichiesta(Richiesta* richiestaTarget, int flagValidita) {
    if (richiestaTarget == NULL) return;
    richiestaTarget->isValidaInHeap = flagValidita;
}
/*
 * setFasciaOrariaRichiesta - Imposta la fascia oraria dell'intervento pianificato.
 *
 * Parametri:
 *   r     - Puntatore alla richiesta da aggiornare (ignorato se NULL)
 *   fascia - Stringa con la fascia oraria (es. "09:00-11:00")
 */
void setFasciaOrariaRichiesta(Richiesta* r, const char* fascia) {
    if (r == NULL) return;
    free(r->fasciaOraria);
    r->fasciaOraria = duplicaStringa(fascia);
}