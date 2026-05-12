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

/* =========================================================================
 * IMPLEMENTAZIONE FUNZIONI DI REPORT (CON DESIGN SYSTEM ASCII)
 * ========================================================================= */

void generaReportPerTipologia(const ArchivioRichieste* archivio) {
    if (archivio == NULL) {
        printf(RED BOLD "\n [ ERRORE ] Archivio non disponibile.\n" RESET);
        return;
    }
    
    RiportoTipologia* tipologie = NULL;
    int numeroTipologie = 0;
    
    raccogliTipologie(archivio, &tipologie, &numeroTipologie);
    
    printf(CYAN BOLD "\n ________________________________________________________________________________________ \n");
    printf("|                                                                                        |\n");
    printf("|                 " MAGENTA BOLD "[ REPORT ] Interventi per Tipologia" CYAN BOLD "                                    |\n");
    printf("|________________________________________________________________________________________|\n" RESET);
    
    if (numeroTipologie == 0) {
        printf(CYAN "|" RESET YELLOW " Nessun intervento registrato al momento.                                               " CYAN "|\n" RESET);
        printf(CYAN "|________________________________________________________________________________________|\n" RESET);
    } else {
        printf(CYAN "| %-42s | %-41s |\n", "TIPOLOGIA PROBLEMA", "NUMERO INTERVENTI");
        printf("|--------------------------------------------|-------------------------------------------|\n" RESET);
        for (int i = 0; i < numeroTipologie; i++) {
            printf(CYAN "|" RESET " %-42s " CYAN "|" RESET " %-41d " CYAN "|\n" RESET, 
                   tipologie[i].tipologia, 
                   tipologie[i].numeroInterventi);
            free(tipologie[i].tipologia);
        }
        printf(CYAN "|____________________________________________|___________________________________________|\n" RESET);
    }
    
    free(tipologie);
    printf("\n");
}

void generaReportStatoInterventi(const ArchivioRichieste* archivio) {
    if (archivio == NULL) {
        printf(RED BOLD "\n [ ERRORE ] Archivio non disponibile.\n" RESET);
        return;
    }
    
    int totale = getDimensioneArchivio(archivio);
    int aperti = contaInterventiPerStato(archivio, APERTA) + 
                 contaInterventiPerStato(archivio, PIANIFICATA);
    int chiusi = contaInterventiPerStato(archivio, CONCLUSA) + 
                 contaInterventiPerStato(archivio, ANNULLATA);
    int inLavorazione = contaInterventiPerStato(archivio, IN_LAVORAZIONE);
    
    float percentualeChiusura = totale > 0 ? (chiusi * 100.0 / totale) : 0;
    
    printf(CYAN BOLD "\n ________________________________________________________________________________________ \n");
    printf("|                                                                                        |\n");
    printf("|                 " MAGENTA BOLD "[ REPORT ] Stato Globale Interventi" CYAN BOLD "                                    |\n");
    printf("|________________________________________________________________________________________|\n" RESET);
    printf(CYAN "| %-42s | %-41s |\n", "METRICA DI VALUTAZIONE", "VALORE ASSOLUTO");
    printf("|--------------------------------------------|-------------------------------------------|\n" RESET);
    printf(CYAN "|" RESET " %-42s " CYAN "|" RESET " %-41d " CYAN "|\n" RESET, "Interventi Totali Registrati", totale);
    printf(CYAN "|" RESET " %-42s " CYAN "|" RESET " %-41d " CYAN "|\n" RESET, "Interventi Aperti / Pianificati", aperti);
    printf(CYAN "|" RESET " %-42s " CYAN "|" RESET " %-41d " CYAN "|\n" RESET, "Interventi Attualmente In Lavorazione", inLavorazione);
    printf(CYAN "|" RESET " %-42s " CYAN "|" RESET " %-41d " CYAN "|\n" RESET, "Interventi Chiusi / Annullati", chiusi);
    printf(CYAN "|--------------------------------------------|-------------------------------------------|\n" RESET);
    
    char bufferPerc[50];
    sprintf(bufferPerc, "%.2f%%", percentualeChiusura);
    printf(CYAN "|" RESET YELLOW " %-42s " CYAN "|" RESET YELLOW " %-41s " CYAN "|\n" RESET, "Tasso di Completamento (Success Rate)", bufferPerc);
    printf(CYAN "|____________________________________________|___________________________________________|\n" RESET);
    printf("\n");
}

