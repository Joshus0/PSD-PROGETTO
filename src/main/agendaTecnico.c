#include "agendaTecnico.h"
#include "main/utilita.h"
#include <stdlib.h>
#include <string.h>

struct NodoAgenda {
    char* data;
    char* fasciaOraria;
    char* codiceRichiesta;
    NodoAgenda* sinistro;
    NodoAgenda* destro;
};

struct AgendaTecnico {
    NodoAgenda* radice;
};

static int confrontaAppuntamenti(const char* data1, const char* ora1, const char* data2, const char* ora2) {
    int g1, m1, a1;
    int g2, m2, a2;

    /* Estraiamo giorno, mese e anno dalle due stringhe */
    sscanf(data1, "%d/%d/%d", &g1, &m1, &a1);
    sscanf(data2, "%d/%d/%d", &g2, &m2, &a2);

    /* 1. Confrontiamo prima gli anni */
    if (a1 != a2) {
        return a1 - a2;
    }
    
    /* 2. Se gli anni sono identici, confrontiamo i mesi */
    if (m1 != m2) {
        return m1 - m2;
    }
    
    /* 3. Se anni e mesi sono identici, confrontiamo i giorni */
    if (g1 != g2) {
        return g1 - g2;
    }

    /* 4. Se la data è esattamente la stessa, usiamo strcmp per l'orario 
       (es. "09:00" vs "14:00" funziona perfettamente con strcmp) */
    return strcmp(ora1, ora2);
}

static NodoAgenda* inserisciNodoRicorsivo(NodoAgenda* radice, const char* data, const char* fasciaOraria, const char* codiceRichiesta, int* successo) {
    if (radice == NULL) {
        NodoAgenda* nuovoNodo = (NodoAgenda*)malloc(sizeof(NodoAgenda));
        if (nuovoNodo == NULL) {
            *successo = 0;
            return NULL;
        }
        
        nuovoNodo->data = malloc(strlen(data) + 1); 
        strcpy(nuovoNodo->data, data);
        nuovoNodo->fasciaOraria = malloc(strlen(fasciaOraria) + 1); 
        strcpy(nuovoNodo->fasciaOraria, fasciaOraria);
        nuovoNodo->codiceRichiesta = malloc(strlen(codiceRichiesta) + 1); 
        strcpy(nuovoNodo->codiceRichiesta, codiceRichiesta);
        
        nuovoNodo->sinistro = NULL;
        nuovoNodo->destro = NULL;
        
        *successo = 1;
        return nuovoNodo;
    }

    int cmp = confrontaAppuntamenti(data, fasciaOraria, radice->data, radice->fasciaOraria);

    if (cmp < 0) {
        radice->sinistro = inserisciNodoRicorsivo(radice->sinistro, data, fasciaOraria, codiceRichiesta, successo);
    } else if (cmp > 0) {
        radice->destro = inserisciNodoRicorsivo(radice->destro, data, fasciaOraria, codiceRichiesta, successo);
    } else {
        *successo = 0; 
    }

    return radice;
}

static void distruggiNodiRicorsivo(NodoAgenda* nodo) {
    if (nodo != NULL) {
        distruggiNodiRicorsivo(nodo->sinistro);
        distruggiNodiRicorsivo(nodo->destro);
        free(nodo->data);
        free(nodo->fasciaOraria);
        free(nodo->codiceRichiesta);
        free(nodo);
    }
}

AgendaTecnico* creaAgendaTecnico() {
    AgendaTecnico* nuovaAgenda = (AgendaTecnico*)malloc(sizeof(AgendaTecnico));
    if (nuovaAgenda != NULL) {
        nuovaAgenda->radice = NULL;
    }
    return nuovaAgenda;
}

void distruggiAgendaTecnico(AgendaTecnico* agendaTarget) {
    if (agendaTarget != NULL) {
        distruggiNodiRicorsivo(agendaTarget->radice);
        free(agendaTarget);
    }
}

int inserisciInterventoInAgenda(AgendaTecnico* agendaTarget, const char* data, const char* fasciaOraria, const char* codiceRichiesta) {
    if (agendaTarget == NULL || data == NULL || fasciaOraria == NULL || codiceRichiesta == NULL) {
        return 0;
    }
    
    int successo = 0;
    agendaTarget->radice = inserisciNodoRicorsivo(agendaTarget->radice, data, fasciaOraria, codiceRichiesta, &successo);
    
    return successo;
}

NodoAgenda* getRadiceAgenda(const AgendaTecnico* agendaTarget) {
    if (agendaTarget == NULL) return NULL;
    return agendaTarget->radice;
}

NodoAgenda* getFiglioSinistroAgenda(const NodoAgenda* nodoCorrente) {
    if (nodoCorrente == NULL) return NULL;
    return nodoCorrente->sinistro;
}

NodoAgenda* getFiglioDestroAgenda(const NodoAgenda* nodoCorrente) {
    if (nodoCorrente == NULL) return NULL;
    return nodoCorrente->destro;
}

const char* getDataIntervento(const NodoAgenda* nodoCorrente) {
    if (nodoCorrente == NULL) return NULL;
    return nodoCorrente->data;
}

const char* getFasciaOrariaIntervento(const NodoAgenda* nodoCorrente) {
    if (nodoCorrente == NULL) return NULL;
    return nodoCorrente->fasciaOraria;
}

