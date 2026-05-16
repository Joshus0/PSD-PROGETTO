#include "archivioRichieste.h"
#include <stdlib.h>
#include <string.h>

struct NodoLista 
{
    Richiesta* dato;      /* Puntatore alla richiesta contenuta nel nodo */
    NodoLista* next;      /* Puntatore al nodo successivo nella lista */
    NodoLista* prev;      /* Puntatore al nodo precedente nella lista */
};


struct ArchivioRichieste
{
    NodoLista* testa;     /* Puntatore al primo nodo della lista */
    NodoLista* coda;      /* Puntatore all'ultimo nodo della lista */
    int dimensione;       /* Numero di richieste attualmente nell'archivio */
};


ArchivioRichieste* creaArchivioRichieste()
{
    ArchivioRichieste* nuovoArchivio = (ArchivioRichieste*)malloc(sizeof(ArchivioRichieste));
    if(nuovoArchivio != NULL)
    {
        nuovoArchivio->testa = NULL;
        nuovoArchivio->coda = NULL;
        nuovoArchivio->dimensione = 0;
    }
    return nuovoArchivio;
}


void distruggiArchivioRichieste(ArchivioRichieste* archivioTarget)
{
    if(archivioTarget != NULL)
    {
        NodoLista* nodoCorrente = archivioTarget->testa;
        while(nodoCorrente != NULL)
        {
            NodoLista* nodoDaEliminare = nodoCorrente;
            nodoCorrente = nodoCorrente->next;
            distruggiRichiesta(nodoDaEliminare->dato); /* Libera la richiesta contenuta nel nodo */
            free(nodoDaEliminare); /* Libera il nodo stesso */
        }
        free(archivioTarget); /* Libera la struttura dell'archivio */
    }
}




void inserisciInCodaArchivio(ArchivioRichieste* archivioTarget, Richiesta* nuovaRichiesta) {
    if (archivioTarget == NULL || nuovaRichiesta == NULL) return;

    NodoLista* nuovoNodo = (NodoLista*)malloc(sizeof(NodoLista));
    if (nuovoNodo == NULL) return;

    nuovoNodo->dato = nuovaRichiesta;
    nuovoNodo->next = NULL;
    nuovoNodo->prev = archivioTarget->coda;

    if (archivioTarget->coda != NULL) {
        archivioTarget->coda->next = nuovoNodo;
    } else {
        archivioTarget->testa = nuovoNodo;
    }
    
    archivioTarget->coda = nuovoNodo;
    archivioTarget->dimensione++;
}

void rimuoviNodoDaArchivio(ArchivioRichieste* archivioTarget, NodoLista* nodoDaRimuovere) {
    if (archivioTarget == NULL || nodoDaRimuovere == NULL) return;

    if (nodoDaRimuovere->prev != NULL) {
        nodoDaRimuovere->prev->next = nodoDaRimuovere->next;
    } else {
        archivioTarget->testa = nodoDaRimuovere->next;
    }

    if (nodoDaRimuovere->next != NULL) {
        nodoDaRimuovere->next->prev = nodoDaRimuovere->prev;
    } else {
        archivioTarget->coda = nodoDaRimuovere->prev;
    }

    free(nodoDaRimuovere);
    archivioTarget->dimensione--;
}

NodoLista* getTestaArchivio(const ArchivioRichieste* archivioTarget) {
    return (archivioTarget != NULL) ? archivioTarget->testa : NULL;
}

NodoLista* getCodaArchivio(const ArchivioRichieste* archivioTarget) {
    return (archivioTarget != NULL) ? archivioTarget->coda : NULL;
}

NodoLista* getNextNodoLista(const NodoLista* nodoCorrente) {
    return (nodoCorrente != NULL) ? nodoCorrente->next : NULL;
}

NodoLista* getPrevNodoLista(const NodoLista* nodoCorrente) {
    return (nodoCorrente != NULL) ? nodoCorrente->prev : NULL;
}

Richiesta* getRichiestaDalNodoLista(const NodoLista* nodoCorrente) {
    return (nodoCorrente != NULL) ? nodoCorrente->dato : NULL;
}

int getDimensioneArchivio(const ArchivioRichieste* archivioTarget) {
    return (archivioTarget != NULL) ? archivioTarget->dimensione : 0;
}
Richiesta* cercaRichiestaPerCodice(const ArchivioRichieste* archivioTarget, const char* codiceDaCercare) {
    if (archivioTarget == NULL || codiceDaCercare == NULL) {
        return NULL;
    }

    NodoLista* nodoCorrente = archivioTarget->testa;
    
    while (nodoCorrente != NULL) {
        Richiesta* req = nodoCorrente->dato;

        if (strcmp(getCodiceRichiesta(req), codiceDaCercare) == 0) {
            return req;
        }
        nodoCorrente = nodoCorrente->next;
    }
    
    return NULL;
}
/*
 * stampaRichiesteArchivioPerStato - Stampa tutte le richieste dell'archivio
 *   il cui stato corrisponde a quello specificato.
 *
 * Scorre linearmente la lista doppia dell'archivio dal nodo testa
 * fino alla fine, confrontando lo stato di ogni richiesta con quello
 * passato come filtro. Per ogni corrispondenza richiama stampaRichiesta.
 * Se nessuna richiesta corrisponde al filtro, stampa un messaggio
 * di avviso su stdout.
 *
 * Parametri:
 *   archivio - Puntatore all'archivio da scorrere (puo' essere NULL)
 *   stato    - Valore dell'enumerazione StatoRichiesta da usare come
 *              filtro (APERTA, PIANIFICATA, IN_LAVORAZIONE,
 *              CONCLUSA, ANNULLATA)
 *
 * Ritorna:
 *   Niente (void).
 */
