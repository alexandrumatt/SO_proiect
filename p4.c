#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <limits.h>

#define BMP_HEADER_SIZE 54

void display_usage() {
    write(STDERR_FILENO, "Usage: ./program <director_intrare> <director_iesire> <c>\n", 57);
    exit(EXIT_FAILURE);
}

void write_file_info(const char *filename, struct stat *file_stat, const char *output_path) {
    int fd_stat = open(output_path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    printf(fd_stat, "\n");
    if (fd_stat == -1) {
        perror("Error 1 opening output file");
        exit(EXIT_FAILURE);
    }

    // Scrie informațiile în fișierul de statistică
    dprintf(fd_stat, "nume fisier: %s\n", filename);
    dprintf(fd_stat, "inaltime: %ld\n", (long)file_stat->st_size);
    dprintf(fd_stat, "lungime: %ld\n", (long)file_stat->st_mtime);
    dprintf(fd_stat, "dimensiune: %lld\n", (long long)file_stat->st_size);
    dprintf(fd_stat, "identificatorul utilizatorului: %d\n", file_stat->st_uid);
    dprintf(fd_stat, "timpul ultimei modificari: %ld\n", (long)file_stat->st_mtime);
    dprintf(fd_stat, "contorul de legaturi: %ld\n", (long)file_stat->st_nlink);
    dprintf(fd_stat, "drepturi de acces user: %o\n", file_stat->st_mode & S_IRWXU);
    dprintf(fd_stat, "drepturi de acces grup: %o\n", file_stat->st_mode & S_IRWXG);
    dprintf(fd_stat, "drepturi de acces altii: %o\n", file_stat->st_mode & S_IRWXO);

    close(fd_stat);
}

void convert_to_grayscale(const char *input_path, const char *output_path) {
    int input_fd = open(input_path, O_RDONLY);
    if (input_fd == -1) {
        perror("Error 2 opening input file");
        exit(EXIT_FAILURE);
    }

    int output_fd = open(output_path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    printf(output_fd, "\n");
    if (output_fd == -1) {
        perror("Error 3 opening output file");
        close(input_fd);
        exit(EXIT_FAILURE);
    }

    char buffer[BMP_HEADER_SIZE];
    if (read(input_fd, buffer, BMP_HEADER_SIZE) != BMP_HEADER_SIZE) {
        perror("Error 4 reading BMP header");
        close(input_fd);
        close(output_fd);
        exit(EXIT_FAILURE);
    }

    write(output_fd, buffer, BMP_HEADER_SIZE); // Copy BMP header

    ssize_t read_bytes;
    while ((read_bytes = read(input_fd, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 2; i < read_bytes; i += 3) {
            // Convert to grayscale using the formula
            buffer[i - 2] = buffer[i - 1] = buffer[i] =
                (char)(0.299 * buffer[i - 2] + 0.587 * buffer[i - 1] + 0.114 * buffer[i]);
        }
        write(output_fd, buffer, read_bytes);
    }

    if (read_bytes == -1) {
        perror("Error 5 reading input file");
        close(input_fd);
        close(output_fd);
        exit(EXIT_FAILURE);
    }

    close(input_fd);
    close(output_fd);
}

void process_entry(const char *input_path, const char *output_dir, char character) {
    struct stat file_stat;
    if (lstat(input_path, &file_stat) == -1) {
        perror("Error 6 getting file information");
        exit(EXIT_FAILURE);
    }

    char output_path[PATH_MAX];
    snprintf(output_path, sizeof(output_path), "%s/%s_statistica.txt", output_dir, input_path);

    pid_t pid = fork();
    if (pid == -1) {
        perror("Error 7 forking process");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {  // Child process
        if (S_ISREG(file_stat.st_mode) && strstr(input_path, ".bmp")) {
            convert_to_grayscale(input_path, output_path);
        } else if (S_ISREG(file_stat.st_mode) && !strstr(input_path, ".bmp")) {
            int fd_pipe[2];
            if (pipe(fd_pipe) == -1) {
                perror("Error 8 creating pipe");
                exit(EXIT_FAILURE);
            }

            pid_t child_pid = fork();
            if (child_pid == -1) {
                perror("Error 9 forking process for file content");
                exit(EXIT_FAILURE);
            } else if (child_pid == 0) {  // Child process for file content
                close(fd_pipe[0]);  // Close read end
                dup2(fd_pipe[1], STDOUT_FILENO);
                close(fd_pipe[1]);

                // Execute the command to get file content
                execlp("cat", "cat", input_path, (char *)NULL);
                perror("Error 10 executing command");
                exit(EXIT_FAILURE);
            } else {  // Parent process
                close(fd_pipe[1]);  // Close write end

                char buffer[4096];
                ssize_t read_bytes;
                int total_sentences = 0;

                while ((read_bytes = read(fd_pipe[0], buffer, sizeof(buffer))) > 0) {
                    // Count correct sentences
                    char *token = strtok(buffer, ".!?");
                    while (token != NULL) {
                        int valid = 1;
                        for (int i = 0; i < strlen(token); i++) {
                            if (!isalnum(token[i]) && !isspace(token[i]) && token[i] != ',' && token[i] != '.' &&
                                token[i] != '!' && token[i] != '?') {
                                valid = 0;
                                break;
                            }
                        }
                        if (valid && (token[strlen(token) - 1] == '.' || token[strlen(token) - 1] == '!' ||
                                      token[strlen(token) - 1] == '?') &&
                            !strstr(token, ",")) {
                            total_sentences++;
                        }
                        token = strtok(NULL, ".!?");
                    }
                }

                close(fd_pipe[0]);

                printf("Au fost identificate in total %d propozitii corecte care contin caracterul %c\n", total_sentences, character);

                int status;
                waitpid(child_pid, &status, 0);
                printf("S-a încheiat procesul cu pid-ul %d și codul %d\n", child_pid, status);
            }
        }
        write_file_info(input_path, &file_stat, output_path);
        exit(EXIT_SUCCESS);
    } else {  // Parent process
        int status;
        waitpid(pid, &status, 0);

        if (S_ISREG(file_stat.st_mode) && strstr(input_path, ".bmp")) {
            printf("S-a încheiat procesul cu pid-ul %d și codul %d\n", pid, status);
        }
    }
}

void process_directory(const char *input_dir, const char *output_dir, char character) {
    DIR *dir = opendir(input_dir);
    if (!dir) {
        perror("Error 11 opening directory");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') {
            continue;  // Ignora intrarile "." si ".."
        }

        char entry_path[PATH_MAX];
        snprintf(entry_path, sizeof(entry_path), "%s/%s", input_dir, entry->d_name);

        process_entry(entry_path, output_dir, character);
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        display_usage();
    }

    const char *input_dir = argv[1];
    const char *output_dir = argv[2];
    char character = argv[3][0];

    process_directory(input_dir, output_dir, character);

    return 0;
}