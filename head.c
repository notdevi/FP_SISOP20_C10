#include "types.h"
#include "stat.h"
#include "user.h"
#define STDIN "_stdin"

char buff[512];

void print_help() {
	printf(1, "usage : head [OPTION]... [FILE]... \nPrint the first 10 lines of each FILE to standard output.\nWith more than one FILE, precede each with a header giving the filename.\nPossible arguments are :\n\t -c NUM\t\tprint the first num bytes of each file;\n\t-n NUM\t\tprint the first NUM lines of each file;\n\t-q\t\tquiet, never print headers giving file names;\n\t-v\t\tverbose, print headers giving file names;\n");
	exit();
}

void errors(uint err, char* str) {
	if(err == 0) {
		printf(2, "head: syntax error\n");
	} else if(err == 1) {
		printf(2, "head: invalid options %s\nTry 'head --help' for more information\n", str);
	} else if(err == 2) {
		printf(2, "head: syntax error, expected at least one argument\n");
	} else if(err == 3) {
		printf(2, "head: nvalid number of lines: %s\n", str);
	} else if(err == 4) {
		printf(2, "head: invalid number of bytes: %s\n", str);
	} else if(err == 5) {
		printf(2, "head: read error\n");
	} else if(err == 6) {
		printf(2, "head: cannot open '%s' for reading: No such file or directory\n", str);
	} else {
		printf(2, "head: error\n");
	}
	exit();
}

void head(char *file_name, int n, int mode, int mode_n) {
	int r, fd, l = 0;
	
	if(strcmp(file_name, STDIN) == 0) {
		fd = 0;
	} else {
		fd = open(file_name, 0);
		if(fd < 0) {
			errors(6, file_name);
		}
	}

	if(mode == 2) {
		printf(1, "filename : %s\n", file_name);
	}

	while((r = read(fd, buff, sizeof(buff))) > 0 && l < n) {
		for(int i = 0; i < r && l < n; i++) {
			printf(1, "%c", buff[i]);

			if(buff[i] == '\n' && mode_n != 2) {
				l++;
			} else if(mode_n == 2) {
				l++;
			}
		}
	}
	
	if (r < 0) {
		close(fd);
		errors(5, 0);
	}
	close(fd);
}

int main(int argc, char *argv[]) {
	int i, mode = 0, mode_n = 0, n = 10;
	char *c;

	if(argc <= 1) {
		head(STDIN, n, mode, mode_n);
		exit();
	}

	for(i = 1; i < argc; i++) {
		strcpy(buff, argv[i]);
		if(buff[0] == '-') {
			if(buff[1] == 'n') {
				if(atoi(argv[++i]) == 0) {
					errors(3, argv[i]);
				}
				n = atoi(argv[i]);
				mode_n = 1;
			} else if(buff[1] == 'c') {
				if(atoi(argv[++i]) == 0) {
					errors(4, argv[i]);
				}
				n = atoi(argv[i]);
				mode_n = 2;
			} else if(buff[1] == 'v') {
				mode = 2;
			} else if(buff[1] == 'q') {
				n = 20;
				mode = 1; 
			} else if(buff[1] == '-') {
				c = buff;
				c = c + 2;
				if(strcmp(c, "help") == 0) {
					print_help();
				} else {
					errors(1, argv[i]);
				}
			} else {
				errors(1, argv[i]);
			}
		} else {
			if(i+1 < argc && mode == 0) {
				mode = 2;
			}
			head(argv[i], n, mode, mode_n);
			if(i+1 < argc) {
				printf(1, "\n");
			}
		}
	}
	exit();
}

