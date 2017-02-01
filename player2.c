#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <errno.h>

int ships[5] = {0, 0, 0, 0, 0};
int oppships[5] = {0, 0, 0, 0, 0};
char myboard[5][5];
char oppboard[5][5]; 
int boardboard[5][5] = {{11,12,13,14,15},
                       {21,22,23,24,25},
                       {31,32,33,34,35},
                       {41,42,43,44,45},
                       {51,52,53,54,55}};

int client_handshake(int *from_server){
  int to_server;
  char buffer[100];

  //server does it's end of the handshake and waits for client
  //client makes a "private" (pid) named pipe
  sprintf(buffer, "%d", getpid());
  mkfifo(buffer, 0644);
  //client connects to server and sends pipe name
  to_server = open("popeye", O_WRONLY);
  //printf("went into popeye\n");
  write(to_server, buffer, sizeof(buffer));
  //client recieves client's message and removes wkp
  //server connects to client's pipe and send message
  *from_server = open(buffer, O_RDONLY);
  remove(buffer);
  //client recieves server's message, removes private pipe
  read(*from_server, buffer, sizeof(buffer));

  printf("Captian %d, Enemy Message Incoming: %s\n", getpid(),buffer);

  return to_server;
}

static void sighandler(int signo){
  if(signo == SIGINT){
    printf("\nYou're done with %d\n", getpid());
    exit(0);
  }
}
int isInShips(int pos){
  int okay = 1;
  if (pos == ships[0] ||
      pos == ships[1] ||
      pos == ships[2] ||
      pos == ships[3] ||
      pos == ships[4]){
    okay = 0;
  }
  return okay;
}

int isInRange(int pos){
  if ( !( ((pos > 10)&&(pos < 16)) ||
          ((pos > 20)&&(pos < 26)) ||
          ((pos > 30)&&(pos < 36)) ||
          ((pos > 40)&&(pos < 46)) ||
          ((pos > 50)&&(pos < 56))  )){
    return 0;
  }

  return 1;
}
/*======== int dontBreak()  ==========
check to see is 11-15, 21-25, 31-35, 41-45, 51-55
also cehck to see no double entries
Inputs: 
Result:
makes it harder for someone to break our program
if okay = 0, then there's a problem.
====================*/
int dontBreak(int pos){
  if ( !( ((pos > 10)&&(pos < 16)) ||
	  ((pos > 20)&&(pos < 26)) ||
	  ((pos > 30)&&(pos < 36)) ||
	  ((pos > 40)&&(pos < 46)) ||
	  ((pos > 50)&&(pos < 56))  )){
    return 0;
  }

  //check to see no double entries
  int okay = isInShips(pos);

  return okay;
}

int isHit(int pos){
  int hit = 0;

  if (pos == ships[0] ||
      pos == ships[1] ||
      pos == ships[2] ||
      pos == ships[3] ||
      pos == ships[4]){
    hit = 1;
  }

  if (hit){
    int i;
    for (i = 0; i < 5 ; i++)
      if (ships[i] == pos)
	ships[i] = 0;
  }

  return hit;
}

