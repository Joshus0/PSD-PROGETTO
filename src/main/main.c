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
    printf(CYAN BOLD "\n____________________________________________________\n");
    printf("|                " RESET BOLD "BENVENUTO" CYAN BOLD "                         |\n");
    printf("|__________________________________________________|\n" RESET);
    printf(BOLD "\nPremi INVIO per entrare nel sistema..." RESET);
    getchar();

    /* 2. Ciclo Principale */
    while (scelta != 0) {
        pulisciSchermo();

        /* --- NUOVA INTERFACCIA A PANNELLO (DASHBOARD) --- */
        printf(CYAN BOLD "\n________________________________________________________________\n");
        printf("|                 " RESET BOLD "PANNELLO DI CONTROLLO GENERALE" CYAN BOLD "               |\n");
        printf("|_____________________________|________________________________|\n");
        printf("| " RESET " [1] Nuovo Tecnico          " CYAN BOLD "|" RESET "  [7] Pianifica Intervento     " CYAN BOLD " |\n");
        printf("| " RESET " [2] Nuova Richiesta        " CYAN BOLD "|" RESET "  [8] Ricerca Avanzata         " CYAN BOLD " |\n");
        printf("| " RESET " [3] Assegnazione Auto      " CYAN BOLD "|" RESET "  [9] Storico Conclusi         " CYAN BOLD " |\n");
        printf("| " RESET " [4] Elenco Tecnici         " CYAN BOLD "|" RESET " [10] Statistiche Sistema      " CYAN BOLD " |\n");
        printf("| " RESET " [5] Coda Attesa (Heap)     " CYAN BOLD "|" RESET " [11] Agenda Tecnico           " CYAN BOLD " |\n");
        printf("| " RESET " [6] Aggiorna Stato         " CYAN BOLD "|" YELLOW BOLD "  [0] Esci dal Programma       " CYAN BOLD " |\n");
        printf("|_____________________________|________________________________|\n" RESET);
        printf(BOLD YELLOW "\n>> Inserisci comando: " RESET);

        if (scanf("%d", &scelta) != 1) {
            printf(RED BOLD "\n [ ERRORE ]" RESET RED " Input non valido. Inserisci un numero.\n" RESET);
            pulisciBuffer();
            pausaSchermo();
            continue;
        }
        pulisciBuffer();

        /* 3. Gestione Scelte (Tutto nel Main) */
        switch (scelta) {
            
            case 1: {
                pulisciSchermo();
                printf(MAGENTA BOLD "\n[ MODULO ] " RESET "Registrazione Nuovo Tecnico\n");
                printf(MAGENTA "----------------------------------------\n\n" RESET);
                
                do {
                    acquisisciStringa("Codice Identificativo (es. T01): ", bCodice, sizeof(bCodice));
                    if (strlen(bCodice) == 0) printf(RED "  >> Il codice non puo' essere vuoto.\n" RESET);
                } while (strlen(bCodice) == 0);

                do {
                    acquisisciStringa("Nome Completo                  : ", bNome, sizeof(bNome));
                    if (strlen(bNome) == 0) printf(RED "  >> Il nome non puo' essere vuoto.\n" RESET);
                } while (strlen(bNome) == 0);

                do {
                    acquisisciStringa("Specializzazione               : ", bSpec, sizeof(bSpec));
                    if (strlen(bSpec) == 0) printf(RED "  >> La specializzazione non puo' essere vuota.\n" RESET);
                } while (strlen(bSpec) == 0);

                if (cercaTecnicoInAlbero(databaseTecnici, bCodice) != NULL) {
                    printf(RED BOLD "\n [ ERRORE ]" RESET RED " Il codice tecnico inserito esiste gia'.\n" RESET);
                } else {
                    Tecnico* nuovoTecnico = creaTecnico(bCodice, bNome, bSpec);
                    if (nuovoTecnico == NULL) {
                        printf(RED BOLD "\n [ ERRORE ]" RESET RED " Fallimento allocazione memoria.\n" RESET);
                    } else {
                        inserisciInAlberoTecnici(databaseTecnici, nuovoTecnico);
                        printf(GREEN BOLD "\n [ OK ]" RESET GREEN " Tecnico salvato nel database.\n" RESET);
                    }
                }
                pausaSchermo();
                break;
            }

            case 2: {
                pulisciSchermo();
                printf(MAGENTA BOLD "\n[ MODULO ] " RESET "Inserimento Nuova Richiesta\n");
                printf(MAGENTA "----------------------------------------\n\n" RESET);

                do {
                    acquisisciStringa("Codice Richiesta       : ", bCodice, sizeof(bCodice));
                    if (strlen(bCodice) == 0) 
                        printf(RED "  >> Il codice richiesta non puo' essere vuoto.\n" RESET);
                } while (strlen(bCodice) == 0);

                if (cercaRichiestaPerCodice(archivioStorico, bCodice) != NULL) {
                    printf(RED BOLD "\n [ ERRORE ]" RESET RED " Esiste gia' una richiesta con questo codice!\n" RESET);
                    pausaSchermo();
                    break; 
                }

                do {
                    acquisisciStringa("Appartamento / Area    : ", bApp, sizeof(bApp));
                    if (strlen(bApp) == 0) 
                        printf(RED "  >> L'area interessata non puo' essere vuota.\n" RESET);
                } while (strlen(bApp) == 0);

                do {
                    acquisisciStringa("Tipologia Problema     : ", bTipo, sizeof(bTipo));
                    if (strlen(bTipo) == 0) 
                        printf(RED "  >> La tipologia non puo' essere vuota.\n" RESET);
                } while (strlen(bTipo) == 0);

                do {
                    acquisisciStringa("Breve Descrizione      : ", bDesc, sizeof(bDesc));
                    if (strlen(bDesc) == 0) 
                        printf(RED "  >> La descrizione non puo' essere vuota.\n" RESET);
                } while (strlen(bDesc) == 0);

                do {
                    acquisisciStringa("Data (GG/MM/AAAA)      : ", bData, sizeof(bData));
                    if (validaData(bData) == 0) 
                        printf(RED "  >> Formato data errato. Riprova.\n" RESET);
                } while (validaData(bData) == 0);

                do {
                    printf("Urgenza (0=Min, 4=Max) : ");
                    if (scanf("%d", &urgenza) != 1) {
                        pulisciBuffer();
                        printf(RED "  >> Inserire un numero da 0 a 4.\n" RESET);
                        urgenza = -1;
                        continue;
                    }
                    pulisciBuffer();
                    if (urgenza < 0 || urgenza > 4)
                        printf(RED "  >> Valore fuori range (0-4).\n" RESET);
                } while (urgenza < 0 || urgenza > 4);

                Richiesta* nuovaRichiesta = creaRichiesta(bCodice, bApp, bTipo, bDesc, bData, urgenza);
                if (nuovaRichiesta == NULL) {
                    printf(RED BOLD "\n [ ERRORE ]" RESET RED " Impossibile allocare memoria.\n" RESET);
                } else {
                    inserisciInCodaArchivio(archivioStorico, nuovaRichiesta);
                    inserisciInCodaPriorita(codaAttesa, nuovaRichiesta);
                    printf(GREEN BOLD "\n [ OK ]" RESET GREEN " Richiesta registrata e accodata con successo.\n" RESET);
                }
                pausaSchermo();
                break;
            }

            case 3: {
                pulisciSchermo();
                printf(MAGENTA BOLD "\n[ MODULO ] " RESET "Assegnazione Automatica (Sistema Priority)\n");
                printf(MAGENTA "--------------------------------------------------------\n\n" RESET);

                if (isCodaPrioritaVuota(codaAttesa)) {
                    printf(YELLOW "Nessuna richiesta valida in attesa nella Coda.\n" RESET);
                    pausaSchermo();
                    break;
                }

                Richiesta* rAssegnare = NULL;
                Tecnico* tSelezionato = NULL;
                
                /* Alloca l'array in base alla dimensione reale della coda per evitare overflow */
                int dimCoda = getDimensioneCodaPriorita(codaAttesa);
                Richiesta** tempArray = (Richiesta**)malloc(dimCoda * sizeof(Richiesta*));
                int tempCount = 0;

                /* Estrai finché non trovi un match o l'heap si svuota */
                while (isCodaPrioritaVuota(codaAttesa) == 0) {
                    rAssegnare = estraiMaxDaCodaPriorita(codaAttesa);
                    
                    /* Se la richiesta è stata rimossa logicamente, ignorala */
                    if (rAssegnare == NULL || isValidaInHeapRichiesta(rAssegnare) == 0) continue;

                    tSelezionato = trovaTecnicoDisponibilePerSpecializzazione(databaseTecnici, getTipologiaProblemaRichiesta(rAssegnare));
                    
                    if (tSelezionato != NULL) {
                        break; /* Match trovato! Usciamo dal ciclo */
                    } else {
                        tempArray[tempCount++] = rAssegnare; /* Parcheggia e passa alla prossima */
                        rAssegnare = NULL; 
                    }
                }

                /* 1. Reinserisci nell'heap tutte le richieste parcheggiate */
                for (int i = 0; i < tempCount; i++) {
                    inserisciInCodaPriorita(codaAttesa, tempArray[i]);
                }
                
                free(tempArray); /* Libera la memoria dell'array temporaneo */

                /* 2. Procedi con l'assegnazione se hai trovato un match */
                if (rAssegnare != NULL && tSelezionato != NULL) {
                    setCodiceTecnicoAssegnatoRichiesta(rAssegnare, getCodiceTecnico(tSelezionato));
                    setStatoRichiesta(rAssegnare, PIANIFICATA);
                    setDisponibilitaTecnico(tSelezionato, 0);

                    printf(GREEN BOLD "[ MATCH TROVATO ]\n" RESET);
                    printf("Tecnico assegnato  : " CYAN "%s\n" RESET, getNomeTecnico(tSelezionato));
                    printf("Dettagli Richiesta :\n");
                    stampaRichiesta(rAssegnare);
                } else {
                    printf(YELLOW "Nessuna richiesta in attesa puo' essere soddisfatta al momento dai tecnici disponibili.\n" RESET);
                }
                
                pausaSchermo();
                break;
            }

case 4: {
                pulisciSchermo();
                printf(MAGENTA BOLD "\n[ VISUALIZZAZIONE ] " RESET "Monitoraggio Carico di Lavoro Tecnici\n\n");
                
                /* Bordo Superiore allargato (91 trattini) */
                printf(CYAN " ___________________________________________________________________________________________ \n");
                printf("| %-10s | %-25s | %-20s | %-25s |\n", "CODICE", "NOME", "SPECIALIZZ.", "STATO CARICO");
                printf("|------------|---------------------------|----------------------|---------------------------|\n" RESET);
                
                if (getRadiceAlberoTecnici(databaseTecnici) == NULL) {
                    /* Stringa perfettamente allineata con i nuovi margini (27 spazi vuoti alla fine) */
                    printf(CYAN "|" RESET YELLOW " Nessun tecnico presente. Usa l'opzione [1] per registrarne uno.                           " CYAN "|\n" RESET);
                } else {
                    visitaAlberoTecnici(databaseTecnici, stampaCaricoLavoroTecnico);
                }
                
                /* Bordo Inferiore allargato (91 trattini) */
                printf(CYAN "|___________________________________________________________________________________________|\n" RESET);
                
                printf("\n");
                pausaSchermo();
                break;
            }

            case 5: {
                pulisciSchermo();
                printf(MAGENTA BOLD "\n[ VISUALIZZAZIONE ] " RESET "Coda di Attesa (Max-Heap)\n");
                printf(MAGENTA "------------------------------------------------\n\n" RESET);
                stampaCodaPriorita(codaAttesa);
                pausaSchermo();
                break;
            }

            case 6: {
                pulisciSchermo();
                printf(MAGENTA BOLD "\n[ MODULO ] " RESET "Aggiornamento Stato Intervento\n");
                printf(MAGENTA "----------------------------------------\n\n" RESET);

                acquisisciStringa("Inserisci Codice Richiesta: ", bCodice, sizeof(bCodice));
                Richiesta* rTrovata = cercaRichiestaPerCodice(archivioStorico, bCodice);

                if (rTrovata == NULL) {
                    printf(RED BOLD "\n [ ERRORE ]" RESET RED " Richiesta inesistente.\n" RESET);
                } else {
                    StatoRichiesta sAttuale = getStatoRichiesta(rTrovata);
                    if (sAttuale == CONCLUSA || sAttuale == ANNULLATA) {
                        printf(YELLOW BOLD "\n [ STOP ]" RESET YELLOW " Pratica gia' archiviata in modo definitivo.\n" RESET);
                    } else {
                        printf("\n");
                        stampaRichiesta(rTrovata);
                        printf("\nNuovo Stato: [0] APERTA, [1] PIANIFICATA, [2] IN LAVORAZIONE, [3] CONCLUSA, [4] ANNULLATA\n");
                        
                        int nuovoS = -1;
                        while (nuovoS < 0 || nuovoS > 4) {
                            printf("Selezione: ");
                            if (scanf("%d", &nuovoS) != 1) { pulisciBuffer(); nuovoS = -1; continue; }
                            pulisciBuffer();
                        }

                        StatoRichiesta nStato = (StatoRichiesta)nuovoS;
                        setStatoRichiesta(rTrovata, nStato);

                        if (nStato == ANNULLATA || nStato == CONCLUSA || nStato == IN_LAVORAZIONE) {
                            setValidaInHeapRichiesta(rTrovata, 0);
                        }

                        if (nStato == CONCLUSA) {
                            /* Avviso se si sta chiudendo una richiesta non in lavorazione */
                            if (sAttuale != IN_LAVORAZIONE) {
                                printf(YELLOW BOLD "\n [ AVVISO ]" RESET YELLOW " La richiesta non e' in stato IN_LAVORAZIONE.\n" RESET);
                            }
                            
                            int dataValida = 0;
                            do {
                                acquisisciStringa("Inserisci data chiusura effettiva (GG/MM/AAAA): ", bData, sizeof(bData));
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
                                }
                            }
                        }

                    }
                        
                        printf(GREEN BOLD "\n [ OK ]" RESET GREEN " Sistema aggiornato.\n" RESET);
                    }
            
            pausaSchermo();
            break;
            }

            case 7: {
                pulisciSchermo();
                printf(MAGENTA BOLD "\n[ MODULO ] " RESET "Pianificazione Intervento in Agenda\n");
                printf(MAGENTA "------------------------------------------------\n\n" RESET);

                acquisisciStringa("Codice Richiesta: ", bCodice, sizeof(bCodice));
                Richiesta* rPianif = cercaRichiestaPerCodice(archivioStorico, bCodice);

                if (rPianif == NULL) {
                    printf(RED BOLD "\n [ ERRORE ]" RESET RED " Richiesta non trovata.\n" RESET);
                } else if (getCodiceTecnicoAssegnatoRichiesta(rPianif) == NULL) {
                    printf(YELLOW "\n [ ATTENZIONE ]" RESET YELLOW " Assegna prima un tecnico alla richiesta (Opzione 3).\n" RESET);
                }  else if (getStatoRichiesta(rPianif) == CONCLUSA || getStatoRichiesta(rPianif) == ANNULLATA) {
                    printf(RED BOLD "\n [ ERRORE ]" RESET RED " Impossibile pianificare: richiesta %s.\n" RESET,
                    getStatoRichiesta(rPianif) == CONCLUSA ? "CONCLUSA" : "ANNULLATA");
                    }else {
                    Tecnico* tAss = cercaTecnicoInAlbero(databaseTecnici, getCodiceTecnicoAssegnatoRichiesta(rPianif));
                    if (tAss != NULL) {
                        printf("\n");
                        stampaRichiesta(rPianif);
                        
                        do { acquisisciStringa("\nData appuntamento (GG-MM-AAAA)    : ", bData, sizeof(bData)); } while (validaData(bData) == 0);
                        acquisisciStringa("Fascia oraria (es. 09:00-11:00) : ", bFascia, sizeof(bFascia));

                        if (pianificaIntervento(rPianif, tAss, bData, bFascia)) {
                            printf(GREEN BOLD "\n [ OK ]" RESET GREEN " Slot prenotato nell'agenda del tecnico!\n" RESET);
                        } else {
                            printf(RED BOLD "\n [ CONFLITTO ]" RESET RED " Il tecnico ha gia' un impegno in questa fascia.\n" RESET);
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
                    printf(CYAN BOLD "\n___________________________________________________________\n");
                    printf("|                                                         |\n");
                    printf("|               " RESET BOLD "MOTORE DI RICERCA AVANZATO" CYAN BOLD "                |\n");
                    printf("|_____________________________|___________________________|\n");
                    printf("|" RESET " [1] Filtra per Stato        " CYAN BOLD "|" RESET " [4] Cerca per App./Area " CYAN BOLD "|\n");
                    printf("|" RESET " [2] Filtra per Urgenza      " CYAN BOLD "|" RESET " [5] Cerca per Tecnico   " CYAN BOLD "|\n");
                    printf("|" RESET " [3] Cerca per Tipologia     " CYAN BOLD "|" RESET " [6] Cerca per Codice    " CYAN BOLD "|\n");
                    printf("|_____________________________|___________________________|\n");
                    printf("|" YELLOW BOLD "                       [0] Indietro                      " CYAN BOLD "|\n");
                    printf("|_________________________________________________________|\n" RESET);
                    printf(BOLD YELLOW "\n>> Seleziona filtro di ricerca: " RESET);

                    if (scanf("%d", &sRicerca) != 1) {
                        pulisciBuffer();
                        sRicerca = -1;
                        continue;
                    }
                    pulisciBuffer(); 

                    switch (sRicerca) {
                        case 1:
                            printf("\nStato (0=Aperta, 1=Pianificata, 2=In Lav., 3=Conclusa, 4=Annullata): ");
                            if (scanf("%d", &urgenza) == 1) { 
                                pulisciBuffer(); 
                                printf("\n"); 
                                stampaRichiesteArchivioPerStato(archivioStorico, (StatoRichiesta)urgenza); 
                            }
                            pausaSchermo(); 
                            break;

                        case 2:
                            printf("Urgenza (0-4): ");
                            if (scanf("%d", &urgenza) == 1) { 
                                pulisciBuffer(); 
                                printf("\n"); 
                                stampaRichiesteArchivioPerUrgenza(archivioStorico, urgenza); 
                            }
                            pausaSchermo(); 
                            break;

                        case 3:
                            acquisisciStringa("Tipologia: ", bTipo, 100);
                            printf("\n"); 
                            stampaRichiesteArchivioPerTipologia(archivioStorico, bTipo);
                            pausaSchermo(); 
                            break;
                        
                        case 4:
                            acquisisciStringa("Appartamento: ", bApp, 100);
                            printf("\n"); 
                            stampaRichiesteArchivioPerAppartamento(archivioStorico, bApp);
                            pausaSchermo(); 
                            break;

                        case 5:
                            acquisisciStringa("Codice Tecnico: ", bCodice, 50);
                            printf("\n"); 
                            stampaRichiesteArchivioPerTecnico(archivioStorico, bCodice);
                            pausaSchermo(); 
                            break;

                        case 6: {
                            acquisisciStringa("\nInserisci Codice Richiesta: ", bCodice, sizeof(bCodice));
                            Richiesta* rTrovata = cercaRichiestaPerCodice(archivioStorico, bCodice);
                            if (rTrovata != NULL) {
                                printf(GREEN BOLD "\n [ TROVATA ]" RESET " Dettagli della pratica:\n\n");
                                stampaRichiesta(rTrovata);
                            } else {
                                printf(RED BOLD "\n [ ERRORE ]" RESET " Nessuna richiesta trovata con codice '%s'.\n", bCodice);
                            }
                            pausaSchermo(); 
                            break;
                        }

                        case 0: 
                            break;

                        default:
                            printf(RED BOLD "\n [ ERRORE ]" RESET " Opzione non valida.\n");
                            pausaSchermo(); 
                            break;
                    }
                } while (sRicerca != 0);
                break;
            }

            case 9: {
                pulisciSchermo();
                printf(MAGENTA BOLD "\n[ VISUALIZZAZIONE ] " RESET "Archivio Storico Interventi\n");
                printf(MAGENTA "------------------------------------------------\n\n" RESET);
                stampaStoricoInterventi(archivioStorico);
                pausaSchermo();
                break;
            }

            case 10: {
                pulisciSchermo();
                printf(MAGENTA BOLD "\n[ REPORT ] " RESET "Sistema Avanzato di Reporting\n");
                printf(MAGENTA "------------------------------------------------\n\n" RESET);
                
                int sceltaReport = -1;
                while (sceltaReport != 0) {
                    printf(CYAN BOLD "\nScelta Report:\n" RESET);
                    printf("  [1] Report Completo (Tutte le statistiche)\n");
                    printf("  [2] Stato Interventi (Aperti/Chiusi)\n");
                    printf("  [3] Interventi per Tipologia\n");
                    printf("  [4] Tempo Medio di Completamento\n");
                    printf("  [5] Tecnico Più Attivo\n");
                    printf("  [6] Aree con Più Problemi\n");
                    printf("  [0] Torna al menu principale\n");
                    printf(BOLD YELLOW "\n>> Inserisci comando: " RESET);
                    
                    if (scanf("%d", &sceltaReport) != 1) {
                        printf(RED BOLD "\n [ ERRORE ]" RESET RED " Input non valido.\n" RESET);
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
                            printf(RED BOLD "\n [ ERRORE ]" RESET RED " Opzione non valida.\n" RESET);
                    }
                    
                    if (sceltaReport != 0 && sceltaReport >= 1 && sceltaReport <= 6) {
                        pausaSchermo();
                        pulisciSchermo();
                    }
                }
                break;
            }

            case 11: {
                pulisciSchermo();
                printf(MAGENTA BOLD "\n[ VISUALIZZAZIONE ] " RESET "Agenda Lavori Tecnico\n");
                printf(MAGENTA "------------------------------------------------\n\n" RESET);

                acquisisciStringa("Inserisci Codice Tecnico: ", bCodice, sizeof(bCodice));
                Tecnico* tAgenda = cercaTecnicoInAlbero(databaseTecnici, bCodice);
                
                if (tAgenda == NULL) {
                    printf(RED BOLD "\n [ ERRORE ]" RESET RED " Tecnico inesistente.\n" RESET);
                } else {
                    printf("\n");
                    stampaAgendaTecnico(tAgenda);
                }
                pausaSchermo();
                break;
            }

            case 0: {
                pulisciSchermo();
                printf(YELLOW BOLD "\n>>> Arresto dei moduli e pulizia memoria in corso...\n" RESET);
                printf(GREEN ">>> Sistema terminato. Arrivederci!\n\n" RESET);
                break;
            }

            default: {
                printf(RED BOLD "\n [ ERRORE ]" RESET RED " Comando non riconosciuto.\n" RESET);
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