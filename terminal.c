#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
int textmode(int);
int randline(FILE* file);
int getch();

static char pattern[80];
static char buffer[80];
static int LengthUsed;
int textmode(int mode) {
    static struct termios con[2];
    if (mode > 0) return (tcsetattr(0, TCSAFLUSH, &con[1]));
    tcgetattr(0, &con[0]);
    tcgetattr(0, &con[1]);
    con[0].c_lflag &= ~(ICANON | ECHO | ISIG);
    con[0].c_iflag &= ~(ISTRIP | IXOFF | IXANY | IXON);
    con[0].c_oflag |= CS8;
    con[0].c_cc[VMIN] = 2;
    con[0].c_cc[VTIME] = 1;
    return (tcsetattr(0, TCSAFLUSH, &con[0]));
}

int getch() {
    unsigned char c[2];
    static int len = 0;
    /*if(len > 1) {
      c[0] = len;
      len = 0;
      return(c[0]);
    }
    */

    c[0] = c[1] = 0;
    if ((len = read(0, c, 2)) < 2) return (c[0]);
    if (c[0] == 27) c[0] = 0;
    /* len = c[1]; */
    ungetc(c[1], stdin);
    return (c[0]);
}

int randline(FILE* file) {
    // memset(pattern,NULL,80);
    int l[2];
    l[0] = 1;
    l[1] = 1;
    int i = 0;
    char c;
    int strammount = 0;
    char line[256];                                 /* or other suitable maximum line size */
    while (fgets(line, sizeof(line), file) != NULL) /* read a line */
    {
        strammount++;
    }
    int strnumber = rand() % strammount + 1;
    write(1, "str N", 6);                                   // Make space for sprintf to work in.
    LengthUsed = sprintf(buffer, "%d", strnumber);  // Format n in decimal.
    write(1, buffer, LengthUsed);
    write(1, ":", 1);
    rewind(file);
    strammount = 0;
    while ((strammount + 1) != strnumber) {
        fgets(line, sizeof(line), file);
        strammount++;
    }
    while (l[1]) {
        c = getc(file);
        switch (c) {
            case ' ':
                break;
            case '\n':
                l[1] = 0;
                break;
            case EOF:
                l[1] = 0;
                break;
            default:
                if (i < 80) {
                    pattern[i] = c;
                    i++;
                } else {
                    l[1] = 0;
                }
                break;
        }
    }

    return i;
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    FILE* text;
    int len;
    int i = 0;
    int err = 0;
    unsigned char c;
    len = 6; /*atoi(argv[1]);*/
    if (!argv[1]) {
        write(1, "NO SOURCE FILE ENTERED\n", 23);
    } else if ((text = fopen(argv[1], "r")) == NULL) {
        write(1, "SOURCE FILE WAS REJECTED\n", 26);
    } else {
        textmode(0);
        int blank = 1;
        int bl=1;
        while (blank) {
            len = randline(text);
            write(1, pattern, len);
            write(1, "\n", 1);
            while (i < len && bl) {
                switch (c = getch()) {
                    case 0:
                        c = '\007';
                        switch (getch()) {
                            case 67:
                                c = pattern[i];
                                write(1, "per", 3);
                                break;
                            case 68:
                                if (i == 0) break;
                                i--;
                                write(1, "\b", 1);
                                continue;
                            default:
                                break;
                        }
                        break;
                    case 27:
                        bl=0;
                        blank = 0;
                        break;
                    default:
                        if (c != pattern[i]) c = '\007';
                        break;
                }
                if(c!=27)
                (c == '\007') ? err++ : i++;
                write(1, &c, 1);
            }
            if (len == 0) {
                write(1, "ignored", 8);
            } else if (i!=0){
            write(1, "\n", 1);
            write(1,"errors:",8);
            LengthUsed = sprintf(buffer, "%d", err);  // Format n in decimal.
            write(1, buffer, LengthUsed);
            write(1," ",1);
            write(1,"total:",7);
            i+=err;
            LengthUsed = sprintf(buffer, "%d", i);  // Format n in decimal.
            write(1, buffer, LengthUsed);
            }           
            write(1, "\n", 1);
            i=0;
            err=0;
        }
        textmode(1);
    }
    return (err);
}
