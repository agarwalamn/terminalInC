#include<stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include<stdlib.h>
#include<string.h>

#define LSH_RL_BUFSIZE 1024
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"


int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);

//lets make builtin commands
char *builtin_str[]={
    "cd",
    "help",
    "exit"
};
int (*builtin_func[])(char **)={
    &lsh_cd,
    &lsh_help,
    &lsh_exit
};
int lsh_num_builtins(){
    return sizeof(builtin_str)/sizeof(char *);
}

int lsh_cd(char **args){
    if(args[1]==NULL){
        fprintf(stderr,"BoI DoN't YoU knOw ThE SyNtAx? let me help:\n \"cd\" expects and arguments\n");
    }else{
        if(chdir(args[1])!=0){
            perror("lsh");
        }
    }
    return 1;
}

int lsh_help(char **args){
    int i;
    printf("Kira's Konsole\n");
    printf("Type the program name and arguments, and hit enter.\n");
    printf("The following are built in:\n");

    for(i=0;i<lsh_num_builtins();i++){
        printf(" %s\n",builtin_str[i]);
    }

    printf("Use the man command for more information on other programs.\n");
    return 1;
}
int lsh_exit(char **args){
    printf("BYE\n");
    return 0;
}

int lsh_launch(char **args){
    pid_t pid,w_pid;
    int status;

    pid = fork();

    if(pid==0){
        if(execvp(args[0],args)==-1){
            perror("yOU YeS YoU! really?");
        }
        exit(EXIT_FAILURE);
    }else if(pid<0){
        perror("Opps my error can't work try again");
    }else{
        do{
            w_pid = waitpid(pid,&status,WUNTRACED);
        }while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}


int lsh_execute(char **args){
    int i;
    if(args[0]==NULL){
        return 1;
    }
    for(i=0;i<lsh_num_builtins();i++){
        if(strcmp(args[0],builtin_str[i]) == 0){
            return (*builtin_func[i])(args);
        }
    }

    return lsh_launch(args);
}
void allocation_error(void){
    fprintf(stderr,"Damn! Seems like memory issue try again may work!\n");
    exit(EXIT_FAILURE);
}
//read the input
char *lsh_read_line(void){
    int bufSize = LSH_RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char)*bufSize);
    int c;

    //if buffer not allocated
    if(!buffer){
        allocation_error();
    }

    while(1){

        c=getchar();
        if(c == EOF || c == '\n'){
            buffer[position] = '\0';
            return buffer;
        }
        else{
            buffer[position]=c;
        }
        position++;

        //if overloaded buffer input
        if(position>=bufSize){
            bufSize +=LSH_RL_BUFSIZE;
            buffer = realloc(buffer,bufSize);
            if(!buffer){
                allocation_error();
            }
        }
    }

}

char **lsh_split_line(char *line){
    int buffSize = LSH_TOK_BUFSIZE;
    char **tokens = malloc(sizeof(char *)*buffSize);
    char *token;
    int position=0;

    if(!tokens){
        allocation_error();
    }

    token = strtok(line,LSH_TOK_DELIM);
    while(token!=NULL){
        tokens[position]=token;
        position++;

        if(position>=buffSize){
            buffSize +=LSH_TOK_BUFSIZE;
            tokens=realloc(tokens,buffSize*sizeof(char *));
            if (!tokens)
            {
                allocation_error();
            }
        }

        token = strtok(NULL,LSH_TOK_DELIM);
    }
    tokens[position]=NULL;
    return tokens;
}


void lsh_loop(void){
    char *line;
    char **args;
    int status;
    do{
        printf(">");
        line=lsh_read_line();
        args=lsh_split_line(line);
        status=lsh_execute(args);

        free(line);
        free(args);
    }while(status);
}

int main(int argc, char const *argv[])
{
    printf("Welcome to my Konsole\n\n");
    printf("Its a basic model of console!\n");
    printf("Chears!\n");
    lsh_loop();
    return EXIT_SUCCESS;
}