#include "report.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define MAGENTA "\033[35m"
#define BLUE    "\033[34m"

/* Funzione helper per convertire una data in numero di giorni dal 1900-01-01
   Formato data: GG/MM/AAAA */
static int dataAGiorni(const char* data) {
    if (data == NULL) return 0;
    
    int giorno, mese, anno;
    sscanf(data, "%d/%d/%d", &giorno, &mese, &anno);
    
    /* Formula per convertire una data gregoriana in numero di giorni */
    int giorni = 0;
    
    /* Aggiungiamo i giorni per gli anni completi */
    for (int a = 1900; a < anno; a++) {
        int bisestile = (a % 4 == 0 && (a % 100 != 0 || a % 400 == 0));
        giorni += bisestile ? 366 : 365;
    }
    
    /* Aggiungiamo i giorni per i mesi completi nell'anno corrente */
    int giorniPerMese[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int bisestile = (anno % 4 == 0 && (anno % 100 != 0 || anno % 400 == 0));
    if (bisestile) giorniPerMese[2] = 29;
    
    for (int m = 1; m < mese; m++) {
        giorni += giorniPerMese[m];
    }
    
    /* Aggiungiamo i giorni rimanenti */
    giorni += giorno;
    
    return giorni;
}

/* Funzione helper per calcolare la differenza di giorni tra due date */
static int differenzaGiorni(const char* dataInizio, const char* dataFine) {
    if (dataInizio == NULL || dataFine == NULL) return 0;
    return dataAGiorni(dataFine) - dataAGiorni(dataInizio);
}

/* Funzione helper per contare interventi per una tipologia specifica */
static int contaInterventiPerTipologia(const ArchivioRichieste* archivio, const char* tipologia) {
    int contatore = 0;
    
    NodoLista* nodoCorrente = getTestaArchivio(archivio);
    while (nodoCorrente != NULL) {
        Richiesta* richiesta = getRichiestaDalNodoLista(nodoCorrente);
        if (richiesta != NULL && strcmp(getTipologiaProblemaRichiesta(richiesta), tipologia) == 0) {
            contatore++;
        }
        nodoCorrente = getNextNodoLista(nodoCorrente);
    }
    
    return contatore;
}

/* Funzione helper per contare interventi per un tecnico specifico */
static int contaInterventiPerTecnico(const ArchivioRichieste* archivio, const char* codiceTecnico) {
    int contatore = 0;
    
    NodoLista* nodoCorrente = getTestaArchivio(archivio);
    while (nodoCorrente != NULL) {
        Richiesta* richiesta = getRichiestaDalNodoLista(nodoCorrente);
        if (richiesta != NULL) {
            const char* codiceAssegnato = getCodiceTecnicoAssegnatoRichiesta(richiesta);
            if (codiceAssegnato != NULL && strcmp(codiceAssegnato, codiceTecnico) == 0) {
                contatore++;
            }
        }
        nodoCorrente = getNextNodoLista(nodoCorrente);
    }
    
    return contatore;
}

/* Funzione helper per contare interventi per un appartamento specifico */
static int contaInterventiPerAppartamento(const ArchivioRichieste* archivio, const char* appartamento) {
    int contatore = 0;
    
    NodoLista* nodoCorrente = getTestaArchivio(archivio);
    while (nodoCorrente != NULL) {
        Richiesta* richiesta = getRichiestaDalNodoLista(nodoCorrente);
        if (richiesta != NULL && strcmp(getAppartamentoRichiesta(richiesta), appartamento) == 0) {
            contatore++;
        }
        nodoCorrente = getNextNodoLista(nodoCorrente);
    }
    
    return contatore;
}

/* Funzione helper per contare interventi per uno stato specifico */
static int contaInterventiPerStato(const ArchivioRichieste* archivio, StatoRichiesta stato) {
    int contatore = 0;
    
    NodoLista* nodoCorrente = getTestaArchivio(archivio);
    while (nodoCorrente != NULL) {
        Richiesta* richiesta = getRichiestaDalNodoLista(nodoCorrente);
        if (richiesta != NULL && getStatoRichiesta(richiesta) == stato) {
            contatore++;
        }
        nodoCorrente = getNextNodoLista(nodoCorrente);
    }
    
    return contatore;
}

/* Raccoglie tutte le tipologie uniche e il relativo numero di interventi */
static void raccogliTipologie(const ArchivioRichieste* archivio, RiportoTipologia** tipologie, int* numeroTipologie) {
    if (archivio == NULL || getDimensioneArchivio(archivio) == 0) {
        *numeroTipologie = 0;
        return;
    }
    
    RiportoTipologia* temp = (RiportoTipologia*)malloc(sizeof(RiportoTipologia) * getDimensioneArchivio(archivio));
    int count = 0;
    
    NodoLista* nodoCorrente = getTestaArchivio(archivio);
    while (nodoCorrente != NULL) {
        Richiesta* richiesta = getRichiestaDalNodoLista(nodoCorrente);
        if (richiesta != NULL) {
            const char* tipologia = getTipologiaProblemaRichiesta(richiesta);
            
            /* Verifica se la tipologia è già stata aggiunta */
            int trovata = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(temp[i].tipologia, tipologia) == 0) {
                    trovata = 1;
                    break;
                }
            }
            
            /* Se non trovata, aggiungila */
            if (!trovata) {
                temp[count].tipologia = (char*)malloc(strlen(tipologia) + 1);
                strcpy(temp[count].tipologia, tipologia);
                temp[count].numeroInterventi = contaInterventiPerTipologia(archivio, tipologia);
                count++;
            }
        }
        nodoCorrente = getNextNodoLista(nodoCorrente);
    }
    
    *tipologie = temp;
    *numeroTipologie = count;
}

