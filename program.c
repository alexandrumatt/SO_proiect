#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h> 
#include <unistd.h> 



int main ( int argc, char **argv )
{
    /* verificare argumente*/
    if (argc!= 2){
        
        printf("Usage ./program <fisier_intrare>");
        exit(1);
    }


    /*deschidere fisier*/
    int fd = open(argv[1], O_RDONLY);

     if (fd == -1) { 
        printf("Fisierul de tip imagine nu a fost deschis."); 
  
        // print program detail "Success or failure" 
        perror("erare");
        close(fd);
        exit(1);
    } 



    printf("Nr arg:");
    printf("fd = %d\n", fd); 


    char sir1[500];
       
    printf("opened the fd = %d\n", fd1); 

    sprintf(sir1, "nume fisier: %d\n", mari); 
        
    
 

    int long sz;  
    
    /*scriere fisier iesire*/
    int fd2 = open("statistica.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);  
        if (fd2 < 0) {  
            perror("eroare");  
            exit(1);  
        }  
            
    
    write(fd2, sir1, strlen(sir1)); 
      
    /*inchidere fisiere*/
    close(fd);
    close(fd2);  


}


/*

    int fd = open("intrare.txt", O_RDONLY | O_CREAT); 
  

    printf("Nr arg:");
    printf("fd = %d\n", fd); 
  
    if (fd == -1) { 
        // print which type of error have in a code 
        printf("Error Number % d\n", errno); 
  
        // print program detail "Success or failure" 
        perror("Program"); 
    } 

    if(fd == 3) {
        int fd1 = open("intrare.txt", O_RDONLY); 
        if (fd1 < 0) { 
            perror("c1"); 
            exit(1); 
        } 


       struct stat fileStat;
        if (fstat(fd1, &fileStat)==-1)
        {
            perror("erare");
            close(fd1);
            exit(1);
        }

        char x;
        char sir1[80];
        char sir2[80];
        char sir3[80];
        char sir4[80];
        char sir5[80];
        x=argv[3];
        printf("Caracterul %c", x);
        char c; 
        int mari = 0; 
        int mici = 0;
        int cifre = 0;
        int ap=0;
        printf("opened the fd = %d\n", fd1); 

        int e = read(fd1, &c, 1); 
        while(e>0){
            printf("c = %c\n", c); 
            if(c >= 'A' && c <= 'Z')
                mari ++;
            if(c >= 'a' && c <= 'z')
                mici ++;
            if('0' <= c && c <= '9')
                cifre ++;
            if(c==argv[3][0])
                ap++;
            e = read(fd1, &c, 1); 

        }

        printf("mari = %d\n", mari); 
        printf("mici = %d\n", mici); 
        printf("cifre = %d\n", cifre);
        printf("aparitii = %d\n", ap);
        printf("dimens fisier = %d\n", fileStat.st_size);

        sprintf(sir1, "mari = %d\n", mari); 
        sprintf(sir2, "mari = %d\n", mici); 
        sprintf(sir3, "cifre = %d\n", cifre); 
        sprintf(sir4, "aparitii = %d\n", ap); 
        sprintf(sir5, "dimens fisier = %d\n", fileStat.st_size); 

        int long sz;  
            
        int fd2 = open("iesire.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);  
        if (fd2 < 0) {  
            perror("r1");  
            exit(1);  
        }  
            
        write(fd2, sir1, strlen(sir1)); 
        write(fd2, sir2, strlen(sir2));   
        write(fd2, sir3, strlen(sir3));  
        write(fd2, sir4, strlen(sir4)); 
        write(fd2, sir5, strlen(sir5));
      


        close(fd2);  



        


        // Using close system Call 
        if (close(fd1) < 0) { 
            perror("c1"); 
            exit(1); 
        } 
        printf("closed the fd.\n"); 
    }

    return 0; 

}

*/


