#include <iostream>
#include <string.h>
#include <unistd.h>
#include <cstdlib>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h> // Include the header for waitpid

#define TOKENSIZE 100

using namespace std;

void StrTokenizer(char *line, char **argv);
void myExecvp(char **argv);
void saveCommand(const char *command);
void displayHistory();
void executeCommand(char **argv);

char history[10][250];
int historyCount = 0;

int main()
{
    char *path2;
    char *arr[250];
    char *Tokenized;
    char input[250];
    char *argv[TOKENSIZE];

    while (true)
    {
        cout << "cwushell-> ";
        cin.getline(input, 250);

        // Save command to history
        saveCommand(input);

        StrTokenizer(input, argv);
        if (strcmp(argv[0], "exit") == 0)
        {
            break;
        }
        else if (strcmp(argv[0], "history") == 0)
        {
            displayHistory();
            continue;
        }
        executeCommand(argv);
    }
    return 0;
}

void executeCommand(char **argv)
{
    if (strcmp(argv[0], "ls") == 0)
    {
        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir(".")) != NULL)
        {
            while ((ent = readdir(dir)) != NULL)
            {
                cout << ent->d_name << endl;
            }
            closedir(dir);
        }
        else
        {
            perror("ls");
        }
    }
    else if (strcmp(argv[0], "mkdir") == 0)
    {
        if (mkdir(argv[1], 0777) == -1)
        {
            perror("mkdir");
        }
    }
    else if (strcmp(argv[0], "rmdir") == 0)
    {
        if (rmdir(argv[1]) == -1)
        {
            perror("rmdir");
        }
    }
    else if (strcmp(argv[0], "cd") == 0)
    {
        if (chdir(argv[1]) == -1)
        {
            perror("cd");
        }
    }
    else if (strcmp(argv[0], "cat") == 0)
    {
        int fd;
        char c;
        fd = open(argv[1], O_RDONLY);
        if (fd == -1)
        {
            perror("cat");
            return;
        }
        while (read(fd, &c, 1) > 0)
        {
            write(STDOUT_FILENO, &c, 1);
        }
        close(fd);
    }
    else if (strcmp(argv[0], "cp") == 0)
    {
        int src_fd, dest_fd;
        char c;
        src_fd = open(argv[1], O_RDONLY);
        if (src_fd == -1)
        {
            perror("cp");
            return;
        }
        dest_fd = open(argv[2], O_WRONLY | O_CREAT, 0666);
        if (dest_fd == -1)
        {
            perror("cp");
            close(src_fd);
            return;
        }
        while (read(src_fd, &c, 1) > 0)
        {
            write(dest_fd, &c, 1);
        }
        close(src_fd);
        close(dest_fd);
    }
    else if (strcmp(argv[0], "touch") == 0)
    {
        if (open(argv[1], O_CREAT | O_WRONLY | O_TRUNC, 0666) == -1)
        {
            perror("touch");
        }
    }
    else if (strcmp(argv[0], "rm") == 0)
    {
        if (remove(argv[1]) == -1)
        {
            perror("rm");
        }
    }
    else if (strcmp(argv[0], "mv") == 0)
    {
        if (rename(argv[1], argv[2]) == -1)
        {
            perror("mv");
        }
    }
    else if (strcmp(argv[0], "echo") == 0)
    {
        for (int i = 1; argv[i] != NULL; i++)
        {
            cout << argv[i] << " ";
        }
        cout << endl;
    }
    else if (strcmp(argv[0], "pwd") == 0)
    {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            cout << cwd << endl;
        }
        else
        {
            perror("pwd");
        }
    }
    else if (strcmp(argv[0], "clear") == 0)
    {
        system("clear");
    }
    else
    {
        myExecvp(argv);
    }
}

void myExecvp(char **argv)
{
    pid_t pid;
    int status;
    int childStatus;
    pid = fork();
    if (pid == 0)
    {
        childStatus = execvp(*argv, argv);
        if (childStatus < 0)
        {
            cout << "ERROR: Wrong input" << endl;
        }
        exit(0);
    }
    else if (pid < 0)
    {
        cout << "Something went wrong!" << endl;
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
    }
}

void StrTokenizer(char *input, char **argv)
{
    char *stringTokenized;
    stringTokenized = strtok(input, " ");
    while (stringTokenized != NULL)
    {
        *argv++ = stringTokenized;
        stringTokenized = strtok(NULL, " ");
    }
    *argv = NULL;
}

void saveCommand(const char *command)
{
    if (historyCount < 10)
    {
        strcpy(history[historyCount], command);
        historyCount++;
    }
    else
    {
        for (int i = 0; i < 9; i++)
        {
            strcpy(history[i], history[i + 1]);
        }
        strcpy(history[9], command);
    }
}

void displayHistory()
{
    for (int i = 0; i < historyCount; i++)
    {
        cout << i + 1 << ": " << history[i] << endl;
    }
}
