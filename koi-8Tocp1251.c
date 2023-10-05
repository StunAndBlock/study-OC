#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#define BUFSIZ 8192
unsigned char* recode(ssize_t,unsigned char* buf);


int main(int argc, char* argv[])
{
	const char note[]="NOTE: PROGRAMM DO NOT USE ARGUMENTS\n";
	unsigned char buf[BUFSIZ];
	ssize_t num;
	if (argc > 1)
		write(STDERR_FILENO,note,sizeof(note)/sizeof(note[0]));
	num = read(STDIN_FILENO,buf, BUFSIZ);
	write(STDOUT_FILENO,recode(num,buf), num);
	return 0;
}

unsigned char* recode(ssize_t n,unsigned char* buf)
{
	unsigned char c;
	static char tab[] = {30, 0, 1, 22 ,4 ,5 , 20, 3,
				21, 8 , 9 , 10, 11, 12 , 13, 
				14, 15 ,31, 16, 17 , 18, 19, 
				6, 2, 28, 27, 7, 24, 29, 25, 
				23, 26
	};
	for (ssize_t i = 0; i < n; i++)
	{
		c = buf[i];
		if (c >192 && c < 224){
			buf[i] = tab[c - 192] + 224;
		} 
		else if(c>=224){
			buf[i] = tab[c - 224] + 192; 
		}
		else if (c==184){ // ~ code
			buf[i]=126;
		}
		else if(c==179){
			buf[i]=168;
		}
	}
	return buf;
}

