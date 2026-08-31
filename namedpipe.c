#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define FIFO_FILE "/tmp/my_fifo"
#define FILE_NAME "hello.txt"

void server(void);
void client(void);

int main()
{
    pid_t pid;

    // Create FIFO if it does not exist
    mkfifo(FIFO_FILE, 0666);

    pid = fork();

    if (pid > 0)
    {
        // Parent process acts as the server
        sleep(1);
        server();
    }
    else if (pid == 0)
    {
        // Child process acts as the client
        client();
    }
    else
    {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    return 0;
}

// Server: Reads from hello.txt and writes to FIFO
void server()
{
    int fifo_fd, file_fd;
    char buffer[1024];
    ssize_t bytes_read;

    // Open the file to read
    file_fd = open(FILE_NAME, O_RDONLY);

    if (file_fd == -1)
    {
        perror("Error opening hello.txt");
        exit(EXIT_FAILURE);
    }

    // Open FIFO for writing
    fifo_fd = open(FIFO_FILE, O_WRONLY);

    if (fifo_fd == -1)
    {
        perror("Error opening FIFO");
        close(file_fd);
        exit(EXIT_FAILURE);
    }

    // Read file and write to FIFO
    while ((bytes_read = read(file_fd, buffer, sizeof(buffer))) > 0)
    {
        write(fifo_fd, buffer, bytes_read);
    }

    close(file_fd);
    close(fifo_fd);
}

// Client: Reads from FIFO and prints the content
void client()
{
    int fifo_fd;
    char buffer[1024];
    ssize_t bytes_read;

    // Open FIFO for reading
    fifo_fd = open(FIFO_FILE, O_RDONLY);

    if (fifo_fd == -1)
    {
        perror("Error opening FIFO");
        exit(EXIT_FAILURE);
    }

    // Read data from FIFO and print
    while ((bytes_read = read(fifo_fd, buffer, sizeof(buffer))) > 0)
    {
        write(STDOUT_FILENO, buffer, bytes_read);
    }

    close(fifo_fd);
}
