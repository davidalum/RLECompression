#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>

int sum = 0;
int main(int argc, char **argv){
  FILE * f;
  f = fopen(argv[1], "r");  
  if(f == NULL){
    printf("Error: File could not be read\n");
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
}
  int numberOfChildren = atoi(argv[2]);

  pid_t *childPids = NULL;
  pid_t p;                                                                                                                                                                                            
  char *childNum= malloc(16);

  int l=0;  //variable for size of the file 
  int boolean;
  char ch;
  int count =0;  //number of alpha characters 
  while(1){      //Checking the file for a count on alpha characters and length of the file 
    boolean = fgetc(f);
    ch = (char)boolean;
    if(boolean == EOF){
      break;
  }
    if(isalpha(ch)){
      count++;
    }
    ++l;
  }
  fclose(f); 
  l--;   //done to account for the newline that is automatically inserted at end of a file 
  
 if(count==0){
    printf("Error: All characters are non alpha characters\n");
    exit(0);
  }

  if(numberOfChildren>l){
    printf("Error: Number of parts greater than total number of characters \n");
    exit(0);
  }
  
  if(numberOfChildren == 0){
    printf("Error: Invalid input for number of parts\n");
    exit(0);
  }

  // Allocate memory for the array of child PIDs in order to wait on them properly.  
  childPids = malloc((numberOfChildren * sizeof(pid_t))+1);
 
 /* Start up children */
  for (int ii = 0; ii < numberOfChildren; ++ii) {
    if ((p = fork()) == 0) {      //inside the child process                                                                                                                                                      
      sprintf(childNum, "%d", ii);
      char *args[] = {"./compressR_worker_LOLS",argv[1],argv[2], childNum,NULL};
      execvp("./compressR_worker_LOLS" , args);   //
      exit(0);
    }
    else {   //the parent process                                                                                                                                                                              
      childPids[ii] = p;
    }
  }

  /* Wait for children to exit */
  int waiting;
  do {
    waiting = 0;
    for (int ii = 0; ii < numberOfChildren; ++ii) { 
      if (childPids[ii] > 0) {
	if (waitpid(childPids[ii], NULL, WNOHANG) != 0) {
	  // Child is done 
	  childPids[ii] = 0;
	}
	else {
	  // Still waiting on this child 
	  waiting = 1;
	}
      }
      sleep(0);
    }
  } while (waiting);

  /* Cleanup */
  free(childPids);

}

