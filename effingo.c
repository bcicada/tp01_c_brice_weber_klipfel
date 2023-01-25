/**
 * \file effingo.c
 * \brief A program for copying from an X file to a Y file.
 * \author Brice W. K. <b.cicada@protonmail.com>
 * \version 0.1
 * \date 21 sept. 2022
 *
 * A program for copying from an X file to a Y file in c file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <pwd.h> 
#include <grp.h> 


void list_files(char *path) {
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    struct passwd *pwd;
    struct group *grp;
    char entrypath[1024];

    if (!(dir = opendir(path))) {
        perror("opendir");
        exit(1);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        sprintf(entrypath, "%s/%s", path, entry->d_name);
        if (stat(entrypath, &statbuf) == -1) {
            perror("stat");
            continue;
        }

        /* Print file name */
        printf("%s", entry->d_name);

        /* Print permissions */
        printf(" %c", (S_ISDIR(statbuf.st_mode)) ? 'd' : '-');
        printf("%c", (statbuf.st_mode & S_IRUSR) ? 'r' : '-');
        printf("%c", (statbuf.st_mode & S_IWUSR) ? 'w' : '-');
        printf("%c", (statbuf.st_mode & S_IXUSR) ? 'x' : '-');
        printf("%c", (statbuf.st_mode & S_IRGRP) ? 'r' : '-');
        printf("%c", (statbuf.st_mode & S_IWGRP) ? 'w' : '-');
        printf("%c", (statbuf.st_mode & S_IXGRP) ? 'x' : '-');
        printf("%c", (statbuf.st_mode & S_IROTH) ? 'r' : '-');
        printf("%c", (statbuf.st_mode & S_IWOTH) ? 'w' : '-');
        printf("%c", (statbuf.st_mode & S_IXOTH) ? 'x' : '-');

        /* Print owner and group name */
        pwd = getpwuid(statbuf.st_uid);
        printf(" %s", pwd->pw_name);
        grp = getgrgid(statbuf.st_gid);
        printf(" %s", grp->gr_name);

        /* Print file size */
        printf(" %ld", statbuf.st_size);

        /* Print file modification time */
        char timestr[20];
        strftime(timestr, sizeof(timestr), "%b %d %H:%M", localtime(&statbuf.st_mtime));
        printf(" %s \n", timestr);

    }

    closedir(dir);
}



int main(int argc, char *argv[]) {
    int fd1, fd2;
    char buffer[1024];
    ssize_t nread;
    int i;

    if (argc == 2) {
        /* Open file for reading */
        fd1 = open(argv[1], O_RDONLY);
        if (fd1 == -1) {
            perror("open");
            exit(1);
        }

        /* Read the contents of file */
        printf("Lecture du fichier : %s \n", argv[1]);
        while ((nread = read(fd1, buffer, sizeof buffer)) > 0) {
            /* Print the contents in reverse order */
            for (i = nread - 1; i >= 0; i--) {
                write(STDOUT_FILENO, &buffer[i], sizeof(char));
            }
        }
        putchar('\n');

        /* Close the file */
        close(fd1);
    } else if (strcmp(argv[1],"-list") == 0)
    {
        list_files(argv[2]);
    } else if (argc == 3) {
        /* Open file1 for reading */
        printf("Lecture du fichier : %s \n", argv[1]);
        fd1 = open(argv[1], O_RDONLY);
        if (fd1 == -1) {
            perror("open");
            exit(1);
        }

        /* Create file2 */
        printf("Création du fichier : %s \n", argv[2]);
        fd2 = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, 0644);
        if (fd2 == -1) {
            perror("open");
            close(fd1);
            exit(1);
        }

        /* Copy the contents of file1 to file2 */
        printf("Copie du fichier : %s vers %s \n", argv[1], argv[2]);
        while ((nread = read(fd1, buffer, sizeof buffer)) > 0) {
            if (write(fd2, buffer, nread) != nread) {
                perror("write");
                close(fd1);
                close(fd2);
                exit(1);
            }
        }

        /* Close both files */
        close(fd1);
        close(fd2);
    } else {
        printf("[ERROR] Usage: %s file1 [file2]\n", argv[0]);
        exit(1);
    }

    return 0;
}
