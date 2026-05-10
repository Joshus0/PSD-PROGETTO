#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "entita/richiesta.h"
#include "entita/tecnico.h"
#include "archivioRichieste.h"
#include "codaPriorita.h"
#include "alberoTecnici.h"
#include "main/utilita.h"

#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define MAGENTA "\033[35m"

FILE* fileTempOracolo = NULL;

void stampaTecnicoSuFile(Tecnico* t) {
    if (fileTempOracolo != NULL && t != NULL) {
        fprintf(fileTempOracolo, "Codice: %s | Nome: %s | Specializzazione: %s\n",
                getCodiceTecnico(t), getNomeTecnico(t), getSpecializzazioneTecnico(t));
    }
}

int main() {
    int scelta = -1;
    
    setvbuf(stdout, NULL, _IONBF, 0);

    while (scelta != 0) {
        pulisciSchermo();
        printf(CYAN BOLD "_____________________________________________________________________\n");
        printf("|                                                                   |\n");
        printf("|                  " RESET BOLD "SUITE DI TESTING AUTOMATIZZATA" CYAN BOLD "                   |\n");
        printf("|_________________________________|_________________________________|\n");
        printf("|" RESET " [1] Reg. Richieste (da file)    " CYAN BOLD "|" RESET " [5] Aggiornamento Stato         " CYAN BOLD "|\n");
        printf("|" RESET " [2] Reg. Tecnici (da file)      " CYAN BOLD "|" RESET " [6] Ricerca e Filtri            " CYAN BOLD "|\n");
        printf("|" RESET " [3] Assegnazione Corretta       " CYAN BOLD "|" RESET " [7] Storico Interventi          " CYAN BOLD "|\n");
        printf("|" RESET " [4] Pianificazione e Conflitti  " CYAN BOLD "|" RESET " [8] Generazione Report          " CYAN BOLD "|\n");
        printf("|_________________________________|_________________________________|\n");
        printf("|" YELLOW BOLD "                             [0] Esci                              " CYAN BOLD "|\n");
        printf("|___________________________________________________________________|\n" RESET);
        printf(BOLD YELLOW "\n>> Seleziona il caso di test da avviare: " RESET);

        if (scanf("%d", &scelta) != 1) {
            pulisciBuffer();
            scelta = -1;
            continue;
        }
        pulisciBuffer();

        switch (scelta) {
            case 1: {
                pulisciSchermo();
                printf(MAGENTA BOLD "\n[ TEST 1 ] " RESET "Verifica Registrazione Richieste (Oracolo)\n");
                printf(MAGENTA "=====================================================================\n" RESET);
                printf(CYAN " OBIETTIVO DEL TEST:\n" RESET);
                printf(" 1. Carica richieste in ordine casuale da 'richieste.txt'.\n");
                printf(" 2. Le inserisce nella Coda di Priorita' (Max-Heap) per ordinarle.\n");
                printf(" 3. Estrae la cima dell'Heap dimostrando che esce prima l'Urgenza 4.\n");
                printf(" 4. Confronta l'estrazione con la soluzione esatta (Oracolo).\n");
                printf(MAGENTA "=====================================================================\n\n" RESET);

                ArchivioRichieste* archivio = creaArchivioRichieste();
                CodaPriorita* coda = creaCodaPriorita(50);

                printf(BOLD "> Fase 1: Lettura dal file 'test/data/richieste.txt'...\n" RESET);
                int richiesteCaricate = caricaRichiesteDaFile(archivio, coda, "test/data/richieste.txt");

                if (richiesteCaricate > 0) {
                    printf(GREEN "  [ OK ] Lette e processate %d richieste.\n\n" RESET, richiesteCaricate);
                    printf(BOLD "> Fase 2: Controllo integrita' delle Strutture Dati...\n" RESET);
                    
                    int dimArchivio = getDimensioneArchivio(archivio);
                    int dimCoda = getDimensioneCodaPriorita(coda);
                    
                    printf("  - Dimensione Archivio Storico (Lista) : %d\n", dimArchivio);
                    printf("  - Dimensione Coda di Attesa (Heap)    : %d\n\n", dimCoda);

                    if (dimArchivio == richiesteCaricate && dimCoda == richiesteCaricate) {
                        printf(GREEN "  [ OK ] Strutture popolate correttamente.\n\n" RESET);
                        
                        printf(BOLD "> Fase 3: Estrazione Heap e Scrittura File Temporaneo...\n" RESET);
                        FILE* out1 = fopen("test/oracoli/output_1.txt", "w");
                        if (out1 != NULL) {
                            while (!isCodaPrioritaVuota(coda)) {
                                Richiesta* r = estraiMaxDaCodaPriorita(coda);
                                fprintf(out1, "Codice: %s, Appartamento: %s, Tipologia: %s, Urgenza: %d\n",
                                        getCodiceRichiesta(r), getAppartamentoRichiesta(r),
                                        getTipologiaProblemaRichiesta(r), getLivelloUrgenzaRichiesta(r));
                            }
                            fclose(out1);
                            printf(GREEN "  [ OK ] Dati estratti e ordinati per Urgenza.\n\n" RESET);

                            int ispeziona = 0;
                            printf(YELLOW ">> Vuoi ispezionare visivamente i dati appena ordinati? (1=Si, 0=No): " RESET);
                            if (scanf("%d", &ispeziona) == 1 && ispeziona == 1) {
                                pulisciBuffer();
                                printf(CYAN "\n--- CONTENUTO GENERATO (Dal piu' urgente al meno urgente) ---\n" RESET);
                                FILE* vis = fopen("test/oracoli/output_1.txt", "r");
                                char buffer[512];
                                while(fgets(buffer, sizeof(buffer), vis)) {
                                    printf("  %s", buffer);
                                }
                                fclose(vis);
                                printf(CYAN "-------------------------------------------------------------\n\n" RESET);
                            } else {
                                pulisciBuffer();
                                printf("\n");
                            }

                            printf(BOLD "> Fase 4: Confronto con file Oracolo...\n" RESET);
                            int match = confrontaFileOracolo("test/oracoli/output_1.txt", "test/oracoli/oracolo_test_1.txt");
                            
                            printf(CYAN BOLD "---------------------------------------------------------------------\n");
                            if (match == 1) {
                                printf(GREEN BOLD "  [ SUCCESS ] Logica Heap perfetta! Combacia con l'oracolo.\n");
                            } else if (match == 0) {
                                printf(RED BOLD "  [ FAILURE ] L'output generato differisce dall'oracolo.\n");
                            } else {
                                printf(YELLOW BOLD "  [ ERROR ] File oracolo non trovato in test/oracoli/\n");
                            }
                            printf(CYAN BOLD "---------------------------------------------------------------------\n\n" RESET);
                            
                            remove("test/oracoli/output_1.txt");
                        }
                    } else {
                        printf(RED BOLD "  [ FAILURE ] Disallineamento nelle strutture dati!\n" RESET);
                    }
                } else {
                    printf(RED BOLD "  [ ERRORE CRITICO ] Impossibile leggere file dati.\n" RESET);
                }

                distruggiArchivioRichieste(archivio);
                distruggiCodaPriorita(coda);
                pausaSchermo();
                break;
            }

            case 2: {
                pulisciSchermo();
                printf(MAGENTA BOLD "\n[ TEST 2 ] " RESET "Verifica Registrazione Tecnici (Oracolo)\n");
                printf(MAGENTA "=====================================================================\n" RESET);
                printf(CYAN " OBIETTIVO DEL TEST:\n" RESET);
                printf(" 1. Carica la lista dei tecnici dal file 'tecnici.txt'.\n");
                printf(" 2. Li posiziona nell'Albero Binario di Ricerca (BST).\n");
                printf(" 3. Genera una vista testuale navigando l'Albero in modo ordinato.\n");
                printf(" 4. Confronta l'albero generato con l'Oracolo atteso.\n");
                printf(MAGENTA "=====================================================================\n\n" RESET);

                AlberoTecnici* db = creaAlberoTecnici();

                printf(BOLD "> Fase 1: Lettura dal file 'test/data/tecnici.txt'...\n" RESET);
                int tecniciCaricati = caricaTecniciDaFile(db, "test/data/tecnici.txt");

                if (tecniciCaricati > 0) {
                    printf(GREEN "  [ OK ] Letti e registrati %d tecnici nel sistema.\n\n" RESET, tecniciCaricati);
                    printf(BOLD "> Fase 2: Controllo integrita' del Database (BST)...\n" RESET);
                    
                    if (getRadiceAlberoTecnici(db) != NULL) {
                        printf(GREEN "  [ OK ] Albero Binario popolato con successo.\n\n" RESET);
                        
                        printf(BOLD "> Fase 3: Visita In-Order BST e Scrittura File Temporaneo...\n" RESET);
                        fileTempOracolo = fopen("test/oracoli/output_2.txt", "w");
                        if (fileTempOracolo != NULL) {
                            visitaAlberoTecnici(db, stampaTecnicoSuFile);
                            fclose(fileTempOracolo);
                            fileTempOracolo = NULL;
                            printf(GREEN "  [ OK ] Nodi dell'albero estratti e convertiti in testo.\n\n" RESET);

                            int ispeziona = 0;
                            printf(YELLOW ">> Vuoi ispezionare visivamente i dati inseriti nel BST? (1=Si, 0=No): " RESET);
                            if (scanf("%d", &ispeziona) == 1 && ispeziona == 1) {
                                pulisciBuffer();
                                printf(CYAN "\n--- CONTENUTO GENERATO (Struttura Albero Tecnici) ---\n" RESET);
                                FILE* vis = fopen("test/oracoli/output_2.txt", "r");
                                char buffer[512];
                                while(fgets(buffer, sizeof(buffer), vis)) {
                                    printf("  %s", buffer);
                                }
                                fclose(vis);
                                printf(CYAN "-----------------------------------------------------\n\n" RESET);
                            } else {
                                pulisciBuffer();
                                printf("\n");
                            }

                            printf(BOLD "> Fase 4: Confronto con file Oracolo...\n" RESET);
                            int match = confrontaFileOracolo("test/oracoli/output_2.txt", "test/oracoli/oracolo_test_2.txt");
                            
                            printf(CYAN BOLD "---------------------------------------------------------------------\n");
                            if (match == 1) {
                                printf(GREEN BOLD "  [ SUCCESS ] Struttura BST perfetta! Combacia con l'oracolo.\n");
                            } else if (match == 0) {
                                printf(RED BOLD "  [ FAILURE ] L'output generato differisce dall'oracolo.\n");
                            } else {
                                printf(YELLOW BOLD "  [ ERROR ] File oracolo non trovato in test/oracoli/\n");
                            }
                            printf(CYAN BOLD "---------------------------------------------------------------------\n\n" RESET);
                            
                            remove("test/oracoli/output_2.txt");
                        } 
                    } else {
                        printf(RED BOLD "  [ FAILURE ] Errore nella creazione dei nodi dell'Albero!\n" RESET);
                    }
                } else {
                    printf(RED BOLD "  [ ERRORE CRITICO ] Impossibile leggere file dati.\n" RESET);
                }

                distruggiAlberoTecnici(db);
                pausaSchermo();
                break;
            }

            case 3: case 4: case 5: case 6: case 7: case 8: {
                printf(YELLOW "\n[ IN CANTIERE ] Questo test sara' implementato a breve.\n" RESET);
                pausaSchermo();
                break;
            }

            case 0: {
                pulisciSchermo();
                break;
            }

            default: {
                printf(RED BOLD "\n[ ERRORE ]" RESET RED " Opzione non valida.\n" RESET);
                pausaSchermo();
                break;
            }
        }
    }

    return 0;
}