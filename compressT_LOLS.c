#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>

  // Let us create a global variable to change it in threads
  int parts ;
  int child_No = -1 ; 
  pthread_mutex_t lock;
 
  // The function to be executed by all threads

#define MAX_RLEN 50

//Function to compress using LOLS algorithm 
char *encode(char *str)
{
  int l;
  char count[MAX_RLEN];
  int len = strlen(str);
  char *final = (char *)malloc(sizeof(char)*(len*2 + 1));
  int i, k;
  int j = 0;

  // traverse the input string one character at a time 
  for(i = 0; i < len; i++)  {
      l = 1;
      while(i + 1 < len && str[i] == str[i+1]){
          l++;
          i++;
        }

      //Storing the length in an array
      sprintf(count, "%d", l);

      //storing the character in the final string
      if(l>2){
        for(k = 0; *(count+k); k++){
            final[j] = count[k];
            j++;
          }
      }
      if(l==2){
        final[j++] = str[i-1];
        final[j++]= str[i];
      }
      else
        final[j++] = str[i];
    }
  final[j] = '\0';
  return final;
}

// Function that encodes each string for each individual thread
void *myThread(void *ptr)
  {
    pthread_mutex_lock(&lock); //creating a mutex lock
    child_No++;
    int n;


    FILE* f;                                                                                                                                
    int l = 0;
    n = child_No;
    int boolean;
    
    f = fopen(ptr, "r"); //opening the input file for each thread

    //Error check for an empty file
    if(f == NULL){
      printf("Error: File could not be read \n");
      exit(0);
    }
    
    //getting total number of   characters in the file                                                                                                                                                                          
    char ch;
    int c = 0;
    
      while(1){
      boolean = fgetc(f);
      ch=(char)boolean;
      if(boolean == EOF){
	 break;
      }
      if(isalpha(ch)){  
	c++;
      }
      l++;
    }
    l--;    

    //checking if file contains alpha charachters 
    if(c==0){
      printf("Error: All characters are non alpha characters\n");
      exit(0);
    }

     if(parts>l){
      printf("Error: Number of parts greater than total characters in the file \n");
      exit(0);
    }
                                                                                                                                                                       
    //getting a substring to encode for each thread 
    int increment = (l/parts);
    int mod = l%parts;
    
    if(mod != 0){ //checking if the number of parts is an odd number
      if(child_No<mod){
	increment = increment+1;
      }
    }

    int sv = 0+(n*increment); //getting the starting value for a substring

    if(mod != 0){
      if(child_No >= mod){
        sv = sv + n ;
      }
    }

    char *before_check = malloc(increment + 1);
    fseek(f, sv, SEEK_SET);
    int len = fread(before_check, 1, increment, f);
    *(before_check+len) = '\0';

    int lString = strlen(before_check);
    int i;
    int count = 0;
    
    //getting the total number of alpha characters
    for(i=0; i<lString;i++){
      if(isalpha(before_check[i])){
	count++;
      }
    }

    int k = 0;
    char *newStr = malloc(count+1);
    for(i=0; i<lString;i++){
      if(isalpha(before_check[i])){
	newStr[k] = before_check[i];
	k++;
      }
      newStr[count] = '\0';
    }

    char *encoded = malloc(count+1);
    
    //to check if the string for this thread has any alpha charachters 
    if(strcmp(newStr,"")==0){
      printf("No alpha characters found in thread %d \n", child_No);
    }
    else{
   encoded = encode(newStr);
   printf("Encoded str: %s\n", encoded);                                                                                                                                                                        

    //freeing memory 
    free(newStr);
    free(before_check);
    fclose(f); 
    
    //creating an output file for the encoded string
    int inFileLen = strlen(ptr);
                                                                                                                                                                      
    char * outputFile = malloc(inFileLen+20); //making space to accomoadte original file name+_LOLS and null terminator                                          
                                                    
    strcpy(outputFile, ptr);  //copying original input file name                                                                                                                                                 
    int j;
                                                                                                                                                                 
    for (j=0; j< inFileLen; j++){
      if(outputFile[j] == '.'){   //replacing the "." with a "_"                                                                                                                                                     
	outputFile[j] = '_';}
    }
                                                                                                                                                                              
    char * _LOLSaddon = "_LOLS";   //creating addon with _LOLS                                                                                                                                                       
    strcat(outputFile, _LOLSaddon);
    if(parts >1){
      char *childNumber = malloc(4);
                                                                                                                                                                                     
      sprintf(childNumber,"%d",child_No);
      strcat(outputFile, childNumber);
    }
    printf("final outFile name: %s\n", outputFile);

    //writing the string into the output file
    FILE *filePtr = fopen(outputFile, "w" );
    if(filePtr != NULL){                                                                                                                                                                               
      fputs(encoded, filePtr);
      fclose(filePtr);
      }
free(encoded); 
    }

    pthread_mutex_unlock(&lock);//end of the mutex lock for the thread 
    return NULL;
  }
 
int main(int argc, char ** argv){
   parts = atoi(argv[2]); //getting the total number of parts

   if(parts == 0){
      printf("Error: Invalid input for number of parts \n");
      exit(0);
    }
int partsLen = strlen(argv[2]);

int m;
char partCh; 

for (m=0; m< partsLen; m++){
partCh = argv[2][m]; 

if(partCh == '0' || partCh == '1' || partCh == '2' || partCh == '3' || partCh == '4' || partCh == '5' || partCh == '6' || partCh == '7' || partCh == '8' || partCh == '9'){}
else{

printf("Error: Invalid input for number of parts\n");
exit(0); 
}

//printf("argv[2][m] = %d\n", partCh); 

}
    pthread_t tid[parts];
    int i;

    
    if (pthread_mutex_init(&lock, NULL) != 0)
      {
        printf("\n mutex initialization failed\n");
        return 1;
      }
   
    //creating all the threads using a for loop
    for (i = 0; i< parts;i++){  
       pthread_create(&tid[i], NULL, myThread,argv[1]); 
    }

    int k;    
    for (k = 0; k< parts;k++){
      pthread_join(tid[k], NULL);
                                                                                                                                                                                               
    }
                                                                                                                                                                                                                                                      
    pthread_mutex_destroy(&lock);
    pthread_exit(NULL);
    return 0; 
  }








