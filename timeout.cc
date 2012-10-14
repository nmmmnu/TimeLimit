#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>

//#define DEBUG

//Default livetime is 1:30
#define LIVETIME 90

#define err(a) fprintf(stderr, "Error: %s", a);

#ifdef DEBUG
#include <stdarg.h>
void debug(const char *pattern, ...){
   printf("Debug: ");	
   
   va_list ap;
   va_start(ap, pattern);
   vprintf(pattern, ap);
   va_end(ap);
}
#else
inline void debug(...){}
#endif

char *command1 = NULL;

void signal_SIGALRM_catch(int){
   err("Time limit expires, process halted\n");

   debug("Executing optional command (if any)\n");
   
   if (command1)
      system(command1);

   debug("Execution complete\n");
  
   raise(SIGTERM); //Terminate it!
}

void setautokill(int livetime){
   if (signal(SIGALRM, signal_SIGALRM_catch) == SIG_ERR){
      err("Can't signal()\n");
      exit(2);
   }

   itimerval t;
   t.it_value.tv_sec  = livetime;
   t.it_value.tv_usec = t.it_interval.tv_sec = t.it_interval.tv_usec = 0;

   setitimer(ITIMER_REAL, &t, NULL);
}

void help(const char *prog){
   printf("TimeLimit Ver 1.1\n");
   printf("CopyLeft (c) 4.2001, Nikolay Mijaylov <nmmm@nmmm.nu>, Sofia\n");
   printf("Distribution: GPL\n");
   printf("\n");
   printf("Usage:\n");
   printf("\t%s time command [command1]\n", prog);
   printf("Remaks:\n");
   printf("\ttime     - time in seconds\n");
   printf("\tcommand  - command to be execute using system()\n");
   printf("\tcommand1 - optional command to be execute using system(),\n");
   printf("\t           only if time limit expires\n");
   printf("\n");
   
   exit(1);
}

int main(int argc, char *argv[]){
   if (argc < 2 + 1)
      help(argv[0]);

   int timeout = LIVETIME;

   sscanf(argv[1], "%d", &timeout);

//   if (argc < 3 + 1) // argv[3] will be eighter NULL or ""
   command1=argv[3];

   debug("Executing \"%s\" for %d seconds with \"%s\" optional command\n",
                      argv[2],
		      timeout,
		      command1 ? command1 : "no");

   setautokill(timeout);

   system(argv[2]);

   debug("Execution complete\n");

   return 0;
}
