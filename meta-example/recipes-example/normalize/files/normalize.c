#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>


//norma vettore
//radice quadrata(sommatoria(vec[i]*vec[i]))


int read_value() {
  int value;
  char input[10];
  int read;
  read = scanf("%s", input);
  if(read != 1) {
    printf("Errore in scanf!\n");
    return -1;
  }  

  int check = -1;
  int i = 0;
  while(input[i] != '\0') {
    check = isdigit(input[i]);
    if(!check)
      return -2;;
    ++i;
  }

   
  value = atoi(input);
  if(value <= 0 || value % 1 != 0) {
    return -3;
  }     

  return value;
}




int main(void) {
  int *vec;
  double norma = 0.0;

  int dim;  
  do {
    printf("Inserire dimensione vettore (intero senza segno diverso da zero): ");
    dim = read_value();

  } while(dim <= 0);


  //popolazione vettore
  vec = malloc(sizeof(int) * dim);
  int value = 0;
  for(int i = 0; i < dim; ++i) {
    value = 0;
    
    do {    
      printf("Inserire valore elemento %d vettore: ", i);
      value = read_value();
    } while(value <= 0);

    vec[i] = value;
  }


  //somma
  int sum = 0;
  for(int i = 0; i < dim; ++i) {
    sum = sum + (vec[i] * vec[i]);   
  }


  //calcolo norma
  norma = sqrt(sum);
  printf("Valore norma: %f\n", norma);


  //normalizzazione: 
  double norma_den = 1 / norma;
  double *vec_nor = malloc(sizeof(double) * dim); 
  printf("Vettore normalizzato: [");
  for(int i = 0; i < dim; ++i) {
    vec_nor[i] = norma_den * vec[i];

    if(i == dim - 1) 
      printf("%f]\n", vec_nor[i]);
    else
      printf("%f   ", vec_nor[i]);
  }


  free(vec);
  free(vec_nor);
  return 0;
}
