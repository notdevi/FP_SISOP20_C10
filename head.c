#include "types.h"
#include "stat.h"
#include "user.h"
#define STDIN "_stdin"

char buff[512];

void print_help() {
	printf(1, "usage : head [OPTION]... [FILE]... \nPrint the first 10 lines of each FILE to standard output.\nWith more than one FILE, precede each with a header giving the filename.\nPossible arguments are :\n\t -c NUM\t\tprint the first num bytes of each file;\n\t-n NUM\t\tprint the first NUM lines of each file;\n\t-q\t\tquiet, never print headers giving file names;\n\t-v\t\tverbose, print headers giving file names;\n");
	exit();
}

void error_head(uint err, char* s) {
	switch(err) {
		case(0) :
			printf(2, "head: syntax error\n");
			break;
		case(1) :
			printf(2, "head: invalid options %s\nTry 'head --help' for more information\n", s);
			break;
		case(2) :
			printf(2, "head: syntax error, expected at least one argument\n");
			break;		
		case(3) :
			printf(2, "head: nvalid number of lines: %s\n", s);
			break;
		case(4) :
			printf(2, "head: invalid number of bytes: %s\n", s);
			break;	
		case(5) :
			printf(2, "head: read error\n");
			break;
		case(6) :
			printf(2, "head: cannot open '%s' for reading: No such file or directory\n", s);
			break;
		default:
			printf(2, "head: error\n");
	}
	exit();
}

void head(char *fn, int n, int mode, int moden) {
	int r, fd, l = 0;
	
	if(strcmp(fn, STDIN) == 0) {
		fd = 0;
	} else {
		fd = open(fn, 0);
		if(fd < 0) {
			error_head(6, fn);
		}
	}

	if(mode == 2) {
		printf(1, "filename : %s\n", fn);
	}

	while((r = read(fd, buff, sizeof(buff))) > 0 && l < n) {
		for(int i = 0; i < r && l < n; i++) {
			printf(1, "%c", buff[i]);

			if(buff[i] == '\n' && moden != 2) {
				l++;
			} else if(moden == 2) {
				l++;
			}
		}
	}
	
	if (r < 0) {
		close(fd);
		error_head(5, 0);
	}
	close(fd);
}

int main(int argc, char *argv[]) {
	int i, mode = 0, moden = 0, n = 10;
	char *c;

	if(argc <= 1) {
		head(STDIN, n, mode, moden);
		exit();
	}

	for(i = 1; i < argc; i++) {
		strcpy(buff, argv[i]);
		if(buff[0] == '-') {
			switch(buff[1]) {
				case 'n' :
					if(atoi(argv[++i]) == 0) {
						error_head(3, argv[i]);
					}
					n = atoi(argv[i]);
					moden = 1;
					break;
				case 'c' :
					if(atoi(argv[++i]) == 0) {
						error_head(4, argv[i]);
					}
					n = atoi(argv[i]);
					moden = 2;
					break;
				case 'v' :
					mode = 2;
					break;
				case 'q' :
					mode = 1;
					break; 
				case '-' :
					c = buff;
					c = c + 2;
					if(strcmp(c, "help") == 0) {
						print_help();
					} else {
						error_head(1, argv[i]);
					}
					break;
				default :
					error_head(1, argv[i]);
					break;
			}
		} else {
			if(i+1 < argc && mode == 0) {
				mode = 2;
			}
			head(argv[i], n, mode, moden);
			if(i+1 < argc) {
				printf(1, "\n");
			}
		}
	}
	exit();
}

				
