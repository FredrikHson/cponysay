#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <sys/ioctl.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include "ponies.h"

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

void printPonyWithText(char* text, size_t bytes, unsigned int pony)
{
    pony = pony % numPonies;

    if(*allponies_topbottom[pony])
    {
        fwrite(allponies_pony[pony], strlen((char*)allponies_pony[pony]), 1, stdout);
        printTextBox(text, bytes, *allponies_balloon_width[pony]);
    }
    else
    {
        printTextBox(text, bytes, *allponies_balloon_width[pony]);
        fwrite(allponies_pony[pony], strlen((char*)allponies_pony[pony]), 1, stdout);
    }

    fflush(stdout);
}

void printHelp()
{

    fprintf(stdout,
            "-h --help\n"
            "-p --pony\n"
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

int main(int argc, char* argv[])
{
    int fd = fileno(stdin);
    size_t len;

    int RANDOM = open("/dev/urandom", O_RDONLY);
    unsigned int pony = numPonies + 1;

    static const struct option longOpts[] =
    {

        { "pony", required_argument, 0, 'p' },
        { "help", no_argument, 0, 'h' },
        { "q", no_argument, 0, 'q' },
        { "list", no_argument, 0, 'l' }
    };

    int c;
    int longIndex = 0;

    while((c = getopt_long(argc, argv, "p:hql", longOpts, &longIndex)) != -1)
    {
        switch(c)
        {
            case 'p':
                pony = getPonyByName(optarg);
                break;

            case 'h':
                printHelp();
                break;

            case 'q':
                break;

            case 'l':
                ListPonies();
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

    if(str_len == 0)
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



                fread(input, len, 1, stdin);
                printPonyWithText(input, len, pony);

                if(input != 0)
                {
                    free(input);
                }

                return 0;
            }
        }

    }
    else
    {
        char* input = (char*)malloc(str_len + 1);

        sprintf(input, "%s", argv[optind]);

        for(int i = optind + 1; i < argc; i++)
        {
            sprintf(input, "%s %s", input, argv[i]);
        }

        sprintf(input, "%s\n", input);
        printPonyWithText(input, str_len, pony);

        if(input != 0)
        {
            free(input);
        }

    }

    return 0;
}
