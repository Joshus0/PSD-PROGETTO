#ifndef RICHIESTA_H //Guardia di inclusione per evitare inclusioni multiple
#define RICHIESTA_H

typedef enum {  //Definizione dell'enumerazione per rappresentare lo stato di una richiesta
    APERTA,
    PIANIFICATA,
    IN_LAVORAZIONE,
    CONCLUSA,
    ANNULLATA
} StatoRichiesta;

typedef struct Richiesta Richiesta; //Forward declaration della struttura Richiesta presente in richiesta.c


//Dichiarazione delle funzioni per la gestione delle richieste

/*
 * Funzione: creaRichiesta
 * -----------------------
 * Alloca e inizializza una nuova richiesta di manutenzione nel sistema.
 * Lo stato iniziale e' APERTA e il flag di validita' nello heap e' impostato a 1.
 *
 * Parametri:
 *   codice       - Identificatore univoco della richiesta (non NULL)
 *   appartamento - Codice o numero dell'appartamento (non NULL)
 *   tipologia    - Categoria del problema (es. "Idraulico") (non NULL)
 *   descrizione  - Descrizione estesa del problema (non NULL)
 *   data         - Data di apertura nel formato "GG/MM/AAAA" (non NULL)
 *   urgenza      - Livello numerico di urgenza (valore > = priorita' maggiore)
 *
 * Pre-condizione:
 *   Tutti i parametri stringa devono essere non NULL. Il codice deve essere
 *   univoco nel sistema. La data deve essere in formato valido.
 *
 * Post-condizione:
 *   Una nuova Richiesta viene allocata in heap, con i campi opzionali
 *   (codiceTecnicoAssegnato, date di lavorazione/chiusura, fasciaOraria)
 *   impostati a NULL e il flag isValidaInHeap a 1.
 *
 * Ritorna:
 *   Puntatore alla nuova Richiesta allocata, oppure NULL in caso di
 *   fallimento della malloc.
 */
Richiesta* creaRichiesta(const char* codice, const char* appartamento, const char* tipologia, const char* descrizione, const char* data, int urgenza);

/*
 * Funzione: distruggiRichiesta
 * ----------------------------
 * Libera tutta la memoria associata a una Richiesta, inclusi tutti i
 * campi stringa allocati dinamicamente.
 *
 * Parametri:
 *   richiestaTarget - Puntatore alla richiesta da deallocare (ignorato se NULL)
 *
 * Pre-condizione:
 *   richiestaTarget deve puntare a memoria allocata precedentemente tramite
 *   creaRichiesta, o essere NULL.
 *
 * Post-condizione:
 *   La memoria della Richiesta viene liberata. Il puntatore richiestaTarget
 *   diventa invalido dopo questa chiamata.
 *
 * Ritorna:
 *   Niente (void).
 */
void distruggiRichiesta(Richiesta* richiestaTarget);


//Funzioni getter per accedere ai campi di una richiesta

/*
 * Funzione: getCodiceRichiesta
 * ----------------------------
 * Restituisce il codice identificativo univoco della richiesta.
 *
 * Parametri:
 *   richiestaTarget - Puntatore alla richiesta (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso richiestaTarget == NULL.
 *
 * Post-condizione:
 *   Nessuna modifica allo stato del sistema.
 *
 * Ritorna:
 *   Stringa con il codice identificativo, oppure NULL se richiestaTarget e' NULL.
 */
const char* getCodiceRichiesta(const Richiesta* richiestaTarget);

/*
 * Funzione: getAppartamentoRichiesta
 * -----------------------------------
 * Restituisce il codice dell'appartamento di provenienza della richiesta.
 *
 * Parametri:
 *   richiestaTarget - Puntatore alla richiesta (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso richiestaTarget == NULL.
 *
 * Post-condizione:
 *   Nessuna modifica allo stato del sistema.
 *
 * Ritorna:
 *   Stringa con l'identificativo dell'appartamento, oppure NULL se richiestaTarget e' NULL.
 */
