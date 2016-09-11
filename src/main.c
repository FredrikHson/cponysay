#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>

size_t utf8len(char* s)
{
    size_t len = 0;
    char escaped = 0;

    for(; *s; ++s)
    {
        if((*s & 0xC0) != 0x80)
        {
            if(*s == '\e')
            {
                escaped = 1;
            }


            if(!escaped)
            {
                ++len;
            }

            if(*s == 'm' && escaped)
            {
                escaped = 0;
            }
        }
    }

    return len;
}
size_t bytelen(char* s)
{
    size_t len = 0;

    for(; *s; ++s)
    {
        ++len;
    }

    return len;
}

char** createNullTerminatedStrings(char* text, size_t bytes, int* numLines)
{
    if(numLines && text)
    {

        *numLines = 0;
        char* zeroedText = malloc(bytes);
        memcpy(zeroedText, text, bytes);

        for(int i = 0; i < bytes; i++)
        {
            if(zeroedText[i] == '\n')
            {
                zeroedText[i] = 0;
                ++*numLines;
            }
        }

        char** lines = malloc(*numLines * sizeof(char*));

        for(int i = 0; i < *numLines; i++)
        {
            size_t linelen = bytelen(zeroedText) + 1;
            lines[i] = malloc(linelen);
            memcpy(lines[i], zeroedText, linelen);
            zeroedText += linelen;

        }

        return lines;
    }
    else
    {
        return 0;
    }
}

void destroyNullTerminatedStrings(char** strings, int numLines)
{
    if(strings)
    {
        for(int i = 0; i < numLines; i++)
        {
            if(strings[i])
            {
                free(strings[i]);
            }
        }

        free(strings);
    }
}

void printTextBox(char* text, size_t bytes)
{
    int numLines = 0;
    char** lines = createNullTerminatedStrings(text, bytes, &numLines);
    int longestLine = 0;

    for(int i = 0; i < numLines; i++)
    {
        int linelen = utf8len(lines[i]);

        if(linelen > longestLine)
        {
            longestLine = linelen;
        }
    }

    size_t totallen = (longestLine + 4) * 4 + 1;
    char* top = malloc(totallen);
    char* bottom = malloc(totallen);
    memset(top, 0, (longestLine + 5) * 4);
    memset(top, 0, (longestLine + 5) * 4);
    sprintf(top, "╭");
    sprintf(bottom, "╰");

    for(int i = 0; i < longestLine + 2 ; i++)
    {

        sprintf(top, "%s─", top);
        sprintf(bottom, "%s─", bottom);
    }

    sprintf(top, "%s╮", top);
    sprintf(bottom, "%s╯", bottom);
    fprintf(stdout, "%s\n", top);

    for(int i = 0; i < numLines; i++)
    {
        int linelen = utf8len(lines[i]);
        fprintf(stdout, "│ %s%*s │\n", lines[i], longestLine - linelen, "");
    }

    fprintf(stdout, "%s\n", bottom);
    free(top);
    free(bottom);
    destroyNullTerminatedStrings(lines, numLines);
    return;
}

int main(int argc, char* argv[])
{
    int fd = fileno(stdin);
    size_t len;

    while(1)
    {
        int ret = ioctl(fd, FIONREAD, &len);

        if(ret == -1)
        {
            return 0;
        }

        if(len)
        {
            char* input = malloc(len);

            fread(input, len, 1, stdin);
            printTextBox(input, len);
            /*fwrite(input, len, 1, stdout);*/

            if(input != 0)
            {
                free(input);
            }

            return 0;
        }
    }

    return 0;
}
