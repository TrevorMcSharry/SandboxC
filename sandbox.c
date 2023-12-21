// Trevor McSharry
// 11/30/2020
// sandbox.c

/*  Sandbox is a program that creates a terminal like
    environment where the user can run commands and programs
    with resource limits. The user can also redirect input and
    output to files. Programs can be run in the background using
    & and tracked using 'jobs'.
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <ctype.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "vector.h"

// Struct to keep track of jobs ran using &
struct Job {
    char *args[4096];
    int pid;
};

// Function to replace environment variables in a string
char* replace_env(char str[1024])
{
    char new_str[1024] = "";
    char *start = str;
    while (*start) {
        if (*start == '$') {
            char *end = start + 1;
            // Environment variables can only contain alphanumeric characters and underscores
            while (*end && (isalnum(*end) || *end == '_')) {
                end++;
            }

            char old_char = *end;
            *end = '\0';

            // Get environment variable value
            char *value = getenv(start + 1);
            if (value == NULL) {
                value = "";
            }

            // Append environment variable value to new string
            strcat(new_str, value);

            *end = old_char;
            start = end;
        } else {
            char temp[2] = {*start, '\0'};
            strcat(new_str, temp);
            start++;
        }
    }
    // Return the new string
    strcpy(str, new_str);
    return str;
}

void update_limits(struct rlimit rl, int max_processes, int max_data_size, int max_stack_size, int max_file_descriptors, int max_file_size, int max_cpu_time)
{
    // Max processes
    rl.rlim_cur = rl.rlim_max = max_processes;
    if (setrlimit(RLIMIT_NPROC, &rl) < 0) {
        perror("setrlimit");
        exit(EXIT_FAILURE);
    }

    // Max data size
    rl.rlim_cur = rl.rlim_max = max_data_size;
    if (setrlimit(RLIMIT_DATA, &rl) < 0) {
        perror("setrlimit");
        exit(EXIT_FAILURE);
    }

    // Max stack size
    rl.rlim_cur = rl.rlim_max = max_stack_size;
    if (setrlimit(RLIMIT_STACK, &rl) < 0) {
        perror("setrlimit");
        exit(EXIT_FAILURE);
    }

    // Max file descriptors
    rl.rlim_cur = rl.rlim_max = max_file_descriptors;
    if (setrlimit(RLIMIT_NOFILE, &rl) < 0) {
        perror("setrlimit");
        exit(EXIT_FAILURE);
    }

    // Max file size
    rl.rlim_cur = rl.rlim_max = max_file_size;
    if (setrlimit(RLIMIT_FSIZE, &rl) < 0) {
        perror("setrlimit");
        exit(EXIT_FAILURE);
    }

    // Max CPU time
    rl.rlim_cur = rl.rlim_max = max_cpu_time;
    if (setrlimit(RLIMIT_CPU, &rl) < 0) {
        perror("setrlimit");
        exit(EXIT_FAILURE);
    }

    
}

// Function to print user input prompt
void print_prompt()
{
    char *user = getenv("USER");
    char *home = getenv("HOME");

    // Get current working directory
    char cwd[1024];
    char *path = getcwd(cwd, sizeof(cwd));

    // Replace home directory with ~
    char *search = home;
    char *replace = "~";
    char *found = strstr(path, search);
    if(found) {
        strncpy(found, replace, strlen(replace));
        found += strlen(replace);
        memmove(found, found + strlen(search) - strlen(replace), strlen(path) - (found - path) + 1);
    }

    // Print prompt
    printf("%s@sandbox: %s> ", user, path);
}

// fork before calling execvp so you are only doing one command on each child process
//execvp - constantly prompt the user for input and pass it into execvp using a null terminated array of chars
int main(int argc, char *argv[])
{
    struct rlimit rl;

    // default limits
    int opt;
    int max_processes = 256;
    int max_data_size = 1 << 30;
    int max_stack_size = 1 << 30;
    int max_file_descriptors = 256;
    int max_file_size = 1 << 30;
    int max_cpu_time = 1 << 30;

    // parse the optional arguments
    while ((opt = getopt(argc, argv, "p:d:s:n:f:t:")) != -1) {
        switch (opt) {
            case 'p':
                sscanf(optarg, "%d", &max_processes);
                break;
            case 'd':
                sscanf(optarg, "%d", &max_data_size);
                break;
            case 's':
                sscanf(optarg, "%d", &max_stack_size);
                break;
            case 'n':
                sscanf(optarg, "%d", &max_file_descriptors);
                break;
            case 'f':
                sscanf(optarg, "%d", &max_file_size);
                break;
            case 't':
                sscanf(optarg, "%d", &max_cpu_time);
                break;
            default:
                fprintf(stderr, "Usage: %s [-p max_processes] [-d max_data_size] [-s max_stack_size] [-n max_file_descriptors] [-f max_file_size] [-t max_cpu_time]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // Vector to track jobs
    Vector *job_vec = vector_new();

    while(1){
        print_prompt();
        // Get command
        char exec[1024];
        scanf("%s", exec);

        char *arg[32];
        char line[1024];
        fgets(line, sizeof(line), stdin);
        replace_env(line);

        // Removes newline
        line[strcspn(line, "\n")] = 0;
        int i = 1;
        // Split up arguments by spaces
        char *token = strtok(line, " ");
        arg[0] = exec;
        while (token != NULL) {
            arg[i] = token;
            i++;
            token = strtok(NULL, " ");
        }
        // Exit function
        if (strcmp(exec, "exit") == 0) {
            int64_t jobs = 0;

            for(unsigned int k = 0; k < vector_size(job_vec); k++){
                vector_get(job_vec, k, &jobs);

                if(waitpid(((struct Job*)jobs)->pid, NULL, WNOHANG) != 0){
                    free((struct Job *)jobs);
                    vector_remove(job_vec, k);
                    k--;
                }
            }
            exit(0);
            
        } 
        // cd function
        else if (strncmp(exec, "cd", 2) == 0) {
            chdir(arg[1]);
        } 
        // jobs function
        else if(strcmp(exec, "jobs") == 0){
            int64_t jobs = 0;

            for(int k = 0; k < vector_size(job_vec); k++){
                vector_get(job_vec, k, &jobs);

                if(waitpid(((struct Job*)jobs)->pid, NULL, WNOHANG) != 0){
                    free((struct Job *)jobs);
                    vector_remove(job_vec, k);
                    k--;
                }
            }

            if(vector_size(job_vec) == 1){
                printf("1 job\n");
            } else{
                printf("%d jobs\n", vector_size(job_vec));
            }

            for(int k = 0; k < vector_size(job_vec); k++){
                vector_get(job_vec, k, &jobs);
                printf("   %d  - %s\n", ((struct Job*)jobs)->pid, ((struct Job*)jobs)->args[0]);
            }
        } else {
            // bool to store whether or not the program is in the background
            bool background = false;
            // check if the last argument is &
            if(i > 0 && strcmp(arg[i - 1], "&") == 0){
                background = true;
                arg[i - 1] = NULL;
            }

            arg[i] = NULL; // execvp needs null terminated array
            pid_t pid = fork();
            if (pid < 0) {
                perror("fork");
                exit(EXIT_FAILURE);
            } else if (pid == 0) {
                // Child process
                int j = 0;
                while(arg[j] != NULL){
                    // Append to file
                    if(strncmp(arg[j], ">>", 2) == 0){   
                        int fd = open( arg[j] + 2, O_CREAT | O_WRONLY | O_APPEND, 0644 );
                        if(fd != -1){
                            dup2(fd, STDOUT_FILENO);
                        }
                        arg[j] = NULL;
                    }
                    // Overwrite file
                    else if(arg[j][0] == '>'){
                        int fd = open(arg[j] + 1, O_CREAT | O_WRONLY | O_TRUNC, 0666);
                        if(fd != -1){
                            dup2(fd, STDOUT_FILENO);
                        }
                        close(fd);
                        arg[j] = NULL;
                    }
                    // Read from file
                    else if(arg[j][0] == '<'){
                        int fd = open(arg[j] + 1, O_RDONLY);
                        if(fd != -1){
                            dup2(fd, STDIN_FILENO);
                        }
                        arg[j] = NULL;
                    }
                    j++;
                }
                // Update limits before calling execvp
                update_limits(rl, max_processes, max_data_size, max_stack_size, max_file_descriptors, max_file_size, max_cpu_time);
                if(execvp(exec, arg) == -1){
                    perror("execvp");
                }
                exit(-1);
            } else{
                // Parent process
                // If the process is in the background, add it to the job vector
                if(background){
                    struct Job *job = malloc(sizeof(struct Job));
                    job->pid = pid;
                    for(int j = 0; j < i-1; j++){
                        job->args[j] = arg[j];
                    }
                    vector_push(job_vec, (int64_t)job);
                }
                // If the process is in the foreground, wait for it to finish
                else{
                    int status;
                    waitpid(pid, &status, 0);
                }
            }
        }
    }
    return 0;
}