const char* getCodiceRichiestaInAgenda(const NodoAgenda* nodoCorrente) {
    if (nodoCorrente == NULL) return NULL;
    return nodoCorrente->codiceRichiesta;
}
/*
 * eliminaNodoRicorsivo - Rimuove ricorsivamente il nodo corrispondente
 *   alla coppia data/fascia dal sottoalbero radicato in radice,
 *   ripristinando la proprieta' di BST dopo la cancellazione.
 *
 * Scende ricorsivamente nell'albero confrontando la coppia data/fascia
 * con quella del nodo corrente tramite confrontaAppuntamenti. Gestisce
 * tre casi distinti alla rimozione:
 *   1. Nodo con solo figlio destro (o foglia): sostituisce il nodo
 *      con il figlio destro e dealloca le stringhe e il nodo stesso.
 *   2. Nodo con solo figlio sinistro: sostituisce il nodo con il
 *      figlio sinistro e dealloca le stringhe e il nodo stesso.
 *   3. Nodo con due figli: trova il successore in-order (minimo del
 *      sottoalbero destro), copia i suoi dati nel nodo corrente e
 *      rimuove ricorsivamente il successore dal sottoalbero destro,
 *      preservando cosi' l'ordinamento cronologico.
 *
 * Parametri:
 *   radice - Radice del sottoalbero corrente (NULL indica albero vuoto)
 *   data   - Data dell'appuntamento da rimuovere nel formato "GG/MM/AAAA"
 *   fascia - Fascia oraria dell'appuntamento da rimuovere,
 *            es. "09:00-11:00"
 *
 * Ritorna:
 *   Puntatore alla radice del sottoalbero aggiornato dopo la rimozione,
 *   oppure NULL se il sottoalbero era vuoto o il nodo non e' stato trovato.
 */
static NodoAgenda* eliminaNodoRicorsivo(NodoAgenda* radice, const char* data, const char* fascia) {
    if (radice == NULL) return NULL;
    
    int cmp = confrontaAppuntamenti(data, fascia, radice->data, radice->fasciaOraria);
    if (cmp < 0) {
        radice->sinistro = eliminaNodoRicorsivo(radice->sinistro, data, fascia);
    } else if (cmp > 0) {
        radice->destro = eliminaNodoRicorsivo(radice->destro, data, fascia);
    } else {
        if (radice->sinistro == NULL) {
            NodoAgenda* tmp = radice->destro;
            free(radice->data); 
            free(radice->fasciaOraria);
            free(radice->codiceRichiesta); 
            free(radice);
            return tmp;
        } else if (radice->destro == NULL) {
            NodoAgenda* tmp = radice->sinistro;
            free(radice->data); 
            free(radice->fasciaOraria);
            free(radice->codiceRichiesta); 
            free(radice);
            return tmp;
        } else {
            NodoAgenda* succ = radice->destro;
            while (succ->sinistro != NULL) succ = succ->sinistro;

            char* tmpData = malloc(strlen(succ->data) + 1);
            char* tmpFascia = malloc(strlen(succ->fasciaOraria) + 1);
            char* tmpCodice = malloc(strlen(succ->codiceRichiesta) + 1);

            if (tmpData != NULL && tmpFascia != NULL && tmpCodice != NULL) {
                strcpy(tmpData, succ->data);
                strcpy(tmpFascia, succ->fasciaOraria);
                strcpy(tmpCodice, succ->codiceRichiesta);

                free(radice->data);
                free(radice->fasciaOraria);
                free(radice->codiceRichiesta);

                radice->data = tmpData;
                radice->fasciaOraria = tmpFascia;
                radice->codiceRichiesta = tmpCodice;
            }

            radice->destro = eliminaNodoRicorsivo(radice->destro, succ->data, succ->fasciaOraria);
        }
    }
    return radice;
}
/*
 * rimuoviInterventoDaAgenda - Rimuove dall'agenda l'intervento
 *   identificato dalla coppia data/fascia oraria.
 *
 * Wrapper pubblico della funzione ricorsiva eliminaNodoRicorsivo.
 * Aggiorna il puntatore alla radice dell'agenda dopo la rimozione,
 * necessario nel caso in cui il nodo eliminato fosse la radice stessa.
 *
 * Parametri:
 *   agenda - Puntatore all'agenda da cui rimuovere l'intervento
 *            (ignorato se NULL)
 *   data   - Data dell'appuntamento da rimuovere nel formato
 *            "GG/MM/AAAA" (non NULL)
 *   fascia - Fascia oraria dell'appuntamento da rimuovere,
 *            es. "09:00-11:00" (non NULL)
 *
 * Ritorna:
 *   Niente (void).
 */
void rimuoviInterventoDaAgenda(AgendaTecnico* agenda, const char* data, const char* fascia) {
    if (agenda == NULL) return;
    agenda->radice = eliminaNodoRicorsivo(agenda->radice, data, fascia);
}

static int contaNodiAgenda(NodoAgenda* nodo) {
    if (nodo == NULL) return 0;
    return 1 + contaNodiAgenda(nodo->sinistro) + contaNodiAgenda(nodo->destro);
}


int getNumeroInterventiAgenda(AgendaTecnico* agenda) {
    if (agenda == NULL || agenda->radice == NULL) return 0;
    return contaNodiAgenda(agenda->radice);
}