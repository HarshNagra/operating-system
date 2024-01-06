#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>  
#include <unistd.h>    
#include <string.h>  

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>  // for using shared memory 
#include <signal.h> 


void sig_cont(int sig){
	printf("Received a SIGCONT. \n");
}  



int main(int argc, char* argv[]) 
{
	printf("This is the BEGINNING of the program.\n\n");
	if(argc-1 != 3){
		printf("Error: The number of input integers now is %d. Please input 3 integers.\n",argc-1);
		return -1;
	}// Don't modify this Error Checking part
		
	{
		// Write your code in this brace
		int shmid;
   		int *shmc, *shmp;
		
		int status, child;

		int size_data = 3 * sizeof(int);
		
		shmid = shmget(IPC_PRIVATE, size_data, 0666 | IPC_CREAT);


		int pid_a, pid_b;

		pid_a = fork();
		
		if ( pid_a < 0 ) {
			printf("Error");
		} else if ( pid_a == 0 ) {
			// child A

			signal(SIGCONT, sig_cont);
			raise(SIGSTOP);
			
			//1
			printf("Child process ID: %d. \n", getpid());
			
			shmc = shmat(shmid, 0, 0);
			
			int child_b = *(shmc+2);

			*(shmc+2) = *(shmc) + *(shmc+1);
			//2
			printf ("Sum of differences: %d.\n", *(shmc+2));
			
			//3
			printf("Send a SIGCONT to process %d.\n\n", child_b);
			kill(child_b, SIGCONT);

			shmdt(shmc);
		} else {
			pid_b = fork();

			if ( pid_b < 0 ) {
				printf("Error");
			} else if ( pid_b == 0 ) {
				// child B

				kill(SIGSTOP, pid_a);

				signal(SIGCONT, sig_cont);
				raise(SIGSTOP);
			
				//1
				//printf("Receive a SIGCONT. \n");
				printf("Child process ID: %d. \n", getpid());

				//2
				shmc = shmat(shmid, 0, 0);
				if ( *(shmc+2) < 0 ) {
					printf ("The 3rd argument is smaller than the 1st argument.\n\n");
				} else if ( *(shmc+2) == 0 ) {
					printf ("The 3rd argument is equal to the 1st argument.\n\n");
				} else if ( *(shmc+2) > 0 ) {
					printf ("The 3rd argument is larger than the 1st argument.\n\n");
				}

				kill(SIGCONT, pid_a);

				shmdt(shmc);

			} else {
				//parent 
				sleep(1);
				// // pause A
				// kill(pid_a, SIGSTOP);
				// // pause B
				// kill(pid_b, SIGSTOP);

				// 1
				printf ("Apply %d bytes.\n",size_data);

				shmp = shmat (shmid, 0, 0);
				*(shmp) = atoi(argv[2]) - atoi(argv[1]);
				*(shmp+1) = atoi(argv[3]) - atoi(argv[2]);
				*(shmp+2) = pid_b;

				// 2 & 3
				printf("Parent process ID: %d.\nDifferences: %d, %d. \n", getpid(), *(shmp), *(shmp+1));
				printf("Send a SIGCONT to process %d.\n\n", pid_a);

				kill(pid_a, SIGCONT);
				int i = 0;
				// 4
				child = wait(&status);
				printf("Exited process ID: %d; Count: %d.\n", child,++i);

				child = wait(&status);
				printf("Exited process ID: %d; Count: %d.\n", child, ++i);

				printf("This is the end of the program.\n");

				shmdt(shmp);
				shmctl(shmid, IPC_RMID, NULL);
			}

			
		}
	}
	return 0;
} 




// Plase use the following lines to print related information if needed.
/*
sleep(1);  // which you may need for registration of signal handlers in child process, 
               //only allowd at the beginning of the parent process after fork();
printf ("Apply %d bytes.\n",***);
printf("Child process A ID: %d.\n", getpid());
printf("Child process B ID: %d.\n", getpid());
printf("Sum of differences: %d.\n\n", ***);
printf("Send a SIGCONT to Process %d.\n\n", ***); 
printf("Parent process ID: %d.\n");
printf("Differences: %d, %d.\n", ***,***);
printf("Send a SIGCONT to process %d.\n\n", ***);
printf("Exited Process ID: %d.\n", **); 
printf("Receive a SIGCONT.\n"); 
printf("Receive a SIGSTOP.\n"); 


printf("Exited Process ID: %d; Count: %d.\n", **, ***); 
printf("The 3rd argument is larger than the 1st argument.\n");
printf("The 3rd argument is smaller than the 1st argument.\n");
printf("The 3rd argument is equal to the 1st argument.\n");   
*/ 



