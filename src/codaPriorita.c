/*
 * Implementazione dell'entita' Coda di Priorita'.
 *
 * Gestisce le richieste di manutenzione in attesa di elaborazione
 * utilizzando una struttura dati a Max-Heap basata su array dinamico.
 * Garantisce che la richiesta con il livello di urgenza piu' alto 
 * si trovi sempre in radice, offrendo tempi logaritmici O(log n) 
 * per gli inserimenti e le estrazioni.
 * 
 * 6 maggio
 * Joshua Sarnelli
 */
#include "codaPriorita.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "main/utilita.h"

/*
 * Struttura interna della Coda di Priorita' (Max-Heap).
 *
 * L'array gestisce dinamicamente i puntatori alle entita' Richiesta.
 * 'capacita' tiene traccia dello spazio totale attualmente allocato in memoria,
 * mentre 'dimensione' indica il numero di nodi effettivamente presenti nello heap.
 */
struct CodaPriorita {
    Richiesta** array;
    int capacita;
    int dimensione;
};

/*
 * scambia - Scambia due puntatori a Richiesta all'interno dell'array.
 *
 * Funzione statica di supporto utilizzata durante i processi di 
 * riordinamento dello heap (heapify).
 *
 * Parametri:
 *   a - Indirizzo del primo puntatore alla richiesta
 *   b - Indirizzo del secondo puntatore alla richiesta
 */
static void scambia(Richiesta** a, Richiesta** b) {
    Richiesta* temp = *a;
    *a = *b;
    *b = temp;
}

/*
 * heapifyUp - Ripristina la proprieta' del max-heap dal basso verso l'alto.
 *
 * Viene invocata tipicamente dopo l'inserimento in coda di un nuovo elemento. 
 * Confronta ricorsivamente l'elemento inserito con il proprio nodo genitore; 
 * se il livello di urgenza del figlio supera quello del padre, li scambia 
 * per far "galleggiare" il nodo verso la radice.
 *
 * Parametri:
 *   coda   - Puntatore alla struttura CodaPriorita
 *   indice - L'indice nell'array del nodo da analizzare/spostare
 */
static void heapifyUp(CodaPriorita* coda, int indice) {
    if (indice == 0) return;
    int padre = (indice - 1) / 2;
    
    if (getLivelloUrgenzaRichiesta(coda->array[indice]) > getLivelloUrgenzaRichiesta(coda->array[padre])) {
        scambia(&coda->array[indice], &coda->array[padre]);
        heapifyUp(coda, padre);
    }
}

/*
 * heapifyDown - Ripristina la proprieta' del max-heap dall'alto verso il basso.
 *
 * Invocata dopo l'estrazione della radice (quando l'ultimo nodo viene spostato 
 * in cima all'albero). Confronta il nodo corrente con i suoi figli e, se 
 * necessario, lo scambia con il figlio avente priorita' maggiore, sprofondando 
 * ricorsivamente verso le foglie.
 *
 * Parametri:
 *   coda   - Puntatore alla struttura CodaPriorita
 *   indice - L'indice nell'array del nodo da far "sprofondare"
 */
static void heapifyDown(CodaPriorita* coda, int indice) {
    int massimo = indice;
    int figlioSinistro = 2 * indice + 1;
    int figlioDestro = 2 * indice + 2;

    if (figlioSinistro < coda->dimensione && 
        getLivelloUrgenzaRichiesta(coda->array[figlioSinistro]) > getLivelloUrgenzaRichiesta(coda->array[massimo])) {
        massimo = figlioSinistro;
    }

    if (figlioDestro < coda->dimensione && 
        getLivelloUrgenzaRichiesta(coda->array[figlioDestro]) > getLivelloUrgenzaRichiesta(coda->array[massimo])) {
        massimo = figlioDestro;
    }

    if (massimo != indice) {
        scambia(&coda->array[indice], &coda->array[massimo]);
        heapifyDown(coda, massimo);
    }
}

