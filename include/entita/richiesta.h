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
Richiesta* creaRichiesta(const char* codice, const char* appartamento, const char* tipologia, const char* descrizione, const char* data, int urgenza);

void distruggiRichiesta(Richiesta* richiestaTarget);


//Funzioni getter per accedere ai campi di una richiesta
const char* getCodiceRichiesta(const Richiesta* richiestaTarget);
const char* getAppartamentoRichiesta(const Richiesta* richiestaTarget);
const char* getTipologiaProblemaRichiesta(const Richiesta* richiestaTarget);
const char* getDescrizioneRichiesta(const Richiesta* richiestaTarget);
const char* getDataRichiesta(const Richiesta* richiestaTarget);
int getLivelloUrgenzaRichiesta(const Richiesta* richiestaTarget);
StatoRichiesta getStatoRichiesta(const Richiesta* richiestaTarget);
const char* getCodiceTecnicoAssegnatoRichiesta(const Richiesta* richiestaTarget);
const char* getDataInizioLavorazioneRichiesta(const Richiesta* richiestaTarget);
const char* getDataChiusuraRichiesta(const Richiesta* richiestaTarget);
int isValidaInHeapRichiesta(const Richiesta* richiestaTarget);


//Funzioni setter per modificare i campi di una richiesta
void setStatoRichiesta(Richiesta* richiestaTarget, StatoRichiesta nuovoStato);
void setCodiceTecnicoAssegnatoRichiesta(Richiesta* richiestaTarget, const char* nuovoCodice);
void setDataInizioLavorazioneRichiesta(Richiesta* richiestaTarget, const char* nuovaData);
void setDataChiusuraRichiesta(Richiesta* richiestaTarget, const char* nuovaData);
void setValidaInHeapRichiesta(Richiesta* richiestaTarget, int flagValidita);

#endif
//Chiusura di #ifndef RICHIESTA_H