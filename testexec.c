#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

#define BUFFSIZE 1024
/**void print_banner()
{
	for (int i = 0; i < 80; i++) printf("#");
	printf("\n");
} // print_banner

int main (int argc, char* argv[])
{

	char* progname = argv[1];
	pid_t pid;

	if (argc > 1) {

		if ((pid = fork()) < 0) perror("fork");
		else if (pid == 0) {	// child process

			printf("attempting to exec: %s\n", progname);
			printf("with options:\n");
			for (int i = 2; i < argc; i++) printf("%s\n", argv[i]);
			print_banner();

			if (execvp(progname, argv + 1) == -1) perror(argv[0]);
			// no more code should be here
		} else {		// parent process
			int status;
			wait(&status);
			if (WIFEXITED(status))
				printf("child exited with status = %d\n",
					WEXITSTATUS(status));
			// could do more checking on how the child terminated
		} // if

	} else {
		fprintf(stderr, "please specify the name of the program as a command line argument\n");
		return EXIT_FAILURE;
	} // if

} // main
*/


/**
 * Function to parse commands input
 * @param command   string (character array) that contains command entered by user
 * @param arguments Store the pointers of command/arguments after parsing command
 */
void parse_command(char *command, char **arguments)
{
    while (*command != '\0')
    {
        while (*command == ' ' || *command == '\t' || *command == '\n')
            *command++ = '\0';

        *arguments++ = command;
        while (*command != '\0' && *command != ' ' && *command != '\t' && *command != '\n')
            command++;
    }

    (*--arguments) = NULL;
}

/**
 * Displays the prompt
 */
void print_prompt(){

    //on first open should be in home directory
    char cwd[1024];
    char prompt[1200] = "1730sh:";
    getcwd(cwd, sizeof(cwd));    // Get current working directory
    char *home = getenv("HOME"); // Get users home directory
    if (strcmp(cwd, home) == 0)  // Check if current working directory is home directory
        strcat(prompt, "~");
    else
        strcat(prompt, cwd);
    strcat(prompt, "$");

    // Display prompt
    printf("%s ", prompt);

    //printf("%s %s ",prompt, getenv("HOME"));
} //display_prompt

/**
 * Function to check if redirection is present
 * and return the file name by making redirection pointer NULL
 *
 * @param char arguments array for command parameters
 * @param int flag to flag the selection of redirection
 */
char *check_redirection(char *arguments[], int *flag)
{
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
}

/**
 *  Function to exit with message and exit code
 *
 * @param char message message to print for exit
 * @param int exit_code to exit program
 */
void exit_with_message(char *message, int exit_code)
{
    fprintf(stderr, "%s\n", message);
    exit(exit_code);
}

/**
 * Main function to run shell and execute using fork and execvp
 *
 */

int main() {
    char command[1024];  //Character array to store command entered by user
    char *argv[64];      //Array of pointers to store command/arguments after parsing command
    pid_t pid, wpid;         //Process ID of child process
    int redirection = 0; //Flag to check if redirection is present in command
    int r;


    while (1){
        setbuf(stdout,NULL);
        print_prompt();

        r =read(STDIN_FILENO,command,sizeof(command)); //read command

        //display_prompt();                       // Display prompt
        parse_command(command, argv);           // Parse command
        //}
        if (strcmp(argv[0], "exit") == 0){       // Check if user entered exit
            exit(0);
        } //IF
        if (strcmp(argv[0], "cd") == 0) {
            //change directory
            if (argv[1] == NULL) {
                perror("bash: Please enter a path to cd");
            } else {
                //printf("Ch dir..");
                if(chdir(argv[1]) != 0) {
                    perror("bash");
                }
            }

        } //cdif
        // Create child process
        if ((pid = fork()) == 0)
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
                    dup2(fd, STDIN_FILENO); // redirect STDIN to file
                }
                else if (redirection == 2)
                {
                    //printf("Using >\n");
                    if ((fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1){
                        exit_with_message("Error: not able to write to file\n", 1);
                    } //if
                    dup2(fd, STDOUT_FILENO); // Redirect STDOUT to file (overwrite to file)
                }
                else if (redirection == 3){
                    //printf("Using >>\n");
                    if ((fd = open(file_name, O_WRONLY | O_CREAT | O_APPEND, 0644)) == -1){
                        exit_with_message("Error: not able to write to file\n", 1);
                    }//if
                    dup2(fd, STDOUT_FILENO); // Redirect STDOUT to file (append output to file)
                }
            }

            // Execute command
            if (execvp(argv[0], argv) == -1){
                //exit_with_message("Error: Command not found", 1);
                printf("Command not found: %s\n", argv[0]);
                exit(EXIT_FAILURE);
            }

        }
        else if (pid == -1){
            // Fork failed.
            //exit_with_message("Fork failed", 1);
            perror("fork");
        }
        else{
            // Parent process
            //wait(NULL);
            int status;
            /** wait(&status);
            if (WIFEXITED(status)) {
                printf("child exited with status = %d\n",WEXITSTATUS(status));
                }*/
            wpid = waitpid(pid, &status, WUNTRACED);
        }


    } //while

} //main
 #include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFSIZE 1024
