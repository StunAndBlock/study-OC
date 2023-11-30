#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include <string.h>

typedef void (*sighandler)(int);
void brake(int);
sigjmp_buf pos;
static char digit[3];
static int j;
static char* who[] = { "Man: ", "Man: ", "Com: ", "Com: ", "Bye.\n" };
static int man_res = 0, com_res = 0;

char* result_to_string(int res, int *num_size);
char* rewrite_1(char* str, size_t size);
char* top(int res1, int res2, int *size_res);

int main(int argc, char** argv) {
	char* result = NULL;
	int timeout = 0, size_win_result = 0;
	j = 0;
	if (argc < 2) {
		write(2, "Usage: dice timeout\n", 20);
		exit(0);
	}
	timeout = atoi(argv[1]);
	if (timeout < 1) {
		timeout = 1;
	}
	digit[0] = '1' + getpid() % 6;
	digit[1] = '\b';
	digit[2] = '\n';

	j = sigsetjmp(pos, SIGINT);  //  сохраняется состояние SIGINT

	signal(SIGALRM, (sighandler) brake);
	signal(SIGINT, (sighandler) brake);

	if (j == 2 || j == 3) {
		alarm(timeout);
		write(1, digit, 2);
	}
	else {
		alarm(0);
	}
	if (j < 4) {
		write(1, who[j], 5);
		while (j < 4) {
			write(1, digit, 2);
			digit[0] = (digit[0] < '6') ? digit[0] + 1 : '1';
			usleep(50000);
		}
	} else {
		result = top(man_res, com_res, &size_win_result);
		if (result == NULL) {
			write(0, "Memory error\n", 13);
			exit(0);
		}
		write(0, result, size_win_result);
		write(0, "\n", 1);
		write(1, who[j], 5);
	}
	signal(SIGINT, SIG_DFL);
	exit(0);
}

void brake(int c) {
	if (j < 2) {
		man_res += digit[0] - '0';
	} else {
		com_res += digit[0] - '0';
	}
	write(1, digit, 3);
	siglongjmp(pos, ++j);
	return;
}

char* top(int res1, int res2, int *size_res) {
	char* res = NULL;
	if (res1 > res2) {
		return result_to_string(res1, size_res);
	} else if (res2 > res1) {
		return result_to_string(res2, size_res);
	} else {
		res = (char*)malloc(5 * sizeof(char));
		strcpy(res, "draw");
		*size_res = 4;
		return res;
	}
}

char* rewrite_1(char* str, size_t size) {
	char* new_str = (char*)malloc((size + 1) * sizeof(char));
	size_t i;
	for (i = size; i > 0; i--) {
		new_str[i] = str[i - 1];
	}
	free(str);
	return new_str;
}

char* result_to_string(int res, int *num_size) {
	char *str = (char*)malloc(sizeof(char));
	size_t i = 1;
	while (res / 10 != 0) {
		str[0] = (res % 10) + '0';
		str = rewrite_1(str, i);
		i++;
		res/=10;
		*num_size += 1;
	}
	str[0] = res + '0';
	*num_size += 1;
	return str;
}
