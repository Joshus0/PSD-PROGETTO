/*
 * Implementazione di un albero binario di ricerca (BST) per la gestione
 * dell'insieme dei tecnici registrati nel sistema.
 *
 * I tecnici sono ordinati in base al loro codice identificativo tramite
 * confronto lessicografico, cosi' da garantire ricerche in O(log n) nel
 * caso medio. Ogni nodo dell'albero possiede e gestisce la memoria del
 * Tecnico che contiene: la distruzione dell'albero dealloca anche tutte
 * le entita' Tecnico al suo interno.
 * 
 * 7 maggio
 * Sabato Pio
 */
#include "alberoTecnici.h"
#include "entita/tecnico.h"
#include <stdlib.h>
#include <string.h>
/* 
 * STRUTTURE DATI INTERNE  
 */
 
/*
 * NodoAlberoTecnici - Nodo interno del BST.
 *
 * Ogni nodo possiede un puntatore al Tecnico (ownership: la distruzione
 * del nodo dealloca anche il Tecnico) e i riferimenti ai due sottoalberi.
 * Il sottoalbero sinistro contiene tecnici con codice lessicograficamente
 * minore, quello destro con codice maggiore.
 */
struct NodoAlberoTecnici {
    Tecnico* dato;                  
    NodoAlberoTecnici* sinistro;    
    NodoAlberoTecnici* destro;      
};

/*
 * AlberoTecnici - Struttura wrapper che espone l'albero al codice esterno.
 *
 * Nasconde il puntatore alla radice dietro un tipo opaco, cosi' da
 * impedire manipolazioni dirette della struttura interna.
 */
struct AlberoTecnici {
    NodoAlberoTecnici* radice;      /* Radice dell'albero */
};

/*
 * FUNZIONI DI SUPPORTO INTERNE (static)
 *
 * Non sono visibili all'esterno del modulo. Operano ricorsivamente sui
 * nodi e vengono richiamate dalle funzioni pubbliche che gestiscono
 * il wrapper AlberoTecnici.
 */

/*
 * inserisciNodoRicorsivo - Inserisce un tecnico nel BST mantenendo l'ordinamento.
 *
 * Scende ricorsivamente nell'albero confrontando il codice del nuovo tecnico
 * con quello della radice corrente, finche' non trova una foglia libera dove
 * allocare il nuovo nodo. I duplicati (stesso codice) vengono silenziosamente
 * ignorati per garantire l'unicita' dei tecnici nell'albero.
 *
 * Parametri:
 *   nodoCorrente - Radice del sottoalbero corrente (NULL indica albero vuoto)
 *   nuovoTecnico - Tecnico da inserire (non NULL)
 *
 * Ritorna:
 *   Il puntatore alla radice del sottoalbero aggiornato.
 */
static NodoAlberoTecnici* inserisciNodoRicorsivo(NodoAlberoTecnici* nodoCorrente, Tecnico* nuovoTecnico) {
    int confronto;

    if (nodoCorrente == NULL) {
        NodoAlberoTecnici* nuovoNodo = (NodoAlberoTecnici*)malloc(sizeof(NodoAlberoTecnici));
        if (nuovoNodo != NULL) {
            nuovoNodo->dato = nuovoTecnico;
            nuovoNodo->sinistro = NULL;
            nuovoNodo->destro = NULL;
        }
        return nuovoNodo;
    }

    /* Confronto alfanumerico tra i codici per determinare il percorso */
    confronto = strcmp(getCodiceTecnico(nuovoTecnico), getCodiceTecnico(nodoCorrente->dato));
    
    /* Se l'elemento e' minore della radice, lo inserisce nel sottoalbero sinistro */
    if (confronto < 0) {
        nodoCorrente->sinistro = inserisciNodoRicorsivo(nodoCorrente->sinistro, nuovoTecnico);
    } 
    /* Se l'elemento e' maggiore della radice, lo inserisce nel sottoalbero destro */
    else if (confronto > 0) {
        nodoCorrente->destro = inserisciNodoRicorsivo(nodoCorrente->destro, nuovoTecnico);
    }
    /* Se confronto == 0, il nodo coincide con la radice e non fa niente (no duplicati) */

    return nodoCorrente;
}

/*
 * cercaTecnicoRicorsivo - Ricerca un tecnico per codice nel BST.
 *
 * Sfrutta la proprieta' di ordinamento del BST per dimezzare lo spazio
 * di ricerca ad ogni passo, ottenendo complessita' O(log n) nel caso medio.
 *
 * Parametri:
 *   nodoCorrente   - Radice del sottoalbero in cui cercare
 *   codiceDaCercare - Codice del tecnico da trovare (non NULL)
 *
 * Ritorna:
 *   Puntatore al Tecnico trovato, oppure NULL se non presente nell'albero.
 */