/* Raccoglie tutte le aree uniche e il relativo numero di problemi */
static void raccogliAree(const ArchivioRichieste* archivio, RiportoArea** aree, int* numeroAree) {
    if (archivio == NULL || getDimensioneArchivio(archivio) == 0) {
        *numeroAree = 0;
        return;
    }
    
    RiportoArea* temp = (RiportoArea*)malloc(sizeof(RiportoArea) * getDimensioneArchivio(archivio));
    int count = 0;
    
    NodoLista* nodoCorrente = getTestaArchivio(archivio);
    while (nodoCorrente != NULL) {
        Richiesta* richiesta = getRichiestaDalNodoLista(nodoCorrente);
        if (richiesta != NULL) {
            const char* appartamento = getAppartamentoRichiesta(richiesta);
            
            /* Verifica se l'area è già stata aggiunta */
            int trovata = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(temp[i].appartamento, appartamento) == 0) {
                    trovata = 1;
                    break;
                }
            }
            
            /* Se non trovata, aggiungila */
            if (!trovata) {
                temp[count].appartamento = (char*)malloc(strlen(appartamento) + 1);
                strcpy(temp[count].appartamento, appartamento);
                temp[count].numeroProblemi = contaInterventiPerAppartamento(archivio, appartamento);
                count++;
            }
        }
        nodoCorrente = getNextNodoLista(nodoCorrente);
    }
    
    *aree = temp;
    *numeroAree = count;
}

/* Implementazione funzioni di report */

void generaReportPerTipologia(const ArchivioRichieste* archivio) {
    if (archivio == NULL) {
        printf(RED "Archivio non disponibile.\n" RESET);
        return;
    }
    
    RiportoTipologia* tipologie = NULL;
    int numeroTipologie = 0;
    
    raccogliTipologie(archivio, &tipologie, &numeroTipologie);
    
    printf(CYAN BOLD "\n╔════════════════════════════════════════════════╗\n" RESET);
    printf(CYAN BOLD "║   INTERVENTI PER TIPOLOGIA                     ║\n" RESET);
    printf(CYAN BOLD "╚════════════════════════════════════════════════╝\n\n" RESET);
    
    if (numeroTipologie == 0) {
        printf(YELLOW "Nessun intervento registrato.\n" RESET);
    } else {
        for (int i = 0; i < numeroTipologie; i++) {
            printf(GREEN "%-30s: %d interventi\n" RESET, 
                   tipologie[i].tipologia, 
                   tipologie[i].numeroInterventi);
            free(tipologie[i].tipologia);
        }
    }
    
    free(tipologie);
    printf("\n");
}

