/*
 * Implementazione del modulo Report.
 *
 * Fornisce funzionalita' per l'aggregazione dei dati e la generazione di 
 * report statistici sullo stato degli interventi di manutenzione. Include 
 * metriche sulle tipologie di problemi, lo stato delle richieste, i tempi 
 * medi di risoluzione, i tecnici piu' attivi e le aree maggiormente 
 * problematiche. Sfrutta sequenze di escape ANSI per un output a terminale 
 * chiaro e leggibile.
 * 
 * 5 maggio
 * Joshua Sarnelli
 */
#include "report.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Sequenze di escape ANSI per la formattazione grafica dell'output a terminale */
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define MAGENTA "\033[35m"
#define BLUE    "\033[34m"

/*
 * dataAGiorni - Converte una data stringa in numero di giorni assoluti.
 *
 * Funzione helper statica. Utilizza come epoca di riferimento il 1 Gennaio 1900.
 * La funzione calcola i giorni totali trascorsi tenendo conto degli anni 
 * bisestili e della differente durata di ogni mese.
 *
 * Parametri:
 *   data - Stringa contenente la data nel formato "GG/MM/AAAA" (puo' essere NULL)
 *
 * Ritorna:
 *   Il numero di giorni trascorsi dall'epoca di riferimento, oppure
 *   0 se il puntatore alla data e' NULL.
 */
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

/*
 * differenzaGiorni - Calcola i giorni di distanza temporale tra due date.
 *
 * Sfrutta la funzione dataAGiorni per ottenere le rappresentazioni numeriche 
 * assolute delle date e ne calcola la differenza aritmetica.
 *
 * Parametri:
 *   dataInizio - Data di partenza nel formato "GG/MM/AAAA"
 *   dataFine   - Data finale nel formato "GG/MM/AAAA"
 *
 * Ritorna:
 *   Il numero intero di giorni di differenza, oppure 0 se uno dei parametri e' NULL.
 */
static int differenzaGiorni(const char* dataInizio, const char* dataFine) {
    if (dataInizio == NULL || dataFine == NULL) return 0;
    return dataAGiorni(dataFine) - dataAGiorni(dataInizio);
}

/*
 * contaInterventiPerTipologia - Calcola il totale di richieste per una data tipologia.
 *
 * Scorre l'archivio linearmente incrementando un contatore per ogni
 * richiesta che fa match con la stringa della tipologia passata in input.
 *
 * Parametri:
 *   archivio  - Puntatore all'archivio delle richieste
 *   tipologia - Stringa rappresentante la tipologia da contare
 *
 * Ritorna:
 *   Il numero di occorrenze trovate nell'archivio.
 */
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

/*
 * contaInterventiPerTecnico - Calcola il totale di richieste assegnate a un tecnico.
 *
 * Parametri:
 *   archivio      - Puntatore all'archivio delle richieste
 *   codiceTecnico - Codice identificativo del tecnico da cercare
 *
 * Ritorna:
 *   Il numero di richieste assegnate al tecnico specificato.
 */
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

/*
 * contaInterventiPerAppartamento - Calcola le richieste associate a un appartamento.
 *
 * Parametri:
 *   archivio     - Puntatore all'archivio delle richieste
 *   appartamento - Identificativo dell'appartamento da cercare
 *
 * Ritorna:
 *   Il numero di richieste provenienti dall'appartamento specificato.
 */
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

/*
 * contaInterventiPerStato - Calcola il totale di richieste in un dato stato.
 *
 * Parametri:
 *   archivio - Puntatore all'archivio delle richieste
 *   stato    - Lo stato dell'enumerazione (APERTA, PIANIFICATA, ecc.) da contare
 *
 * Ritorna:
 *   Il numero di richieste che si trovano nello stato indicato.
 */
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

/*
 * raccogliTipologie - Costruisce un array dinamico di aggregazione per tipologia.
 *
 * Scorre l'intero archivio estraendo le tipologie uniche. Per ogni tipologia
 * individuata, calcola il totale degli interventi associati. L'array risultante
 * viene restituito tramite passaggio per riferimento.
 *
 * Parametri:
 *   archivio        - Puntatore all'archivio da analizzare
 *   tipologie       - Puntatore al puntatore dell'array strutturato (output)
 *   numeroTipologie - Puntatore in cui verra' salvato il numero di elementi (output)
 */
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

/*
 * raccogliAree - Costruisce un array dinamico di aggregazione per appartamento.
 *
 * Analogo a raccogliTipologie, ma raggruppa le statistiche isolando gli
 * appartamenti (o aree) univoci presenti nello storico delle richieste.
 *
 * Parametri:
 *   archivio   - Puntatore all'archivio da analizzare
 *   aree       - Puntatore al puntatore dell'array strutturato (output)
 *   numeroAree - Puntatore in cui verra' salvato il numero di elementi (output)
 */
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

/*
 * generaReportPerTipologia - Mostra le statistiche degli interventi per tipo.
 *
 * Raccoglie i dati e stampa una tabella formattata indicante quante richieste
 * sono state registrate per ogni tipologia di problema nota al sistema.
 *
 * Parametri:
 *   archivio - Puntatore all'archivio centrale delle richieste
 */
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

/*
 * generaReportStatoInterventi - Mostra le percentuali e i tassi di completamento.
 *
 * Analizza e raggruppa le richieste secondo la classificazione del loro ciclo di 
 * vita (Aperte, In Lavorazione, Chiuse), fornendo un colpo d'occhio
 * rapido sulle performance complessive di smaltimento dei ticket.
 *
 * Parametri:
 *   archivio - Puntatore all'archivio centrale delle richieste
 */
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

/*
 * generaReportTempoMedio - Calcola e stampa la media dei giorni di risoluzione.
 *
 * Filtra solo le richieste nello stato CONCLUSA, valutando la differenza 
 * temporale tra la data di creazione e quella di chiusura, e ne fa una media.
 *
 * Parametri:
 *   archivio - Puntatore all'archivio centrale delle richieste
 */
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

/*
 * generaReportTecnicoPiuAttivo - Evidenzia il tecnico con il carico maggiore.
 *
 * Scorre l'archivio per identificare il tecnico che e' stato storicamente 
 * assegnato al maggior numero di richieste (indipendentemente dal loro stato).
 *
 * Parametri:
 *   archivio - Puntatore all'archivio centrale delle richieste
 */
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

/*
 * generaReportAreeProblematiche - Classifica gli appartamenti per volume di richieste.
 *
 * Utilizza la logica di raggruppamento per appartamento, ordinando (Bubble Sort) 
 * i risultati in ordine decrescente, cosi' da mostrare per primi gli 
 * appartamenti/aree che hanno generato piu' chiamate.
 *
 * Parametri:
 *   archivio - Puntatore all'archivio centrale delle richieste
 */
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

/*
 * generaReportCompleto - Genera ed emette l'intera suite di diagnostica.
 *
 * Funzione wrapper di alto livello che richiama in sequenza tutte le funzioni
 * specifiche di generazione dei report, fornendo un output completo del sistema.
 *
 * Parametri:
 *   archivio - Puntatore all'archivio centrale delle richieste
 *   albero   - Puntatore all'albero dei tecnici (riservato per espansioni future)
 */
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