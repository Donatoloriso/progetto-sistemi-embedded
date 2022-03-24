#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>


//variabili globali
int camere;			// numero di camere totali presenti nell'albergo
int N_camere[4];		// array che indica il numero di stanze di ciascuna tipologia: 
        			// N_camere[0] = camere con 1 letto
			  	// N_camere[1] = camere con 2 letti
				// N_camere[2] = camere con 3 letti
				// N_camere[3] = camere con 4 letti 


//mutex che funge da monitor
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

//variabili condition
pthread_cond_t coda_gruppi = PTHREAD_COND_INITIALIZER;  	
pthread_cond_t coda_singoli = PTHREAD_COND_INITIALIZER;

//Variabili usate come "code" delle variabili condition
int gruppi_sospesi = 0;			// contatore dei gruppi sospesi
int singoli_sospesi = 0;		// contatore dei singoli sospesi




// procedure dei gruppi
void prenotaGruppo(int id, int *stanze, int componenti) {
  //acquisizione
  pthread_mutex_lock(&mtx);

  int i;
  int servono[4];			//array che indica il numero di stanze di ogni tipologia necessarie al gruppo
  bool trovate_stanze = false;
  int temp = 0;
  
  // inizializzazione array
  for(i = 0; i < 4; i++) {
    servono[i] = 0;
  }
      
  //Calcolo delle stanze necessarie: si cerca di minimizzarne il numero
  servono[3] = componenti / 4;
  if((componenti / 4) > 0) {
    temp = componenti % 4;
  }
  else {
    temp = componenti;
  }

  if (temp > 0) 
    servono[temp-1]++;

  printf("[GRUPPO %d] Servono %d stanze da 1 posto, %d stanze da 2 posti, %d stanze da 3 posti, %d stanze da 4 posti\n", id, servono[0], servono[1], servono[2], servono[3]);
       
  // Finché non si trovano le stanze necessarie
  while (!trovate_stanze) {
    printf("[GRUPPO %d] Devo prenotare le stanze\n", id);
            	  
    if(N_camere[0] < servono[0] || N_camere[1] < servono[1] || N_camere[2] < servono[2] || N_camere[3] < servono[3]) {  
      // mi sospendo solo se non ci sono abbastanza stanze libere
      printf("[GRUPPO %d] Non ci sono abbastanza stanze libere, mi devo sospendere...\n", id);
      gruppi_sospesi++;
      pthread_cond_wait(&coda_gruppi, &mtx);
      gruppi_sospesi--;
    }
    else {
      trovate_stanze = true;
    } 
                 
  }


  // Occupo le risorse
  for(i = 0; i < 4; i++) {
    N_camere[i] -= servono[i];
    stanze[i] = servono[i];
  }
               

  //rilascio
  pthread_mutex_unlock(&mtx);  
}



void rilasciaGruppo(int id, int *stanze) {
  pthread_mutex_lock(&mtx);

  int i;
  int ss = singoli_sospesi;
  int s = gruppi_sospesi;
    
  printf("[GRUPPO %d] Soggiorno terminato, se ne va!\n",id);
  for(i = 0; i < 4; i++) {
    N_camere[i] += stanze[i];
  }
        
  // risveglio i processi sospesi rispettando la priorità
  for(i = 0; i < ss; i++) {
    pthread_cond_signal(&coda_singoli);
  }

  for(i = 0; i < s; i++) {
    pthread_cond_signal(&coda_gruppi);
  }
  
  pthread_mutex_unlock(&mtx);  

}



// procedure clienti singoli
void prenotaSingola(int id) {
  pthread_mutex_lock(&mtx);

  printf("PERSONA %d deve prenotare una stanza singola\n", id);
  while(N_camere[0] == 0) {
    printf("[PERSONA %d] Non ci sono stanze disponibili, mi sospendo...\n", id);
    singoli_sospesi++;
    pthread_cond_wait(&coda_singoli, &mtx);
    singoli_sospesi--;
  }
  
  // Persona singola ha trovato una stanza singola, procede ad occuparla
  N_camere[0]--;
  printf("[PERSONA %d] Ho prenotato una stanza!\n", id);

  pthread_mutex_unlock(&mtx);
  
}




void rilasciaSingola(int id) {
  int s = gruppi_sospesi;
  int i;
  
  pthread_mutex_lock(&mtx);

  printf("[PERSONA %d] Lascio la stanza...\n", id);
  N_camere[0]++;
   	
  //Risveglio rispettando la priorità: prima singoli, poi gruppi
  if(singoli_sospesi > 0)
    pthread_cond_signal(&coda_singoli);
  else {
    for(i = 0; i < s; i++)
      pthread_cond_signal(&coda_gruppi);
  }	

  pthread_mutex_unlock(&mtx);

}





