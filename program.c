#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BMP_HEADER_SIZE 54

void display_usage() {
    write(STDERR_FILENO, "Usage: ./program <director_intrare>\n", 37);
    exit(EXIT_FAILURE);
}

void write_file_info(const char *filename, struct stat *file_stat) {
    int fd_stat = open("statistica.txt", O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if (fd_stat == -1) {
        perror("Error opening statistica.txt");
        exit(EXIT_FAILURE);
    }

    char buffer[256];

    // Nume fisier sau director
    sprintf(buffer, "nume: %s\n", filename);
    write(fd_stat, buffer, strlen(buffer));

    // Dimensiune (doar pentru fisiere)
    if (S_ISREG(file_stat->st_mode)) {
        sprintf(buffer, "dimensiune: %ld\n", (long)file_stat->st_size);
        write(fd_stat, buffer, strlen(buffer));
    }

    // Identificator utilizator
    sprintf(buffer, "identificatorul utilizatorului: %d\n", file_stat->st_uid);
    write(fd_stat, buffer, strlen(buffer));

    // Timpul ultimei modificari
    struct tm *tm_info = localtime(&file_stat->st_mtime);
    strftime(buffer, sizeof(buffer), "timpul ultimei modificari: %d.%m.%Y\n", tm_info);
    write(fd_stat, buffer, strlen(buffer));

    // Contor de legaturi
    sprintf(buffer, "contorul de legaturi: %ld\n", (long)file_stat->st_nlink);
    write(fd_stat, buffer, strlen(buffer));

    // Drepturi de acces
    sprintf(buffer, "drepturi de acces user: %c%c%c\n",
            (file_stat->st_mode & S_IRUSR) ? 'R' : '-',
            (file_stat->st_mode & S_IWUSR) ? 'W' : '-',
            (file_stat->st_mode & S_IXUSR) ? 'X' : '-');
    write(fd_stat, buffer, strlen(buffer));

    sprintf(buffer, "drepturi de acces grup: %c%c%c\n",
            (file_stat->st_mode & S_IRGRP) ? 'R' : '-',
            (file_stat->st_mode & S_IWGRP) ? 'W' : '-',
            (file_stat->st_mode & S_IXGRP) ? 'X' : '-');
    write(fd_stat, buffer, strlen(buffer));

    sprintf(buffer, "drepturi de acces altii: %c%c%c\n",
            (file_stat->st_mode & S_IROTH) ? 'R' : '-',
            (file_stat->st_mode & S_IWOTH) ? 'W' : '-',
            (file_stat->st_mode & S_IXOTH) ? 'X' : '-');
    write(fd_stat, buffer, strlen(buffer));

    close(fd_stat);
}

void process_directory(const char *dir_path) {
    DIR *dir = opendir(dir_path);
    if (!dir) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') {
            continue;  // Ignora intrarile "." si ".."
        }

        char entry_path[PATH_MAX];
        snprintf(entry_path, sizeof(entry_path), "%s/%s", dir_path, entry->d_name);

        struct stat file_stat;
        if (lstat(entry_path, &file_stat) == -1) {
            perror("Error getting file information");
            exit(EXIT_FAILURE);
        }

        if (S_ISREG(file_stat.st_mode) && strstr(entry->d_name, ".bmp")) {
            write_file_info(entry->d_name, &file_stat);
        } else if (S_ISREG(file_stat.st_mode)) {
            write_file_info(entry->d_name, &file_stat);
        } else if (S_ISDIR(file_stat.st_mode)) {
            int fd_stat = open("statistica.txt", O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
            if (fd_stat == -1) {
                perror("Error opening statistica.txt");
                exit(EXIT_FAILURE);
            }

            char buffer[256];

            // Nume director
            sprintf(buffer, "nume director: %s\n", entry->d_name);
            write(fd_stat, buffer, strlen(buffer));

            // Identificator utilizator
            sprintf(buffer, "identificatorul utilizatorului: %d\n", file_stat.st_uid);
            write(fd_stat, buffer, strlen(buffer));

            // Drepturi de acces
            sprintf(buffer, "drepturi de acces user: RWX\n");
            write(fd_stat, buffer, strlen(buffer));

            sprintf(buffer, "drepturi de acces grup: R--\n");
            write(fd_stat, buffer, strlen(buffer));

            sprintf(buffer, "drepturi de acces altii: ---\n");
            write(fd_stat, buffer, strlen(buffer));

            close(fd_stat);
        } else if (S_ISLNK(file_stat.st_mode)) {
            char target_path[PATH_MAX];
            ssize_t target_len = readlink(entry_path, target_path, sizeof(target_path) - 1);
            if (target_len == -1) {
                perror("Error reading symbolic link");
                exit(EXIT_FAILURE);
            }
            target_path[target_len] = '\0';

            int fd_stat = open("statistica.txt", O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
            if (fd_stat == -1) {
                perror("Error opening statistica.txt");
                exit(EXIT_FAILURE);
            }

            char buffer[256];

            // Nume legatura
            sprintf(buffer, "nume legatura: %s\n", entry->d_name);
            write(fd_stat, buffer, strlen(buffer));

            // Dimensiune legatura
            sprintf(buffer, "dimensiune: %ld\n", (long)target_len);
            write(fd_stat, buffer, strlen(buffer));

            // Dimensiune fisier target
            struct stat target_stat;
            if (lstat(target_path, &target_stat) == -1) {
                perror("Error getting target file information");
                exit(EXIT_FAILURE);
            }
            sprintf(buffer, "dimensiune fisier: %ld\n", (long)target_stat.st_size);
            write(fd_stat, buffer, strlen(buffer));

            // Drepturi de acces
            sprintf(buffer, "drepturi de acces user: RWX\n");
            write(fd_stat, buffer, strlen(buffer));

            sprintf(buffer, "drepturi de acces grup: R--\n");
            write(fd_stat, buffer, strlen(buffer));

            sprintf(buffer, "drepturi de acces altii: ---\n");
            write(fd_stat, buffer, strlen(buffer));

            close(fd_stat);
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        display_usage();
    }

    const char *dir_path = argv[1];

    process_directory(dir_path);

    return 0;
}