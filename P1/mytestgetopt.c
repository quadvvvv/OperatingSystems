#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int
main (int argc, char **argv)
{
  char *fvalue = NULL;
  char c;
  int index = 1;

  opterr = 0;

  while ((c = getopt(argc, argv, "Vhf:")) != -1)
    {
        switch (c)
        {
        case 'V':
            printf("my-look from CS357 Summer 2021");
            exit(0);
        case 'h':
            printf("this utility takes a string as input and return any lines\n in a file that contain that string as a prefix.\n (Cr: P1 Instruction))");
            exit(0);
            break;
        case 'f':
            fvalue = optarg;
            break;
        case '?':
            printf("my-look: invalid command line\n");
            exit(1);
        default:
            abort();
        }
    }

  printf ("fvalue = %s\n",fvalue);
  printf ("optind = %d\n",optind);

  for (index = optind; index < argc; index++)
    printf ("arg[%d]   Non-option argument %s\n", index, argv[index]);
  return 0;
}