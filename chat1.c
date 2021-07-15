#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>


int pid;

int main(int argc,char* argv[]){
	
printf("You are USER1. Welcome in chat with USER2\n press M to start writing and press enter to submit it or use ESC to exit\n");
	
	pid = fork(); //podzial na dwa procesy(rodzic,dziecko)
	
	if(pid == 0){
		//Proces dziecko
		int fd;
		char message [30];
		char messageEnd[]="USER2 HUNG UP!";
		
		//tworzenie kolejki wiadosmosci z U2 do U1
		if(mkfifo("U2toU1",0777) == -1){
			if(errno != EEXIST) {
				//jesli error jest inny niz taki, ze kolejka juz istnieje
				printf("%d", errno);
				return 1;
			}
		}
		
		while(1){
			// w petli chcemy odczytywac przychodzace wiadomosci
			fd = open("U2toU1",O_RDONLY); //otwieramy odpowiedni plik do czytania
			read(fd,&message, sizeof(message)); // czytamy wiadomosc
			printf("USER2: %s\n",message); // wypisujemy ja
			close(fd); // zamykamy pipe
            int check =1;
            int i=0;	
            while(message[i]!='\0'){ // sprawdzamy czy wiadomosc nie oznacza rozlaczenia sie drugiego uzytkownika
                if(message[i]!=messageEnd[i]){
                    check=0;
                    break;
                }
                i++;
            }
			if(check){
				// jesli tak, zabijamy proces rodzica i konczymy zwracamy 0 przy wyjsciu z programu
				kill (getppid(), SIGTERM);
				return 0;
			}		
		}	
	}

	else{
		// Proces rodzic
		int fd1;
		char choice = ' ';
		char messageFrom [30];
		
		//tworzenie kolejki wiadosmosci z U1 do U2
		
		if(mkfifo("U1toU2",0777) == -1){
			if(errno != EEXIST) {
				return 1;
			}
		}	

		while(1)
		{
			// wybor trybu przez uzytkownika
			system("stty -echo"); // tlumimy echo w konsoli aby nie bylo widac klikanej opcji(klawisz m lub esc)
			system("stty cbreak"); // przechodzimy do trybu RAW
			choice = getchar(); // pobieramy wcisnieta opcje
			system ("stty echo"); // przywracamy echo
			system("stty -cbreak");// do trybu COOKED
			if(choice == 'm')
			{
				printf("\nUSER1: ");
				scanf("%[^\n]s",messageFrom); //pobieramy wpisana wiadomosc
				printf("\n");
				fd1 = open("U1toU2", O_WRONLY); // otwieramy pipe do pisania
				write(fd1,&messageFrom,sizeof(messageFrom)); // piszemy do niego wiadomosc	
				close(fd1);	 //zamykamy


			}
			else if((int)choice == 27)
			{
				// wysylamy wiadomosc o rozlaczeniu sie
				char messageFrom[]="USER1 HUNG UP!";
				fd1 = open("U1toU2", O_WRONLY);
				write(fd1,&messageFrom,sizeof(messageFrom));			
				close(fd1);	
				//zabijamy proces dziecko
				kill(pid,SIGTERM);
				unlink("U1toU2"); //usuwamy pliki z kolejkami fifo
				unlink("U2toU1");
				break;
			}
		}
	}	
	
	return 0;
}
