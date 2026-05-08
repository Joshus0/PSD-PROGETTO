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
        printf("5. Visualizza Richieste\n");
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

                if (isCodaPrioritaVuota(codaAttesa)) {
                    printf(YELLOW "Nessuna richiesta in coda di priorita'.\n" RESET);
                    pausaSchermo();
                    break;
                }

                Richiesta* richiestaDaAssegnare = estraiMaxDaCodaPriorita(codaAttesa);
                
                if (richiestaDaAssegnare == NULL) {
                    printf(RED "Errore nell'estrazione della richiesta.\n" RESET);
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
                int sceltaVisualizzazione = 0;
                
                do {
                    pulisciSchermo();
                    printf(CYAN BOLD "\n-- VISUALIZZAZIONE DELLE RICHIESTE --\n" RESET);
                    printf("1. Per stato\n");
                    printf("2. Per urgenza\n");
                    printf("3. Per tipologia\n");
                    printf("4. Per appartamento\n");
                    printf("5. Per tecnico\n");
                    printf("0. Torna indietro\n");
                    printf("\nScegli un'opzione: ");
                    
                    if (scanf("%d", &sceltaVisualizzazione) != 1) {
                        sceltaVisualizzazione = -1; // Gestione errore se l'utente inserisce una lettera
                    }
                    pulisciBuffer(); 

                    switch (sceltaVisualizzazione) {
                        case 1: {
                            int statoInput;
                            pulisciSchermo();
                            printf(CYAN BOLD "\n-- RICHIESTE PER STATO --\n" RESET);
                            printf("Stati disponibili:\n");
                            printf("0 = APERTA\n1 = PIANIFICATA\n2 = IN_LAVORAZIONE\n3 = CONCLUSA\n4 = ANNULLATA\n");
                            printf("\nInserisci il numero dello stato da cercare: ");
                            
                            scanf("%d", &statoInput);
                            pulisciBuffer();
                            
                            printf("\n");
                            stampaRichiestePerStato(codaAttesa, (StatoRichiesta)statoInput);
                            
                            pausaSchermo();
                            break;
                        }
                        case 2: {
                            int urgenzaInput;
                            pulisciSchermo();
                            printf(CYAN BOLD "\n-- RICHIESTE PER URGENZA --\n" RESET);
                            printf("Inserisci il livello di urgenza da cercare (numero): ");
                            
                            scanf("%d", &urgenzaInput);
                            pulisciBuffer();
                            
                            printf("\n");
                            stampaRichiestePerUrgenza(codaAttesa, urgenzaInput);
                            
                            pausaSchermo();
                            break;
                        }
                        case 3: {
                            char tipoInput[50];
                            pulisciSchermo();
                            printf(CYAN BOLD "\n-- RICHIESTE PER TIPOLOGIA --\n" RESET);
                            acquisisciStringa("Inserisci la tipologia (es. Elettrico, Idraulico): ", tipoInput, 50);
                            
                            printf("\n");
                            stampaRichiestePerTipologia(codaAttesa, tipoInput);
                            
                            pausaSchermo();
                            break;
                        }
                        case 4: {
                            char appInput[50];
                            pulisciSchermo();
                            printf(CYAN BOLD "\n-- RICHIESTE PER APPARTAMENTO --\n" RESET);
                            acquisisciStringa("Inserisci il codice/nome dell'appartamento: ", appInput, 50);
                            
                            printf("\n");
                            stampaRichiestePerAppartamento(codaAttesa, appInput);
                            
                            pausaSchermo();
                            break;
                        }
                        case 5: {
                            char tecnicoInput[50];
                            pulisciSchermo();
                            printf(CYAN BOLD "\n-- RICHIESTE PER TECNICO --\n" RESET);
                            acquisisciStringa("Inserisci il codice del tecnico assegnato: ", tecnicoInput, 50);
                            
                            printf("\n");
                            stampaRichiestePerTecnico(codaAttesa, tecnicoInput);
                            
                            pausaSchermo();
                            break;
                        }
                        case 0:
                            // Torna al menu principale
                            break;
                        default:
                            printf("\nScelta non valida. Riprova.\n");
                            pausaSchermo();
                            break;
                    }
                } while (sceltaVisualizzazione != 0);
                
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