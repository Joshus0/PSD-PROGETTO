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

int main() {
    /* 1. Inizializzazione delle nuove strutture ADT */
    AlberoTecnici* databaseTecnici = creaAlberoTecnici();
    
    /* FIX: Aggiunta la capacità iniziale (es. 100) richiesta dalla tua firma */
    CodaPriorita* codaAttesa = creaCodaPriorita(100); 
    
    ArchivioRichieste* archivioStorico = creaArchivioRichieste();
    
    int scelta = -1;
    char bufferCodice[50];
    char bufferNome[50];
    char bufferSpec[50];

    pulisciSchermo();
    
    printf(CYAN BOLD "=========================================\n");
    printf(" SISTEMA GESTIONE MANUTENZIONE CONDOMINIO \n");
    printf("=========================================\n" RESET);
    
    printf(BOLD "\nPremi INVIO per iniziare..." RESET);
    getchar();

    /* Ciclo principale  */
    while (scelta != 0) {
        pulisciSchermo();

        printf(CYAN BOLD "\n--- MENU PRINCIPALE ---\n" RESET);
        printf("1. Registra Nuovo Tecnico\n");
        printf("2. Inserisci Nuova Richiesta\n");
        printf("3. Assegna Richiesta Prioritaria (Logica Automatica)\n");
        printf("4. Visualizza Elenco Tecnici\n");
        printf("5. Visualizza Coda di Attesa (Heap)\n");
        printf("6. Aggiornamento Stato Richiesta\n");
        printf("0. Esci dal Programma\n");
        printf(BOLD "Seleziona un'opzione: " RESET);

        if (scanf("%d", &scelta) != 1) {
            printf(RED BOLD "\n[ERRORE]" RESET RED " Inserisci un numero valido.\n" RESET);
            pulisciBuffer();
            pausaSchermo();
            continue;
        }
        
        pulisciBuffer();

        switch (scelta) {
            case 1: {
                pulisciSchermo();
                printf(CYAN BOLD "\n-- REGISTRAZIONE NUOVO TECNICO --\n" RESET);
                
                printf(BOLD "Codice (es. T01): " RESET);
                fgets(bufferCodice, 50, stdin);
                bufferCodice[strcspn(bufferCodice, "\n")] = 0;

                printf(BOLD "Nome: " RESET);
                fgets(bufferNome, 50, stdin);
                bufferNome[strcspn(bufferNome, "\n")] = 0;

                printf(BOLD "Specializzazione (es. Idraulico): " RESET);
                fgets(bufferSpec, 50, stdin);
                bufferSpec[strcspn(bufferSpec, "\n")] = 0;

                /* Logica di inserimento nell'Albero (BST) */
                if (cercaTecnicoInAlbero(databaseTecnici, bufferCodice) != NULL) {
                    printf(RED BOLD "\n[ERRORE]" RESET RED " Esiste già un tecnico con questo codice.\n" RESET);
                } else {
                    Tecnico* nuovoTecnico = creaTecnico(bufferCodice, bufferNome, bufferSpec);
                    
                    if (nuovoTecnico == NULL) {
                        printf(RED BOLD "\n[ERRORE]" RESET RED " Impossibile allocare memoria per il tecnico.\n" RESET);
                    } else {
                        inserisciInAlberoTecnici(databaseTecnici, nuovoTecnico);
                        printf(GREEN BOLD "\n[OK]" RESET GREEN " Tecnico registrato con successo.\n" RESET);
                    }
                }
                
                break;
            }

            case 2: {
                pulisciSchermo();
                printf(CYAN BOLD "\n-- INSERIMENTO RICHIESTA --\n" RESET);

                char bufferAppartamento[100];
                char bufferTipologia[100];
                char bufferDescrizione[200];
                char bufferData[50];
                int urgenza = -1;

                acquisisciStringa("Codice richiesta: ", bufferCodice, sizeof(bufferCodice));
                acquisisciStringa("Appartamento / Area: ", bufferAppartamento, sizeof(bufferAppartamento));
                acquisisciStringa("Tipologia (es. Idraulico): ", bufferTipologia, sizeof(bufferTipologia));
                acquisisciStringa("Descrizione: ", bufferDescrizione, sizeof(bufferDescrizione));

                /* MODIFICA: Controllo robusto sulla data */
                do {
                    acquisisciStringa("Data (DD/MM/YYYY): ", bufferData, sizeof(bufferData));
                    if (validaData(bufferData) == 0) {
                        printf(RED "Formato data non valido o data inesistente. Riprova.\n" RESET);
                    }
                } while (validaData(bufferData) == 0);

                do {
                    printf(BOLD "Livello di urgenza (0=min, 4=max): " RESET);
                    
                    if (scanf("%d", &urgenza) != 1) {
                        pulisciBuffer();
                        printf(RED "Valore non valido. Riprova.\n" RESET);
                        urgenza = -1;
                        continue;
                    }
                    
                    pulisciBuffer();
                    
                    if (urgenza < 0 || urgenza > 4) {
                        printf(RED "Valore fuori range. Inserisci 0-4.\n" RESET);
                    }
                    
                } while (urgenza < 0 || urgenza > 4);

                Richiesta* nuovaRichiesta = creaRichiesta(bufferCodice, bufferAppartamento, bufferTipologia, bufferDescrizione, bufferData, urgenza);
                
                if (nuovaRichiesta == NULL) {
                    printf(RED BOLD "\n[ERRORE]" RESET RED " Impossibile allocare memoria per la richiesta.\n" RESET);
                } else {
                    inserisciInCodaArchivio(archivioStorico, nuovaRichiesta);
                    inserisciInCodaPriorita(codaAttesa, nuovaRichiesta);
                    printf(GREEN BOLD "\n[OK]" RESET GREEN " Richiesta inserita in archivio e coda di priorita'.\n" RESET);
                }
                
                pausaSchermo();
                break;
            }

case 3: {
                pulisciSchermo();
                printf(CYAN BOLD "\n-- ASSEGNAZIONE AUTOMATICA --\n" RESET);

                Richiesta* richiestaDaAssegnare = NULL;
                
                while (isCodaPrioritaVuota(codaAttesa) == 0) {
                    
                    richiestaDaAssegnare = estraiMaxDaCodaPriorita(codaAttesa);
                    
                    if (richiestaDaAssegnare != NULL && isValidaInHeapRichiesta(richiestaDaAssegnare) == 1) {
                        break; 
                    }
                    
                    richiestaDaAssegnare = NULL;
                }

                if (richiestaDaAssegnare == NULL) {
                    printf(YELLOW "Nessuna richiesta valida in coda di priorita'.\n" RESET);
                    pausaSchermo();
                    break;
                }

                Tecnico* tecnicoSelezionato = trovaTecnicoDisponibilePerSpecializzazione(
                    databaseTecnici,
                    getTipologiaProblemaRichiesta(richiestaDaAssegnare)
                );

                if (tecnicoSelezionato == NULL) {
                    printf(YELLOW "Nessun tecnico disponibile con specializzazione '%s'.\n" RESET,
                           getTipologiaProblemaRichiesta(richiestaDaAssegnare));
                    inserisciInCodaPriorita(codaAttesa, richiestaDaAssegnare);
                } else {
                    setCodiceTecnicoAssegnatoRichiesta(richiestaDaAssegnare, getCodiceTecnico(tecnicoSelezionato));
                    setStatoRichiesta(richiestaDaAssegnare, PIANIFICATA);
                    setDisponibilitaTecnico(tecnicoSelezionato, 0);

                    printf(GREEN "Richiesta assegnata: \n" RESET);
                    stampaRichiesta(richiestaDaAssegnare);
                }
                
                pausaSchermo();
                break;
            }

            case 4: {
                pulisciSchermo();
                printf(CYAN BOLD "\n-- ELENCO TECNICI (IN ORDINE ALFABETICO) --\n" RESET);
                
                if (getRadiceAlberoTecnici(databaseTecnici) == NULL) {
                    printf(YELLOW "Nessun tecnico registrato.\n" RESET);
                } else {
                    visitaAlberoTecnici(databaseTecnici, stampaTecnico);
                }
                
                pausaSchermo();
                break;
            }

            case 5: {
                pulisciSchermo();
                printf(CYAN BOLD "\n-- CODA DI ATTESA (PRIORITA' MAX) --\n" RESET);
                stampaCodaPriorita(codaAttesa);
                
                pausaSchermo();
                break;
            }

 case 6: {
                pulisciSchermo();
                printf(CYAN BOLD "\n-- AGGIORNAMENTO STATO RICHIESTA --\n" RESET);

                char bufferCodiceCerca[50];
                acquisisciStringa("Inserisci il codice della richiesta da aggiornare: ", bufferCodiceCerca, sizeof(bufferCodiceCerca));

                Richiesta* richiestaTrovata = cercaRichiestaPerCodice(archivioStorico, bufferCodiceCerca);

                if (richiestaTrovata == NULL) {
                    printf(RED BOLD "\n[ERRORE]" RESET RED " Richiesta non trovata nell'archivio.\n" RESET);
                } else {
                    StatoRichiesta statoAttuale = getStatoRichiesta(richiestaTrovata);
                    
                    if (statoAttuale == CONCLUSA || statoAttuale == ANNULLATA) {
                        printf(YELLOW "\n[ATTENZIONE]" RESET YELLOW " Questa richiesta e' %s e non puo' piu' essere modificata.\n" RESET, 
                               statoAttuale == CONCLUSA ? "CONCLUSA" : "ANNULLATA");
                    } else {
                        printf("\nRichiesta trovata:\n");
                        stampaRichiesta(richiestaTrovata);

                        printf("\nSeleziona il nuovo stato:\n");
                        printf("0. APERTA\n");
                        printf("1. PIANIFICATA\n");
                        printf("2. IN_LAVORAZIONE\n");
                        printf("3. CONCLUSA\n");
                        printf("4. ANNULLATA\n");
                        
                        int nuovoStatoInt = -1;
                        while (nuovoStatoInt < 0 || nuovoStatoInt > 4) {
                            printf(BOLD "Scelta (0-4): " RESET);
                            if (scanf("%d", &nuovoStatoInt) != 1) {
                                pulisciBuffer();
                                nuovoStatoInt = -1;
                                continue;
                            }
                            pulisciBuffer();
                        }

                        StatoRichiesta nuovoStato = (StatoRichiesta)nuovoStatoInt;
                        setStatoRichiesta(richiestaTrovata, nuovoStato);

                        if (nuovoStato == ANNULLATA || nuovoStato == CONCLUSA || nuovoStato == IN_LAVORAZIONE) {
                            setValidaInHeapRichiesta(richiestaTrovata, 0);
                        }

                        if (nuovoStato == CONCLUSA) {
                            char bufferDataChiusura[50];
                            do {
                                acquisisciStringa("Inserisci data di chiusura (DD/MM/YYYY): ", bufferDataChiusura, sizeof(bufferDataChiusura));
                            } while (validaData(bufferDataChiusura) == 0);
                            
                            setDataChiusuraRichiesta(richiestaTrovata, bufferDataChiusura);
                        }

                        printf(GREEN BOLD "\n[OK]" RESET GREEN " Stato aggiornato.\n" RESET);
                    }
                }

                pausaSchermo();
                break;
            }

            case 0: {
                pulisciSchermo();
                printf(YELLOW BOLD "\nPulizia memoria e uscita in corso... Arrivederci!\n\n" RESET);
                break;
            }

            default: {
                printf(RED BOLD "\n[ERRORE]" RESET RED " Opzione non valida. Riprova.\n" RESET);
                pausaSchermo();
                break;
            }
        }
    }

    /* 3. Pulizia Memoria finale (Zero Memory Leak) */
    distruggiAlberoTecnici(databaseTecnici);
    distruggiCodaPriorita(codaAttesa);
    distruggiArchivioRichieste(archivioStorico);

    return 0;
}