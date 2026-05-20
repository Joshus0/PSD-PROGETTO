/*
 * Implementazione dell'entita' AgendaTecnico.
 *
 * Gestisce gli appuntamenti (interventi) di un tecnico utilizzando un
 * Albero Binario di Ricerca (BST) ordinato cronologicamente (per data 
 * e fascia oraria). Fornisce funzionalita' per inserire, rimuovere,
 * consultare e contare gli interventi programmati senza creare sovrapposizioni.
 * 
 * 8 maggio
 * Joshua Sarnelli
 */
#include "agendaTecnico.h"
#include "main/utilita.h"
#include <stdlib.h>
#include <string.h>

/*
 * Struttura interna di un nodo dell'Agenda (BST).
 * 
 * Rappresenta un singolo intervento schedulato. Contiene i dettagli 
 * dell'appuntamento allocati dinamicamente (data, fascia oraria, 
 * codice richiesta) e i puntatori ai nodi figli per la struttura ad albero.
 */
struct NodoAgenda {
    char* data;
    char* fasciaOraria;
    char* codiceRichiesta;
    NodoAgenda* sinistro;
    NodoAgenda* destro;
};

/*
 * Struttura interna dell'AgendaTecnico.
 * 
 * Contiene unicamente il puntatore alla radice dell'albero binario 
 * di ricerca contenente gli appuntamenti assegnati a un tecnico.
 */
struct AgendaTecnico {
    NodoAgenda* radice;
};

/*
 * confrontaAppuntamenti - Confronta due appuntamenti cronologicamente.
 *
 * Estrae giorno, mese e anno dalle stringhe della data e li confronta 
 * per stabilire l'ordine temporale. A parita' di data, confronta le 
 * fasce orarie in modo lessicografico. E' una funzione statica di utilita' 
 * interna al modulo.
 *
 * Parametri:
 *   data1 - Data del primo appuntamento nel formato "GG/MM/AAAA"
 *   ora1  - Fascia oraria del primo appuntamento (es. "09:00-11:00")
 *   data2 - Data del secondo appuntamento
 *   ora2  - Fascia oraria del secondo appuntamento
 *
 * Ritorna:
 *   Valore < 0 se il primo precede il secondo,
 *   Valore > 0 se il primo segue il secondo,
 *   0 se coincidono (stessa data e ora).
 */
static int confrontaAppuntamenti(const char* data1, const char* ora1, const char* data2, const char* ora2) {
    int g1, m1, a1;
    int g2, m2, a2;

    /* Estraiamo le date */
    sscanf(data1, "%d/%d/%d", &g1, &m1, &a1);
    sscanf(data2, "%d/%d/%d", &g2, &m2, &a2);

    /* 1. Confronto Cronologico della Data */
    if (a1 != a2) return a1 - a2;
    if (m1 != m2) return m1 - m2;
    if (g1 != g2) return g1 - g2;

    /* 2. STESSA DATA: Controllo Matematico della Sovrapposizione Oraria */
    int h1_start, m1_start, h1_end, m1_end;
    int h2_start, m2_start, h2_end, m2_end;

    sscanf(ora1, "%d:%d-%d:%d", &h1_start, &m1_start, &h1_end, &m1_end);
    sscanf(ora2, "%d:%d-%d:%d", &h2_start, &m2_start, &h2_end, &m2_end);

    /* Convertiamo tutto in minuti dalla mezzanotte per un confronto assoluto */
    int start1 = h1_start * 60 + m1_start;
    int end1   = h1_end * 60 + m1_end;
    int start2 = h2_start * 60 + m2_start;
    int end2   = h2_end * 60 + m2_end;

    /* Se l'intervento 1 finisce prima o esattamente quando inizia il 2 */
    if (end1 <= start2) {
        return -1;
    }
    /* Se l'intervento 1 inizia dopo o esattamente quando finisce il 2 */
    else if (start1 >= end2) {
        return 1;
    }
    
    /* Se arriviamo qui, gli orari si INCROCIANO! (Es. 09-11 e 10-12). 
       Restituiamo 0. L'albero lo vedrà come "duplicato" e bloccherà l'inserimento. */
    return 0;
}