const char* getAppartamentoRichiesta(const Richiesta* richiestaTarget);

/*
 * Funzione: getTipologiaProblemaRichiesta
 * ----------------------------------------
 * Restituisce la categoria o tipologia del problema segnalato nella richiesta.
 *
 * Parametri:
 *   richiestaTarget - Puntatore alla richiesta (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso richiestaTarget == NULL.
 *
 * Post-condizione:
 *   Nessuna modifica allo stato del sistema.
 *
 * Ritorna:
 *   Stringa con la tipologia (es. "Idraulico"), oppure NULL se richiestaTarget e' NULL.
 */
const char* getTipologiaProblemaRichiesta(const Richiesta* richiestaTarget);

/*
 * Funzione: getDescrizioneRichiesta
 * --------------------------------
 * Restituisce la descrizione dettagliata del problema segnalato.
 *
 * Parametri:
 *   richiestaTarget - Puntatore alla richiesta (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso richiestaTarget == NULL.
 *
 * Post-condizione:
 *   Nessuna modifica allo stato del sistema.
 *
 * Ritorna:
 *   Stringa con la descrizione del problema, oppure NULL se richiestaTarget e' NULL.
 */
const char* getDescrizioneRichiesta(const Richiesta* richiestaTarget);

/*
 * Funzione: getDataRichiesta
 * --------------------------
 * Restituisce la data di apertura della richiesta nel formato "GG/MM/AAAA".
 *
 * Parametri:
 *   richiestaTarget - Puntatore alla richiesta (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso richiestaTarget == NULL.
 *
 * Post-condizione:
 *   Nessuna modifica allo stato del sistema.
 *
 * Ritorna:
 *   Stringa con la data in formato "GG/MM/AAAA", oppure NULL se richiestaTarget e' NULL.
 */
const char* getDataRichiesta(const Richiesta* richiestaTarget);

/*
 * Funzione: getLivelloUrgenzaRichiesta
 * ------------------------------------
 * Restituisce il livello numerico di urgenza della richiesta,
 * utilizzato come criterio di priorita' nella coda di attesa.
 *
 * Parametri:
 *   richiestaTarget - Puntatore alla richiesta (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso richiestaTarget == NULL.
 *
 * Post-condizione:
 *   Nessuna modifica allo stato del sistema.
 *
 * Ritorna:
 *   Intero con il livello di urgenza, oppure -1 se richiestaTarget e' NULL.
 */
int getLivelloUrgenzaRichiesta(const Richiesta* richiestaTarget);

/*
 * Funzione: getStatoRichiesta
 * ---------------------------
 * Restituisce lo stato corrente della richiesta nel suo ciclo di vita.
 *
 * Parametri:
 *   richiestaTarget - Puntatore alla richiesta (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso richiestaTarget == NULL.
 *
 * Post-condizione:
 *   Nessuna modifica allo stato del sistema.
 *
 * Ritorna:
 *   Valore dell'enumerazione StatoRichiesta, oppure ANNULLATA (valore di fallback)
 *   se richiestaTarget e' NULL.
 */
StatoRichiesta getStatoRichiesta(const Richiesta* richiestaTarget);

/*
 * Funzione: getCodiceTecnicoAssegnatoRichiesta
 * -------------------------------------------
 * Restituisce il codice del tecnico cui e' stata assegnata la richiesta.
 *
 * Parametri:
 *   richiestaTarget - Puntatore alla richiesta (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso richiestaTarget == NULL.
 *
 * Post-condizione:
 *   Nessuna modifica allo stato del sistema.
 *
 * Ritorna:
 *   Stringa con il codice del tecnico, oppure NULL se non ancora assegnato
 *   o se richiestaTarget e' NULL.
 */
const char* getCodiceTecnicoAssegnatoRichiesta(const Richiesta* richiestaTarget);