void *eseguiGruppo(void *id) {
  int *pi = (int *)id; 
  int i;
  int componenti; 
  int stanze[4];
  
  // all'inizio per questo gruppo occupo zero camere di ogni tipologia
  for(i = 0; i < 4; ++i) {
    stanze[i] = 0;
  }
  
  //Si genera un numero random di componenti, compresi tra 2 e N camere:
  //random: rand() % (upper - lower + 1) + lower; upper=camere, lower=2
  componenti = (rand() % (camere - 2 + 1)) + 2;
  printf("[GRUPPO %d] Numero componenti: %d\n", *pi, componenti);
  
  //prenotazione delle stanze
  prenotaGruppo(*pi, stanze, componenti);
  
  // alloggio simulato con sleep 
  sleep(2);

  // rilascio delle stanze
  rilasciaGruppo(*pi, stanze);
  
  //TERMINE
  printf("GRUPPO %d ha finito, termina\n", *pi);
  
  return *pi;
}




void *eseguiPersona(void *id) {
  int *pi = (int *)id;   

  // prenotazione di una stanza singola
  prenotaSingola(*pi);
  
  // alloggio in albergo simulato con sleep
  printf("Persona %d alloggia in albergo...\n", *pi);
  sleep(2);
  
  // rilascio della stanza
  rilasciaSingola(*pi);
  
  
  //termine
  printf("PERSONA %d ha finito, termina\n", *pi);
  
  return *pi;
}





	
	
int main (int argc, char **argv) {
  pthread_t *thread;
  int *taskids;
  int i;
  int *p; 
  int NUM_THREADS; 
  
  int gruppi;
  int singoli;
  	
 /* Controlli sui parametri */
  if (argc != 4) {
    printf("Errore nel numero dei parametri (%d, richiesti: 3)\n(Numero di stanze, numero di gruppi, numero di persone singole)\n", argc-1);
    exit(1);
  }
	
  camere = atoi(argv[1]);
  if (camere <= 0 || ((camere % 4) != 0)) {
    printf("Numero di camere deve essere MAGGIORE DI ZERO e DIVISIBILE PER QUATTRO!\n");
    exit(2);
  }  
 
 
  gruppi = atoi(argv[2]);
  if (gruppi <= 0) {
    printf("Numero di gruppi deve essere MAGGIORE DI ZERO!\n");
    exit(3);
  }  
  
  
  singoli = atoi(argv[3]);
  if (singoli <= 0) {
    printf("Numero di singoli deve essere MAGGIORE DI ZERO!\n");
    exit(4);
  }  
  
  
  //inizializzazione altre variabili
  for(i = 0; i < 4; ++i) {
    // all'inizio ho il numero massimo di camere disponibili per ogni tipologia
    N_camere[i] = camere / 4;
  }
  
  // numero di thread totale: numero di clienti singoli + numero di gruppi
  NUM_THREADS = gruppi + singoli;
  
	
  thread=(pthread_t *) malloc(NUM_THREADS * sizeof(pthread_t));
  if (thread == NULL) {
    printf("Problemi con l'allocazione dell'array thread\n");
    exit(5);
  }
  taskids = (int *) malloc(NUM_THREADS * sizeof(int));
  if (taskids == NULL) {
    printf("Problemi con l'allocazione dell'array taskids\n");
    exit(6);
  }    
	
	
	
  //inizializzazione seed random
  srand(time(0));
  

  //CREAZIONE PROCESSI:      
  //gruppi
  for(i = 0; i < gruppi; i++) {
    taskids[i] = i;
    printf("Sto per creare il thread 'GRUPPO' %d\n", taskids[i]);
    if (pthread_create(&thread[i], NULL, eseguiGruppo, (void *) (&taskids[i])) != 0)
      printf("SONO IL MAIN E CI SONO STATI PROBLEMI DELLA CREAZIONE DEL thread 'GRUPPO' %d\n", taskids[i]);
    printf("SONO IL MAIN e ho creato il Pthread 'GRUPPO' %i con id=%lu\n", i, thread[i]);
  }

  //singoli
  for(; i < NUM_THREADS; i++) {
    taskids[i] = i;
    printf("Sto per creare il thread 'PERSONA' %d\n", taskids[i]);
    if (pthread_create(&thread[i], NULL, eseguiPersona, (void *) (&taskids[i])) != 0)
      printf("SONO IL MAIN E CI SONO STATI PROBLEMI DELLA CREAZIONE DEL thread 'PERSONA' %d\n", taskids[i]);
    printf("SONO IL MAIN e ho creato il Pthread 'PERSONA' %i con id=%lu\n", i, thread[i]);
  }  	
	
	
  //FINE  
  sleep(25);
	
	
  exit(0);
}
