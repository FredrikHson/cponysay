#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <errno.h>


int main(int argc, char* argv[])
{
    int fd = fileno(stdin);
    int len;
    ioctl(fd, FIONREAD, &len);
    fprintf(stdout, "%i, %i\n", len, errno);

    if(len)
    {
        char* input = malloc(len);

        fread(input, len, 1, stdin);
        fwrite(input, len, 1, stdout);

        if(input != 0)
        {
            free(input);
        }
    }

    return 0;
}