/*
 * Funzione: getDataInizioLavorazioneRichiesta
 * -------------------------------------------
 * Restituisce la data in cui il tecnico ha preso in carico la richiesta
 * (data di inizio lavorazione).
 *
 * Parametri:
 *   richiestaTarget - Puntatore alla richiesta (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso richiestaTarget == NULL.
 *
 * Post-condizione:
 *   Nessuna modifica allo stato del sistema.
 *
 * Ritorna:
 *   Stringa con la data in formato "GG/MM/AAAA", oppure NULL se la lavorazione
 *   non e' ancora iniziata o se richiestaTarget e' NULL.
 */
const char* getDataInizioLavorazioneRichiesta(const Richiesta* richiestaTarget);

/*
 * Funzione: getDataChiusuraRichiesta
 * ----------------------------------
 * Restituisce la data di chiusura (completamento) della richiesta.
 *
 * Parametri:
 *   richiestaTarget - Puntatore alla richiesta (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso richiestaTarget == NULL.
 *
 * Post-condizione:
 *   Nessuna modifica allo stato del sistema.
 *
 * Ritorna:
 *   Stringa con la data di chiusura in formato "GG/MM/AAAA", oppure NULL
 *   se la richiesta e' ancora aperta/in lavorazione o se richiestaTarget e' NULL.
 */
const char* getDataChiusuraRichiesta(const Richiesta* richiestaTarget);

/*
 * Funzione: getFasciaOrariaRichiesta
 * ----------------------------------
 * Restituisce la fascia oraria pianificata per l'intervento,
 * nel formato "HH:MM-HH:MM".
 *
 * Parametri:
 *   r - Puntatore alla richiesta (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso r == NULL.
 *
 * Post-condizione:
 *   Nessuna modifica allo stato del sistema.
 *
 * Ritorna:
 *   Stringa con la fascia oraria pianificata, oppure NULL se non ancora
 *   pianificata o se r e' NULL.
 */
const char* getFasciaOrariaRichiesta(const Richiesta* r);

/*
 * Funzione: isValidaInHeapRichiesta
 * ---------------------------------
 * Indica se la richiesta e' ancora valida e attiva nella coda di priorita'.
 * Utilizzata per implementare la "lazy deletion": una richiesta viene
 * marcata come non valida anziché rimossa fisicamente dallo heap.
 *
 * Parametri:
 *   richiestaTarget - Puntatore alla richiesta (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso richiestaTarget == NULL.
 *
 * Post-condizione:
 *   Nessuna modifica allo stato del sistema.
 *
 * Ritorna:
 *   1 se la richiesta e' valida nello heap, 0 altrimenti (anche se richiestaTarget e' NULL).
 */
int isValidaInHeapRichiesta(const Richiesta* richiestaTarget);


//Funzioni setter per modificare i campi di una richiesta

/*
 * Funzione: setStatoRichiesta
 * ---------------------------
 * Modifica lo stato della richiesta nel suo ciclo di vita.
 *
 * Parametri:
 *   richiestaTarget - Puntatore alla richiesta da aggiornare (ignorato se NULL)
 *   nuovoStato      - Nuovo valore dell'enumerazione StatoRichiesta
 *
 * Pre-condizione:
 *   richiestaTarget deve puntare a una Richiesta valida allocata precedentemente,
 *   o essere NULL.
 *
 * Post-condizione:
 *   Se richiestaTarget != NULL, lo stato viene aggiornato al nuovo valore.
 *
 * Ritorna:
 *   Niente (void).
 */
void setStatoRichiesta(Richiesta* richiestaTarget, StatoRichiesta nuovoStato);

/*
 * Funzione: setCodiceTecnicoAssegnatoRichiesta
 * -------------------------------------------
 * Assegna o riassegna un tecnico alla richiesta, registrando il suo codice.
 *
 * Parametri:
 *   richiestaTarget - Puntatore alla richiesta da aggiornare (ignorato se NULL)
 *   nuovoCodice     - Codice del tecnico da assegnare
 *
 * Pre-condizione:
 *   richiestaTarget deve puntare a una Richiesta valida allocata precedentemente,
 *   o essere NULL.
 *
 * Post-condizione:
 *   Se richiestaTarget != NULL, il codice tecnico assegnato viene liberato e
 *   sostituito con la nuova stringa duplicata.
 *
 * Ritorna:
 *   Niente (void).
 */