void generaReportTempoMedio(const ArchivioRichieste* archivio) {
    if (archivio == NULL) {
        printf(RED BOLD "\n [ ERRORE ] Archivio non disponibile.\n" RESET);
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
    
    printf(CYAN BOLD "\n ________________________________________________________________________________________ \n");
    printf("|                                                                                        |\n");
    printf("|                 " MAGENTA BOLD "[ REPORT ] Tempo Medio di Completamento" CYAN BOLD "                                |\n");
    printf("|________________________________________________________________________________________|\n" RESET);
    
    if (interventiCompletati == 0) {
        printf(CYAN "|" RESET YELLOW " Nessun intervento completato al momento. Impossibile calcolare il tempo.               " CYAN "|\n" RESET);
        printf(CYAN "|________________________________________________________________________________________|\n" RESET);
    } else {
        printf(CYAN "| %-42s | %-41s |\n", "PARAMETRO", "RISULTATO");
        printf("|--------------------------------------------|-------------------------------------------|\n" RESET);
        
        char bufferTempo[50];
        sprintf(bufferTempo, "%.2f giorni", tempoMedio);
        
        printf(CYAN "|" RESET " %-42s " CYAN "|" RESET " %-41d " CYAN "|\n" RESET, "Interventi Valutati nel Calcolo", interventiCompletati);
        printf(CYAN "|" RESET GREEN BOLD " %-42s " CYAN "|" RESET GREEN BOLD " %-41s " CYAN "|\n" RESET, "Tempo Medio Risoluzione Pratiche", bufferTempo);
        printf(CYAN "|____________________________________________|___________________________________________|\n" RESET);
    }
    printf("\n");
}

void generaReportTecnicoPiuAttivo(const ArchivioRichieste* archivio) {
    if (archivio == NULL) {
        printf(RED BOLD "\n [ ERRORE ] Archivio non disponibile.\n" RESET);
        return;
    }
    
    const char** codiTecnici = (const char**)malloc(sizeof(const char*) * getDimensioneArchivio(archivio));
    int numeroTecnici = 0;
    
    NodoLista* nodoCorrente = getTestaArchivio(archivio);
    while (nodoCorrente != NULL) {
        Richiesta* richiesta = getRichiestaDalNodoLista(nodoCorrente);
        if (richiesta != NULL) {
            const char* codiceTecnico = getCodiceTecnicoAssegnatoRichiesta(richiesta);
            if (codiceTecnico != NULL) {
                int trovato = 0;
                for (int i = 0; i < numeroTecnici; i++) {
                    if (strcmp(codiTecnici[i], codiceTecnico) == 0) {
                        trovato = 1;
                        break;
                    }
                }
                
                if (!trovato) {
                    codiTecnici[numeroTecnici] = codiceTecnico;
                    numeroTecnici++;
                }
            }
        }
        nodoCorrente = getNextNodoLista(nodoCorrente);
    }
    
    const char* tecnicoPiuAttivo = NULL;
    int maxInterventi = 0;
    
    for (int i = 0; i < numeroTecnici; i++) {
        int numeroInterventi = contaInterventiPerTecnico(archivio, codiTecnici[i]);
        if (numeroInterventi > maxInterventi) {
            maxInterventi = numeroInterventi;
            tecnicoPiuAttivo = codiTecnici[i];
        }
    }
    
    printf(CYAN BOLD "\n ________________________________________________________________________________________ \n");
    printf("|                                                                                        |\n");
    printf("|                 " MAGENTA BOLD "[ REPORT ] Tecnico Piu' Attivo nel Sistema" CYAN BOLD "                             |\n");
    printf("|________________________________________________________________________________________|\n" RESET);
    
    if (tecnicoPiuAttivo == NULL) {
        printf(CYAN "|" RESET YELLOW " Nessun tecnico ha interventi assegnati nello storico.                                  " CYAN "|\n" RESET);
        printf(CYAN "|________________________________________________________________________________________|\n" RESET);
    } else {
        printf(CYAN "| %-42s | %-41s |\n", "INFORMAZIONE", "DETTAGLIO");
        printf("|--------------------------------------------|-------------------------------------------|\n" RESET);
        printf(CYAN "|" RESET " %-42s " CYAN "|" RESET " %-41s " CYAN "|\n" RESET, "Codice Tecnico Dominante", tecnicoPiuAttivo);
        printf(CYAN "|" RESET GREEN BOLD " %-42s " CYAN "|" RESET GREEN BOLD " %-41d " CYAN "|\n" RESET, "Numero Massimo di Interventi Gestiti", maxInterventi);
        printf(CYAN "|____________________________________________|___________________________________________|\n" RESET);
    }
    
    free(codiTecnici);
    printf("\n");
}

void generaReportAreeProblematiche(const ArchivioRichieste* archivio) {
    if (archivio == NULL) {
        printf(RED BOLD "\n [ ERRORE ] Archivio non disponibile.\n" RESET);
        return;
    }
    
    RiportoArea* aree = NULL;
    int numeroAree = 0;
    
    raccogliAree(archivio, &aree, &numeroAree);
    
    printf(CYAN BOLD "\n ________________________________________________________________________________________ \n");
    printf("|                                                                                        |\n");
    printf("|                 " MAGENTA BOLD "[ REPORT ] Aree e Appartamenti Piu' Problematici" CYAN BOLD "                       |\n");
    printf("|________________________________________________________________________________________|\n" RESET);
    
    if (numeroAree == 0) {
        printf(CYAN "|" RESET YELLOW " Nessun appartamento/area registrato al momento.                                        " CYAN "|\n" RESET);
        printf(CYAN "|________________________________________________________________________________________|\n" RESET);
    } else {
        for (int i = 0; i < numeroAree - 1; i++) {
            for (int j = 0; j < numeroAree - i - 1; j++) {
                if (aree[j].numeroProblemi < aree[j + 1].numeroProblemi) {
                    RiportoArea temp = aree[j];
                    aree[j] = aree[j + 1];
                    aree[j + 1] = temp;
                }
            }
        }
        
        printf(CYAN "| %-42s | %-41s |\n", "APPARTAMENTO / AREA DI INTERVENTO", "NUMERO PROBLEMI SEGNALATI");
        printf("|--------------------------------------------|-------------------------------------------|\n" RESET);
        for (int i = 0; i < numeroAree; i++) {
            printf(CYAN "|" RESET " %-42s " CYAN "|" RESET " %-41d " CYAN "|\n" RESET, 
                   aree[i].appartamento, 
                   aree[i].numeroProblemi);
            free(aree[i].appartamento);
        }
        printf(CYAN "|____________________________________________|___________________________________________|\n" RESET);
    }
    
    free(aree);
    printf("\n");
}

void generaReportCompleto(const ArchivioRichieste* archivio, const AlberoTecnici* albero) {
    if (archivio == NULL) {
        printf(RED BOLD "\n [ ERRORE ] Archivio non disponibile.\n" RESET);
        return;
    }
    
    printf(MAGENTA BOLD "\n ________________________________________________________________________________________ \n");
    printf("|                                                                                        |\n");
    printf("|                     *** INIZIO REPORT COMPLETO DI SISTEMA ***                          |\n");
    printf("|________________________________________________________________________________________|\n" RESET);
    
    generaReportStatoInterventi(archivio);
    generaReportPerTipologia(archivio);
    generaReportTempoMedio(archivio);
    generaReportTecnicoPiuAttivo(archivio);
    generaReportAreeProblematiche(archivio);
    
    printf(MAGENTA BOLD " ________________________________________________________________________________________ \n");
    printf("|                                                                                        |\n");
    printf("|                      *** FINE REPORT COMPLETO DI SISTEMA ***                           |\n");
    printf("|________________________________________________________________________________________|\n\n" RESET);
}