void generaReportStatoInterventi(const ArchivioRichieste* archivio) {
    if (archivio == NULL) {
        printf(RED "Archivio non disponibile.\n" RESET);
        return;
    }
    
    int totale = getDimensioneArchivio(archivio);
    int aperti = contaInterventiPerStato(archivio, APERTA) + 
                 contaInterventiPerStato(archivio, PIANIFICATA);
    int chiusi = contaInterventiPerStato(archivio, CONCLUSA) + 
                 contaInterventiPerStato(archivio, ANNULLATA);
    int inLavorazione = contaInterventiPerStato(archivio, IN_LAVORAZIONE);
    
    float percentualeChiusura = totale > 0 ? (chiusi * 100.0 / totale) : 0;
    
    printf(CYAN BOLD "\n╔════════════════════════════════════════════════╗\n" RESET);
    printf(CYAN BOLD "║   STATO INTERVENTI                             ║\n" RESET);
    printf(CYAN BOLD "╚════════════════════════════════════════════════╝\n\n" RESET);
    
    printf(BLUE "Interventi Totali:        " RESET "%d\n", totale);
    printf(GREEN "Interventi Aperti:        " RESET "%d\n", aperti);
    printf(MAGENTA "Interventi in Lavorazione: " RESET "%d\n", inLavorazione);
    printf(RED "Interventi Chiusi:        " RESET "%d\n", chiusi);
    printf(YELLOW "Percentuale Chiusura:     " RESET "%.2f%%\n\n", percentualeChiusura);
}

void generaReportTempoMedio(const ArchivioRichieste* archivio) {
    if (archivio == NULL) {
        printf(RED "Archivio non disponibile.\n" RESET);
        return;
    }
    
    int interventiCompletati = 0;
    int totaleGiorni = 0;
    
    NodoLista* nodoCorrente = getTestaArchivio(archivio);
    while (nodoCorrente != NULL) {
        Richiesta* richiesta = getRichiestaDalNodoLista(nodoCorrente);
        if (richiesta != NULL && getStatoRichiesta(richiesta) == CONCLUSA) {
            const char* dataCreazione = getDataRichiesta(richiesta);
            const char* dataChiusura = getDataChiusuraRichiesta(richiesta);
            
            if (dataCreazione != NULL && dataChiusura != NULL) {
                int giorni = differenzaGiorni(dataCreazione, dataChiusura);
                totaleGiorni += giorni;
                interventiCompletati++;
            }
        }
        nodoCorrente = getNextNodoLista(nodoCorrente);
    }
    
    float tempoMedio = interventiCompletati > 0 ? (totaleGiorni / (float)interventiCompletati) : 0;
    
    printf(CYAN BOLD "\n╔════════════════════════════════════════════════╗\n" RESET);
    printf(CYAN BOLD "║   TEMPO MEDIO DI COMPLETAMENTO                 ║\n" RESET);
    printf(CYAN BOLD "╚════════════════════════════════════════════════╝\n\n" RESET);
    
    if (interventiCompletati == 0) {
        printf(YELLOW "Nessun intervento completato.\n" RESET);
    } else {
        printf(GREEN "Interventi Completati:    " RESET "%d\n", interventiCompletati);
        printf(BLUE "Tempo Medio:              " RESET "%.2f giorni\n\n", tempoMedio);
    }
}

