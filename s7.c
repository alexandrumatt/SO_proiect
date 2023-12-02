#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pwd.h>
#include <dirent.h>
#include <string.h>

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
    p[0] = (S_IRUSR) ? 'R' : '-';
    p[1] = (S_IWUSR) ? 'W' : '-';
    p[2] = (S_IXUSR) ? 'X' : '-';
    p[3] = (S_IRGRP) ? 'R' : '-';
    p[4] = (S_IWGRP) ? 'W' : '-';
    p[5] = (S_IXGRP) ? 'X' : '-';
    p[6] = (S_IROTH) ? 'R' : '-';
    p[7] = (S_IWOTH) ? 'W' : '-';
    p[8] = (mode & S_IXOTH) ? 'X' : '-';
    p[9] = '\0';

    // Formatare mesaj în conformitate cu cerințele
    sprintf(message, "drepturi de acces user: %c%c%c\ndrepturi de acces grup: %c%c%c\ndrepturi de acces altii: %c%c%c\n",
            p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8]);
    

    return message;
}

void processFile(const char *filePath, const char *filePath2) {
    // Deschide fisierul BMP
    int bmpFile = open(filePath, O_RDONLY);
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
    int statFile = open("statistica.txt", O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if (statFile == -1) {
        handleError("Error opening statistic file");
    }

    // Scrie informatiile in fisierul de statistica
    char statBuffer[500];
    int statLength = sprintf(statBuffer, "nume fisier: %s\n", filePath2);
    statLength += sprintf(statBuffer + statLength, "inaltime: %d\n", height);
    statLength += sprintf(statBuffer + statLength, "lungime: %d\n", width);
    statLength += sprintf(statBuffer + statLength, "dimensiune: %ld\n", (long)fileSize);
    statLength += sprintf(statBuffer + statLength, "identificatorul utilizatorului: %d\n", userId);
    statLength += sprintf(statBuffer + statLength, "timpul ultimei modificari: %s", ctime(&modificationTime));
    
    // Adauga contorul de legaturi
    struct stat fileStat;
    if (lstat(filePath, &fileStat) == 0) {
        statLength += sprintf(statBuffer + statLength, "contorul de legaturi: %ld\n", fileStat.st_nlink);
    } else {
        statLength += sprintf(statBuffer + statLength, "contorul de legaturi: N/A\n");
    }

    statLength += sprintf(statBuffer + statLength, "%s\n", getPermissionsString(fileStat.st_mode));

    // Scrie in fisier
    if (write(statFile, statBuffer, statLength) != statLength) {
        handleError("Error writing to statistic file");
    }

    // Inchide fisierul de statistica
    close(statFile);
}

void processFile2 (const char *filePath, const char *filePath2) {
    // Deschide fisierul BMP
    int bmpFile = open(filePath, O_RDONLY);
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
    int statFile = open("statistica.txt", O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if (statFile == -1) {
        handleError("Error opening statistic file");
    }

    // Scrie informatiile in fisierul de statistica
    char statBuffer[500];
    int statLength = sprintf(statBuffer, "nume fisier: %s\n", filePath2);
    statLength += sprintf(statBuffer + statLength, "dimensiune: %ld\n", (long)fileSize);
    statLength += sprintf(statBuffer + statLength, "identificatorul utilizatorului: %d\n", userId);
    statLength += sprintf(statBuffer + statLength, "timpul ultimei modificari: %s", ctime(&modificationTime));
    
    // Adauga contorul de legaturi
    struct stat fileStat;
    if (lstat(filePath, &fileStat) == 0) {
        statLength += sprintf(statBuffer + statLength, "contorul de legaturi: %ld\n", fileStat.st_nlink);
    } else {
        statLength += sprintf(statBuffer + statLength, "contorul de legaturi: N/A\n");
    }

    statLength += sprintf(statBuffer + statLength, "%s\n", getPermissionsString(fileStat.st_mode));

    // Scrie in fisier
    if (write(statFile, statBuffer, statLength) != statLength) {
        handleError("Error writing to statistic file");
    }

    // Inchide fisierul de statistica
    close(statFile);
}


void processSymbolicLink(const char *linkPath, const char *linkPath2) {

    struct stat linkStat;
    if (lstat(linkPath, &linkStat) == -1) {
        handleError("Error getting symbolic link information");
    }

    char targetPath[500];
    ssize_t targetSize = readlink(linkPath, targetPath, sizeof(targetPath) - 1);
    if (targetSize == -1) {
        handleError("Error reading symbolic link target");
    }
    targetPath[targetSize] = '\0';

    struct stat targetStat;
    if (stat(targetPath, &targetStat) == -1) {
        handleError("Error getting target file information");
    }

    // Deschide fisierul de statistica
    int statFile = open("statistica.txt", O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if (statFile == -1) {
        handleError("Error opening statistic file");
    }

    // Scrie informatiile in fisierul de statistica
    char statBuffer[500];
    int statLength = sprintf(statBuffer, "nume legatura: %s\n", linkPath2);
    statLength += sprintf(statBuffer + statLength, "dimensiune: %ld bytes\n", (long)linkStat.st_size);
    statLength += sprintf(statBuffer + statLength, "dimensiune fisier: %ld bytes\n", (long)targetStat.st_size);
    statLength += sprintf(statBuffer + statLength, "%s\n", getPermissionsString(linkStat.st_mode));

    // Scrie in fisier
    if (write(statFile, statBuffer, statLength) != statLength) {
        handleError("Error writing to statistic file");
    }

    // Inchide fisierul de statistica
    close(statFile);
}


void Director2 (const char *dirPath) {
    
    // Deschide fisierul de statistica
    int statFile = open("statistica.txt", O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if (statFile == -1) {
        handleError("Error opening statistic file");
    }

    // Scrie informatiile in fisierul de statistica
    char statBuffer[500];
    uid_t userId = getuid();
    struct stat dirStat;
    int statLength = sprintf(statBuffer, "nume director: %s\n", dirPath);
    statLength += sprintf(statBuffer + statLength, "identificatorul utilizatorului: %d\n", userId);
    statLength += sprintf(statBuffer + statLength, "%s\n", getPermissionsString(dirStat.st_mode));

    // Scrie in fisier
    if (write(statFile, statBuffer, statLength) != statLength) {
        handleError("Error writing to statistic file");
    }

    // Inchide fisierul de statistica
    close(statFile);
}


void processDirectory(const char *dirPath) {
    printf( "Directorul primit ca arg: %s\n", dirPath); // directorul d1, d2, d3 etc
    
    DIR *dir = opendir(dirPath); // deschide directorul primit ca parametru
    

    if (dir == NULL) {
        handleError("Directorul este gol.\n");
    
    }



    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {  // Daca este un fisier obisnuit
            if (strstr(entry->d_name, ".bmp") != NULL) { // si are extensia .bmp
                char filePath[500];
                char filePath2[500];
                sprintf(filePath, "%s/%s", dirPath, entry->d_name);
                sprintf(filePath2, "%s", entry->d_name);
                printf("Cale fisier: %s\n",filePath);
                printf("Fisier: %s\n",filePath2);
                processFile(filePath, filePath2);
            }
            
            else {  // orice alt fisier obisnuit
                char filePath[500];
                char filePath2[500];
                sprintf(filePath, "%s/%s", dirPath, entry->d_name);
                sprintf(filePath2, "%s", entry->d_name);
                printf("Cale fisier: %s\n",filePath);
                printf("Fisier: %s\n",filePath2);
                processFile2(filePath, filePath2);
            }
        } 
        
        if (entry->d_type == DT_DIR) {  // Daca este un director
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                char dirPath[500];          
                sprintf(dirPath, "%s", entry->d_name);
                printf("Directorul gasit: %s\n",dirPath);
                Director2(dirPath);
                processDirectory(dirPath);
            }
        }

        else{
                char linkPath[500];
                char linkPath2[500];
                sprintf(linkPath, "%s/%s", dirPath, entry->d_name);
                sprintf(linkPath2, "%s", entry->d_name);
                
                struct stat fileStat;
                if (lstat(linkPath, &fileStat) == 0 && S_ISLNK(fileStat.st_mode)) {
                    printf("Cale fisier: %s\n",linkPath);
                    printf("Legatura simbolica: %s\n",linkPath2);

                    processSymbolicLink(linkPath, linkPath2);
                    
                } 

        }
    
    }



    closedir(dir);
}

int main(int argc, char *argv[]) {
    // Verifica daca programul a primit un singur argument
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <director_intrare>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Proceseaza directorul
    processDirectory(argv[1]);

    return 0;
}