/*
 * creaCodaPriorita - Alloca e inizializza una nuova coda di priorita'.
 *
 * Configura l'array interno con la capacita' di partenza specificata.
 * Se la capacita' fornita non e' valida (<=0), applica un valore di fallback.
 *
 * Parametri:
 *   capacitaIniziale - Spazio iniziale da allocare per l'array di richieste
 *
 * Ritorna:
 *   Puntatore alla nuova coda creata, o NULL in caso di errore di allocazione.
 */
CodaPriorita* creaCodaPriorita(int capacitaIniziale) {
    if (capacitaIniziale <= 0) capacitaIniziale = 10;
    
    CodaPriorita* nuovaCoda = (CodaPriorita*)malloc(sizeof(CodaPriorita));
    if (nuovaCoda != NULL) {
        nuovaCoda->array = (Richiesta**)malloc(capacitaIniziale * sizeof(Richiesta*));
        if (nuovaCoda->array == NULL) {
            free(nuovaCoda);
            return NULL;
        }
        nuovaCoda->capacita = capacitaIniziale;
        nuovaCoda->dimensione = 0;
    }
    return nuovaCoda;
}

/*
 * distruggiCodaPriorita - Dealloca la memoria della struttura dati dello heap.
 *
 * IMPORTANTE: Libera solo l'array dei puntatori e la struttura della coda, 
 * ma NON dealloca le singole istanze di Richiesta contenute (il cui ciclo 
 * di vita appartiene all'Archivio principale).
 *
 * Parametri:
 *   codaTarget - Puntatore alla coda da eliminare (ignorato se NULL)
 */
void distruggiCodaPriorita(CodaPriorita* codaTarget) {
    if (codaTarget != NULL) {
        if (codaTarget->array != NULL) {
            free(codaTarget->array);
        }
        free(codaTarget);
    }
}

/*
 * inserisciInCodaPriorita - Accoda una richiesta ripristinando le proprieta' dell'heap.
 *
 * Inserisce l'elemento come nuova foglia alla fine dell'array per poi richiamare
 * heapifyUp. Se l'array e' pieno, ne raddoppia automaticamente la capacita' 
 * tramite realloc.
 *
 * Parametri:
 *   codaTarget          - Puntatore alla coda in cui inserire (ignorato se NULL)
 *   richiestaDaAccodare - Puntatore alla richiesta da inserire (ignorato se NULL)
 */
void inserisciInCodaPriorita(CodaPriorita* codaTarget, Richiesta* richiestaDaAccodare) {
    if (codaTarget == NULL || richiestaDaAccodare == NULL) return;

    if (codaTarget->dimensione == codaTarget->capacita) {
        int nuovaCapacita = codaTarget->capacita * 2;
        Richiesta** nuovoArray = (Richiesta**)realloc(codaTarget->array, nuovaCapacita * sizeof(Richiesta*));
        if (nuovoArray == NULL) return; 
        codaTarget->array = nuovoArray;
        codaTarget->capacita = nuovaCapacita;
    }

    codaTarget->array[codaTarget->dimensione] = richiestaDaAccodare;
    codaTarget->dimensione++;
    heapifyUp(codaTarget, codaTarget->dimensione - 1);
}

/*
 * estraiMaxDaCodaPriorita - Restituisce e rimuove la richiesta con l'urgenza massima.
 *
 * Estrae l'elemento in cima all'heap (radice, indice 0). Sposta l'ultima 
 * foglia dell'albero nella posizione della radice e richiama heapifyDown 
 * per farla sprofondare nella posizione corretta.
 *
 * Parametri:
 *   codaTarget - Puntatore alla coda da cui estrarre
 *
 * Ritorna:
 *   Puntatore alla richiesta piu' urgente, oppure NULL se lo heap e' vuoto.
 */