static Tecnico* cercaTecnicoRicorsivo(NodoAlberoTecnici* nodoCorrente, const char* codiceDaCercare) {
    int confronto;

    if (nodoCorrente == NULL) {
        return NULL;
    }

    confronto = strcmp(codiceDaCercare, getCodiceTecnico(nodoCorrente->dato));

    /* Se l'elemento cercato coincide con la radice restituisce il dato */
    if (confronto == 0) {
        return nodoCorrente->dato; 
    } 
    /* Se l'elemento cercato e' minore, restituisce il risultato della ricerca nel sottoalbero sinistro */
    else if (confronto < 0) {
        return cercaTecnicoRicorsivo(nodoCorrente->sinistro, codiceDaCercare);
    } 
    /* Se l'elemento cercato e' maggiore, restituisce il risultato della ricerca nel sottoalbero destro */
    else {
        return cercaTecnicoRicorsivo(nodoCorrente->destro, codiceDaCercare);
    }
}

/*
 * distruggiNodiRicorsivo - Dealloca ricorsivamente tutti i nodi del sottoalbero.
 *
 * Utilizza una visita in post-ordine (sinistro -> destro -> radice) per
 * garantire che i figli vengano liberati prima del padre, evitando
 * di perdere i riferimenti ai sottoalberi prima di averli deallocati.
 * Ogni nodo dealloca anche il Tecnico che possiede.
 *
 * Parametri:
 *   nodoCorrente - Radice del sottoalbero da deallocare (ignorato se NULL)
 */
static void distruggiNodiRicorsivo(NodoAlberoTecnici* nodoCorrente) {
    if (nodoCorrente != NULL) {
        /* Distrugge prima i sottoalberi */
        distruggiNodiRicorsivo(nodoCorrente->sinistro);
        distruggiNodiRicorsivo(nodoCorrente->destro);
        
        /* Libera la memoria allocata per l'entita' Tecnico e per il nodo stesso */
        distruggiTecnico(nodoCorrente->dato); 
        free(nodoCorrente);                   
    }
}

/*
 * visitaRicorsivo - Visita in ordine simmetrico (in-order) il sottoalbero.
 *
 * La visita in-order (sinistro -> radice -> destro) garantisce che i tecnici
 * vengano visitati in ordine lessicografico crescente per codice, utile
 * per stampe e iterazioni ordinate.
 *
 * Parametri:
 *   nodo      - Radice del sottoalbero da visitare
 *   visitatore - Funzione da applicare ad ogni Tecnico incontrato
 */
static void visitaRicorsivo(NodoAlberoTecnici* nodo, void (*visitatore)(Tecnico*)) {
    if (nodo == NULL) return;
    visitaRicorsivo(nodo->sinistro, visitatore);
    visitatore(nodo->dato);
    visitaRicorsivo(nodo->destro, visitatore);
}

/*
 * FUNZIONI PUBBLICHE
 */

/*
 * creaAlberoTecnici - Alloca e restituisce un albero vuoto.
 *
 * Ritorna:
 *   Puntatore al nuovo AlberoTecnici, oppure NULL se la malloc fallisce.
 */
AlberoTecnici* creaAlberoTecnici() {
    AlberoTecnici* nuovoAlbero = (AlberoTecnici*)malloc(sizeof(AlberoTecnici));
    if (nuovoAlbero != NULL) {
        nuovoAlbero->radice = NULL;
    }
    return nuovoAlbero;
}

/*
 * distruggiAlberoTecnici - Dealloca l'intero albero e tutti i tecnici contenuti.
 *
 * Dopo questa chiamata il puntatore alberoTarget non e' piu' valido.
 *
 * Parametri:
 *   alberoTarget - Albero da deallocare (ignorato se NULL)
 */
void distruggiAlberoTecnici(AlberoTecnici* alberoTarget) {
    if (alberoTarget != NULL) {
        distruggiNodiRicorsivo(alberoTarget->radice);
        free(alberoTarget);
    }
}

/*
 * inserisciInAlberoTecnici - Inserisce un tecnico nell'albero mantenendo l'ordine BST.
 *
 * Se un tecnico con lo stesso codice e' gia' presente, l'inserimento viene
 * ignorato (niente duplicati). La proprieta' di ordinamento e' basata sul
 * confronto lessicografico dei codici tecnico.
 *
 * Parametri:
 *   alberoTarget  - Albero in cui inserire (ignorato se NULL)
 *   nuovoTecnico  - Tecnico da inserire (ignorato se NULL)
 */
