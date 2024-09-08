#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <mysql.h>
#include <stdlib.h>

#define CHUNK 1024

struct connection
{
    char server[256];
    char user[256];
    char password[256];
    char database[256];
};

char **linesSeperateBySemicolon(char **linesOfText, char *buffer, short *lineCount)
{
    short i;
    short startPoint = 0;
    short count = 0;
    for (i = 0; i <= strlen(buffer); i++)
    {
        if (buffer[i] == ';' || buffer[i] == '\0')
        {
            linesOfText = (char **)realloc(linesOfText, (count + 1) * sizeof(char *));
            if (linesOfText == NULL)
            {
                perror("Memory allocation failed 1");
                exit(3);
            }
            linesOfText[count] = (char *)malloc((i - startPoint + 1) * sizeof(char));
            if (linesOfText[count] == NULL)
            {
                perror("Memory allocation failed 2");
                exit(3);
            }
            strncpy(linesOfText[count], buffer + startPoint, i - startPoint);
            linesOfText[count][i - startPoint] = '\0';
            startPoint = i + 1;
            count++;
        }
    }
    *lineCount = count;
    return linesOfText;
}

char **wordsSeparatedBySpace(char **words, char *buffer, short *wordCount)
{
    short i;
    short startPoint = 0;
    short count = 0;
    for (i = 0; i <= strlen(buffer); i++)
    {
        if (buffer[i] == ' ' || buffer[i] == '\0')
        {
            if (i - startPoint > 0)
            {
                words = (char **)realloc(words, (count + 1) * sizeof(char *));
                if (words == NULL)
                {
                    perror("Memory allocation failed 3");
                    exit(3);
                }
                words[count] = (char *)malloc((i - startPoint + 1) * sizeof(char));
                if (words[count] == NULL)
                {
                    perror("Memory allocation failed 4");
                    exit(3);
                }
                strncpy(words[count], buffer + startPoint, i - startPoint);
                words[count][i - startPoint] = '\0';
                startPoint = i + 1;
                count++;
            }
            else
            {
                startPoint = i + 1;
            }
        }
    }
    *wordCount = count;
    return words;
}

int main(int argc, char **argv)
{
    char buffer[CHUNK];
    char logicBuffer[1024];
    char exampleBuffer[] = "DENEMELİK TEXT DOLAR SIGN $MERHABA$ BAKALIM OLCAK MI??";
    char insertBuffer[] = "NABER";
    int insertBufferLength = strlen(insertBuffer);
    int lengthExample = strlen(exampleBuffer);
    short i;
    short j;
    int fd = open("index.html", O_RDONLY);
    int fileSize = lseek(fd, (off_t)0, SEEK_END);
    lseek(fd, (off_t)0, SEEK_SET);
    if (read(fd, buffer, fileSize) < 0)
    {
        perror("Something happened while reading");
    }
    buffer[fileSize] = '\0';
    short indexInterval;
    short count = 0;
    short countLogic = 0;
    short startPoint = -1;
    short endPoint = -1;
    short startPointLogic = -1;
    short endPointLogic = -1;

    struct connection connection1 = {"localhost", "root", "Erayubuntu82889**", "bruh"};
    MYSQL *conn1;
    char *server1 = connection1.server;
    char *user1 = connection1.user;
    char *password1 = connection1.password;
    char *database1 = connection1.database;

    conn1 = mysql_init(NULL);
    if (!mysql_real_connect(conn1, server1, user1, password1, database1, 0, NULL, 0))
    {
        printf("Error for conn1 %s\n", mysql_error(conn1));
        exit(1);
    }

    for (i = 0; i < strlen(buffer); i++)
    {
        if (buffer[i] == '&')
        {
            countLogic++;
            switch (countLogic)
            {
            case 1:
                startPointLogic = i;
                break;
            case 2:
                endPointLogic = i;
                indexInterval = endPointLogic - startPointLogic - 1;
                strncpy(logicBuffer, &buffer[startPointLogic + 1], indexInterval);
                logicBuffer[indexInterval] = '\0';
                printf("Logic buffer is: %s\n", logicBuffer);
                char **linesOfText = NULL;
                short lineCount;
                linesOfText = linesSeperateBySemicolon(linesOfText, logicBuffer, &lineCount);
                printf("Here is the first line: %s\n", linesOfText[0]);

                for (j = 0; j < lineCount; j++)
                {
                    char **wordsOfLine = NULL;
                    short wordCount;
                    wordsOfLine = wordsSeparatedBySpace(wordsOfLine, linesOfText[j], &wordCount);
                    printf("Line %d:\n", j + 1);
                    for (short k = 0; k < wordCount; k++)
                    {
                        printf("Word %d: %s\n", k + 1, wordsOfLine[k]);
                    }

                    if (strcmp(wordsOfLine[1], "mysql") == 0)
                    {
                        printf("Mysql activation");
                    }
                    else if (strcmp(wordsOfLine[1], "post") == 0)
                    {
                        printf("Post activiation");
                    } else {
                        printf("bruh");
                    }
                    for (short k = 0; k < wordCount; k++)
                    {
                        free(wordsOfLine[k]);
                    }

                    free(wordsOfLine);
                }

                for (j = 0; j < lineCount; j++)
                {
                    free(linesOfText[j]);
                }
                free(linesOfText);

                countLogic = 0; // Reset countLogic after processing logic
                break;
            default:
                break;
            }
        }

        if (buffer[i] == '$')
        {
            count++;
            switch (count)
            {
            case 1:
                startPoint = i;
                break;
            case 2:
                endPoint = i;
                indexInterval = endPoint - startPoint;
                for (j = 0; j < insertBufferLength; j++)
                {
                    if (insertBuffer[j] != '\0')
                    {
                        buffer[j + startPoint] = insertBuffer[j];
                    }
                }
                memmove(&buffer[startPoint + insertBufferLength], &buffer[endPoint + 1], fileSize - endPoint);
                fileSize = fileSize - indexInterval + insertBufferLength;
                buffer[fileSize] = '\0';
                break;
            default:
                break;
            }
        }
    }

    mysql_close(conn1);
    // printf("File: %s\n", buffer);
    return 0;
}
