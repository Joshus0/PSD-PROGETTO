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

/* Funzione per pulire il buffer (Solo ciclo while) */
void pulisciBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* Funzione per mantenere il terminale pulito cross-platform */
void pulisciSchermo() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

/* Funzione helper per mettere in pausa lo schermo */
void pausaSchermo() {
    printf(BOLD "\nPremi INVIO per tornare al menu..." RESET);
    getchar();
}

static void acquisisciStringa(const char* prompt, char* buffer, int dim) {
    printf(BOLD "%s" RESET, prompt);
    
    if (fgets(buffer, dim, stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
    } else {
        buffer[0] = '\0';
    }
}

static const char* statoRichiestaToString(StatoRichiesta stato) {
    switch (stato) {
        case APERTA: 
            return "APERTA";
        case PIANIFICATA: 
            return "PIANIFICATA";
        case IN_LAVORAZIONE: 
            return "IN_LAVORAZIONE";
        case CONCLUSA: 
            return "CONCLUSA";
        case ANNULLATA: 
            return "ANNULLATA";
        default: 
            return "SCONOSCIUTO";
    }
}

static void stampaTecnico(Tecnico* tecnico) {
    if (tecnico == NULL) {
        return;
    }
    
    printf("Codice: %s | Nome: %s | Specializzazione: %s | Stato: %s\n",
           getCodiceTecnico(tecnico),
           getNomeTecnico(tecnico),
           getSpecializzazioneTecnico(tecnico),
           isDisponibileTecnico(tecnico) ? "Disponibile" : "Occupato");
}

static void stampaRichiesta(const Richiesta* richiesta) {
    if (richiesta == NULL) {
        return;
    }
    
    printf("Codice: %s | Appartamento: %s | Tipologia: %s | Urgenza: %d | Stato: %s",
           getCodiceRichiesta(richiesta),
           getAppartamentoRichiesta(richiesta),
           getTipologiaProblemaRichiesta(richiesta),
           getLivelloUrgenzaRichiesta(richiesta),
           statoRichiestaToString(getStatoRichiesta(richiesta)));
           
    if (getCodiceTecnicoAssegnatoRichiesta(richiesta) != NULL) {
        printf(" | Tecnico: %s", getCodiceTecnicoAssegnatoRichiesta(richiesta));
    }
    
    printf("\n");
}

static Tecnico* trovaTecnicoDisponibilePerSpecializzazioneRic(const NodoAlberoTecnici* nodo, const char* specializzazione) {
    if (nodo == NULL) {
        return NULL;
    }

    Tecnico* trovato = NULL;
    NodoAlberoTecnici* sinistro = getFiglioSinistroTecnici(nodo);
    
    if (sinistro != NULL) {
        trovato = trovaTecnicoDisponibilePerSpecializzazioneRic(sinistro, specializzazione);
        if (trovato != NULL) {
            return trovato;
        }
    }

    Tecnico* tecnico = getTecnicoDalNodoAlbero(nodo);
    
    if (tecnico != NULL && isDisponibileTecnico(tecnico) && strcmp(getSpecializzazioneTecnico(tecnico), specializzazione) == 0) {
        return tecnico;
    }

    NodoAlberoTecnici* destro = getFiglioDestroTecnici(nodo);
    
    if (destro != NULL) {
        trovato = trovaTecnicoDisponibilePerSpecializzazioneRic(destro, specializzazione);
        if (trovato != NULL) {
            return trovato;
        }
    }

    return NULL;
}

static Tecnico* trovaTecnicoDisponibilePerSpecializzazione(const AlberoTecnici* albero, const char* specializzazione) {
    if (albero == NULL || specializzazione == NULL) {
        return NULL;
    }
    
    return trovaTecnicoDisponibilePerSpecializzazioneRic(getRadiceAlberoTecnici(albero), specializzazione);
}

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
                pulisciSchermo();
                printf(CYAN BOLD "\n-- CODA DI ATTESA (PRIORITA' MAX) --\n" RESET);
                stampaCodaPriorita(codaAttesa);
                
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