void setCodiceTecnicoAssegnatoRichiesta(Richiesta* richiestaTarget, const char* nuovoCodice);

/*
 * Funzione: setDataInizioLavorazioneRichiesta
 * ------------------------------------------
 * Registra la data in cui il tecnico ha iniziato la lavorazione della richiesta.
 *
 * Parametri:
 *   richiestaTarget - Puntatore alla richiesta da aggiornare (ignorato se NULL)
 *   nuovaData       - Data di inizio lavorazione nel formato "GG/MM/AAAA"
 *
 * Pre-condizione:
 *   richiestaTarget deve puntare a una Richiesta valida allocata precedentemente,
 *   o essere NULL.
 *
 * Post-condizione:
 *   Se richiestaTarget != NULL, la data di inizio lavorazione viene aggiornata.
 *
 * Ritorna:
 *   Niente (void).
 */
void setDataInizioLavorazioneRichiesta(Richiesta* richiestaTarget, const char* nuovaData);

/*
 * Funzione: setDataChiusuraRichiesta
 * ----------------------------------
 * Registra la data di chiusura (completamento) della richiesta.
 *
 * Parametri:
 *   richiestaTarget - Puntatore alla richiesta da aggiornare (ignorato se NULL)
 *   nuovaData       - Data di chiusura nel formato "GG/MM/AAAA"
 *
 * Pre-condizione:
 *   richiestaTarget deve puntare a una Richiesta valida allocata precedentemente,
 *   o essere NULL.
 *
 * Post-condizione:
 *   Se richiestaTarget != NULL, la data di chiusura viene aggiornata.
 *
 * Ritorna:
 *   Niente (void).
 */
void setDataChiusuraRichiesta(Richiesta* richiestaTarget, const char* nuovaData);

/*
 * Funzione: setValidaInHeapRichiesta
 * ----------------------------------
 * Aggiorna il flag di validita' della richiesta nella coda di priorita'.
 * Impostare il flag a 0 equivale a una rimozione logica dalla coda (lazy deletion).
 *
 * Parametri:
 *   richiestaTarget - Puntatore alla richiesta da aggiornare (ignorato se NULL)
 *   flagValidita    - 1 per marcare come valida, 0 per marcare come rimossa
 *
 * Pre-condizione:
 *   richiestaTarget deve puntare a una Richiesta valida allocata precedentemente,
 *   o essere NULL.
 *
 * Post-condizione:
 *   Se richiestaTarget != NULL, il flag di validita' viene aggiornato.
 *
 * Ritorna:
 *   Niente (void).
 */
void setValidaInHeapRichiesta(Richiesta* richiestaTarget, int flagValidita);

/*
 * Funzione: setFasciaOrariaRichiesta
 * ----------------------------------
 * Imposta la fascia oraria pianificata per l'intervento (es. "09:00-11:00").
 *
 * Parametri:
 *   r     - Puntatore alla richiesta da aggiornare (ignorato se NULL)
 *   fascia - Stringa con la fascia oraria nel formato "HH:MM-HH:MM"
 *
 * Pre-condizione:
 *   r deve puntare a una Richiesta valida allocata precedentemente, o essere NULL.
 *   fascia dovrebbe rispettare il formato standardizzato.
 *
 * Post-condizione:
 *   Se r != NULL, la fascia oraria viene liberata e sostituita con la nuova.
 *
 * Ritorna:
 *   Niente (void).
 */
void setFasciaOrariaRichiesta(Richiesta* r, const char* fascia);

#endif
//Chiusura di #ifndef RICHIESTA_H