#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>


int pid;

int main(int argc,char* argv[]){
	
printf("You are USER2. Welcome in chat with USER1\n press M to start writing and press enter to submit it or use ESC to exit\n");
	
	pid = fork();
	
	if(pid == 0){
		int fd;
		char message [30];
		char messageEnd[]="USER1 HUNG UP!";

		if(mkfifo("U1toU2",0777) == -1){
			if(errno != EEXIST) {
				printf("%d", errno);
				return 1;
			}
		}
		
		while(1){
			
			fd = open("U1toU2",O_RDONLY);
			read(fd,&message, sizeof(message));
			printf("USER1: %s\n",message);
			close(fd);
            int check =1;
            int i=0;	
            while(message[i]!='\0'){
                if(message[i]!=messageEnd[i]){
                    check=0;
                    break;
                }
                i++;
            }
			if(check){

				kill (getppid(), SIGTERM);
				return 0;
			}
		}	
	}

	else{
	
		int fd1;
		char choice = ' ';
		char messageFrom [30];
	
		if(mkfifo("U2toU1",0777) == -1){
			if(errno != EEXIST) {
				return 1;
			}
		}	

		while(1)
		{
			system("stty -echo"); 
			system("stty cbreak"); 
			choice = getchar();
			system ("stty echo");
			system("stty -cbreak");
			if(choice == 'm')
			{
				printf("\nUSER2: ");
				scanf("%[^\n]s",messageFrom);
				printf("\n");
				fd1 = open("U2toU1", O_WRONLY);
				write(fd1,&messageFrom,sizeof(messageFrom));			
				close(fd1);	

			}
			else if((int)choice == 27)
			{

				char messageFrom[]="USER2 HUNG UP!";
				fd1 = open("U2toU1", O_WRONLY);
				write(fd1,&messageFrom,sizeof(messageFrom));			
				close(fd1);	
				kill(pid,SIGTERM);
				unlink("U1toU2");
				unlink("U2toU1");
				break;
			}

		}
	}	
	
	return 0;
}