void stampaRichiesteArchivioPerStato(const ArchivioRichieste* archivio, StatoRichiesta stato) {
    NodoLista* nodo = getTestaArchivio(archivio);
    int trovate = 0;
    while (nodo != NULL) {
        Richiesta* r = getRichiestaDalNodoLista(nodo);
        if (r != NULL && getStatoRichiesta(r) == stato) {
            stampaRichiesta(r);
            trovate++;
        }
        nodo = getNextNodoLista(nodo);
    }
    if (trovate == 0) printf("Nessuna richiesta trovata.\n");
}

/*
 * stampaRichiesteArchivioPerUrgenza - Stampa tutte le richieste dell'archivio
 *   il cui livello di urgenza corrisponde a quello specificato.
 *
 * Parametri:
 *   archivio - Puntatore all'archivio da scorrere (puo' essere NULL)
 *   urgenza  - Livello di urgenza da usare come filtro (0-4)
 *
 * Ritorna:
 *   Niente (void).
 */
void stampaRichiesteArchivioPerUrgenza(const ArchivioRichieste* archivio, int urgenza) {
    NodoLista* nodo = getTestaArchivio(archivio);
    int trovate = 0;
    while (nodo != NULL) {
        Richiesta* r = getRichiestaDalNodoLista(nodo);
        if (r != NULL && getLivelloUrgenzaRichiesta(r) == urgenza) {
            stampaRichiesta(r);
            trovate++;
        }
        nodo = getNextNodoLista(nodo);
    }
    if (trovate == 0) printf("Nessuna richiesta trovata con urgenza %d.\n", urgenza);
}

/*
 * stampaRichiesteArchivioPerTipologia - Stampa tutte le richieste dell'archivio
 *   la cui tipologia di problema corrisponde esattamente alla stringa specificata.
 *
 * Parametri:
 *   archivio  - Puntatore all'archivio da scorrere (puo' essere NULL)
 *   tipologia - Stringa con la tipologia da usare come filtro
 *               (es. "Idraulico", "Elettrico") (puo' essere NULL)
 *
 * Ritorna:
 *   Niente (void).
 */
void stampaRichiesteArchivioPerTipologia(const ArchivioRichieste* archivio, const char* tipologia) {
    NodoLista* nodo = getTestaArchivio(archivio);
    int trovate = 0;
    while (nodo != NULL) {
        Richiesta* r = getRichiestaDalNodoLista(nodo);
        if (r != NULL && strcmp(getTipologiaProblemaRichiesta(r), tipologia) == 0) {
            stampaRichiesta(r);
            trovate++;
        }
        nodo = getNextNodoLista(nodo);
    }
    if (trovate == 0) printf("Nessuna richiesta trovata per tipologia: %s\n", tipologia);
}

/*
 * stampaRichiesteArchivioPerAppartamento - Stampa tutte le richieste dell'archivio
 *   il cui appartamento o area di provenienza corrisponde esattamente
 *   alla stringa specificata.
 *
 * Parametri:
 *   archivio     - Puntatore all'archivio da scorrere (puo' essere NULL)
 *   appartamento - Stringa con il codice appartamento o area da usare
 *                  come filtro (puo' essere NULL)
 *
 * Ritorna:
 *   Niente (void).
 */
void stampaRichiesteArchivioPerAppartamento(const ArchivioRichieste* archivio, const char* appartamento) {
    NodoLista* nodo = getTestaArchivio(archivio);
    int trovate = 0;
    while (nodo != NULL) {
        Richiesta* r = getRichiestaDalNodoLista(nodo);
        if (r != NULL && strcmp(getAppartamentoRichiesta(r), appartamento) == 0) {
            stampaRichiesta(r);
            trovate++;
        }
        nodo = getNextNodoLista(nodo);
    }
    if (trovate == 0) printf("Nessuna richiesta trovata per appartamento: %s\n", appartamento);
}

/*
 * stampaRichiesteArchivioPerTecnico - Stampa tutte le richieste dell'archivio
 *   assegnate al tecnico il cui codice corrisponde esattamente alla stringa
 *   specificata. Salta le richieste senza tecnico assegnato (campo NULL).
 *
 * Parametri:
 *   archivio      - Puntatore all'archivio da scorrere (puo' essere NULL)
 *   codiceTecnico - Codice identificativo del tecnico da usare come filtro
 *                   (puo' essere NULL)
 *
 * Ritorna:
 *   Niente (void).
 */
void stampaRichiesteArchivioPerTecnico(const ArchivioRichieste* archivio, const char* codiceTecnico) {
    NodoLista* nodo = getTestaArchivio(archivio);
    int trovate = 0;
    while (nodo != NULL) {
        Richiesta* r = getRichiestaDalNodoLista(nodo);
        if (r != NULL) {
            const char* tec = getCodiceTecnicoAssegnatoRichiesta(r);
            if (tec != NULL && strcmp(tec, codiceTecnico) == 0) {
                stampaRichiesta(r);
                trovate++;
            }
        }
        nodo = getNextNodoLista(nodo);
    }
    if (trovate == 0) printf("Nessuna richiesta trovata per tecnico: %s\n", codiceTecnico);
}