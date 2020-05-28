/*
    Name: Elmer Rivera
    ID:   1001529110
*/


#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 11  // Max arguments changed to 11 so upoort at least 10 arguments plus 
                              //the command given

//Function definition in order to implement "cd" command.
int cd(char *path)
{
  return chdir(path);
}

int main()
{
  //Initialize all arrays and variables in order to implement command functionality
  char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );
  int i;
  int n=0;
  int count=0;
  char *history[50];
  int pids[15];

  //For loop used to allocate memory in an array so that 
  //we will be able to store characters
  //and access later for history command
  for(i=0;i<15;i++)
  {
    history[i]=(char*)malloc(1024);
    memset(history[i],0,1024);
  }

  while( 1 )
  {
    // Print out the msh prompt
    printf ("msh> ");

    
    
    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin));

    //We are going to copy the string given by the user
    //into the history array and increment the counter
    //for each command entered.
    strncpy(history[count++],cmd_str,strlen(cmd_str));
    if(count==15)
    {
      count=0;
    } 
    //Function compares the command entered with a
    //newline character so if the user enters nothing
    //the shell will not seg fault and will print another
    //prompt and accept more commands.
    if(strcmp(cmd_str,"\n")==0)
    {
      continue;
    }

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int   token_count = 0;                                 
   
    // Pointer to point to the token
    // parsed by strsep
    char *arg_ptr;
    
    //Check to see if command given was the exclemation 
    //point in order to store the number given by the user
    //and change it from a char to a int, and use that int to
    //locate the index of the desired command to be run from the 
    //history array.
    if(cmd_str[0]=='!')
    {
      int index=atoi(&cmd_str[1]);
      memset(cmd_str,0,255);
      strncpy(cmd_str,history[index],strlen(history[index]));
    } 
                                          
    
    char *working_str  = strdup( cmd_str );                

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;


    

    // Tokenize the input stringswith whitespace used as the delimiter
    while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && (token_count<MAX_NUM_ARGUMENTS))
    {
		  token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
		  if( strlen( token[token_count] ) == 0 )
		  {
			  token[token_count] = NULL;
		  }
      token_count++;
    }

   
    
     //Check to if the "cd" commmand was given and
     //call the defined function in order to give the path
     //of the directory we want to enter into
     if (strcmp(token[0], "cd") == 0) 
      {
          if (cd(token[1]) < 0) 
          {
            perror(token[1]);
          }
          continue;
      }
     
     //Check to see if "exit" was entered in order to 
     //simply call "exit(0)" and terminate the shell
     else if(strcmp(token[0], "exit") == 0)
      {
        exit(0);
        continue;
      } 
    
    //Same functionality as the "exit" function above
    //except we are checking if "quit" was entered
     else if(strcmp(token[0], "quit") == 0)
      {
        exit(0);
        continue;
      } 

    //We check to see if the history command was entered
    //and if true we simply print the contents of the history
    //array we created earlier that stores the commands entered
    //by the user.
     else if(strcmp(token[0],"history")==0)
      {
        for(i=0; i<15;i++) 
        {
          printf("%d: %s\n",i,history[i]);
        }
        continue; 
      }

    //Check for the showpids command and if true we simply
    //take the pids array and print the pids of the current 
    //running processes of the shell
     else if(strcmp(token[0],"showpids")==0)
      {
        for(i=0; i<n;i++) 
        {
           printf("%d: %d\n",i,pids[i]);
        } 
        continue;
      }
    
    else 
    {
    
    //Calling the fork function so we can create
    //a copy of the parent process and then through
    //the child we can run commands from the user bin,etc.
    pid_t pid= fork();
    
    //Stores the pid of each process created as a result of
    //the fork function,in order to be able to print the 
    //pids of all the running processes if the command was 
    //entered
    pids[n++]=pid;
    
    //check to see if we are in the child so we can run usr
    // and bin commands.
    if(pid==0)
    {

      int ret= execvp(token[0],token);//Take the command entered and call execvp so the 
                                      //shell searches all paths so that out shell supports
                                      //any valid commmand
      if(ret<0)
      {
        printf("%s:command not found \n",token[0]); //if execvp fails(returns -1) then we print
        exit(1);                                    //a message to the user telling 
                                                    //the command is wrong or invalid
                                                    //then we call exit(0) to exit the child
                                                    //and allow for another command to be
                                                    //entered and not crash
       
      }
      
    }
    else 
    {
      int status;         
      wait( & status );   //call wait function so the parent process waits for the 
                          //child process to finish. This allows each command to be 
                          //entered and excecuted until finished
      
    }
   
    }


    free( working_root );  //free allocated memory to avoid seg faulting and crashing the 
                          //program.
    
    
  }
  return 0;

}

