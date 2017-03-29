#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <sys/ioctl.h>
#include <string.h>
#include <fcntl.h>
#include <getopt.h>
#include <unistd.h>
#include "ponies.h"

size_t utf8len(char* s)
{
    size_t len = 0;
    char escaped = 0;

    for(; *s; ++s)
    {
        if((*s & 0xC0) != 0x80)
        {
            if(*s == 0x1b)
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

        if(*numLines > 0)
        {
            char** lines = malloc(*numLines * sizeof(char*));
            char* zeroedstart = zeroedText;

            for(int i = 0; i < *numLines; i++)
            {
                size_t linelen = bytelen(zeroedText) + 1;
                lines[i] = malloc(linelen);
                memcpy(lines[i], zeroedText, linelen);
                zeroedText += linelen;

            }

            if(zeroedstart)
            {
                free(zeroedstart);
            }

            return lines;
        }
        else
        {
            if(zeroedText)
            {
                free(zeroedText);
            }

            return 0;
        }
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

void printTextBox(char* text, size_t bytes, int minWidth)
{
    int numLines = 0;
    char** lines = createNullTerminatedStrings(text, bytes, &numLines);
    int longestLine = minWidth;

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

        strcat(top, "─");
        strcat(bottom, "─");
    }

    strcat(top, "╮");
    strcat(bottom, "╯");
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
void clipPony(int width, unsigned int pony)
{
    size_t len = strlen((char*)allponies_pony[pony]);
    char* p = (char*)allponies_pony[pony];
    size_t linelen = 0;

    for(int i = 0; i < len; i++)
    {

        if((p[i] & 0x80) == 0x80)
        {
            if((p[i] & 0xF8) == 0xF0)
            {
                if(linelen < width)
                {
                    fwrite(p + i, 4, 1, stdout);
                    printf("wtf");
                }

                i += 3;
            }
            else if((p[i] & 0xF0) == 0xE0)
            {
                if(linelen < width)
                {
                    fwrite(p + i, 3, 1, stdout);
                }

                i += 2;
            }
            else if((p[i] & 0xE0) == 0xC0)
            {
                if(linelen < width)
                {
                    fwrite(p + i, 2, 1, stdout);
                }

                i += 1;
            }
            else
            {
                if(linelen < width)
                {
                    fwrite(p + i, 1, 1, stdout);
                }
            }

            linelen++;
        }
        else if(p[i] == 0x20)
        {
            linelen++;

            if(linelen < width)
            {
                putchar(0x20);
            }
        }
        else
        {
            putchar(p[i]);
        }

        if(p[i] == '\n')
        {
            linelen = 0;
        }
    }
}
void printPonyWithText(char* text, size_t bytes, unsigned int pony, unsigned int width)
{
    pony = pony % numPonies;

    if(*allponies_topbottom[pony])
    {
        clipPony(width, pony);
        printTextBox(text, bytes, *allponies_balloon_width[pony]);
    }
    else
    {
        printTextBox(text, bytes, *allponies_balloon_width[pony]);
        clipPony(width, pony);
    }

    fflush(stdout);
}

void printHelp()
{

    fprintf(stdout,
            "-h --help\n"
            "-p --pony\n"
            "-w --width\n"
            "-q --quote\n"
            "-l --list\n");
    exit(0);
}

void ListPonies()
{
    for(int i = 0; i < numPonies; i++)
    {
        fprintf(stdout, "%-30s", allponies_name[i]);

        if(i % 5 == 4)
        {
            fprintf(stdout, "\n");
        }
    }

    fprintf(stdout, "\n");
    exit(0);

}

unsigned int getPonyByName(char* name)
{
    int pony = numPonies + 1;

    for(int i = 0; i < numPonies; i++)
    {
        if(strcmp(name, allponies_name[i]) == 0)
        {
            pony = i;
            break;
        }

    }

    return pony;
}
extern unsigned char* quotes_mute;
int main(int argc, char* argv[])
{
    int fd = fileno(stdin);
    size_t len;
    int width = 0;

    int RANDOM = open("/dev/urandom", O_RDONLY);
    unsigned int pony = numPonies + 1;
    _Bool quote = 0;
    _Bool requestedpony = 0;


    static const struct option longOpts[] =
    {

        { "pony", required_argument, 0, 'p' },
        { "help", no_argument, 0, 'h' },
        { "quote", no_argument, 0, 'q' },
        { "list", no_argument, 0, 'l' },
        { "width", no_argument, 0, 'w' }
    };

    int c;
    int longIndex = 0;

    while((c = getopt_long(argc, argv, "p:hqlw:q", longOpts, &longIndex)) != -1)
    {
        switch(c)
        {
            case 'p':
                pony = getPonyByName(optarg);
                requestedpony = 1;
                break;

            case 'h':
                printHelp();
                break;

            case 'l':
                ListPonies();
                break;

            case 'q':
                quote = 1;
                break;

            case 'w':
                width = atoi(optarg);
                break;

            default:
                break;
        }
    }

    size_t str_len = 0;


    for(int i = optind; i < argc; i++)
    {
        str_len += strlen(argv[i]) + 1;
    }

    if(numPonies == 0)
    {
        fprintf(stderr, "All the ponies are missing, call the Princess!\n");
        exit(EX_UNAVAILABLE);
    }

    if(pony == numPonies + 1)
    {
        read(RANDOM, &pony, sizeof(int));
    }

    pony = pony % numPonies;

    if(width == 0)
    {

        struct winsize sz;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &sz);

        if(sz.ws_col == 0)
        {
            sz.ws_col = 65535;
        }

        width = sz.ws_col;
    }

    if(quote)
    {
        if(!requestedpony)
        {
            while(*allponies_quotes[pony] == quotes_mute)
            {
                read(RANDOM, &pony, sizeof(int));
                pony = pony % numPonies;
            }
        }

        unsigned short randomquote = 0;
        read(RANDOM, &randomquote, sizeof(unsigned short));
        randomquote = randomquote % (*allponies_numquotes[pony]);
        char* text = (char*)allponies_quotes[pony][randomquote];
        size_t textlen = strlen(text);
        printPonyWithText(text, textlen, pony, width);
    }
    else if(str_len == 0)
    {
        while(1)
        {
            int ret = ioctl(fd, FIONREAD, &len);

            if(ret == -1)
            {
                return 0;
            }

            if(len)
            {
                char* input = (char*)malloc(len);

                if(input != 0)
                {
                    fread(input, len, 1, stdin);
                    printPonyWithText(input, len, pony, width);
                    free(input);
                }

                return 0;
            }
        }

    }
    else
    {
        char* input = (char*)malloc(str_len + 1);

        if(input != 0)
        {
            sprintf(input, "%s", argv[optind]);

            for(int i = optind + 1; i < argc; i++)
            {
                strcat(strcat(input, " "), argv[i]);
            }

            strcat(input, "\n");
            printPonyWithText(input, str_len, pony, width);
            free(input);
        }

    }

    return 0;
}
