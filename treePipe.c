#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define BUFFER_LEN 11


void single_head_pipe(int curDepth, int maxDepth, int lr, int num1, int num2){
    int fd[2];
    pipe(fd);
    int pid = fork();

    if(pid < 0 ){
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (pid == 0){
        // Child process
        //close(fd[1]); // end of writing
        dup2(fd[1], STDOUT_FILENO);
        dup2(fd[0], STDIN_FILENO); // stdin will keep reediong from pipe
        //close(fd[0]);

        char *leftWay = "./left";
        char *args[] = {leftWay, NULL};
        execvp(leftWay, args);
        printf("#1 This shouldn't print");
        exit(1);
    } else {
        // Parent process
        // close(fd[0]);
        char buffer[BUFFER_LEN];
        sprintf(buffer, "%d %d\n", num1, num2);
        write(fd[1], buffer, strlen(buffer));
        // close(fd[1]);
        
        wait(NULL);

        read(fd[0], buffer, BUFFER_LEN);
        int result = atoi(buffer);
        printf("> my num1 is: %d\n", num1);
        printf("> my result is: %d\n", result);

        printf("The final result is: %d\n", result);
        close(fd[0]);
        close(fd[1]);
    }
    close(fd[0]);
    close(fd[1]);
    exit(0);
}

void execute_child(char *program, int num1, int num2, int fd[2]) {
    char buffer[BUFFER_LEN];
    sprintf(buffer, "%d %d\n", num1, num2);
    write(fd[1], buffer, strlen(buffer));

    dup2(fd[1], STDOUT_FILENO); // Since ./left sends into to console, it will write to anon. file
    dup2(fd[0], STDIN_FILENO);

    close(fd[0]);
    close(fd[1]);

    char *args[] = {program, NULL};
    execvp(program, args);
    printf("This shouldn't print");
    exit(1);
}

int recur_child(int curDepth, int maxDepth, int lr, int num1, int result_fd[2]){
    int num2 = 1;
    int result;
 
    if(curDepth == 0){
        printf("> Current depth: %d, lr: %d\n", curDepth, lr);
        // Meaning we are on root, and there exist only one node
        printf("Please enter num1 for the root: ");
        scanf("%d", &num1);
        // We might have to override "LR" since tree with d-0 is always left
    } else {
        for(int i=0; i<curDepth; i++){
            printf("---");
        }
        printf("> Current depth: %d, lr: %d\n", curDepth, lr);
    }

    if(maxDepth == 0){
       single_head_pipe(curDepth, maxDepth, lr, num1, num2);
       return 0;
    }

    if(curDepth < maxDepth){
        int left_fd[2];
        pipe(left_fd);

        int left_pid = fork();
        if(left_pid < 0){
            fprintf(stderr, "fork failed\n");
            exit(1);
        }else if (left_pid == 0) {
            //printf("#2 I’m child process, with PID: %d Depth: %d, Num1: %d, Num2: %d\n",(int)getpid(), curDepth, num1, num2);
            recur_child(curDepth + 1, maxDepth, 0, num1, left_fd);
            execute_child("./left", num1, num2, left_fd);
        } else {
            // Parent Process
            close(left_fd[1]); // We do not need to write to left child anymore, we will wait for scanf

            waitpid(left_pid, NULL, 0);
            
            char buffer[BUFFER_LEN]; // Read from anon. file that child gives
            read(left_fd[0], buffer, sizeof(buffer));
            num1 = atoi(buffer);
            //printf("Num1: %d\n", num1);
            close(left_fd[0]); // We do not need to read anymore
            
            sprintf(buffer, "%d\n", num1);
            write(result_fd[1], buffer, strlen(buffer));

            // for (int i = 0; i < curDepth; i++) {
            //     fprintf(stderr, "---");
            // }
            // //fprintf(stderr, "> My num1 after left is: %d\n", num1);
        }
    
    }

    //printf("#3 I’m parent process, with PID: %d Depth: %d, Num1: %d, Num2: %d\n",(int)getpid(), curDepth, num1, num2);
    if(curDepth != maxDepth){
        for(int i=0; i<curDepth; i++){
            printf("---");
        }
        printf("> My num1 is: %d\n", num1);
    }
    if (curDepth < maxDepth) {
        int right_fd[2];
        pipe(right_fd);

        int right_pid = fork();

        if(right_pid<0){
            fprintf(stderr, "fork failed\n");
		    exit(1);
        }else if (right_pid == 0) {
            recur_child(curDepth + 1, maxDepth, 1, num1, right_fd);
            execute_child("./right", num1, num2, right_fd);
            
        } else {
            // Parent Process
            close(right_fd[1]); // We do not need to write to left child anymore, we will wait for scanf

            //printf("#5 I’m parent process, with PID: %d Depth: %d, Num1: %d, Num2: %d\n",(int)getpid(), curDepth, num1, num2);

            waitpid(right_pid, NULL, 0);

            char buffer[BUFFER_LEN];
            read(right_fd[0], buffer, sizeof(buffer));
            result = atoi(buffer);
            close(right_fd[0]); 

            num2 = result;
            sprintf(buffer, "%d\n", num2);
            write(result_fd[1], buffer, strlen(buffer));


        }
    }

    // Root Node
    if (curDepth == maxDepth){
        if (lr == 0) {
            result = num1 + num2;
        } else {
            result = num1 * num2;
        }
        for(int i=0; i<curDepth; i++){
            printf("---");
        }
        printf("> My num1 is: %d\n", num1);
        for(int i=0; i<curDepth; i++){
            printf("---");
        }
        printf("> My result is: %d\n", result);

        num1 = result;
        char buffer[BUFFER_LEN];
        sprintf(buffer, "%d\n", num1);
        write(result_fd[1], buffer, strlen(buffer));
        exit(1);
    } else {
        for(int i=0; i<curDepth; i++){
            printf("---");
        }
        printf("> Current depth: %d, lr: %d, my num1: %d, my num2: %d\n", curDepth, lr, num1, num2);
        for(int i=0; i<curDepth; i++){
            printf("---");
        }
        if (lr == 0) {
            result = num1 + num2;
        } else {
            result = num1 * num2;
        }
        printf("> My result is: %d\n", result);
    }

    return result;
}

int main(int argc, char *argv[]) {

    if (argc != 4){
        fprintf(stderr, "Usage: treePipe <current depth> <max depth> <left - right>\n");
        return 1;
    }

    int curDepth = atoi(argv[1]);
    int maxDepth = atoi(argv[2]);
    int lr = atoi(argv[3]);

    int result_fd[2];
    pipe(result_fd);

    int finalResult = recur_child(curDepth, maxDepth, lr, 0, result_fd);

    printf("The final result is: %d\n", finalResult);

    return 0;
    
}
