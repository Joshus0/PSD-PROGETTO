#ifndef TEST_H
#define TEST_H

/*
 * Funzione: testCase1_RegistrazioneRichieste
 * ------------------------------------------
 * Verifica che il caricamento da file inserisca le richieste nell'Archivio
 * e nella Coda di Priorita' (Max-Heap) rispettando l'ordinamento per urgenza.
 * Le richieste vengono estratte in ordine decrescente di urgenza, scritte
 * su file e confrontate con l'Oracolo noto.
 *
 * Parametri:
 * Nessuno.
 *
 * Pre-condizione:
 * I file "test/data/richieste.txt" e "test/oracoli/oracolo_test_1.txt"
 * devono essere presenti e leggibili.
 *
 * Post-condizione:
 * Il file temporaneo "output_1.txt" viene eliminato. Tutte le strutture
 * allocate vengono distrutte. La coda viene svuotata prima dell'archivio
 * perche' quest'ultimo e' l'unico owner delle Richiesta*.
 *
 * Ritorna:
 * Niente (void). L'esecuzione si blocca (assert) se l'ordine di estrazione
 * non corrisponde all'Oracolo.
 */
void testCase1_RegistrazioneRichieste(void);

/*
 * Funzione: testCase2_RegistrazioneTecnici
 * ----------------------------------------
 * Verifica che il caricamento da file inserisca i tecnici nel BST mantenendo
 * l'ordinamento lessicografico per codice. La visita in-order dell'albero
 * deve produrre i tecnici in ordine alfabetico, confermato dal confronto
 * con l'Oracolo.
 *
 * Parametri:
 * Nessuno.
 *
 * Pre-condizione:
 * I file "test/data/tecnici.txt" e "test/oracoli/oracolo_test_2.txt"
 * devono esistere ed essere leggibili.
 *
 * Post-condizione:
 * Il file temporaneo "output_2.txt" viene eliminato. L'albero viene
 * deallocato integralmente insieme ai Tecnico* che contiene.
 *
 * Ritorna:
 * Niente (void). L'esecuzione si blocca (assert) se l'ordinamento BST
 * non corrisponde all'Oracolo.
 */
void testCase2_RegistrazioneTecnici(void);

/*
 * Funzione: testCase3_AssegnazioneCorretta
 * ----------------------------------------
 * Verifica il flusso di assegnazione: la richiesta con urgenza massima viene
 * estratta dall'heap, il tecnico compatibile viene trovato nel BST e l'
 * assegnazione viene completata tramite setter. Lo stato post-assegnazione
 * viene confrontato con l'Oracolo.
 *
 * Parametri:
 * Nessuno.
 *
 * Pre-condizione:
 * I file di input (richieste.txt, tecnici.txt) e oracolo_test_3.txt
 * devono esistere. Il file richieste.txt deve contenere almeno una
 * richiesta con tipologia corrispondente a un tecnico disponibile.
 *
 * Post-condizione:
 * Lo stato della richiesta estratta e' PIANIFICATA. Il tecnico assegnato
 * ha disponibilita' impostata a 0. Tutte le strutture vengono deallocate
 * nell'ordine corretto: heap prima, poi archivio, poi albero.
 *
 * Ritorna:
 * Niente (void). L'esecuzione si blocca (assert) se l'assegnazione
 * non produce lo stato atteso o se l'Oracolo non combacia.
 */
void testCase3_AssegnazioneCorretta(void);
/*
 * Funzione: testCase4_PianificazioneConflitti
 * -------------------------------------------
 * Verifica che l'Agenda del Tecnico (BST) gestisca correttamente i conflitti
 * orari: il primo inserimento su una fascia libera deve riuscire, il secondo
 * tentativo sulla stessa fascia deve essere respinto senza modificare lo stato
 * della richiesta coinvolta.
 *
 * Parametri:
 * Nessuno.
 *
 * Pre-condizione:
 * I file "test/data/tecnici.txt" e "test/oracoli/oracolo_test_4.txt"
 * devono essere presenti. Il file tecnici.txt deve contenere T001.
 *
 * Post-condizione:
 * L'agenda di T001 contiene un solo appuntamento. Le richieste di test
 * vengono deallocate tramite arcTemp che ne e' l'owner.
 *
 * Ritorna:
 * Niente (void). L'esecuzione si blocca (assert) se il conflitto non viene
 * rilevato o se l'Oracolo non combacia.
 */
void testCase4_PianificazioneConflitti(void);