void inserisciInAlberoTecnici(AlberoTecnici* alberoTarget, Tecnico* nuovoTecnico) {
    if (alberoTarget == NULL || nuovoTecnico == NULL) return;
    alberoTarget->radice = inserisciNodoRicorsivo(alberoTarget->radice, nuovoTecnico);
}
 
/*
 * cercaTecnicoInAlbero - Cerca un tecnico per codice nell'albero.
 *
 * Parametri:
 *   alberoTarget    - Albero in cui cercare (puo' essere NULL)
 *   codiceDaCercare - Codice del tecnico cercato (puo' essere NULL)
 *
 * Ritorna:
 *   Puntatore al Tecnico trovato, oppure NULL se non presente o se
 *   uno degli argomenti e' NULL.
 */
Tecnico* cercaTecnicoInAlbero(const AlberoTecnici* alberoTarget, const char* codiceDaCercare) {
    if (alberoTarget == NULL || codiceDaCercare == NULL) return NULL;
    return cercaTecnicoRicorsivo(alberoTarget->radice, codiceDaCercare);
}
 
/*
 * visitaAlberoTecnici - Applica una funzione a ogni tecnico in ordine di codice.
 *
 * Esegue una visita in-order, quindi i tecnici vengono passati al visitatore
 * in ordine lessicografico crescente per codice. Utile per stampare l'elenco
 * completo o per applicare operazioni bulk sull'intero insieme dei tecnici.
 *
 * Parametri:
 *   alberoTarget - Albero da visitare (ignorato se NULL)
 *   visitatore   - Funzione da applicare a ogni Tecnico (ignorata se NULL)
 */
void visitaAlberoTecnici(const AlberoTecnici* alberoTarget, void (*visitatore)(Tecnico*)) {
    if (alberoTarget == NULL || visitatore == NULL) return;
    visitaRicorsivo(alberoTarget->radice, visitatore);
}

/*
 * GETTER DEI NODI
 *
 * Espongono in sola lettura la struttura interna dell'albero, permettendo
 * al codice esterno di navigarlo senza accedere direttamente ai campi.
 * Tutte le funzioni restituiscono NULL se il puntatore ricevuto e' NULL.
 */

/*
 * getRadiceAlberoTecnici - Restituisce il nodo radice dell'albero.
 *
 * Parametri:
 *   alberoTarget - Albero di cui ottenere la radice (puo' essere NULL)
 *
 * Ritorna:
 *   Puntatore al nodo radice, oppure NULL se l'albero e' vuoto o NULL.
 */
NodoAlberoTecnici* getRadiceAlberoTecnici(const AlberoTecnici* alberoTarget) {
    if (alberoTarget != NULL) {
        return alberoTarget->radice;
    } else {
        return NULL;
    }
}

/*
 * getFiglioSinistroTecnici - Restituisce il figlio sinistro di un nodo.
 *
 * Parametri:
 *   nodoCorrente - Nodo di cui ottenere il figlio sinistro (puo' essere NULL)
 *
 * Ritorna:
 *   Puntatore al figlio sinistro, oppure NULL se il nodo e' una foglia o NULL.
 */
NodoAlberoTecnici* getFiglioSinistroTecnici(const NodoAlberoTecnici* nodoCorrente) {
    if (nodoCorrente != NULL) {
        return nodoCorrente->sinistro;
    } else {
        return NULL;
    }
}

/*
 * getFiglioDestroTecnici - Restituisce il figlio destro di un nodo.
 *
 * Parametri:
 *   nodoCorrente - Nodo di cui ottenere il figlio destro (puo' essere NULL)
 *
 * Ritorna:
 *   Puntatore al figlio destro, oppure NULL se il nodo e' una foglia o NULL.
 */
NodoAlberoTecnici* getFiglioDestroTecnici(const NodoAlberoTecnici* nodoCorrente) {
    if (nodoCorrente != NULL) {
        return nodoCorrente->destro;
    } else {
        return NULL;
    }
}

/*
 * getTecnicoDalNodoAlbero - Restituisce il Tecnico contenuto in un nodo.
 *
 * Parametri:
 *   nodoCorrente - Nodo da cui estrarre il Tecnico (puo' essere NULL)
 *
 * Ritorna:
 *   Puntatore al Tecnico, oppure NULL se nodoCorrente e' NULL.
 */
Tecnico* getTecnicoDalNodoAlbero(const NodoAlberoTecnici* nodoCorrente) {
    if (nodoCorrente != NULL) {
        return nodoCorrente->dato;
    } else {
        return NULL;
    }
}