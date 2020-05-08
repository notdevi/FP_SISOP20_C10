#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "fs.h"

char buffer[512], str1[200];

char *file_name(char *str) {
	char *filename = str;
	char *temp = str;
	
	for(int i = strlen(temp); i >= 0; i--) {
		if(temp[i] == '/') {
			filename = &temp[i+1];
			break;
		}
	}
	return filename;
}

char *str_gabung(char *string1, char *string2) {
	char str2[200];
	int i;
	
	strcpy(str1, string1);
	strcpy(str2, string2);
	
	for(i=0; str1[i] != '\0'; ++i);
	for(int j=0; str2[j] != '\0'; ++j, ++i) {
		str1[i] = str2[j];
	}
	str1[i] = '\0';
	return str1;
}	

void cp_biasa(char *src, char *dest) {
	int fs, fd, fd1, n;
	struct stat st;

	if((fs = open(src, O_RDONLY)) < 0) {
		printf(1, "cp: cant open %s\n", src);
		exit();
	}

	if((fd = open(dest, 0)) < 0);
	else {
		if(fstat(fd, &st) < 0) {
			printf(2, "cant stat\n");
			close(fd);
			return;
		}

		if(st.type == T_DIR) {
			char *fname;
			
			fname = file_name(src);
			// p = dest + strlen(dest)-1;
			if(dest[strlen(dest)-1] != '/') {
				dest = str_gabung(dest, "/");
			}
			fname = str_gabung(dest, fname);
			strcpy(dest, fname);
		}
	}
	
	if((fd1 = open(dest, O_CREATE | O_RDWR)) < 0) {
		printf(1, "cp: cannot open %s\n", dest);
		exit();
	}
	
	while((n = read(fs, buffer, sizeof(buffer))) > 0) {
		if(write(fd1, buffer, n) != n) {		
			printf(1, "write failed\n");
			exit();
		}
	}
	close(fs);
	close(fd1);
	close(fd);
}

void cp_star(char *path, char *dest) {
	char buf[512], *p, temp[200];
	int fd;
	struct dirent de;
	struct stat st;

	strcpy(temp, dest);

	if((fd = open(path, 0)) < 0) {
		printf(2, "cp: cant open %s\n", path);
		return;
	}

	if(fstat(fd, &st) < 0) {
		printf(2, "cp: cant stat %s\n", path);
		close(fd);
		return;
	}

	switch(st.type) {
		case T_FILE :
			printf(1, "failed\n");
			break;
		case T_DIR : 
			if((strlen(path)+1+DIRSIZ+1) > sizeof(buf)) {
				printf(1, "cp: path is too long\n");
				break;
			}
			strcpy(buf, path);
			p = buf + strlen(buf);
			*p = '/';
			p++;

			while(read(fd, &de, sizeof(de)) == sizeof(de)) {
				if(de.inum == 0) {
					continue;
				}
				memmove(p, de.name, DIRSIZ);
				p[DIRSIZ] = 0;
		
				if(stat(buf, &st) < 0) {
					printf(1, "cp: cannot stat %s\n", buf);
					continue;
				}
				if(st.type == T_FILE) {
					strcpy(dest, temp);
					printf(1, "CP Success : %s to %s\n", buf, dest);
					cp_biasa(buf, dest);
				}
			}
			break;
	}
	close(fd);
}
	
char *no_star(char *str) {
	strcpy(str1, ".");
	if(strlen(str) == 1) {
		return str1;
	}
	strcpy(str1, str);
	str1[strlen(str1) - 2] = '\0';

	return str1;
}

void cp_rekursif(char *path, char *dest) {
	char buf[512], *p, temp[200];
	int fd;
	struct dirent de;
	struct stat st;

	strcpy(temp, dest);

	if((fd = open(path, 0)) < 0) {
		printf(2, "ls: cant open %s\n", path);
		return;
	}

	if(fstat(fd, &st) < 0) {
		printf(2, "ls: cant stat %s\n", path);
		close(fd);
		return;
	}

	switch(st.type) {
		case T_FILE :
			printf(1, "failed\n");
			break;
		case T_DIR : 
			if((strlen(path)+1+DIRSIZ+1) > sizeof(buf)) {
				printf(1, "path is too long\n");
				break;
			}
			strcpy(buf, path);
			p = buf + strlen(buf);
			*p = '/';
			p++;

			while(read(fd, &de, sizeof(de)) == sizeof(de)) {
				if(de.inum == 0) {
					continue;
				}
				memmove(p, de.name, DIRSIZ);
				p[DIRSIZ] = 0;
		
				if(stat(buf, &st) < 0) {
					printf(1, "cp: cannot stat %s\n", buf);
					continue;
				}
				if(st.type == T_FILE) {
					strcpy(dest, temp);
					printf(1, "CP Success : %s to %s\n", buf, dest);
					cp_biasa(buf, dest);
				}
				else if(st.type == T_DIR) {
					char cmp[50];

					strcpy(cmp, file_name(buf));
					if(strcmp(cmp, ".") == 0 || strcmp(cmp, "..") == 0) {
						continue;
					}
					str_gabung(dest, "/");
					str_gabung(dest, file_name(buf));
					mkdir(dest);

					printf(1, "Create Directory %s inside %s\n", buf, dest);
					cp_rekursif(buf, str_gabung(str_gabung(dest,"/"), file_name(buf)));
				}
			}
			break;
	}
	close(fd);
}

int main(int argc, char *argv[]){
	if(argc < 2) {
		printf(1, "arguments not enough\n");
		exit();
	} else if(argc < 3) {
		printf(1, "cp: missing destination file operand after %s\n", argv[1]);
		exit();
	}

	if(strcmp(file_name(argv[1]), "*") == 0) {
		cp_star(no_star(argv[1]), argv[2]);
	} else if(strcmp(argv[1], "-r") == 0) {
		cp_rekursif(argv[2], argv[3]);
	} else {
		cp_biasa(argv[1], argv[2]);
	}
	exit();
}
