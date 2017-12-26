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

/* Menu to select different functions        
 *
 */
int main(int argc, const char* argv[]) {

    struct rusage ruse;
    struct timeval start, end;
    char* input;
    int status;
    int local;
    while (!feof(stdin)) {
	int bytes_read = 1;
    int nbytes;

    input = (char *)malloc(nbytes+1);

	int pid = getpid();

        printf("===== Mid-Day Commander, v0 =====\n");
        printf("G’day, Commander! What command would you like to run?\n");
        printf("0. whoami : Prints out the result of the whoamicommand\n");
        printf("1. last   : Prints out the result of the last command\n");
        printf("2. ls     : Prints out the result of a listing on a user-specified path\n");
        printf("Option?:");
        //fgets(input, 100, stdin);
	//__fpurge(stdin);
	bytes_read = getline(&input, &nbytes, stdin);

    if (bytes_read == -1)
    {
        break;
    }

        int parent = getpid();
        int cpid = fork();


        getrusage(RUSAGE_CHILDREN, &ruse);
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
	     else{
		printf("Illegal Command! No command set at that location.\n");
		_exit(3);
	     }

        }
        if (cpid > 0) { //Parent Process
            wait(NULL);
            
            gettimeofday(&end, NULL);
	    printf("\n\n");
            printf("-- Statistics ---\n");
            printf("Elapsed time: %ld milliseconds\n",
                   (( (end.tv_sec * 1000+ end.tv_usec) - (start.tv_sec * 1000 + start.tv_usec) )) );

            printf("Page Faults: %ld\n", ruse.ru_majflt);
            printf("Page Faults (reclaimed): %ld\n", ruse.ru_minflt);
        }
    }
if(feof(stdin)){
   printf("Logging you out, Commander\n");
   return 0;
}
}
