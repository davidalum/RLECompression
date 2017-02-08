#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h> 
#define MAX_RLEN 50

char *encode(char *src)   //Function that returns LOLS encoded string 
{
  int rLen;
  char count[MAX_RLEN];
  int len = strlen(src);

  char *dest = (char *)malloc(sizeof(char)*(len*2 + 1));

  int i, j = 0, k;

  
  for(i = 0; i < len; i++)
    {
      /* Count the number of occurrences of the new character */
      rLen = 1;
      while(i + 1 < len && src[i] == src[i+1])  //Check if two consecutive characters are the same
        {
          rLen++;
          i++;
        }
      // Store rLen in character array count 
      sprintf(count, "%d", rLen);

      if(rLen>2){
        for(k = 0; *(count+k); k++, j++)
          {
            dest[j] = count[k];
          }
      }
      if(rLen==2){   
	dest[j++] = src[i-1];
        dest[j++]= src[i];
      }
      else
        dest[j++] = src[i];
    }

  dest[j] = '\0';
  return dest;
}

int main( int argc, char** argv)
{
  int parts = atoi(argv[2]);
  int child_No = atoi(argv[3]);
  int n;
  
  FILE* f;      
  
  int l = 0;  
  n = child_No;
  int boolean; 
    f = fopen(argv[1], "r");
  
    //getting total number of characters
  while(1){
    boolean = fgetc(f);
    if(boolean == EOF)
      break;

    ++l;
  }  
  l--;   //done to account for the newline that is automatically inserted at end of a file 
 

  //gettting string for LOLS compression
  int increment = (l/parts);
  int mod = l%parts;
  if(mod != 0){
    if(child_No<mod){
      increment = increment+1;  
    }
  }
  int sv = 0+(n*increment);   //the starting value/index that this child should go to in the file. 
  if(mod != 0){
    if(child_No >= mod){
      sv = sv+n;
    }
  }
  char *str = malloc(increment + 1);  //Child is reading characters from file to be compressed 
  fseek(f, sv, SEEK_SET);
  int len = fread(str, 1, increment, f);
  *(str+len) = '\0';
    
  int lString = strlen(str);
  int i;
  int count = 0;
  for(i=0; i<lString;i++){  //checking for amount of alpha chars to malloc approporiate amount
    if(isalpha(str[i])){
      count++;
     }
  }
 
  if(count ==0){  
    printf("No alpha characters found in process # %d \n", child_No);
    exit(0);
  }

  int k = 0;
  char *newStr = malloc(count+1);
  for(i=0; i<lString;i++){   //creating the new string with just alpha characters which will be sent to the compression function 
    if(isalpha(str[i])){
      newStr[k] = str[i];   
      k++;
     }
  }
    newStr[count] = '\0';
  
  char *res = encode(newStr);   //sending the string with only alpha characters to be encoded 
  printf("Encoded str: %s\n", res);

  int inFileLen = strlen(argv[1]);
  char * outputFile = malloc(inFileLen+20); //making space to accomoadte original file name+_LOLS and null terminator 
  strcpy(outputFile, argv[1]);  //copying original input file name to the new output file name 
  
  int j;
    for (j=0; j< inFileLen; j++){
    if(outputFile[j] == '.'){   //replacing the "." with a "_" 
      outputFile[j] = '_';}
  }
  char * _LOLSaddon = "_LOLS";   //concatenating _LOLS to file name 
  strcat(outputFile, _LOLSaddon); 
  if(parts >1){
  char *childNumber = malloc(4); 
  strcpy(childNumber, argv[3]);
  // childNumber- "SOH"; 
  strcat(outputFile, childNumber);  //concatenating child number onto file name 
  }    
  printf("final outFile name: %s\n", outputFile); 

 FILE *filePtr = fopen(outputFile, "w" );
  if(filePtr != NULL){
    //fprintf(filePtr, res );
    fputs(res, filePtr);  //writing the encoded string to the file 
    fclose(filePtr); 
  }
  fclose(f);
  free (str);
  free(newStr);
  free(outputFile);
  return 0;
}