/*
 * Funzione: testCase5_AggiornamentoStato
 * --------------------------------------
 * Verifica le transizioni di stato di una richiesta lungo il ciclo di vita
 * previsto: APERTA -> IN_LAVORAZIONE -> CONCLUSA. Controlla anche che la
 * data di chiusura venga registrata correttamente.
 *
 * Parametri:
 * Nessuno.
 *
 * Pre-condizione:
 * I file "test/data/richieste.txt" e "test/oracoli/oracolo_test_5.txt"
 * devono essere presenti. Il file richieste.txt deve contenere R002
 * con stato iniziale APERTA.
 *
 * Post-condizione:
 * Lo stato di R002 e' CONCLUSA con data di chiusura impostata.
 * Il file temporaneo viene eliminato. La coda viene svuotata prima
 * della distruzione dell'archivio.
 *
 * Ritorna:
 * Niente (void). L'esecuzione si blocca (assert) se una transizione
 * non produce lo stato atteso o se l'Oracolo non combacia.
 */
void testCase5_AggiornamentoStato(void);
/*
 * Funzione: testCase6_RicercaFiltri
 * ---------------------------------
 * Verifica il motore di ricerca per codice dell'archivio. Testa sia il caso
 * positivo (codice presente) sia quello negativo (codice assente), garantendo
 * che la ricerca non alteri lo stato dell'archivio in nessuno dei due casi.
 *
 * Parametri:
 * Nessuno.
 *
 * Pre-condizione:
 * I file "test/data/richieste.txt" e "test/oracoli/oracolo_test_6.txt"
 * devono esistere. Il codice "R_INVAL" non deve essere presente nel
 * file di dati (garantito per costruzione).
 *
 * Post-condizione:
 * L'archivio non e' stato modificato. Il file temporaneo viene eliminato.
 * La coda viene svuotata prima della distruzione dell'archivio.
 *
 * Ritorna:
 * Niente (void). L'esecuzione si blocca (assert) se la ricerca restituisce
 * un risultato inatteso o se l'Oracolo non combacia.
 */
void testCase6_RicercaFiltri(void);

/*
 * Funzione: testCase7_StoricoInterventi
 * -------------------------------------
 * Testa la corretta generazione dello storico degli interventi.
 * Simula la chiusura di alcune pratiche, scorre l'Archivio tramite i getter
 * filtrando solo le richieste CONCLUSE e scrive il report su un file
 * temporaneo per il confronto con l'Oracolo.
 *
 * Parametri:
 * Nessuno.
 *
 * Pre-condizione:
 * I file "test/data/richieste.txt" e "test/oracoli/oracolo_test_7.txt"
 * devono essere disponibili e validi.
 *
 * Post-condizione:
 * Lo stato di alcune richieste viene alterato per la simulazione. La memoria
 * viene deallocata rigorosamente (heap svuotato prima dell'archivio).
 *
 * Ritorna:
 * Niente (void). Interrompe l'esecuzione (assert) se l'oracolo fallisce.
 */
void testCase7_StoricoInterventi(void);

/*
 * Funzione: testCase8_GenerazioneReport
 * -------------------------------------
 * Testa l'aggregazione dei dati statistici di sistema. Simula un ambiente
 * eterogeneo, itera l'archivio tramite i getter e calcola le metriche.
 * Il report generato viene scritto su un file temporaneo e confrontato
 * con l'Oracolo per validarne l'esattezza matematica.
 *
 * Parametri:
 * Nessuno.
 *
 * Pre-condizione:
 * I file "test/data/richieste.txt" e "test/oracoli/oracolo_test_8.txt"
 * devono esistere.
 *
 * Post-condizione:
 * Viene generato e poi rimosso il file "output_8.txt". Memoria allocata
 * liberata in ordine di ownership corretto senza memory leak.
 *
 * Ritorna:
 * Niente (void). L'esecuzione si interrompe in caso di errore.
 */
void testCase8_GenerazioneReport(void);
/*
 * Funzione: testCase9_MultiSlotGiornoStesso
 * ------------------------------------------
 * Verifica che un tecnico possa avere piu' interventi nello stesso giorno
 * allocati in fasce orarie differenti e non sovrapposte.
 * Testa il flusso completo di assegnazione e duplice pianificazione,
 * assicurando che l'agenda del tecnico registri entrambi gli interventi
 * e che eventuali conflitti successivi vengano correttamente respinti.
 *
 * Parametri:
 * Nessuno.
 *
 * Pre-condizione:
 * Il file "test/data/tecnici.txt" deve contenere il tecnico "T001".
 *
 * Post-condizione:
 * Entrambe le richieste risultano PIANIFICATA e presenti nell'agenda.
 * Tutte le strutture allocate (Albero, Archivio, Richieste) vengono 
 * deallocate correttamente garantendo la corretta ownership.
 *
 * Ritorna:
 * Niente (void). L'esecuzione si blocca (assert) se il flusso non 
 * produce lo stato atteso.
 */
void testCase9_MultiSlotGiornoStesso(void);

#endif