Richiesta* estraiMaxDaCodaPriorita(CodaPriorita* codaTarget) {
    if (codaTarget == NULL) return NULL;

    while (codaTarget->dimensione > 0) {
        Richiesta* maxRichiesta = codaTarget->array[0];
        codaTarget->dimensione--;
        codaTarget->array[0] = codaTarget->array[codaTarget->dimensione];
        codaTarget->array[codaTarget->dimensione] = NULL;

        if (codaTarget->dimensione > 0) {
            heapifyDown(codaTarget, 0);
        }

        if (maxRichiesta != NULL && isValidaInHeapRichiesta(maxRichiesta) != 0) {
            return maxRichiesta;
        }
        /* Se la richiesta estratta non è più valida, continua a cercare la successiva. */
    }

    return NULL;
}

/*
 * GETTER e FUNZIONI INFORMATIVE
 */

/*
 * isCodaPrioritaVuota - Verifica se l'heap contiene almeno una richiesta valida.
 *
 * Ritorna:
 *   1 se la coda e' vuota (o NULL) o se tutte le richieste sono invalide,
 *   0 se esiste almeno una richiesta valida.
 */
int isCodaPrioritaVuota(const CodaPriorita* codaTarget) {
    if (codaTarget == NULL || codaTarget->dimensione == 0) return 1;

    for (int i = 0; i < codaTarget->dimensione; i++) {
        if (codaTarget->array[i] != NULL && isValidaInHeapRichiesta(codaTarget->array[i]) != 0) {
            return 0;
        }
    }

    return 1;
}

/*
 * getDimensioneCodaPriorita - Restituisce il numero di richieste attualmente nello heap.
 */
int getDimensioneCodaPriorita(const CodaPriorita* codaTarget) {
    return (codaTarget != NULL) ? codaTarget->dimensione : 0;
}

/*
 * stampaCodaPriorita - Stampa una sintesi delle richieste nello heap.
 *
 * Ignora visivamente (pur mantenendole nello heap) le richieste marcate
 * come invalide (lazy deletion). Stampa gli elementi seguendo la
 * disposizione interna dell'array.
 *
 * Parametri:
 *   codaTarget - Puntatore alla coda da stampare
 */
void stampaCodaPriorita(const CodaPriorita* codaTarget) {
    if (codaTarget == NULL) {
        printf("Coda di priorita' non inizializzata.\n");
        return;
    }

    if (codaTarget->dimensione == 0) {
        printf("Nessuna richiesta in coda.\n");
        return;
    }

    int richiesteStampate = 0;
    
    printf("Richieste in coda (heap, ordine interno):\n");
    for (int i = 0; i < codaTarget->dimensione; i++) {
        Richiesta* richiesta = codaTarget->array[i];
        
        if (richiesta == NULL || isValidaInHeapRichiesta(richiesta) == 0) {
            continue;
        }

        printf("[%d] Codice: %s, Appartamento: %s, Tipologia: %s, Urgenza: %d\n",
               richiesteStampate + 1,
               getCodiceRichiesta(richiesta),
               getAppartamentoRichiesta(richiesta),
               getTipologiaProblemaRichiesta(richiesta),
               getLivelloUrgenzaRichiesta(richiesta));
               
        richiesteStampate++;
    }

    if (richiesteStampate == 0) {
        printf("Tutte le richieste presenti sono gia' state gestite o annullate.\n");
    }
}

/*
 * stampaRichiestePerStato - Filtra e stampa le richieste presenti nell'heap per stato.
 *
 * Esegue una scansione lineare dell'array interno dell'heap per visualizzare 
 * solo gli elementi il cui stato corrisponde a quello richiesto.
 */
void stampaRichiestePerStato(const CodaPriorita* codaTarget, StatoRichiesta stato) {
    if (codaTarget == NULL || codaTarget->dimensione == 0) {
        printf("Nessuna richiesta in coda.\n");
        return;
    }
    
    int trovate = 0;
    for (int i = 0; i < codaTarget->dimensione; i++) {
        Richiesta* richiesta = codaTarget->array[i];
        if (richiesta == NULL || isValidaInHeapRichiesta(richiesta) == 0) continue;
        if (getStatoRichiesta(richiesta) == stato) {
            stampaRichiesta(richiesta); // Richiama la funzione di utilita.c
            trovate++;
        }
    }
    
    if (trovate == 0) printf("Nessuna richiesta trovata per questo stato.\n");
}

