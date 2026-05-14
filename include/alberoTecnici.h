#ifndef ALBERO_TECNICI_H    //Guardia di inclusione per evitare inclusioni multiple
#define ALBERO_TECNICI_H

#include "entita/tecnico.h" //Inclusione dell'header per la definizione della struttura Tecnico e delle funzioni correlate, necessaria per gestire i tecnici nell'albero binario di ricerca

typedef struct AlberoTecnici AlberoTecnici; //Forward declaration della struttura AlberoTecnici presente in alberoTecnici.c, utilizzata per implementare l'albero binario di ricerca dei tecnici
typedef struct NodoAlberoTecnici NodoAlberoTecnici; //Forward declaration della struttura NodoAlberoTecnici presente in alberoTecnici.c, utilizzata per implementare i nodi dell'albero binario di ricerca dei tecnici

//Dichiarazione delle funzioni per la gestione dell'albero dei tecnici
AlberoTecnici* creaAlberoTecnici();
void distruggiAlberoTecnici(AlberoTecnici* alberoTarget);

//Funzioni per la manipolazione dell'albero dei tecnici
void inserisciInAlberoTecnici(AlberoTecnici* alberoTarget, Tecnico* nuovoTecnico);
Tecnico* cercaTecnicoInAlbero(const AlberoTecnici* alberoTarget, const char* codiceDaCercare);

//Funzioni per accedere ai nodi e ai tecnici nell'albero
NodoAlberoTecnici* getRadiceAlberoTecnici(const AlberoTecnici* alberoTarget);
NodoAlberoTecnici* getFiglioSinistroTecnici(const NodoAlberoTecnici* nodoCorrente);
NodoAlberoTecnici* getFiglioDestroTecnici(const NodoAlberoTecnici* nodoCorrente);
Tecnico* getTecnicoDalNodoAlbero(const NodoAlberoTecnici* nodoCorrente);

/*
 * Funzione: visitaAlberoTecnici
 * ------------------------------
 * Esegue una visita in ordine simmetrico (in-order: sinistro -> radice
 * -> destro) dell'albero, applicando la funzione visitatore a ogni
 * Tecnico incontrato. Poiche' la visita e' in-order, i tecnici vengono
 * passati al visitatore in ordine lessicografico crescente per codice.
 * Usata per operazioni bulk come la stampa dell'elenco completo o il
 * monitoraggio del carico di lavoro.
 *
 * Parametri:
 *   alberoTarget - Albero da visitare (puo' essere NULL)
 *   visitatore   - Funzione da applicare a ogni Tecnico incontrato
 *                  (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce
 *   internamente il caso alberoTarget == NULL o visitatore == NULL.
 *
 * Post-condizione:
 *   La funzione visitatore viene chiamata una volta per ogni Tecnico
 *   presente nell'albero, in ordine lessicografico di codice.
 *   La struttura dell'albero non viene modificata (a meno che il
 *   visitatore stesso non modifichi i Tecnici).
 *
 * Ritorna:
 *   Niente (void).
 */
void visitaAlberoTecnici(const AlberoTecnici* alberoTarget, void (*visitatore)(Tecnico*));
#endif
//Chiusura di #ifndef ALBERO_TECNICI_H