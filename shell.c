#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#define BUFFSIZE 1024


/**
 * Function to parse commands input
 * @param command   string (character array) that contains command entered by user
 * @param arguments Store the pointers of command/arguments after parsing command
 */
void parse(char *command, char **arguments){
    //command[0] = " ";
    while (*command != '\0')
    {
        while (*command == ' ' || *command == '\t' || *command == '\n')
            *command++ = '\0';

        *arguments++ = command;
        while (*command != '\0' && *command != ' ' && *command != '\t' && *command != '\n')
            command++;
    }

    (*--arguments) = NULL;
} //parse_command

/**
 * Displays the prompt
 */
void print_prompt(){
    //on first open should be in home directory?
    char cwd[1024];
    char prompt[1200] = "1730sh:";
    getcwd(cwd, sizeof(cwd));    // Get current working directory
    char *home = getenv("HOME"); // Get users home directory
    //cwd = getenv("HOME");
    //strcat(prompt,cwd);
    //printf("%s ~",prompt);
    if (strcmp(cwd, home) == 0)  // Check if current working directory is home directory
        strcat(prompt, "~");
    else
        strcat(prompt, cwd);
    strcat(prompt, "$");

    // Display prompt
    printf("%s ", prompt);

    //printf("%s %s ",prompt, getenv("HOME"));
} //print_prompt

/**
 * Function to check if redirection is present
 * and return the file name by making redirection pointer NULL
 *
 * @param char arguments array for command parameters
 * @param int flag to flag the selection of redirection
 */
char *check_redirection(char *arguments[], int *flag){
    int i = 0;
    while (arguments[i] != NULL)
    {
        *flag = 0;
        if (strcmp(arguments[i], "<") == 0)
            *flag = 1;
        else if (strcmp(arguments[i], ">") == 0)
            *flag = 2;
        else if (strcmp(arguments[i], ">>") == 0)
            *flag = 3;

        if (*flag != 0)
        {
            arguments[i] = NULL;
            return arguments[i + 1];
        }

        i++;
    }
    return NULL;
} //check_redirection

/**
 *  Function to exit with message and exit code
 *
 * @param char message message to print for exit
 * @param int exit_code to exit program
 */
void exit_with_message(char *message, int exit_code){
    fprintf(stderr, "%s\n", message);
    exit(exit_code);
}

/**
 * Main function to run shell and execute using fork and execvp
 *
 */
int main() {
    char command[1024] = {0};  //Character array to store command entered by user
    char *argv[64];      //Array of pointers to store command/arguments after parsing command
    pid_t pid;         //Process ID of child process
    int redirection = 0; //Flag to check if redirection is present in command
    int r __attribute__((unused));
    char *h = getenv("HOME");

//    for(int i =1; i < command[i];i++) {
    //      command[i] = i + 1;
    //}
    chdir(getenv("HOME"));
    while (1){
        //int r = 0;
        setbuf(stdout,NULL);


        print_prompt();

        r = read(STDIN_FILENO,command,sizeof(command)); //read command
        parse(command, argv);           // Parse command

        if (strcmp(argv[0], "exit") == 0){       // Check if user entered exit
            exit(0);
        } //IF
        else if (strcmp(argv[0], "cd") == 0) {
            //change directory
            //chdir(argv[1]);
            if (argv[1] == NULL) {
                // perror("bash: Please enter a path to cd");
                chdir(h);
            } else if((strcmp(argv[1], "~") ==0)|| (strcmp(argv[1],"~/")==0)) {
                chdir(h);
                //printf("Ch dir..");
                /** if(chdir(argv[1]) != 0) {
                    perror("bash ");
                    }*/
            } else {
                chdir(argv[1]);
                //perror("bash");
                //printf("%s :No such file or directory\n", argv[1]);

            } //else

        } //cdif

        // Create child process
        else if ((pid = fork()) == 0)
        {
            // Child process
            // Get name of the file if redirection is present
            char *file_name = check_redirection(argv, &redirection);
            int fd = -1;

            if (file_name != NULL)
            {
                if (redirection == 1)
                {
                    //printf("Using <\n");
                    if ((fd = open(file_name, O_RDONLY)) == -1){
                        exit_with_message("Error: File not found", 1);
                    } //if
                    dup2(fd, STDIN_FILENO); //redirect STDIN to file
                }
                else if (redirection == 2)
                {
                    //printf("Using >\n");
                    if ((fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1){
                        exit_with_message("Error: not able to write to file\n", 1);
                    } //if
                    dup2(fd, STDOUT_FILENO); //Redirects STDOUT to file (overwrite to file)
                }
                else if (redirection == 3){
                    //printf("Using >>\n");
                    if ((fd = open(file_name, O_WRONLY | O_CREAT | O_APPEND, 0644)) == -1){
                        exit_with_message("Error: not able to write to file\n", 1);
                    }//if
                    dup2(fd, STDOUT_FILENO); //Redirect STDOUT to file (append output to file)
                }
            }

            // Execute command
            if (execvp(argv[0], argv) == -1){
                //exit_with_message("Error: Command not found", 1);
                printf("Command not found: %s\n", argv[0]);
                exit(EXIT_FAILURE);
            }

        } //else if
        else if (pid == -1){
            // Fork failed.
            //exit_with_message("Fork failed", 1);
            perror("fork");
        }
        else{
            // Parent process
            //wait(NULL);
            //pid_t wpid;
            int status;
            /** wait(&status);
            if (WIFEXITED(status)) {
                printf("child exited with status = %d\n",WEXITSTATUS(status));
                }*/
            waitpid(pid, &status, WUNTRACED);
        }


    } //while

} //main