int isAllHit(){
  int allhit = 0;

  if (0 == ships[0] &&
      0 == ships[1] &&
      0 == ships[2] &&
      0 == ships[3] &&
      0 == ships[4]){
    allhit = 1;
  }

  return allhit;
}
/*======== int makeFleet() ==========                                                                                                                                               Inputs:
int place: user's inputted placement of a ship                                                                                                                                       
int places[]: array holding all ship places

Result:
fills up places[] with user's inputs (sets coordinates of ships
====================*/
void makeFleet(){
  int pos;
  int i;
  int y;
  int z;
  printf("Ahoy, captain! Welcome to Battleship Bash Bros.!\n\n");
  printf("BBB is a condensed, digital form of the childhood favorite boardgame, Battleship\n");
  printf("On a 5x5 Grid, you and your opponent will be asked to pick coordinates that will hold your ship\n");
  printf("Turn by turn, you'll be asked to guess where eachother's ships are, getting a 'Ship Hit!' or 'Ship Missed!' Message\n");
  printf("When a user hits all of the opponent's ships, they are declared the winner and the game ends!\n");
  printf("Before we connect with an opponent, we'll set up your board\n");
  printf("Here are how boards are set up (Row and Column #s Starts at 1 and End at 5, and coordinates are written as one, double-digit # e.g 11 instead of 1,1)\n");
  for (y = 0; y < 5; y ++){
    for (z = 0; z < 5; z++){
      printf(" %d ", boardboard[y][z]);
    }
    printf("\n");
  }
  printf("\n");
  
  for(i = 0; i < 5;i++){
    printf("Input a ship location on the grid (Note: Grid is 5x5 and Input as coordinate as one number e.g 11 instead of 1,1): ");
    scanf("%d", &pos);
    if (dontBreak(pos) == 1)
      ships[i] = pos;
    else{
      while (dontBreak(pos) != 1){
	printf("Please enter a valid location: ");
	scanf("%d", &pos);
      }
      ships[i] = pos;
    }
  }
  int a;
  int b;
  for (a = 0; a < 5; a ++){ 
    for (b = 0; b < 5; b++){
      myboard[a][b] = '-';
    }
  }
  int c;
  for (c = 0; c < 5; c++){
    int temp = ships[c];
    if (temp != 0){
      int row = temp/10;
      int col = temp%10;
      myboard[row - 1][col - 1] = 'S';
    }
  }
  int e;
  int f;
  for (e = 0; e<5; e++){
    for (f = 0; f<5; f++){
      oppboard[e][f] = '-';
    }
  }  
}

void printMyBoard(){
  int a;
  int b;
  printf("Your Board\n\n");
  for (a = 0; a < 5; a ++){
    for (b = 0; b < 5; b++){
      printf(" %c ", myboard[a][b]);
    }
    printf("\n");
  }
  printf("\n");
}

void printOppBoard(){
  int a;
  int b;
  printf("Your Opponent's Board\n\n");
  for (a = 0; a < 5; a ++){
    for (b = 0; b < 5; b++){
      printf(" %c ", oppboard[a][b]);
    }
    printf("\n");
  }
  printf("\n");
}


