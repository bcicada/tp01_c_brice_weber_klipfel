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
#include <fcntl.h>

int main(int argc, char *argv[]) {
    int fd1, fd2;
    char buffer[1024];
    ssize_t nread;

    if (argc < 3) {
        printf("Usage: %s file1 file2\n", argv[0]);
        exit(1);
    }

    /* Open file1 for reading */
    fd1 = open(argv[1], O_RDONLY);
    if (fd1 == -1) {
        perror("open");
        exit(1);
    }

    /* Create file2 */
    fd2 = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd2 == -1) {
        perror("open");
        close(fd1);
        exit(1);
    }

    /* Copy the contents of file1 to file2 */
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

    return 0;
}