/*
 * inserisciNodoRicorsivo - Inserisce un nuovo nodo nell'albero binario di ricerca.
 *
 * Scende ricorsivamente nell'albero seguendo l'ordinamento cronologico
 * per trovare una posizione vuota. Se individua un conflitto (stessa data 
 * e fascia), rifiuta l'inserimento per evitare sovrapposizioni.
 * I campi stringa vengono allocati e copiati in modo indipendente.
 *
 * Parametri:
 *   radice          - Radice del sottoalbero corrente
 *   data            - Data dell'intervento da inserire
 *   fasciaOraria    - Fascia oraria da inserire
 *   codiceRichiesta - Codice univoco della richiesta
 *   successo        - Puntatore a una flag (impostata a 1 se l'inserimento va a buon fine, 0 altrimenti)
 *
 * Ritorna:
 *   Puntatore alla radice del sottoalbero aggiornato.
 */
static NodoAgenda* inserisciNodoRicorsivo(NodoAgenda* radice, const char* data, const char* fasciaOraria, const char* codiceRichiesta, int* successo) {
    if (radice == NULL) {
        NodoAgenda* nuovoNodo = (NodoAgenda*)malloc(sizeof(NodoAgenda));
        if (nuovoNodo == NULL) {
            *successo = 0;
            return NULL;
        }
        
        nuovoNodo->data = (char*)malloc(strlen(data) + 1);
        nuovoNodo->fasciaOraria = (char*)malloc(strlen(fasciaOraria) + 1);
        nuovoNodo->codiceRichiesta = (char*)malloc(strlen(codiceRichiesta) + 1);
        
        if (nuovoNodo->data == NULL || nuovoNodo->fasciaOraria == NULL || nuovoNodo->codiceRichiesta == NULL) {
            free(nuovoNodo->data);
            free(nuovoNodo->fasciaOraria);
            free(nuovoNodo->codiceRichiesta);
            free(nuovoNodo);
            
            *successo = 0;
            return NULL;
        }
                
        strcpy(nuovoNodo->data, data);
        strcpy(nuovoNodo->fasciaOraria, fasciaOraria);
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

/*
 * distruggiNodiRicorsivo - Dealloca ricorsivamente tutti i nodi di un sottoalbero.
 *
 * Utilizza un attraversamento post-order (figli prima del genitore) per
 * garantire che la memoria venga liberata in modo sicuro.
 *
 * Parametri:
 *   nodo - Radice del sottoalbero da distruggere (puo' essere NULL)
 */
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

/*
 * creaAgendaTecnico - Alloca e inizializza una nuova agenda vuota.
 *
 * Ritorna:
 *   Puntatore alla nuova AgendaTecnico, oppure NULL in caso di fallimento.
 */
AgendaTecnico* creaAgendaTecnico() {
    AgendaTecnico* nuovaAgenda = (AgendaTecnico*)malloc(sizeof(AgendaTecnico));
    if (nuovaAgenda != NULL) {
        nuovaAgenda->radice = NULL;
    }
    return nuovaAgenda;
}

/*
 * distruggiAgendaTecnico - Libera la memoria associata all'intera agenda.
 *
 * Distrugge l'intero albero di ricerca prima di deallocare la struttura
 * contenitore dell'agenda.
 *
 * Parametri:
 *   agendaTarget - Puntatore all'agenda da deallocare (ignorato se NULL)
 */
void distruggiAgendaTecnico(AgendaTecnico* agendaTarget) {
    if (agendaTarget != NULL) {
        distruggiNodiRicorsivo(agendaTarget->radice);
        free(agendaTarget);
    }
}

/*
 * inserisciInterventoInAgenda - Aggiunge un intervento nell'agenda del tecnico.
 *
 * Funzione pubblica wrapper per l'inserimento. Verifica la validita' dei
 * puntatori prima di delegare il lavoro alla funzione ricorsiva interna.
 *
 * Parametri:
 *   agendaTarget    - Puntatore all'agenda in cui inserire
 *   data            - Data dell'intervento ("GG/MM/AAAA")
 *   fasciaOraria    - Fascia oraria (es. "09:00-11:00")
 *   codiceRichiesta - Codice univoco della richiesta di manutenzione
 *
 * Ritorna:
 *   1 in caso di successo, 0 in caso di sovrapposizione o parametri invalidi.
 */
int inserisciInterventoInAgenda(AgendaTecnico* agendaTarget, const char* data, const char* fasciaOraria, const char* codiceRichiesta) {
    if (agendaTarget == NULL || data == NULL || fasciaOraria == NULL || codiceRichiesta == NULL) {
        return 0;
    }
    
    int successo = 0;
    agendaTarget->radice = inserisciNodoRicorsivo(agendaTarget->radice, data, fasciaOraria, codiceRichiesta, &successo);
    
    return successo;
}

/*
 * GETTER — navigazione dell'albero e lettura dei nodi
 *
 * Tutte le funzioni restituiscono NULL se il puntatore ricevuto e' nullo.
 */

/*
 * getRadiceAgenda - Restituisce la radice dell'albero binario dell'agenda.
 *
 * Parametri:
 *   agendaTarget - Puntatore all'agenda
 *
 * Ritorna:
 *   Puntatore al nodo radice dell'agenda, oppure NULL.
 */
NodoAgenda* getRadiceAgenda(const AgendaTecnico* agendaTarget) {
    if (agendaTarget == NULL) return NULL;
    return agendaTarget->radice;
}

/*
 * getFiglioSinistroAgenda - Restituisce il figlio sinistro di un nodo.
 *
 * Parametri:
 *   nodoCorrente - Puntatore al nodo
 *
 * Ritorna:
 *   Puntatore al figlio sinistro, oppure NULL.
 */
NodoAgenda* getFiglioSinistroAgenda(const NodoAgenda* nodoCorrente) {
    if (nodoCorrente == NULL) return NULL;
    return nodoCorrente->sinistro;
}

/*
 * getFiglioDestroAgenda - Restituisce il figlio destro di un nodo.
 *
 * Parametri:
 *   nodoCorrente - Puntatore al nodo
 *
 * Ritorna:
 *   Puntatore al figlio destro, oppure NULL.
 */
NodoAgenda* getFiglioDestroAgenda(const NodoAgenda* nodoCorrente) {
    if (nodoCorrente == NULL) return NULL;
    return nodoCorrente->destro;
}

/*
 * getDataIntervento - Restituisce la data dell'intervento ospitato dal nodo.
 *
 * Parametri:
 *   nodoCorrente - Puntatore al nodo
 *
 * Ritorna:
 *   Stringa con la data, oppure NULL.
 */
const char* getDataIntervento(const NodoAgenda* nodoCorrente) {
    if (nodoCorrente == NULL) return NULL;
    return nodoCorrente->data;
}

/*
 * getFasciaOrariaIntervento - Restituisce la fascia oraria dal nodo corrente.
 *
 * Parametri:
 *   nodoCorrente - Puntatore al nodo
 *
 * Ritorna:
 *   Stringa con la fascia oraria, oppure NULL.
 */
const char* getFasciaOrariaIntervento(const NodoAgenda* nodoCorrente) {
    if (nodoCorrente == NULL) return NULL;
    return nodoCorrente->fasciaOraria;
}

/*
 * getCodiceRichiestaInAgenda - Restituisce il codice della richiesta dal nodo.
 *
 * Parametri:
 *   nodoCorrente - Puntatore al nodo
 *
 * Ritorna:
 *   Stringa con il codice della richiesta, oppure NULL.
 */
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

/*
 * contaNodiAgenda - Conta ricorsivamente il numero di nodi in un sottoalbero.
 *
 * Parametri:
 *   nodo - Radice del sottoalbero corrente (puo' essere NULL)
 *
 * Ritorna:
 *   Il numero totale di nodi presenti nel sottoalbero.
 */
static int contaNodiAgenda(NodoAgenda* nodo) {
    if (nodo == NULL) return 0;
    return 1 + contaNodiAgenda(nodo->sinistro) + contaNodiAgenda(nodo->destro);
}

/*
 * getNumeroInterventiAgenda - Restituisce il totale degli interventi nell'agenda.
 *
 * Wrapper pubblico per ottenere la dimensione corrente dell'agenda 
 * (numero di appuntamenti programmati).
 *
 * Parametri:
 *   agenda - Puntatore all'agenda
 *
 * Ritorna:
 *   Il numero di interventi schedulati, oppure 0 se l'agenda e' vuota o NULL.
 */
int getNumeroInterventiAgenda(AgendaTecnico* agenda) {
    if (agenda == NULL || agenda->radice == NULL) return 0;
    return contaNodiAgenda(agenda->radice);
}