int main(){
  signal(SIGINT, sighandler);
  
  //Access Semaphore Created by Player1
  int semid;
  int ret;
  //printf("Trying to access the semaphore...\n");
  semid = semget(ftok("makefile", 47), 1, 0644);
  if(semid == -1){
    printf("There was a problem in accessing the semaphore\n");
    printf("Error %d: %s\n", errno, strerror(errno));
  }/* else{
    printf("Access granted!\n");
  }*/

  
  int to_server;
  int from_server;
  int readpos;
  int incoord;
  char result[100];
    

  makeFleet();
  printMyBoard();
  to_server = client_handshake(&from_server);

  while( read(from_server, result, sizeof(result) )){
    //Reads from Opponent Whether or Not Your Hit was successful                                                                                                                  
    printf("You Got Back from Opponent: %s\n", result);
    if (!strcmp(result,"All Ships Eliminated! We Surrender! You Win!\n")){
      oppboard[incoord/10 - 1][incoord%10 - 1] = 'H';
      break;
    }
    else if (!strcmp(result,"Ship Missed!\n")){
      if (!(oppboard[incoord/10 - 1][incoord%10 - 1] == 'H'))
	oppboard[incoord/10 - 1][incoord%10 - 1] = 'M';
    }
    else if (!strcmp(result,"Ship Hit!\n")){
      oppboard[incoord/10 - 1][incoord%10 - 1] = 'H';
    }
    //Attempts to Down Semaphore to Access Shared Memory
    struct sembuf new = {0, -1, SEM_UNDO};
    //printf("Trying to access the semaphore...\n");
    semid = semget(ftok("makefile", 47), 1, 0644);
    if(semid == -1){
      printf("There was a problem in accessing the semaphore\n");
      printf("Error %d: %s\n", errno, strerror(errno));
    } 
    else
      //printf("Access granted!\n");
    printf("Waiting for Opponent to Finish Turn...\n");
    ret = semop(semid, &new, 1);
    if(ret == -1){
      printf("There was a problem in downing the semaphore\n");
      printf("Error: %d: %s\n", errno, strerror(errno));
      return 0;
    } 
    else
      printf("Opponent has finished turn!\n");
    //Once Semaphore is Downed, Reads Shared Memory to See What Coordinate Opponnet Wrote                                                                                    
    int shmid = shmget(ftok("makefile", 13), sizeof(int), 0664);
    if(shmid == -1){
	printf("There was a problem getting the shared memory\n");
	printf("Error: %d: %s\n", errno, strerror(errno));
	return 1;
    }
    int *currentcoordinate = (int *) shmat(shmid,0,0);
    if(*currentcoordinate == -1){
      printf("There was a problem in attaching the shared memory to a variable\n");
      printf("Error %d: %s\n", errno, strerror(errno));
      return 1;
    }
    printf("Opponent has targetted coordinate: %d\n", *currentcoordinate);
    readpos = *currentcoordinate;
    if (isHit(readpos)){
      printf("Captain! Opponent Hit Your Ship at Coordinate: %d!\n\n", readpos);
      myboard[readpos/10 - 1][readpos%10 - 1] = 'H';
      if (isAllHit()){
	printf("Captain! Opponent has taken out all our ships! The war is lost!\n\n");
	strncpy(result,"All Ships Eliminated! We Surrender! You Win!\n",sizeof(result));
	write(to_server, result, sizeof(result) );
	break;
      }
      else{
	strncpy(result,"Ship Hit!\n" ,sizeof(result));
	write(to_server, result, sizeof(result) );
      }
    }
    else{
      if (!(myboard[readpos/10 - 1][readpos%10 - 1] == 'H'))
	myboard[readpos/10 - 1][readpos%10 - 1] = 'M';
      printf("Captain! Opponent was unsuccesful at hitting any of your ships!\n\n");
      strncpy(result,"Ship Missed!\n",sizeof(result));
      write(to_server, result, sizeof(result) );
    }
    strncpy(result,"",sizeof(result));
    printMyBoard();
    printOppBoard();
    
    //Player Gives Program a Ship Location                                                                                                      
    printf("Input a ship location on to hit your opponent's grid (Note input as two-digit e.g 11 instead of 1,1): ");
    scanf("%d", &incoord);
    if (isInRange(incoord) == 1){
      //Ship Location is Put Into Shared Memory if Valid the First Time
      *currentcoordinate = incoord;
    }
    else{
      while (isInRange(incoord) != 1){
	printf("Please enter a valid location: ");
	scanf("%d", &incoord);
      }
      //Ship Location is Put Into Shared Memory Once Valid   
      *currentcoordinate = incoord;
    }
    printf("\nCaptain! You have targetted possible opponent ship at coordinate: %d\n", *currentcoordinate);
    //Semaphore is Upped
    new.sem_op = 1;
    semid = semget(ftok("makefile", 47), 1, 0644);
    //printf("Trying to up the semaphore...\n");
    ret = semop(semid, &new, 1);
    if(ret == -1){
      printf("There was a problem in upping the semaphore\n");
      printf("Error %d: %s\n", errno, strerror(errno));
    }
    else{
      //printf("Success!\n");
    }
    //shared memory is detached
    shmdt(currentcoordinate);
  }
 
  printf("The Game has ended!\n");
  printf("Final Boards!\n");
  printMyBoard();
  printOppBoard();
  
  close(to_server);
  close(from_server);

  return 0;
}