/*
 * stampaRichiestePerUrgenza - Filtra e stampa le richieste per un livello di urgenza.
 */
void stampaRichiestePerUrgenza(const CodaPriorita* codaTarget, int urgenza) {
    if (codaTarget == NULL || codaTarget->dimensione == 0) {
        printf("Nessuna richiesta in coda.\n");
        return;
    }
    
    int trovate = 0;
    for (int i = 0; i < codaTarget->dimensione; i++) {
        Richiesta* richiesta = codaTarget->array[i];
        if (richiesta == NULL || isValidaInHeapRichiesta(richiesta) == 0) continue;
        if (getLivelloUrgenzaRichiesta(richiesta) == urgenza) {
            stampaRichiesta(richiesta);
            trovate++;
        }
    }
    
    if (trovate == 0) printf("Nessuna richiesta trovata con urgenza %d.\n", urgenza);
}

/*
 * stampaRichiestePerTipologia - Filtra e stampa le richieste in base al tipo di problema.
 */
void stampaRichiestePerTipologia(const CodaPriorita* codaTarget, const char* tipologia) {
    if (codaTarget == NULL || codaTarget->dimensione == 0) {
        printf("Nessuna richiesta in coda.\n");
        return;
    }
    
    int trovate = 0;
    for (int i = 0; i < codaTarget->dimensione; i++) {
        Richiesta* richiesta = codaTarget->array[i];
        if (richiesta == NULL || isValidaInHeapRichiesta(richiesta) == 0) continue;
        if (strcmp(getTipologiaProblemaRichiesta(richiesta), tipologia) == 0) {
            stampaRichiesta(richiesta);
            trovate++;
        }
    }
    
    if (trovate == 0) printf("Nessuna richiesta trovata per la tipologia: %s\n", tipologia);
}

/*
 * stampaRichiestePerAppartamento - Filtra e stampa le richieste per codice appartamento.
 */
void stampaRichiestePerAppartamento(const CodaPriorita* codaTarget, const char* appartamento) {
    if (codaTarget == NULL || codaTarget->dimensione == 0) {
        printf("Nessuna richiesta in coda.\n");
        return;
    }
    
    int trovate = 0;
    for (int i = 0; i < codaTarget->dimensione; i++) {
        Richiesta* richiesta = codaTarget->array[i];
        if (richiesta == NULL || isValidaInHeapRichiesta(richiesta) == 0) continue;
        if (strcmp(getAppartamentoRichiesta(richiesta), appartamento) == 0) {
            stampaRichiesta(richiesta);
            trovate++;
        }
    }
    
    if (trovate == 0) printf("Nessuna richiesta trovata per l'appartamento: %s\n", appartamento);
}

/*
 * stampaRichiestePerTecnico - Filtra e stampa le richieste per codice del tecnico assegnato.
 */
void stampaRichiestePerTecnico(const CodaPriorita* codaTarget, const char* codiceTecnico) {
    if (codaTarget == NULL || codaTarget->dimensione == 0) {
        printf("Nessuna richiesta in coda.\n");
        return;
    }
    
    int trovate = 0;
    for (int i = 0; i < codaTarget->dimensione; i++) {
        Richiesta* richiesta = codaTarget->array[i];
        if (richiesta == NULL || isValidaInHeapRichiesta(richiesta) == 0) continue;
        const char* tecnicoAssegnato = getCodiceTecnicoAssegnatoRichiesta(richiesta);
        if (tecnicoAssegnato != NULL && strcmp(tecnicoAssegnato, codiceTecnico) == 0) {
            stampaRichiesta(richiesta);
            trovate++;
        }
    }
    
    if (trovate == 0) printf("Nessuna richiesta trovata assegnata al tecnico: %s\n", codiceTecnico);
}