#define DELIM "\t\r\n\a"
#define TOKBUFFSIZE 64

/* Function declarations */

char *read_line();
void type_prompt();
char **split_line(char *line);
int shell_exit(char **args);
int launch(char **args);
int args_length(char **args);
//void parse_command(char *command, char **arguments);
char **get_input(char *input);

/**
 * Function to print the prompt
 */
void type_prompt() {
    char *line;
    char **args;
    //char* prog;
    int status = 1;
    char cwd[1000];
    //char *argv[64]; //the command line argument
    //char command[1024];  //!< Character array to store command entered by user
    char **command;
    char *input;
    do {
        if (getcwd(cwd,sizeof(cwd)) != NULL) {
            printf("1730sh:~%s $",cwd);
        }//if
        //printf("1730sh:~ $");
         line = read_line();
        //parse_command(command,argv);
        args =split_line(line);
        status = launch(args);

        /*
        input = read_line();
        command = get_input(input);
        status = launch(command);
        */
        free(line);
        free(args);
        //free(input);
        //free(command);
    } while(status);
}//type_prompt



/**
 * @brief Function to parse commands
 * @param command   string (character array) that contains command entered by user
 * @param arguments Store the pointers of command/arguments after parsing command
 */
void parse_command(char *command, char **arguments)
{
    while (*command != '\0')
    {
        while (*command == ' ' || *command == '\t' || *command == '\n')
            *command++ = '\0';

        *arguments++ = command;
        while (*command != '\0' && *command != ' ' && *command != '\t' && *command != '\n')
            command++;
    }

    (*--arguments) = NULL;
} //parsecommand



/**
 * Read line function to read user input commands
 */
char *read_line() {
    int buffsize = BUFFSIZE;
    char *buffer = malloc(sizeof(char) * buffsize);
    int position = 0;
    int c;

    if(!buffer){
        perror("Error for buffer");
        exit(EXIT_SUCCESS);
    }
    while(1) {
        c = getchar();
        if(c == '\n'|| c == EOF || c == ' ') {
            buffer[position] = '\0';
            return buffer;
        } else{
            buffer[position] = c;
        }
        position++;

        if(position >= buffsize) {
            buffsize += BUFFSIZE;
            buffer = realloc(buffer,buffsize);
            if(!buffer) {
                perror("Allocation error");
                exit(EXIT_FAILURE);
            }
        }//if

    }//while


}//read_line

/**
 * Args length
 */
int args_length(char **args) {
    int j = 0;
    while(args[j] != NULL) {
        j++;
    }
    return j;
} //argslength
/**
 * Function spilt the command line and read commands
 * easier.
 */
char **split_line(char *line) {
    int buffsize = TOKBUFFSIZE;
    int position = 0;
    char **tokens = malloc(buffsize * sizeof(char*));
    char *token;

    token = strtok(line, DELIM);
    while(token != NULL) {
        tokens[position] = token;
        position++;

        if(position >= buffsize) {
            buffsize += BUFFSIZE;
            tokens = realloc(tokens, buffsize * sizeof(char*));
            if(!tokens) {
                perror("Allocation error");
                exit(EXIT_FAILURE);
            }//if
        }//if

        token = strtok(NULL, DELIM);
    }//while
    tokens[position] = NULL;
    return tokens;



}//split_line

/**
 * Function to get command
 *
 * @param input string
 */
char **get_input(char *input) {
    char **command = malloc(8 *sizeof(char *));
    char *separator = " ";
    int index = 0;
    char *parsed;

    parsed =strtok(input, separator);
    while(parsed != NULL) {
        command[index] = parsed;
        index++;
        parsed = strtok(NULL, separator);
    }//while

    command[index] = NULL;
    return command;
}//getcommand
/**
 * To exit the shell
 *
 */
int shell_exit(char **args) {
    return 0;
}
/**
 *
 *
 */
int launch(char **args) {
    pid_t pid, wpid;
    int stat;
    //char *argv[] = {"-l", "~" , NULL};
    //char *argv[64];
    if(strcmp(args[0],"exit") == 0) {
        //return shell_exit();
        return 0;
    }//if

    //char* progname = argv[1];
    pid = fork();
    if (pid < 0) {
        perror("Fork");
    } else if (pid == 0) {
        //for (int i = 0;i < args_length(args);i++) {
            if (execvp(args[0], args) == -1) {
                printf("command not found: %s\n",args[0]);
                exit(EXIT_FAILURE);
            }
            //}//for
    } else {
        waitpid(pid, &stat, WUNTRACED);
    }

    return 1;
}
/**
 * Main function to run
 *
 */
int main() {
    //int status;

    //do {

        type_prompt();

        //}//do
    // while (status);

}//main
