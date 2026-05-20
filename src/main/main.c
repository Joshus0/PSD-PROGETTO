#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Inclusioni per il progetto */
#include "entita/tecnico.h"
#include "entita/richiesta.h"
#include "alberoTecnici.h"
#include "codaPriorita.h"
#include "archivioRichieste.h"
#include "agendaTecnico.h"
#include "main/utilita.h"
#include "report.h"

/* --- DEFINIZIONE COLORI ANSI --- */
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define MAGENTA "\033[35m"

/* --- FUNZIONI AUSILIARIE DI CONTROLLO OCCUPAZIONE --- */

/* Controlla matematicamente se due fasce orarie HH:MM-HH:MM si sovrappongono */
static int controllaIncrocioOrari(const char* ora1, const char* ora2) {
    int h1_start, m1_start, h1_end, m1_end;
    int h2_start, m2_start, h2_end, m2_end;
    if (sscanf(ora1, "%d:%d-%d:%d", &h1_start, &m1_start, &h1_end, &m1_end) != 4) return 0;
    if (sscanf(ora2, "%d:%d-%d:%d", &h2_start, &m2_start, &h2_end, &m2_end) != 4) return 0;
    
    int start1 = h1_start * 60 + m1_start;
    int end1   = h1_end * 60 + m1_end;
    int start2 = h2_start * 60 + m2_start;
    int end2   = h2_end * 60 + m2_end;
    
    if (end1 <= start2 || start1 >= end2) return 0; /* Nessuna sovrapposizione */
    return 1; /* Si sovrappongono! */
}

/* Scorre l'archivio storico alla ricerca di interventi IN_LAVORAZIONE che si sovrappongono */
static int isTecnicoStaLavorando(const ArchivioRichieste* archivio, const char* codiceTecnico, const char* data, const char* fascia) {
    NodoLista* nodo = getTestaArchivio(archivio);
    while (nodo != NULL) {
        Richiesta* r = getRichiestaDalNodoLista(nodo);
        if (r != NULL && getStatoRichiesta(r) == IN_LAVORAZIONE) {
            const char* tec = getCodiceTecnicoAssegnatoRichiesta(r);
            if (tec != NULL && strcmp(tec, codiceTecnico) == 0) {
                /* CORRETTO: Modificato il typo da getDataIninizioLavorazioneRichiesta a getDataInizioLavorazioneRichiesta */
                const char* dRich = getDataInizioLavorazioneRichiesta(r);
                const char* fRich = getFasciaOrariaRichiesta(r);
                if (dRich != NULL && fRich != NULL && strcmp(dRich, data) == 0) {
                    if (controllaIncrocioOrari(fRich, fascia)) {
                        return 1; /* Il tecnico sta effettivamente lavorando in questo esatto momento */
                    }
                }
            }
        }
        nodo = getNextNodoLista(nodo);
    }
    return 0;
}

