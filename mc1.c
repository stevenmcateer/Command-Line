/* Steven McAteer & Ethan Schutzman
 * smcateer & ehschutzman
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>


void addcmd(char command[]);
void printUserAdded(int count);
char cmds[100][100];
int cmdCount = 0;

/* Menu to select different functions        
 *
 */
int main(int argc, const char* argv[]) {
    
    struct rusage ruse;
    struct timeval start, end;
    char* input;
    int status;
    int local;
    int numInput;

    while (!feof(stdin)) {
	int bytes_read = 1;
        int nbytes;

        

        printf("===== Mid-Day Commander, v1 =====\n");
        printf("G’day, Commander! What command would you like to run?\n");
        printf("0. whoami : Prints out the result of the whoamicommand\n");
        printf("1. last   : Prints out the result of the last command\n");
        printf("2. ls     : Prints out the result of a listing on a user-specified path\n");
	printUserAdded(cmdCount);
	printf("a. Add Command: Adds a new command to the menu\n");
	printf("c. Change Directory: Changes the working directory\n");
	printf("e. Exit: Leave Midday Commander\n");
	printf("p. Print Working Directory: Prints the current working directory\n");
	printf("Option?\n");

	input = (char *)malloc(nbytes+1);
        bytes_read = getline(&input, &nbytes, stdin);
	

        int parent = getpid();
        int cpid = fork();


        getrusage(RUSAGE_CHILDREN, &ruse);
	gettimeofday(&start, NULL);
        if (cpid == 0) { // Child process options
            printf("\n");
            if (*input == '0') { //0 option -- whoami
	        printf("\n\n");
                printf("-- Who Am I? -- \n");
                char const *params[] = {"whoami", NULL};
                execvp(params[0], (char *const *) params); // call whoami command
		exit(0);
            }
	    
            if (*input == '1') {//1 Option -- last
	        printf("\n\n");
                printf("-- Last Logins -- \n");
                char const *params[] = {"last", "-n 1", NULL};
                execvp(params[0], (char *const *) params);
                exit(0);
	    }


            if (*input == '2') { //2 Option -- ls
                //Declare args array and path array and ask for inputs
		char* args;
                char* path;


                printf("Arguments?: ");
		args = (char *)malloc(nbytes+1);
		bytes_read = getline(&args, &nbytes, stdin);	
		
                printf("Path?: ");
		path = (char *)malloc(nbytes+1);
		bytes_read = getline(&path, &nbytes, stdin);
		size_t ln = strlen(path)-1;
		size_t argsln = strlen(args)-1;
	        printf("\n\n");

		//If both empty
		if(ln == 0 && path[ln] == '\n' && argsln == 0 && args[argsln] == '\n'){
		  char const *params[] = {"ls", NULL};
		  execvp(params[0], (char* const* )params);
		}

		//If path empty
		if(ln == 0 && path[ln] == '\n' && argsln != 0){
		  args[argsln] = '\00';
		  getcwd(path, 1024);
		  char const *params[] = {"ls",args,path, NULL};
		  execvp(params[0], (char* const* )params);
		}

		//If args empty
		if(argsln == 0 && args[argsln] == '\n' && ln != 0){
		  path[ln] = '\00';
		  char const *params[] = {"ls",path, NULL};
		  execvp(params[0], (char* const* )params);
		}
	
		//If neither empty
		args[argsln] = '\00';
		path[ln] = '\00';
				
	
                char const *params[] = {"ls", args, path, NULL};
                execvp(params[0], (char *const *) params);
                exit(0);
             }
	     

	     if(*input == 'p'){ //p Option -- Print working directory
		char workingDir[1024];    
	        getcwd(workingDir, 1024);
		printf("-- Current Directory -- \n");
	        printf("%s\n", workingDir);
		_exit(3);
             }

	     if (48 <= (int) *input && *input <= 57){ //If its a user added command
		if(cmdCount == 0){
		  printf("Illegal Command, No command set at that address\n");
		  _exit(3);
		}
		int cmdToRun = *input - '0'; //Accounting for offset of spot '3' being needed to run first command
		/*Since '3' is used to run the first command we need to account for the offset of it being 			stored in cmd[0] */

		//Take the command and split it on spaces to produce arguments		
		int i = 0;
		char *p = strtok (cmds[cmdToRun-3], " ");
		/*Since '3' is used to run the first command we need to account for the offset of it being 			stored in cmd[0] */
    		char *cmdAndArgs[2];

   		while (p != NULL)
   		{
      		  cmdAndArgs[i++] = p;
      	  	  p = strtok (NULL, " ");
   		}

		char const* params[] = {cmdAndArgs[0], cmdAndArgs[1], NULL};
		execvp(params[0], (char* const*) params);
	     }
	     if(*input != 'e' && *input !='a' && *input !='c'){
	      	printf("Illegal Command, No command set at that address\n");
		_exit(3);
	
	     }
	     _exit(3);


        }

	
        else if (cpid > 0) {//Parent Options
	    wait(NULL);

            if(*input == 'e'){//Option e -- Exit
	      printf("Logging you out, Commander\n");
	     
	      _exit(3);
	      	
	    }
	    if(*input == 'c'){//Option c -- Change working directory
	      char* newPath;
	      printf("New Path?: ");
	      newPath = (char *)malloc(nbytes+1);
	      bytes_read = getline(&newPath, &nbytes, stdin);
	      

	      int dirln = strlen(newPath)-1;
              newPath[dirln] = '\00';
	      int dirRes = chdir((const char*)newPath);
            }
	    if(*input == 'a'){//a Option -- add user command
		char* command;
		printf("New Command?: ");
	        command = (char *)malloc(nbytes+1);
	        bytes_read = getline(&command, &nbytes, stdin);

		int cmdln = strlen(command)-1;
		command[cmdln] = '\00';
		addcmd(command);
		printf("new count is %d\n", cmdCount);
		printf("next command is '%s'\n", cmds[cmdCount-1]);
	     }

	    //Statistics
            gettimeofday(&end, NULL);
	    printf("\n\n");
            printf("-- Statistics ---\n");
            printf("Elapsed time: %ld miliseconds\n",
                   (( (end.tv_sec * 1000 + end.tv_usec) - (start.tv_sec *1000 + start.tv_usec) )) );

            printf("Page Faults: %ld\n", ruse.ru_majflt);
            printf("Page Faults (reclaimed): %ld\n", ruse.ru_minflt);
        }
    }
if(feof(stdin)){
   printf("Logging you out, Commander\n");
   return 0;
}
}



void addcmd(char command[]){
	strcpy(cmds[cmdCount],command);
	cmdCount +=1;
	return; 


}
void printUserAdded(int count){
	if(count == 0){
		return;
	}
	for(int i = 0; i < count; i++){
		//printf("i is %d\n", i);
		//printf("cmds[0] is '%s'\n", cmds[i]);
		printf("%d. %s : User added command\n", i+3, cmds[i]);
	}
	return;
}
