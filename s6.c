#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pwd.h>

void handleError(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

char *getPermissionsString(mode_t mode) {
    char *message = (char *)malloc(200 * sizeof(char)); // Alocare spațiu pentru mesajul complet
    if (message == NULL) {
        perror("Error allocating memory");
        exit(EXIT_FAILURE);
    }

    char p[10];
    p[0] = (mode & S_IRUSR) ? 'R' : '-';
    p[1] = (mode & S_IWUSR) ? 'W' : '-';
    p[2] = (mode & S_IXUSR) ? 'X' : '-';
    p[3] = (mode & S_IRGRP) ? 'R' : '-';
    p[4] = (mode & S_IWGRP) ? 'W' : '-';
    p[5] = (mode & S_IXGRP) ? 'X' : '-';
    p[6] = (mode & S_IROTH) ? 'R' : '-';
    p[7] = (mode & S_IWOTH) ? 'W' : '-';
    p[8] = (mode & S_IXOTH) ? 'X' : '-';
    p[9] = '\0';

    // Formatare mesaj în conformitate cu cerințele
    sprintf(message, "drepturi de acces user: %c%c%c\ndrepturi de acces grup: %c%c%c\ndrepturi de acces altii: %c%c%c",
            p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8]);

    return message;
}

int main(int argc, char *argv[]) {
    // Verifica daca programul a primit un singur argument
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <fisier_intrare>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Deschide fisierul BMP
    int bmpFile = open(argv[1], O_RDONLY);
    if (bmpFile == -1) {
        handleError("Error opening input file");
    }

    // Citeste header-ul BMP
    char header[54];
    if (read(bmpFile, header, sizeof(header)) != sizeof(header)) {
        handleError("Error reading BMP header");
    }

    // Extrage informatiile necesare
    int height = *(int*)&header[18];
    int width = *(int*)&header[22];
    off_t fileSize = lseek(bmpFile, 0, SEEK_END);
    uid_t userId = getuid();
    time_t modificationTime = time(NULL);

    // Inchide fisierul BMP
    close(bmpFile);

    // Deschide fisierul de statistica
    int statFile = open("statistica.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (statFile == -1) {
        handleError("Error opening statistic file");
    }

    // Scrie informatiile in fisierul de statistica
    char statBuffer[500];
    int statLength = sprintf(statBuffer, "nume fisier: %s\n", argv[1]);
    statLength += sprintf(statBuffer + statLength, "inaltime: %d\n", height);
    statLength += sprintf(statBuffer + statLength, "lungime: %d\n", width);
    statLength += sprintf(statBuffer + statLength, "dimensiune: %ld\n", (long)fileSize);
    statLength += sprintf(statBuffer + statLength, "identificatorul utilizatorului: %d\n", userId);
    statLength += sprintf(statBuffer + statLength, "timpul ultimei modificari: %s", ctime(&modificationTime));
    


    // Adauga contorul de legaturi
    struct stat fileStat;
    if (lstat(argv[1], &fileStat) == 0) {
        statLength += sprintf(statBuffer + statLength, "contorul de legaturi: %ld\n", fileStat.st_nlink);
    } else {
        statLength += sprintf(statBuffer + statLength, "contorul de legaturi: N/A\n");
    }
    
    statLength += sprintf(statBuffer + statLength, "%s", getPermissionsString(fileStat.st_mode));
 

    // Scrie in fisier
    if (write(statFile, statBuffer, statLength) != statLength) {
        handleError("Error writing to statistic file");
    }

    // Inchide fisierul de statistica
    close(statFile);

    return 0;
}