int main() {
    /* 1. Inizializzazione delle strutture ADT */
    AlberoTecnici* databaseTecnici = creaAlberoTecnici();
    CodaPriorita* codaAttesa = creaCodaPriorita(100); 
    ArchivioRichieste* archivioStorico = creaArchivioRichieste();
    
    int scelta = -1;
    
    /* Buffer riutilizzabili per gli input */
    char bCodice[50], bNome[50], bSpec[50], bApp[100], bTipo[100], bDesc[200], bData[50], bFascia[50];
    int urgenza = -1;

    pulisciSchermo();
    printf(CYAN BOLD "\n ________________________________________________________________________________________ \n");
    printf("|                                                                                        |\n");
    printf("|                 " RESET BOLD "BENVENUTO NEL SISTEMA GESTIONE CONDOMINIO" CYAN BOLD "                              |\n");
    printf("|________________________________________________________________________________________|\n" RESET);
    printf(BOLD YELLOW "\n>> Premi INVIO per entrare nel sistema..." RESET);
    getchar();

    /* 2. Ciclo Principale */
    while (scelta != 0) {
        pulisciSchermo();

        printf(CYAN BOLD "\n ________________________________________________________________________________________ \n");
        printf("|                                                                                        |\n");
        printf("|                 " RESET BOLD "PANNELLO DI CONTROLLO GENERALE" CYAN BOLD "                                         |\n");
        printf("|___________________________________________|____________________________________________|\n");
        printf("|" RESET " [1] Nuovo Tecnico                         " CYAN BOLD "|" RESET " [7] Avvia Intervento (Inizia Lav.)         " CYAN BOLD "|\n");
        printf("|" RESET " [2] Nuova Richiesta                       " CYAN BOLD "|" RESET " [8] Ricerca Avanzata                       " CYAN BOLD "|\n");
        printf("|" RESET " [3] Assegnazione e Pianificazione         " CYAN BOLD "|" RESET " [9] Storico Conclusi                       " CYAN BOLD "|\n");
        printf("|" RESET " [4] Elenco Tecnici                        " CYAN BOLD "|" RESET " [10] Statistiche Sistema                   " CYAN BOLD "|\n");
        printf("|" RESET " [5] Coda Attesa (Heap)                    " CYAN BOLD "|" RESET " [11] Agenda Tecnico                        " CYAN BOLD "|\n");
        printf("|" RESET " [6] Avanzamento Stato / Chiusura          " CYAN BOLD "|" YELLOW BOLD " [0] Esci dal Programma                     " CYAN BOLD "|\n");
        printf("|___________________________________________|____________________________________________|\n" RESET);
        printf(BOLD YELLOW "\n>> Inserisci comando: " RESET);

        if (scanf("%d", &scelta) != 1) {
            printf(RED BOLD "\n [ ERRORE ] Input non valido. Inserisci un numero.\n" RESET);
            pulisciBuffer();
            pausaSchermo();
            continue;
        }
        pulisciBuffer();

        switch (scelta) {
            
            case 1: {
                pulisciSchermo();
                printf(CYAN BOLD "\n ________________________________________________________________________________________ \n");
                printf("|                                                                                        |\n");
                printf("|                 " MAGENTA BOLD "[ MODULO ] Registrazione Nuovo Tecnico" CYAN BOLD "                                 |\n");
                printf("|________________________________________________________________________________________|\n\n" RESET);
                
                do {
                    acquisisciStringa("Codice Identificativo (es. T01): ", bCodice, sizeof(bCodice));
                    if (strlen(bCodice) == 0) printf(RED BOLD "  [ ERRORE ] Il codice non puo' essere vuoto.\n" RESET);
                } while (strlen(bCodice) == 0);

                do {
                    acquisisciStringa("Nome Completo                  : ", bNome, sizeof(bNome));
                    if (strlen(bNome) == 0) printf(RED BOLD "  [ ERRORE ] Il nome non puo' essere vuoto.\n" RESET);
                } while (strlen(bNome) == 0);

                do {
                    acquisisciStringa("Specializzazione               : ", bSpec, sizeof(bSpec));
                    if (strlen(bSpec) == 0) printf(RED BOLD "  [ ERRORE ] La specializzazione non puo' essere vuota.\n" RESET);
                } while (strlen(bSpec) == 0);

                if (cercaTecnicoInAlbero(databaseTecnici, bCodice) != NULL) {
                    printf(RED BOLD "\n [ ERRORE ] Il codice tecnico inserito esiste gia'.\n" RESET);
                } else {
                    Tecnico* nuovoTecnico = creaTecnico(bCodice, bNome, bSpec);
                    if (nuovoTecnico == NULL) {
                        printf(RED BOLD "\n [ ERRORE ] Fallimento allocazione memoria.\n" RESET);
                    } else {
                        inserisciInAlberoTecnici(databaseTecnici, nuovoTecnico);
                        printf(GREEN BOLD "\n [ OK ] Operazione completata. Tecnico salvato nel database.\n" RESET);
                    }
                }
                pausaSchermo();
                break;
            }

            case 2: {
                pulisciSchermo();
                printf(CYAN BOLD "\n ________________________________________________________________________________________ \n");
                printf("|                                                                                        |\n");
                printf("|                 " MAGENTA BOLD "[ MODULO ] Inserimento Nuova Richiesta" CYAN BOLD "                                 |\n");
                printf("|________________________________________________________________________________________|\n\n" RESET);

                do {
                    acquisisciStringa("Codice Richiesta       : ", bCodice, sizeof(bCodice));
                    if (strlen(bCodice) == 0) 
                        printf(RED BOLD "  [ ERRORE ] Il codice richiesta non puo' essere vuoto.\n" RESET);
                } while (strlen(bCodice) == 0);

                if (cercaRichiestaPerCodice(archivioStorico, bCodice) != NULL) {
                    printf(RED BOLD "\n [ ERRORE ] Esiste gia' una richiesta con questo codice!\n" RESET);
                    pausaSchermo();
                    break; 
                }

                do {
                    acquisisciStringa("Appartamento / Area    : ", bApp, sizeof(bApp));
                    if (strlen(bApp) == 0) 
                        printf(RED BOLD "  [ ERRORE ] L'area interessata non puo' essere vuota.\n" RESET);
                } while (strlen(bApp) == 0);

                do {
                    acquisisciStringa("Tipologia Problema     : ", bTipo, sizeof(bTipo));
                    if (strlen(bTipo) == 0) 
                        printf(RED BOLD "  [ ERRORE ] La tipologia non puo' essere vuota.\n" RESET);
                } while (strlen(bTipo) == 0);

                do {
                    acquisisciStringa("Breve Descrizione      : ", bDesc, sizeof(bDesc));
                    if (strlen(bDesc) == 0) 
                        printf(RED BOLD "  [ ERRORE ] La descrizione non puo' essere vuota.\n" RESET);
                } while (strlen(bDesc) == 0);

                do {
                    acquisisciStringa("Data (GG/MM/AAAA)      : ", bData, sizeof(bData));
                    if (validaData(bData) == 0) 
                        printf(RED BOLD "  [ ERRORE ] Formato data errato. Riprova.\n" RESET);
                } while (validaData(bData) == 0);

                do {
                    printf(BOLD "\n>> Urgenza (0=Min, 4=Max) : " RESET);
                    if (scanf("%d", &urgenza) != 1) {
                        pulisciBuffer();
                        printf(RED BOLD "  [ ERRORE ] Inserire un numero da 0 a 4.\n" RESET);
                        urgenza = -1;
                        continue;
                    }
                    pulisciBuffer();
                    if (urgenza < 0 || urgenza > 4)
                        printf(RED BOLD "  [ ERRORE ] Valore fuori range (0-4).\n" RESET);
                } while (urgenza < 0 || urgenza > 4);

                Richiesta* nuovaRichiesta = creaRichiesta(bCodice, bApp, bTipo, bDesc, bData, urgenza);
                if (nuovaRichiesta == NULL) {
                    printf(RED BOLD "\n [ ERRORE ] Impossibile allocare memoria.\n" RESET);
                } else {
                    inserisciInCodaArchivio(archivioStorico, nuovaRichiesta);
                    inserisciInCodaPriorita(codaAttesa, nuovaRichiesta);
                    printf(GREEN BOLD "\n [ OK ] Richiesta registrata in stato APERTA e inserita in coda d'attesa.\n" RESET);
                }
                pausaSchermo();
                break;
            }

            case 3: {
                pulisciSchermo();
                printf(CYAN BOLD "\n ________________________________________________________________________________________ \n");
                printf("|                                                                                        |\n");
                printf("|                 " MAGENTA BOLD "[ MODULO ] Assegnazione e Pianificazione" CYAN BOLD "                               |\n");
                printf("|________________________________________________________________________________________|\n\n" RESET);

                int subScelta = -1;
                printf("[1] Assegnazione Automatica (Estrai Massima Urgenza da Heap)\n");
                printf("[2] Assegnazione Manuale (Seleziona Richiesta Aperta specifica)\n");
                printf(BOLD "\n>> Seleziona modalità: " RESET);
                if (scanf("%d", &subScelta) != 1) { pulisciBuffer(); pausaSchermo(); break; }
                pulisciBuffer();

                Richiesta* rAssegnare = NULL;
                Tecnico* tSelezionato = NULL;

                if (subScelta == 1) {
                    if (isCodaPrioritaVuota(codaAttesa)) {
                        printf(YELLOW " Nessuna richiesta valida in attesa nello Heap.\n" RESET);
                        pausaSchermo();
                        break;
                    }

                    int dimCoda = getDimensioneCodaPriorita(codaAttesa);
                    Richiesta** tempArray = (Richiesta**)malloc(dimCoda * sizeof(Richiesta*));
                    int tempCount = 0;

                    while (isCodaPrioritaVuota(codaAttesa) == 0) {
                        rAssegnare = estraiMaxDaCodaPriorita(codaAttesa);
                        if (rAssegnare == NULL) break;

                        tSelezionato = trovaTecnicoDisponibilePerSpecializzazione(databaseTecnici, getTipologiaProblemaRichiesta(rAssegnare));
                        if (tSelezionato != NULL) {
                            break; 
                        } else {
                            tempArray[tempCount++] = rAssegnare;
                            rAssegnare = NULL; 
                        }
                    }

                    for (int i = 0; i < tempCount; i++) {
                        inserisciInCodaPriorita(codaAttesa, tempArray[i]);
                    }
                    free(tempArray);

                    if (rAssegnare == NULL || tSelezionato == NULL) {
                        printf(YELLOW "\n [ AVVISO ] Nessun tecnico disponibile compatibile con le richieste nello heap.\n" RESET);
                        if (rAssegnare != NULL) inserisciInCodaPriorita(codaAttesa, rAssegnare);
                        pausaSchermo();
                        break;
                    }
                    printf(GREEN BOLD "\n [ MATCH AUTOMATICO ] Richiesta: %s abbinata a Tecnico: %s (%s)\n" RESET, 
                           getCodiceRichiesta(rAssegnare), getNomeTecnico(tSelezionato), getSpecializzazioneTecnico(tSelezionato));

                } else if (subScelta == 2) {
                    acquisisciStringa("Codice Richiesta Aperta: ", bCodice, sizeof(bCodice));
                    rAssegnare = cercaRichiestaPerCodice(archivioStorico, bCodice);

                    if (rAssegnare == NULL) {
                        printf(RED BOLD "\n [ ERRORE ] Richiesta inesistente.\n" RESET);
                        pausaSchermo(); break;
                    }
                    if (getStatoRichiesta(rAssegnare) != APERTA) {
                        printf(RED BOLD "\n [ ERRORE ] La richiesta selezionata non è in stato APERTA.\n" RESET);
                        pausaSchermo(); break;
                    }

                    printf(" Tipologia richiesta: " CYAN "%s\n" RESET, getTipologiaProblemaRichiesta(rAssegnare));
                    acquisisciStringa("Codice Tecnico da assegnare: ", bCodice, sizeof(bCodice));
                    tSelezionato = cercaTecnicoInAlbero(databaseTecnici, bCodice);

                    if (tSelezionato == NULL) {
                        printf(RED BOLD "\n [ ERRORE ] Tecnico non trovato.\n" RESET);
                        pausaSchermo(); break;
                    }
                    if (strcmp(getSpecializzazioneTecnico(tSelezionato), getTipologiaProblemaRichiesta(rAssegnare)) != 0) {
                        printf(RED BOLD "\n [ ERRORE ] Specializzazione del tecnico non compatibile con il problema.\n" RESET);
                        pausaSchermo(); break;
                    }
                } else {
                    printf(RED BOLD "\n [ ERRORE ] Opzione errata.\n" RESET);
                    pausaSchermo(); break;
                }

                /* Fase Comune: Pianificazione Orario e Controllo Occupazione */
                do { 
                    acquisisciStringa("Data intervento (GG/MM/AAAA)  : ", bData, sizeof(bData)); 
                } while (validaData(bData) == 0);

                do {
                    acquisisciStringa("Fascia oraria (es. 09:00-11:00) : ", bFascia, sizeof(bFascia));
                    if (validaFasciaOraria(bFascia) == 0) printf(RED BOLD "  [ ERRORE ] Formato errato.\n" RESET);
                } while (validaFasciaOraria(bFascia) == 0);

                /* REGOLA CRITICA: Verifica se il tecnico sta LAVORANDO sul campo in quel momento */
                if (isTecnicoStaLavorando(archivioStorico, getCodiceTecnico(tSelezionato), bData, bFascia)) {
                    printf(RED BOLD "\n [ ERRORE DI PIANIFICAZIONE ] Il tecnico %s e' occupato perche' sta gia' lavorando sul posto ad un altro intervento in questa fascia oraria!\n" RESET, getNomeTecnico(tSelezionato));
                    if (subScelta == 1) {
                        printf(YELLOW " La richiesta viene re-inserita nella coda delle emergenze.\n" RESET);
                        inserisciInCodaPriorita(codaAttesa, rAssegnare);
                    }
                } else {
                    if (pianificaIntervento(rAssegnare, tSelezionato, bData, bFascia)) {
                        setValidaInHeapRichiesta(rAssegnare, 0); /* Rimozione logica dallo Heap */
                        printf(GREEN BOLD "\n [ OK ] Intervento pianificato con successo! Stato impostato su PIANIFICATA.\n" RESET);
                    } else {
                        printf(RED BOLD "\n [ ERRORE ] Conflitto orario nell'agenda strutturata del tecnico.\n" RESET);
                        if (subScelta == 1) inserisciInCodaPriorita(codaAttesa, rAssegnare);
                    }
                }
                pausaSchermo();
                break;
            }

            case 4: {
                pulisciSchermo();
                printf(CYAN BOLD "\n ________________________________________________________________________________________ \n");
                printf("|                                                                                        |\n");
                printf("|                 " MAGENTA BOLD "[ VISUALIZZAZIONE ] Monitoraggio Carico Tecnici" CYAN BOLD "                        |\n");
                printf("|________________________________________________________________________________________|\n");
                printf("| %-10s | %-22s | %-18s | %-27s |\n", "CODICE", "NOME", "SPECIALIZZ.", "STATO CARICO");
                printf("|------------|------------------------|--------------------|-----------------------------|\n" RESET);
                
                if (getRadiceAlberoTecnici(databaseTecnici) == NULL) {
                    printf(CYAN "|" RESET YELLOW " Nessun tecnico presente nel sistema.                                                   " CYAN "|\n" RESET);
                } else {
                    visitaAlberoTecnici(databaseTecnici, stampaCaricoLavoroTecnico);
                }
                printf(CYAN "|________________________________________________________________________________________|\n\n" RESET);
                pausaSchermo();
                break;
            }

            case 5: {
                pulisciSchermo();
                printf(CYAN BOLD "\n ________________________________________________________________________________________ \n");
                printf("|                                                                                        |\n");
                printf("|                 " MAGENTA BOLD "[ VISUALIZZAZIONE ] Coda di Attesa (Max-Heap)" CYAN BOLD "                          |\n");
                printf("|________________________________________________________________________________________|\n\n" RESET);
                stampaCodaPriorita(codaAttesa);
                pausaSchermo();
                break;
            }

            case 6: {
                pulisciSchermo();
                printf(CYAN BOLD "\n ________________________________________________________________________________________ \n");
                printf("|                                                                                        |\n");
                printf("|                 " MAGENTA BOLD "[ MODULO ] Avanzamento Stato / Chiusura Intervento" CYAN BOLD "                     |\n");
                printf("|________________________________________________________________________________________|\n\n" RESET);

                acquisisciStringa("Codice Richiesta: ", bCodice, sizeof(bCodice));
                Richiesta* rTrovata = cercaRichiestaPerCodice(archivioStorico, bCodice);

                if (rTrovata == NULL) {
                    printf(RED BOLD "\n [ ERRORE ] Richiesta inesistente.\n" RESET);
                } else {
                    StatoRichiesta sAttuale = getStatoRichiesta(rTrovata);
                    
                    printf(CYAN " ________________________________________________________________________________________ \n");
                    printf("| %-8s | %-14s | %-15s | %-7s | %-14s | %-13s |\n", "CODICE", "APPARTAMENTO", "TIPOLOGIA", "URGENZA", "STATO", "TECNICO");
                    printf("|----------|----------------|-----------------|---------|----------------|---------------|\n" RESET);
                    stampaRichiesta(rTrovata);
                    printf(CYAN "|________________________________________________________________________________________|\n" RESET);

                    if (sAttuale == CONCLUSA || sAttuale == ANNULLATA) {
                        printf(YELLOW BOLD "\n [ AVVISO ] Pratica gia' chiusa o annullata definitivamente.\n" RESET);
                    } else if (sAttuale == APERTA) {
                        printf(YELLOW BOLD "\n [ INFO ] La richiesta e' ancora APERTA. Usa l'opzione 3 per pianificarla, oppure premi 4 per ANNULLARLA.\n" RESET);
                        printf(">> Inserisci 4 per ANNULLARE o 0 per annullare operazione: ");
                        int op = 0;
                        if (scanf("%d", &op) == 1 && op == 4) {
                            setStatoRichiesta(rTrovata, ANNULLATA);
                            setValidaInHeapRichiesta(rTrovata, 0);
                            printf(GREEN BOLD "\n [ OK ] Richiesta Annullata con successo.\n" RESET);
                        }
                        pulisciBuffer();
                    } else if (sAttuale == PIANIFICATA) {
                        printf(YELLOW BOLD "\n [ INFO ] L'intervento è PIANIFICATO. Per avviarlo sul posto usa l'Opzione 7.\n" RESET);
                        printf(">> Inserisci 4 per ANNULLARE l'appuntamento in agenda o 0 per uscire: ");
                        int op = 0;
                        if (scanf("%d", &op) == 1 && op == 4) {
                            setStatoRichiesta(rTrovata, ANNULLATA);
                            Tecnico* tAss = cercaTecnicoInAlbero(databaseTecnici, getCodiceTecnicoAssegnatoRichiesta(rTrovata));
                            if (tAss != NULL) {
                                rimuoviInterventoDaAgenda(getAgendaTecnico(tAss), getDataInizioLavorazioneRichiesta(rTrovata), getFasciaOrariaRichiesta(rTrovata));
                            }
                            printf(GREEN BOLD "\n [ OK ] Intervento annullato e slot liberato dall'agenda.\n" RESET);
                        }
                        pulisciBuffer();
                    } else if (sAttuale == IN_LAVORAZIONE) {
                        printf("\n" CYAN BOLD "[ TRANSIZIONE STATO ] Scegli l'operazione di chiusura da effettuare:" RESET "\n");
                        printf(" [3] IMPOSTA COME CONCLUSA (Lavoro Terminato)\n");
                        printf(" [4] IMPOSTA COME ANNULLATA (Impossibile Completare)\n");
                        printf(" [0] Annulla Operazione\n");
                        printf(BOLD "\n>> Comando: " RESET);
                        
                        int op = -1;
                        if (scanf("%d", &op) != 1) { pulisciBuffer(); pausaSchermo(); break; }
                        pulisciBuffer();

                        if (op == 3) {
                            int dataValida = 0;
                            do {
                                acquisisciStringa("Data chiusura effettiva (GG/MM/AAAA): ", bData, sizeof(bData));
                                dataValida = validaDataChiusuraRichiesta(bData, rTrovata);
                            } while (dataValida == 0);

                            setDataChiusuraRichiesta(rTrovata, bData);
                            setStatoRichiesta(rTrovata, CONCLUSA);

                            /* Libera l'agenda attiva del tecnico poiche' l'intervento e' concluso */
                            const char* codiceTecnico = getCodiceTecnicoAssegnatoRichiesta(rTrovata);
                            if (codiceTecnico != NULL) {
                                Tecnico* tConcluso = cercaTecnicoInAlbero(databaseTecnici, codiceTecnico);
                                if (tConcluso != NULL) {
                                    rimuoviInterventoDaAgenda(
                                        getAgendaTecnico(tConcluso),
                                        getDataInizioLavorazioneRichiesta(rTrovata),
                                        getFasciaOrariaRichiesta(rTrovata)
                                    );
                                }
                            }
                            printf(GREEN BOLD "\n [ OK ] Intervento completato con successo. Slot rimosso dall'agenda del tecnico.\n" RESET);
                        } else if (op == 4) {
                            setStatoRichiesta(rTrovata, ANNULLATA);
                            const char* codiceTecnico = getCodiceTecnicoAssegnatoRichiesta(rTrovata);
                            if (codiceTecnico != NULL) {
                                Tecnico* tConcluso = cercaTecnicoInAlbero(databaseTecnici, codiceTecnico);
                                if (tConcluso != NULL) {
                                    rimuoviInterventoDaAgenda(getAgendaTecnico(tConcluso), getDataInizioLavorazioneRichiesta(rTrovata), getFasciaOrariaRichiesta(rTrovata));
                                }
                            }
                            printf(GREEN BOLD "\n [ OK ] Pratica contrassegnata come ANNULLATA.\n" RESET);
                        }
                    }
                }
                pausaSchermo();
                break;
            }

            case 7: {
                pulisciSchermo();
                printf(CYAN BOLD "\n ________________________________________________________________________________________ \n");
                printf("|                                                                                        |\n");
                printf("|                 " MAGENTA BOLD "[ MODULO ] Avvia Intervento (Inizia Lavorazione)" CYAN BOLD "                       |\n");
                printf("|________________________________________________________________________________________|\n\n" RESET);

                acquisisciStringa("Codice Richiesta Pianificata da avviare: ", bCodice, sizeof(bCodice));
                Richiesta* rPianif = cercaRichiestaPerCodice(archivioStorico, bCodice);

                if (rPianif == NULL) {
                    printf(RED BOLD "\n [ ERRORE ] Richiesta non trovata.\n" RESET);
                } else {
                    StatoRichiesta st = getStatoRichiesta(rPianif);
                    if (st != PIANIFICATA) {
                        printf(RED BOLD "\n [ ERRORE ] Non e' possibile avviare l'intervento. Stato attuale: %s.\n" RESET, statoRichiestaToString(st));
                        printf(YELLOW " Nota: La richiesta deve essere prima PIANIFICATA tramite il Modulo 3.\n" RESET);
                    } else {
                        /* Avanzamento di stato guidato coerente */
                        setStatoRichiesta(rPianif, IN_LAVORAZIONE);
                        printf(GREEN BOLD "\n [ OK ] Sincronizzazione Moduli Avvenuta con Successo!\n" RESET);
                        printf(" La richiesta %s e' ora in stato " MAGENTA "IN LAVORAZIONE" RESET ".\n", getCodiceRichiesta(rPianif));
                        printf(" Il tecnico assegnato e' ufficialmente " RED "OCCUPATO" RESET " sul posto nell'esecuzione del lavoro.\n");
                    }
                }
                pausaSchermo();
                break;
            }

            case 8: {
                int sRicerca = -1;
                do {
                    pulisciSchermo();
                    printf(CYAN BOLD "\n ________________________________________________________________________________________ \n");
                    printf("|                                                                                        |\n");
                    printf("|                 " MAGENTA BOLD "[ MODULO ] Ricerca Avanzata" CYAN BOLD "                                            |\n");
                    printf("|___________________________________________|____________________________________________|\n");
                    printf("|" RESET " [1] Filtra per Stato                      " CYAN BOLD "|" RESET " [4] Cerca per App./Area                    " CYAN BOLD "|\n");
                    printf("|" RESET " [2] Filtra per Urgenza                    " CYAN BOLD "|" RESET " [5] Cerca per Tecnico                      " CYAN BOLD "|\n");
                    printf("|" RESET " [3] Cerca per Tipologia                   " CYAN BOLD "|" RESET " [6] Cerca per Codice                       " CYAN BOLD "|\n");
                    printf("|___________________________________________|____________________________________________|\n");
                    printf("|" YELLOW BOLD "                                      [0] Indietro                                      " CYAN BOLD "|\n");
                    printf("|________________________________________________________________________________________|\n" RESET);
                    printf(BOLD YELLOW "\n>> Seleziona filtro di ricerca: " RESET);

                    if (scanf("%d", &sRicerca) != 1) { pulisciBuffer(); sRicerca = -1; continue; }
                    pulisciBuffer(); 

                    switch (sRicerca) {
                        case 1:
                            printf(BOLD "\n>> Stato (0=Aperta, 1=Pianificata, 2=In Lav., 3=Conclusa, 4=Annullata): " RESET);
                            if (scanf("%d", &urgenza) == 1) { pulisciBuffer(); printf("\n"); stampaRichiesteArchivioPerStato(archivioStorico, (StatoRichiesta)urgenza); }
                            pausaSchermo(); break;

                        case 2:
                            printf(BOLD "\n>> Urgenza (0-4): " RESET);
                            if (scanf("%d", &urgenza) == 1) { pulisciBuffer(); printf("\n"); stampaRichiesteArchivioPerUrgenza(archivioStorico, urgenza); }
                            pausaSchermo(); break;

                        case 3:
                            acquisisciStringa("Tipologia: ", bTipo, sizeof(bTipo));
                            printf("\n"); stampaRichiesteArchivioPerTipologia(archivioStorico, bTipo);
                            pausaSchermo(); break;
                        
                        case 4:
                            acquisisciStringa("Appartamento: ", bApp, sizeof(bApp));
                            printf("\n"); stampaRichiesteArchivioPerAppartamento(archivioStorico, bApp);
                            pausaSchermo(); break;

                        case 5:
                            acquisisciStringa("Codice Tecnico: ", bCodice, sizeof(bCodice));
                            printf("\n"); stampaRichiesteArchivioPerTecnico(archivioStorico, bCodice);
                            pausaSchermo(); break;

                        case 6: {
                            acquisisciStringa("Codice Richiesta: ", bCodice, sizeof(bCodice));
                            Richiesta* rTrovata = cercaRichiestaPerCodice(archivioStorico, bCodice);
                            if (rTrovata != NULL) {
                                printf(GREEN BOLD "\n [ OK ] Ricerca completata.\n\n" RESET);
                                printf(CYAN " ________________________________________________________________________________________ \n");
                                printf("| %-8s | %-14s | %-15s | %-7s | %-14s | %-13s |\n", "CODICE", "APPARTAMENTO", "TIPOLOGIA", "URGENZA", "STATO", "TECNICO");
                                printf("|----------|----------------|-----------------|---------|----------------|---------------|\n" RESET);
                                stampaRichiesta(rTrovata);
                                printf(CYAN "|________________________________________________________________________________________|\n" RESET);
                            } else {
                                printf(RED BOLD "\n [ ERRORE ] Nessuna richiesta trouvata con codice '%s'.\n" RESET, bCodice);
                            }
                            pausaSchermo(); break;
                        }
                        case 0: break;
                        default: printf(RED BOLD "\n [ ERRORE ] Opzione non valida.\n" RESET); pausaSchermo(); break;
                    }
                } while (sRicerca != 0);
                break;
            }

            case 9: {
                pulisciSchermo();
                printf(CYAN BOLD "\n ________________________________________________________________________________________ \n");
                printf("|                                                                                        |\n");
                printf("|                 " MAGENTA BOLD "[ VISUALIZZAZIONE ] Archivio Storico Interventi" CYAN BOLD "                        |\n");
                printf("|________________________________________________________________________________________|\n\n" RESET);
                stampaStoricoInterventi(archivioStorico);
                pausaSchermo();
                break;
            }

            case 10: {
                int sceltaReport = -1;
                while (sceltaReport != 0) {
                    pulisciSchermo();
                    printf(CYAN BOLD "\n ________________________________________________________________________________________ \n");
                    printf("|                                                                                        |\n");
                    printf("|                 " MAGENTA BOLD "[ REPORT ] Statistiche Sistema" CYAN BOLD "                                         |\n");
                    printf("|___________________________________________|____________________________________________|\n");
                    printf("|" RESET " [1] Report Completo                       " CYAN BOLD "|" RESET " [4] Tempo Medio Completamento             " CYAN BOLD " |\n");
                    printf("|" RESET " [2] Stato Interventi                      " CYAN BOLD "|" RESET " [5] Tecnico Piu' Attivo                   " CYAN BOLD " |\n");
                    printf("|" RESET " [3] Tipologia Interventi                  " CYAN BOLD "|" RESET " [6] Aree Piu' Problematiche               " CYAN BOLD " |\n");
                    printf("|___________________________________________|____________________________________________|\n");
                    printf("|" YELLOW BOLD "                                      [0] Indietro                                      " CYAN BOLD "|\n");
                    printf("|________________________________________________________________________________________|\n" RESET);
                    printf(BOLD YELLOW "\n>> Seleziona tipologia report: " RESET);
                    
                    if (scanf("%d", &sceltaReport) != 1) { printf(RED BOLD "\n [ ERRORE ] Input non valido.\n" RESET); pulisciBuffer(); continue; }
                    pulisciBuffer();
                    pulisciSchermo();
                    
                    switch (sceltaReport) {
                        case 1: generaReportCompleto(archivioStorico, databaseTecnici); break;
                        case 2: generaReportStatoInterventi(archivioStorico); break;
                        case 3: generaReportPerTipologia(archivioStorico); break;
                        case 4: generaReportTempoMedio(archivioStorico); break;
                        case 5: generaReportTecnicoPiuAttivo(archivioStorico); break;
                        case 6: generaReportAreeProblematiche(archivioStorico); break;
                        case 0: break;
                        default: printf(RED BOLD "\n [ ERRORE ] Opzione non valida.\n" RESET);
                    }
                    if (sceltaReport != 0 && sceltaReport >= 1 && sceltaReport <= 6) pausaSchermo();
                }
                break;
            }

            case 11: {
                pulisciSchermo();
                printf(CYAN BOLD "\n ________________________________________________________________________________________ \n");
                printf("|                                                                                        |\n");
                printf("|                 " MAGENTA BOLD "[ VISUALIZZAZIONE ] Agenda Lavori Tecnico" CYAN BOLD "                              |\n");
                printf("|________________________________________________________________________________________|\n\n" RESET);

                acquisisciStringa("Codice Tecnico: ", bCodice, sizeof(bCodice));
                Tecnico* tAgenda = cercaTecnicoInAlbero(databaseTecnici, bCodice);
                
                if (tAgenda == NULL) {
                    printf(RED BOLD "\n [ ERRORE ] Tecnico inesistente.\n" RESET);
                } else {
                    printf("\n");
                    stampaAgendaTecnico(tAgenda);
                }
                pausaSchermo();
                break;
            }

            case 0: {
                pulisciSchermo();
                printf(CYAN BOLD "\n ________________________________________________________________________________________ \n");
                printf("|                                                                                        |\n");
                printf("|                 " YELLOW BOLD "Arresto dei moduli e pulizia memoria in corso..." CYAN BOLD "                       |\n");
                printf("|________________________________________________________________________________________|\n" RESET);
                printf(GREEN BOLD "\n [ OK ] Sistema terminato con successo. Arrivederci!\n\n" RESET);
                break;
            }

            default: {
                printf(RED BOLD "\n [ ERRORE ] Comando non riconosciuto.\n" RESET);
                pausaSchermo();
                break;
            }
        }
    }

    /* 4. Deallocazione della memoria heap */
    distruggiAlberoTecnici(databaseTecnici);
    distruggiCodaPriorita(codaAttesa);
    distruggiArchivioRichieste(archivioStorico);

    return 0;
}