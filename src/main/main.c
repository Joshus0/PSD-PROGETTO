#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Inclusioni corrette per il nuovo progetto */
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

int main() {
    /* 1. Inizializzazione delle strutture ADT */
    AlberoTecnici* databaseTecnici = creaAlberoTecnici();
    CodaPriorita* codaAttesa = creaCodaPriorita(100); 
    ArchivioRichieste* archivioStorico = creaArchivioRichieste();
    
    int scelta = -1;
    
    /* Buffer riutilizzabili per gli input  */
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

        /* --- NUOVA INTERFACCIA A PANNELLO (DASHBOARD) --- */
        printf(CYAN BOLD "\n ________________________________________________________________________________________ \n");
        printf("|                                                                                        |\n");
        printf("|                 " RESET BOLD "PANNELLO DI CONTROLLO GENERALE" CYAN BOLD "                                         |\n");
        printf("|___________________________________________|____________________________________________|\n");
        printf("|" RESET " [1] Nuovo Tecnico                         " CYAN BOLD "|" RESET " [7] Pianifica Intervento                   " CYAN BOLD "|\n");
        printf("|" RESET " [2] Nuova Richiesta                       " CYAN BOLD "|" RESET " [8] Ricerca Avanzata                       " CYAN BOLD "|\n");
        printf("|" RESET " [3] Assegnazione Auto                     " CYAN BOLD "|" RESET " [9] Storico Conclusi                       " CYAN BOLD "|\n");
        printf("|" RESET " [4] Elenco Tecnici                        " CYAN BOLD "|" RESET " [10] Statistiche Sistema                   " CYAN BOLD "|\n");
        printf("|" RESET " [5] Coda Attesa (Heap)                    " CYAN BOLD "|" RESET " [11] Agenda Tecnico                        " CYAN BOLD "|\n");
        printf("|" RESET " [6] Aggiorna Stato                        " CYAN BOLD "|" YELLOW BOLD " [0] Esci dal Programma                     " CYAN BOLD "|\n");
        printf("|___________________________________________|____________________________________________|\n" RESET);
        printf(BOLD YELLOW "\n>> Inserisci comando: " RESET);

        if (scanf("%d", &scelta) != 1) {
            printf(RED BOLD "\n [ ERRORE ] Input non valido. Inserisci un numero.\n" RESET);
            pulisciBuffer();
            pausaSchermo();
            continue;
        }
        pulisciBuffer();

        /* 3. Gestione Scelte (Tutto nel Main) */
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
                    printf(GREEN BOLD "\n [ OK ] Operazione completata. Richiesta registrata e accodata con successo.\n" RESET);
                }
                pausaSchermo();
                break;
            }

            case 3: {
                pulisciSchermo();
                printf(CYAN BOLD "\n ________________________________________________________________________________________ \n");
                printf("|                                                                                        |\n");
                printf("|                 " MAGENTA BOLD "[ MODULO ] Assegnazione Automatica" CYAN BOLD "                                     |\n");
                printf("|________________________________________________________________________________________|\n\n" RESET);

                if (isCodaPrioritaVuota(codaAttesa)) {
                    printf(YELLOW " Nessuna richiesta valida in attesa nella Coda.\n" RESET);
                    pausaSchermo();
                    break;
                }

                Richiesta* rAssegnare = NULL;
                Tecnico* tSelezionato = NULL;
                
                int dimCoda = getDimensioneCodaPriorita(codaAttesa);
                Richiesta** tempArray = (Richiesta**)malloc(dimCoda * sizeof(Richiesta*));
                int tempCount = 0;

                while (isCodaPrioritaVuota(codaAttesa) == 0) {
                    rAssegnare = estraiMaxDaCodaPriorita(codaAttesa);
                    
                    if (rAssegnare == NULL || isValidaInHeapRichiesta(rAssegnare) == 0) continue;

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

                if (rAssegnare != NULL && tSelezionato != NULL) {
                    setCodiceTecnicoAssegnatoRichiesta(rAssegnare, getCodiceTecnico(tSelezionato));
                    setStatoRichiesta(rAssegnare, PIANIFICATA);
                    setDisponibilitaTecnico(tSelezionato, 0);
                    setValidaInHeapRichiesta(rAssegnare, 0);

                    printf(GREEN BOLD "\n [ OK ] Match Trovato!\n" RESET);
                    printf(" Tecnico assegnato  : " CYAN "%s\n\n" RESET, getNomeTecnico(tSelezionato));
                    
                    printf(CYAN " ________________________________________________________________________________________ \n");
                    printf("| %-8s | %-14s | %-15s | %-7s | %-14s | %-13s |\n", "CODICE", "APPARTAMENTO", "TIPOLOGIA", "URGENZA", "STATO", "TECNICO");
                    printf("|----------|----------------|-----------------|---------|----------------|---------------|\n" RESET);
                    stampaRichiesta(rAssegnare);
                    printf(CYAN "|________________________________________________________________________________________|\n" RESET);
                } else {
                    printf(YELLOW "\n [ AVVISO ] Nessuna richiesta in attesa puo' essere soddisfatta dai tecnici disponibili.\n" RESET);
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
                printf("|                 " MAGENTA BOLD "[ MODULO ] Aggiornamento Stato Intervento" CYAN BOLD "                              |\n");
                printf("|________________________________________________________________________________________|\n\n" RESET);

                acquisisciStringa("Codice Richiesta: ", bCodice, sizeof(bCodice));
                Richiesta* rTrovata = cercaRichiestaPerCodice(archivioStorico, bCodice);

                if (rTrovata == NULL) {
                    printf(RED BOLD "\n [ ERRORE ] Richiesta inesistente.\n" RESET);
                } else {
                    StatoRichiesta sAttuale = getStatoRichiesta(rTrovata);
                    if (sAttuale == CONCLUSA || sAttuale == ANNULLATA) {
                        printf(YELLOW BOLD "\n [ AVVISO ] Pratica gia' archiviata in modo definitivo.\n" RESET);
                    } else {
                        printf("\n");
                        printf(CYAN " ________________________________________________________________________________________ \n");
                        printf("| %-8s | %-14s | %-15s | %-7s | %-14s | %-13s |\n", "CODICE", "APPARTAMENTO", "TIPOLOGIA", "URGENZA", "STATO", "TECNICO");
                        printf("|----------|----------------|-----------------|---------|----------------|---------------|\n" RESET);
                        stampaRichiesta(rTrovata);
                        printf(CYAN "|________________________________________________________________________________________|\n" RESET);

                        printf("\n" CYAN BOLD "Stati disponibili: " RESET "[0] APERTA, [1] PIANIFICATA, [2] IN LAVORAZIONE, [3] CONCLUSA, [4] ANNULLATA\n");
                        
                        int nuovoS = -1;
                        while (nuovoS < 0 || nuovoS > 4) {
                            printf(BOLD "\n>> Inserisci Nuovo Stato: " RESET);
                            if (scanf("%d", &nuovoS) != 1) { pulisciBuffer(); nuovoS = -1; continue; }
                            pulisciBuffer();
                        }

                        StatoRichiesta nStato = (StatoRichiesta)nuovoS;
                        setStatoRichiesta(rTrovata, nStato);

                        if (nStato == ANNULLATA || nStato == CONCLUSA || nStato == IN_LAVORAZIONE) {
                            setValidaInHeapRichiesta(rTrovata, 0);
                        }

                        if (nStato == CONCLUSA) {
                            if (sAttuale != IN_LAVORAZIONE) {
                                printf(YELLOW BOLD "\n [ AVVISO ] La richiesta non e' in stato IN_LAVORAZIONE.\n" RESET);
                            }
                            
                            int dataValida = 0;
                            do {
                                acquisisciStringa("Data chiusura effettiva (GG/MM/AAAA): ", bData, sizeof(bData));
                                dataValida = validaDataChiusuraRichiesta(bData, rTrovata);
                            } while (dataValida == 0);
                            setDataChiusuraRichiesta(rTrovata, bData);

                            const char* codiceTecnico = getCodiceTecnicoAssegnatoRichiesta(rTrovata);
                            if (codiceTecnico != NULL) {
                                Tecnico* tConcluso = cercaTecnicoInAlbero(databaseTecnici, codiceTecnico);
                                if (tConcluso != NULL) {
                                    rimuoviInterventoDaAgenda(
                                        getAgendaTecnico(tConcluso),
                                        getDataInizioLavorazioneRichiesta(rTrovata),
                                        getFasciaOrariaRichiesta(rTrovata)
                                    );
                                    setDisponibilitaTecnico(tConcluso, 1);
                                }
                            }
                        }
                        printf(GREEN BOLD "\n [ OK ] Operazione completata. Sistema aggiornato.\n" RESET);
                    }
                }
                pausaSchermo();
                break;
            }

            case 7: {
                pulisciSchermo();
                printf(CYAN BOLD "\n ________________________________________________________________________________________ \n");
                printf("|                                                                                        |\n");
                printf("|                 " MAGENTA BOLD "[ MODULO ] Pianificazione Intervento in Agenda" CYAN BOLD "                         |\n");
                printf("|________________________________________________________________________________________|\n\n" RESET);

                acquisisciStringa("Codice Richiesta: ", bCodice, sizeof(bCodice));
                Richiesta* rPianif = cercaRichiestaPerCodice(archivioStorico, bCodice);

                if (rPianif == NULL) {
                    printf(RED BOLD "\n [ ERRORE ] Richiesta non trovata.\n" RESET);
                } else if (getCodiceTecnicoAssegnatoRichiesta(rPianif) == NULL) {
                    printf(YELLOW "\n [ AVVISO ] Assegna prima un tecnico alla richiesta (Opzione 3).\n" RESET);
                } else if (getStatoRichiesta(rPianif) == CONCLUSA || getStatoRichiesta(rPianif) == ANNULLATA) {
                    printf(RED BOLD "\n [ ERRORE ] Impossibile pianificare: richiesta %s.\n" RESET,
                    getStatoRichiesta(rPianif) == CONCLUSA ? "CONCLUSA" : "ANNULLATA");
                } else {
                    Tecnico* tAss = cercaTecnicoInAlbero(databaseTecnici, getCodiceTecnicoAssegnatoRichiesta(rPianif));
                    if (tAss != NULL) {
                        printf("\n");
                        printf(CYAN " ________________________________________________________________________________________ \n");
                        printf("| %-8s | %-14s | %-15s | %-7s | %-14s | %-13s |\n", "CODICE", "APPARTAMENTO", "TIPOLOGIA", "URGENZA", "STATO", "TECNICO");
                        printf("|----------|----------------|-----------------|---------|----------------|---------------|\n" RESET);
                        stampaRichiesta(rPianif);
                        printf(CYAN "|________________________________________________________________________________________|\n\n" RESET);
                        
                        do { acquisisciStringa("Data appuntamento (GG/MM/AAAA)  : ", bData, sizeof(bData)); } while (validaData(bData) == 0);
                        acquisisciStringa("Fascia oraria (es. 09:00-11:00) : ", bFascia, sizeof(bFascia));

                        if (pianificaIntervento(rPianif, tAss, bData, bFascia)) {
                            printf(GREEN BOLD "\n [ OK ] Operazione completata. Slot prenotato nell'agenda del tecnico!\n" RESET);
                        } else {
                            printf(RED BOLD "\n [ ERRORE ] Conflitto orario. Il tecnico ha gia' un impegno in questa fascia.\n" RESET);
                        }
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

                    if (scanf("%d", &sRicerca) != 1) {
                        pulisciBuffer();
                        sRicerca = -1;
                        continue;
                    }
                    pulisciBuffer(); 

                    switch (sRicerca) {
                        case 1:
                            printf(BOLD "\n>> Stato (0=Aperta, 1=Pianificata, 2=In Lav., 3=Conclusa, 4=Annullata): " RESET);
                            if (scanf("%d", &urgenza) == 1) { 
                                pulisciBuffer(); 
                                printf("\n"); 
                                stampaRichiesteArchivioPerStato(archivioStorico, (StatoRichiesta)urgenza); 
                            }
                            pausaSchermo(); 
                            break;

                        case 2:
                            printf(BOLD "\n>> Urgenza (0-4): " RESET);
                            if (scanf("%d", &urgenza) == 1) { 
                                pulisciBuffer(); 
                                printf("\n"); 
                                stampaRichiesteArchivioPerUrgenza(archivioStorico, urgenza); 
                            }
                            pausaSchermo(); 
                            break;

                        case 3:
                            acquisisciStringa("Tipologia: ", bTipo, sizeof(bTipo));
                            printf("\n"); 
                            stampaRichiesteArchivioPerTipologia(archivioStorico, bTipo);
                            pausaSchermo(); 
                            break;
                        
                        case 4:
                            acquisisciStringa("Appartamento: ", bApp, sizeof(bApp));
                            printf("\n"); 
                            stampaRichiesteArchivioPerAppartamento(archivioStorico, bApp);
                            pausaSchermo(); 
                            break;

                        case 5:
                            acquisisciStringa("Codice Tecnico: ", bCodice, sizeof(bCodice));
                            printf("\n"); 
                            stampaRichiesteArchivioPerTecnico(archivioStorico, bCodice);
                            pausaSchermo(); 
                            break;

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
                                printf(RED BOLD "\n [ ERRORE ] Nessuna richiesta trovata con codice '%s'.\n" RESET, bCodice);
                            }
                            pausaSchermo(); 
                            break;
                        }

                        case 0: 
                            break;

                        default:
                            printf(RED BOLD "\n [ ERRORE ] Opzione non valida.\n" RESET);
                            pausaSchermo(); 
                            break;
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
                    
                    if (scanf("%d", &sceltaReport) != 1) {
                        printf(RED BOLD "\n [ ERRORE ] Input non valido.\n" RESET);
                        pulisciBuffer();
                        continue;
                    }
                    pulisciBuffer();
                    
                    pulisciSchermo();
                    
                    switch (sceltaReport) {
                        case 1:
                            generaReportCompleto(archivioStorico, databaseTecnici);
                            break;
                        case 2:
                            generaReportStatoInterventi(archivioStorico);
                            break;
                        case 3:
                            generaReportPerTipologia(archivioStorico);
                            break;
                        case 4:
                            generaReportTempoMedio(archivioStorico);
                            break;
                        case 5:
                            generaReportTecnicoPiuAttivo(archivioStorico);
                            break;
                        case 6:
                            generaReportAreeProblematiche(archivioStorico);
                            break;
                        case 0:
                            break;
                        default:
                            printf(RED BOLD "\n [ ERRORE ] Opzione non valida.\n" RESET);
                    }
                    
                    if (sceltaReport != 0 && sceltaReport >= 1 && sceltaReport <= 6) {
                        pausaSchermo();
                    }
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