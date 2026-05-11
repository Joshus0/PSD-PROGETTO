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
    
                acquisisciStringa("Codice Identificativo (es. T01): ", bCodice, sizeof(bCodice));
                acquisisciStringa("Nome Completo                  : ", bNome, sizeof(bNome));
                acquisisciStringa("Specializzazione               : ", bSpec, sizeof(bSpec));

                if (cercaTecnicoInAlbero(databaseTecnici, bCodice) != NULL) {
                    printf(RED BOLD "\n [ ERRORE ]" RESET RED " Il codice tecnico inserito esiste già.\n" RESET);
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

                /* 1. Chiediamo il codice */
                acquisisciStringa("Codice Richiesta       : ", bCodice, sizeof(bCodice));

                /* --- NUOVO CONTROLLO UNIVOCO --- */
                if (cercaRichiestaPerCodice(archivioStorico, bCodice) != NULL) {
                    printf(RED BOLD "\n [ ERRORE ]" RESET RED " Esiste gia' una richiesta con questo codice!\n" RESET);
                    pausaSchermo();
                    break; /* Esce dal case 2 e torna al menu principale */
                }
                /* ------------------------------- */

                /* 2. Se il codice è nuovo, procediamo col resto */
                acquisisciStringa("Appartamento / Area    : ", bApp, sizeof(bApp));
                acquisisciStringa("Tipologia Problema     : ", bTipo, sizeof(bTipo));
                acquisisciStringa("Breve Descrizione      : ", bDesc, sizeof(bDesc));

                do {
                    acquisisciStringa("Data (GG/MM/AAAA))      : ", bData, sizeof(bData));
                    if (validaData(bData) == 0) printf(RED "  >> Formato data errato. Riprova.\n" RESET);
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

                Richiesta* rAssegnare = NULL;
                while (isCodaPrioritaVuota(codaAttesa) == 0) {
                    rAssegnare = estraiMaxDaCodaPriorita(codaAttesa);
                    if (rAssegnare != NULL && isValidaInHeapRichiesta(rAssegnare) == 1) break; 
                    rAssegnare = NULL;
                }

                if (rAssegnare == NULL) {
                    printf(YELLOW "Nessuna richiesta valida in attesa nella Coda.\n" RESET);
                } else {
                    Tecnico* tSelezionato = trovaTecnicoDisponibilePerSpecializzazione(databaseTecnici, getTipologiaProblemaRichiesta(rAssegnare));
                    if (tSelezionato == NULL) {
                        printf(YELLOW "Nessun tecnico disponibile per la specializzazione richiesta (%s).\n" RESET, getTipologiaProblemaRichiesta(rAssegnare));
                        printf(YELLOW "La richiesta torna in coda.\n" RESET);
                        inserisciInCodaPriorita(codaAttesa, rAssegnare);
                    } else {
                        setCodiceTecnicoAssegnatoRichiesta(rAssegnare, getCodiceTecnico(tSelezionato));
                        setStatoRichiesta(rAssegnare, PIANIFICATA);
                        setDisponibilitaTecnico(tSelezionato, 0);

                        printf(GREEN BOLD "[ MATCH TROVATO ]\n" RESET);
                        printf("Tecnico assegnato  : " CYAN "%s\n" RESET, getNomeTecnico(tSelezionato));
                        printf("Dettagli Richiesta :\n");
                        stampaRichiesta(rAssegnare);
                    }
                }
                pausaSchermo();
                break;
            }

            case 4: {
                pulisciSchermo();
                printf(MAGENTA BOLD "\n[ VISUALIZZAZIONE ] " RESET "Database Tecnici (BST)\n");
                printf(MAGENTA "------------------------------------------------\n\n" RESET);
                if (getRadiceAlberoTecnici(databaseTecnici) == NULL) {
                    printf(YELLOW "Il database tecnici e' attualmente vuoto.\n" RESET);
                } else {
                    visitaAlberoTecnici(databaseTecnici, stampaTecnico);
                }
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
                            do {
                                acquisisciStringa("Inserisci data chiusura effettiva (GG/MM/AAAA): ", bData, sizeof(bData));
                                } while (validaData(bData) == 0);
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
                int sRicerca = 0;
                do {
                    pulisciSchermo();
                    printf(MAGENTA BOLD "\n[ MODULO ] " RESET "Motore di Ricerca\n");
                    printf(MAGENTA "----------------------------------------\n" RESET);
                    printf(" [1] Cerca per Stato\n [2] Cerca per Urgenza\n [3] Cerca per Tipologia\n [4] Cerca per Appartamento\n [5] Cerca per Tecnico\n [0] Indietro\n");
                    printf("\nFiltro: ");
                    
                    if (scanf("%d", &sRicerca) != 1) sRicerca = -1;
                    pulisciBuffer(); 

                    switch (sRicerca) {
                        case 1:
                            printf("Stato (0=Aperta, 1=Pianificata, 2=In Lav., 3=Conclusa, 4=Annullata): ");
                            if (scanf("%d", &urgenza) == 1) { pulisciBuffer(); printf("\n"); stampaRichiesteArchivioPerStato(archivioStorico, (StatoRichiesta)urgenza); }
                            pausaSchermo(); break;
                        case 2:
                            printf("Urgenza (0-4): ");
                            if (scanf("%d", &urgenza) == 1) { pulisciBuffer(); printf("\n"); stampaRichiestePerUrgenza(codaAttesa, urgenza); }
                            pausaSchermo(); break;
                        case 3:
                            acquisisciStringa("Tipologia: ", bTipo, 100);
                            printf("\n"); stampaRichiestePerTipologia(codaAttesa, bTipo);
                            pausaSchermo(); break;
                        case 4:
                            acquisisciStringa("Appartamento: ", bApp, 100);
                            printf("\n"); stampaRichiestePerAppartamento(codaAttesa, bApp);
                            pausaSchermo(); break;
                        case 5:
                            acquisisciStringa("Codice Tecnico: ", bCodice, 50);
                            printf("\n"); stampaRichiestePerTecnico(codaAttesa, bCodice);
                            pausaSchermo(); break;
                        case 0: break;
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
                printf(MAGENTA BOLD "\n[ REPORT ] " RESET "Statistiche Globali di Sistema\n");
                printf(MAGENTA "------------------------------------------------\n\n" RESET);
                stampaReportStatistiche(archivioStorico, databaseTecnici);
                pausaSchermo();
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