void generaReportTecnicoPiuAttivo(const ArchivioRichieste* archivio) {
    if (archivio == NULL) {
        printf(RED "Archivio non disponibile.\n" RESET);
        return;
    }
    
    /* Raccogliamo tutti i codici tecnici unici */
    const char** codiTecnici = (const char**)malloc(sizeof(const char*) * getDimensioneArchivio(archivio));
    int numeroTecnici = 0;
    
    NodoLista* nodoCorrente = getTestaArchivio(archivio);
    while (nodoCorrente != NULL) {
        Richiesta* richiesta = getRichiestaDalNodoLista(nodoCorrente);
        if (richiesta != NULL) {
            const char* codiceTecnico = getCodiceTecnicoAssegnatoRichiesta(richiesta);
            if (codiceTecnico != NULL) {
                /* Verifica se il codice è già stato aggiunto */
                int trovato = 0;
                for (int i = 0; i < numeroTecnici; i++) {
                    if (strcmp(codiTecnici[i], codiceTecnico) == 0) {
                        trovato = 1;
                        break;
                    }
                }
                
                /* Se non trovato, aggiungilo */
                if (!trovato) {
                    codiTecnici[numeroTecnici] = codiceTecnico;
                    numeroTecnici++;
                }
            }
        }
        nodoCorrente = getNextNodoLista(nodoCorrente);
    }
    
    /* Trovi il tecnico con più interventi */
    const char* tecnicoPiuAttivo = NULL;
    int maxInterventi = 0;
    
    for (int i = 0; i < numeroTecnici; i++) {
        int numeroInterventi = contaInterventiPerTecnico(archivio, codiTecnici[i]);
        if (numeroInterventi > maxInterventi) {
            maxInterventi = numeroInterventi;
            tecnicoPiuAttivo = codiTecnici[i];
        }
    }
    
    printf(CYAN BOLD "\n╔════════════════════════════════════════════════╗\n" RESET);
    printf(CYAN BOLD "║   TECNICO PIÙ ATTIVO                           ║\n" RESET);
    printf(CYAN BOLD "╚════════════════════════════════════════════════╝\n\n" RESET);
    
    if (tecnicoPiuAttivo == NULL) {
        printf(YELLOW "Nessun tecnico con interventi assegnati.\n" RESET);
    } else {
        printf(GREEN "Codice Tecnico:           " RESET "%s\n", tecnicoPiuAttivo);
        printf(BLUE "Numero Interventi:        " RESET "%d\n\n", maxInterventi);
    }
    
    free(codiTecnici);
}

void generaReportAreeProblematiche(const ArchivioRichieste* archivio) {
    if (archivio == NULL) {
        printf(RED "Archivio non disponibile.\n" RESET);
        return;
    }
    
    RiportoArea* aree = NULL;
    int numeroAree = 0;
    
    raccogliAree(archivio, &aree, &numeroAree);
    
    printf(CYAN BOLD "\n╔════════════════════════════════════════════════╗\n" RESET);
    printf(CYAN BOLD "║   AREE CON PIÙ PROBLEMI                       ║\n" RESET);
    printf(CYAN BOLD "╚════════════════════════════════════════════════╝\n\n" RESET);
    
    if (numeroAree == 0) {
        printf(YELLOW "Nessun appartamento registrato.\n" RESET);
    } else {
        /* Ordina le aree per numero di problemi (bubbleSort) */
        for (int i = 0; i < numeroAree - 1; i++) {
            for (int j = 0; j < numeroAree - i - 1; j++) {
                if (aree[j].numeroProblemi < aree[j + 1].numeroProblemi) {
                    RiportoArea temp = aree[j];
                    aree[j] = aree[j + 1];
                    aree[j + 1] = temp;
                }
            }
        }
        
        for (int i = 0; i < numeroAree; i++) {
            printf(GREEN "%-30s: %d problemi\n" RESET, 
                   aree[i].appartamento, 
                   aree[i].numeroProblemi);
            free(aree[i].appartamento);
        }
    }
    
    free(aree);
    printf("\n");
}

void generaReportCompleto(const ArchivioRichieste* archivio, const AlberoTecnici* albero) {
    if (archivio == NULL) {
        printf(RED "Archivio non disponibile.\n" RESET);
        return;
    }
    
    printf(MAGENTA BOLD "\n\n╔════════════════════════════════════════════════╗\n" RESET);
    printf(MAGENTA BOLD "║           REPORT COMPLETO SISTEMA              ║\n" RESET);
    printf(MAGENTA BOLD "╚════════════════════════════════════════════════╝\n" RESET);
    
    generaReportStatoInterventi(archivio);
    generaReportPerTipologia(archivio);
    generaReportTempoMedio(archivio);
    generaReportTecnicoPiuAttivo(archivio);
    generaReportAreeProblematiche(archivio);
    
    printf(MAGENTA BOLD "╔════════════════════════════════════════════════╗\n" RESET);
    printf(MAGENTA BOLD "║           FINE REPORT                         ║\n" RESET);
    printf(MAGENTA BOLD "╚════════════════════════════════════════════════╝\n\n" RESET);
}
