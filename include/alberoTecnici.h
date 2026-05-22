#ifndef ALBERO_TECNICI_H    //Guardia di inclusione per evitare inclusioni multiple
#define ALBERO_TECNICI_H

#include "entita/tecnico.h" //Inclusione dell'header per la definizione della struttura Tecnico e delle funzioni correlate, necessaria per gestire i tecnici nell'albero binario di ricerca

typedef struct AlberoTecnici AlberoTecnici; //Forward declaration della struttura AlberoTecnici presente in alberoTecnici.c, utilizzata per implementare l'albero binario di ricerca dei tecnici
typedef struct NodoAlberoTecnici NodoAlberoTecnici; //Forward declaration della struttura NodoAlberoTecnici presente in alberoTecnici.c, utilizzata per implementare i nodi dell'albero binario di ricerca dei tecnici

//Dichiarazione delle funzioni per la gestione dell'albero dei tecnici

/*
 * Funzione: creaAlberoTecnici
 * ----------------------------
 * Alloca e inizializza un nuovo albero binario di ricerca (BST) vuoto
 * per la gestione dei tecnici del sistema.
 *
 * Parametri:
 *   Nessuno.
 *
 * Pre-condizione:
 *   Nessuna.
 *
 * Post-condizione:
 *   Un nuovo AlberoTecnici viene allocato in heap con radice NULL.
 *
 * Ritorna:
 *   Puntatore al nuovo AlberoTecnici allocato, oppure NULL in caso di
 *   fallimento della malloc.
 */
AlberoTecnici* creaAlberoTecnici();

/*
 * Funzione: distruggiAlberoTecnici
 * --------------------------------
 * Libera tutta la memoria associata all'albero, inclusi tutti i tecnici
 * e i nodi contenuti all'interno.
 *
 * Parametri:
 *   alberoTarget - Puntatore all'albero da deallocare (ignorato se NULL)
 *
 * Pre-condizione:
 *   alberoTarget deve puntare a memoria allocata precedentemente tramite
 *   creaAlberoTecnici, o essere NULL.
 *
 * Post-condizione:
 *   La memoria dell'albero e di tutti i tecnici viene liberata. Il puntatore
 *   alberoTarget diventa invalido dopo questa chiamata.
 *
 * Ritorna:
 *   Niente (void).
 */
void distruggiAlberoTecnici(AlberoTecnici* alberoTarget);

//Funzioni per la manipolazione dell'albero dei tecnici

/*
 * Funzione: inserisciInAlberoTecnici
 * -----------------------------------
 * Inserisce un nuovo tecnico nell'albero mantenendo la proprieta' del BST
 * (ordinamento lessicografico per codice del tecnico). I duplicati vengono
 * silenziosamente ignorati per mantenere l'unicita' dei tecnici.
 *
 * Parametri:
 *   alberoTarget - Puntatore all'albero dove inserire (ignorato se NULL)
 *   nuovoTecnico - Puntatore al tecnico da inserire (ignorato se NULL)
 *
 * Pre-condizione:
 *   alberoTarget deve puntare a un AlberoTecnici valido allocato precedentemente.
 *   nuovoTecnico deve puntare a un Tecnico valido allocato precedentemente.
 *
 * Post-condizione:
 *   Se alberoTarget e nuovoTecnico sono validi e il codice del tecnico non
 *   e' gia' presente, il tecnico viene inserito nell'albero mantenendo l'ordine BST.
 *
 * Ritorna:
 *   Niente (void).
 */
void inserisciInAlberoTecnici(AlberoTecnici* alberoTarget, Tecnico* nuovoTecnico);

/*
 * Funzione: cercaTecnicoInAlbero
 * --------------------------------
 * Effettua una ricerca nel BST per individuare il tecnico con il codice
 * specificato, sfruttando l'ordinamento lessicografico per ridurre
 * lo spazio di ricerca (complessita' O(log n) nel caso medio).
 *
 * Parametri:
 *   alberoTarget    - Puntatore all'albero dove cercare (puo' essere NULL)
 *   codiceDaCercare - Codice identificativo del tecnico (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   i casi alberoTarget == NULL o codiceDaCercare == NULL.
 *
 * Post-condizione:
 *   Nessuna modifica allo stato dell'albero.
 *
 * Ritorna:
 *   Puntatore al Tecnico trovato, oppure NULL se nessun tecnico corrisponde
 *   al codice o se l'albero e' vuoto.
 */
Tecnico* cercaTecnicoInAlbero(const AlberoTecnici* alberoTarget, const char* codiceDaCercare);

//Funzioni per accedere ai nodi e ai tecnici nell'albero

/*
 * Funzione: getRadiceAlberoTecnici
 * --------------------------------
 * Restituisce il puntatore al nodo radice dell'albero binario di ricerca.
 *
 * Parametri:
 *   alberoTarget - Puntatore all'albero (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso alberoTarget == NULL.
 *
 * Post-condizione:
 *   Nessuna modifica allo stato del sistema.
 *
 * Ritorna:
 *   Puntatore al nodo radice, oppure NULL se l'albero e' vuoto o alberoTarget e' NULL.
 */
NodoAlberoTecnici* getRadiceAlberoTecnici(const AlberoTecnici* alberoTarget);

/*
 * Funzione: getFiglioSinistroTecnici
 * -----------------------------------
 * Restituisce il puntatore al figlio sinistro del nodo corrente nel BST.
 * Il figlio sinistro contiene tecnici con codice lessicograficamente minore.
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
 *   Puntatore al figlio sinistro, oppure NULL se il nodo non ha figlio sinistro
 *   o se nodoCorrente e' NULL.
 */
NodoAlberoTecnici* getFiglioSinistroTecnici(const NodoAlberoTecnici* nodoCorrente);

/*
 * Funzione: getFiglioDestroTecnici
 * --------------------------------
 * Restituisce il puntatore al figlio destro del nodo corrente nel BST.
 * Il figlio destro contiene tecnici con codice lessicograficamente maggiore.
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
 *   Puntatore al figlio destro, oppure NULL se il nodo non ha figlio destro
 *   o se nodoCorrente e' NULL.
 */
NodoAlberoTecnici* getFiglioDestroTecnici(const NodoAlberoTecnici* nodoCorrente);

/*
 * Funzione: getTecnicoDalNodoAlbero
 * ---------------------------------
 * Estrae il puntatore al Tecnico contenuto nel nodo dell'albero.
 *
 * Parametri:
 *   nodoCorrente - Puntatore al nodo dalla quale estrarre il tecnico (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso nodoCorrente == NULL.
 *
 * Post-condizione:
 *   Nessuna modifica allo stato del sistema.
 *
 * Ritorna:
 *   Puntatore al Tecnico ospitato dal nodo, oppure NULL se nodoCorrente e' NULL.
 */
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