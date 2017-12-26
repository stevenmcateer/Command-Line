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
#include <stdio_ext.h>
#include <sys/mman.h>
#include <fcntl.h>


void addcmd(char command[]);

void printUserAdded(int count);

void printMenu();

char cmds[100][100];
int cmdCount = 0;
int bgPipe[2];
static int *needToRead;
int firstBgProc = 0;

/* Menu to select different functions
 *
 */
int main(int argc, const char *argv[]) {

    struct rusage ruse;
    struct timeval start, end;
    pid_t pid;
    char* input;
    int status;
    int local;
    int numInput;
    int isbg;
    pid_t bgArray[100];
    int bgArraySize = 0;
    //Creates pipe
    pipe(bgPipe);


    int oldflags = fcntl(bgPipe[0], F_GETFL, 0);
    if (oldflags == -1)
        return -1;
    oldflags |= O_NONBLOCK;
    fcntl(bgPipe[0], F_SETFL, oldflags);
    while (!feof(stdin)) {
        int waitOnChild = 0;
	int bytes_read = 1;
        int nbytes;

        printf("===== Mid-Day Commander, v2 =====\n");
        printf("G’day, Commander! What command would you like to run?\n");
        printf("0. whoami : Prints out the result of the whoamicommand\n");
        printf("1. last   : Prints out the result of the last command\n");
        printf("2. ls     : Prints out the result of a listing on a user-specified path\n");
        printUserAdded(cmdCount);
        printf("a. Add Command: Adds a new command to the menu\n");
        printf("c. Change Directory: Changes the working directory\n");
        printf("e. Exit: Leave Midday Commander\n");
        printf("p. Print Working Directory: Prints the current working directory\n");
        printf("r. running processes : Print list of running processes\n");

        printf("Option?\n");
        input = (char *)malloc(nbytes+1);
        bytes_read = getline(&input, &nbytes, stdin);


        int parent = getpid();
        int cpid = fork();
	
        //int cpid =0;

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
                size_t ln = strlen(path) - 1;
                size_t argsln = strlen(args) - 1;
                printf("\n\n");

                //If both empty
                if (ln == 0 && path[ln] == '\n' && argsln == 0 && args[argsln] == '\n') {
                    char const *params[] = {"ls", NULL};
                    execvp(params[0], (char *const *) params);
                }

                //If path empty
                if (ln == 0 && path[ln] == '\n' && argsln != 0) {
                    args[argsln] = '\00';
                    getcwd(path, 1024);
                    char const *params[] = {"ls", args, path, NULL};
                    execvp(params[0], (char *const *) params);
                }

                //If args empty
                if (argsln == 0 && args[argsln] == '\n' && ln != 0) {
                    path[ln] = '\00';
                    char const *params[] = {"ls", path, NULL};
                    execvp(params[0], (char *const *) params);
                }

                //If neither empty
                args[argsln] = '\00';
                path[ln] = '\00';


                char const *params[] = {"ls", args, path, NULL};
                execvp(params[0], (char *const *) params);
                exit(0);
            }


            if (*input == 'p') { //p Option -- Print working directory
                char workingDir[1024];
                getcwd(workingDir, 1024);
                printf("-- Current Directory -- \n");
                printf("%s\n", workingDir);
                exit(0);
            }

            if (*input == 'r') {
                printf("-- Background Processes --\n");
                for (int g = firstBgProc; g < bgArraySize; g++) {
                    printf("%d\n", bgArray[g]);

                }
                _exit(3);


            }

            if (48 <= (int) *input && *input <= 57) { //If its a user added command
                if (cmdCount == 0) {
                    printf("Illegal Command, No command set at that address\n");
                    _exit(3);
                }
                int cmdToRun = *input - '0';
                //Take the command and split it on spaces to produce arguments
                int i = 0;
                char *p = strtok(cmds[cmdToRun - 3], " ");
                char *cmdAndArgs[2];

                while (p != NULL) {
                    cmdAndArgs[i++] = p;
                    p = strtok(NULL, " ");
                }

                char args[128];
                strcpy(args, cmdAndArgs[1]);
                int argln = strlen(args) - 1;
                if (args[argln] == '&') {
                    pid_t cpid2 = fork();
                    if (cpid2 != 0) {
                        //write the cpid2 to the pipe
                        int writeSuc = write(bgPipe[1], &cpid2, sizeof(cpid2));
                        _exit(3);
                    }
                    args[argln] = '\00';
                    strcpy(cmdAndArgs[1], args);

                }
                char const *params[] = {cmdAndArgs[0], cmdAndArgs[1], NULL};
                status = execvp(params[0], (char *const *) params);
            }
            if (*input != 'e' && *input != 'a' && *input != 'c') {
                printf("Illegal Command, No command set at that address\n");
                _exit(3);

            }
            _exit(3);


        } else if (cpid > 0) {//Parent Options
            wait(NULL);
            pid_t output;


            int lastpid;
            int readSuc = read(bgPipe[0], &output, sizeof(output));


            if (output != bgArray[bgArraySize] && readSuc == sizeof(output)) {

                memcpy(bgArray + bgArraySize, &output, sizeof(output));
                bgArraySize++;

            }
            readSuc = 0;

            for (int j = 0; j < bgArraySize; j++) {
                int zero = 0;

                waitpid(bgArray[j], &lastpid, WNOHANG);

                if (kill(bgArray[j], 0) != 0 && bgArray[j] != 0) {
                    firstBgProc++;
                }

            }

            if (*input == 'e') {//Option e -- Exit
                printf("Thank you for using the program\n");
                //input = NULL;
                _exit(3);

            }
            if (*input == 'c') {//Option c -- Change working directory
                char* newPath;
	      printf("New Path?: ");
	      newPath = (char *)malloc(nbytes+1);
	      bytes_read = getline(&newPath, &nbytes, stdin);


                int dirln = strlen(newPath) - 1;
                newPath[dirln] = '\00';
                int dirRes = chdir((const char *) newPath);
            }
            if (*input == 'a') {//a Option -- add user command
                char* command;
		printf("New Command?: ");
	        command = (char *)malloc(nbytes+1);
	        bytes_read = getline(&command, &nbytes, stdin);

                int cmdln = strlen(command) - 1;
                command[cmdln] = '\00';
                int secondToLast = strlen(command) - 2;
                if (secondToLast == 26) {


                }
                addcmd(command);
            }
            //Statistics
            gettimeofday(&end, NULL);
            printf("\n\n");
            printf("-- Statistics ---\n");
            printf("Elapsed time: %ld miliseconds\n",
                   (((end.tv_sec * 1000 + end.tv_usec) - (start.tv_sec * 1000 + start.tv_usec))));

            printf("Page Faults: %ld\n", ruse.ru_majflt);
            printf("Page Faults (reclaimed): %ld\n", ruse.ru_minflt);
        }
    }


    if (feof(stdin)) {
        printf("Logging you out, Commander\n");
        return 0;
    }
}

void addcmd(char command[]) {
    strcpy(cmds[cmdCount], command);
    cmdCount += 1;
    return;


}

void printUserAdded(int count) {
    if (count == 0) {
        return;
    }
    for (int i = 0; i < count; i++) {
        //printf("i is %d\n", i);
        //printf("cmds[0] is '%s'\n", cmds[i]);
        printf("%d. %s : User added command\n", i + 3, cmds[i]);
    }
    return;
}


 
