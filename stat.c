#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

struct stat stt;

void info(char *path) {
  char *nama = path;
  if(stt.type == T_DIR){
      printf(1, ">>> Informasi direktori dari %s <<<\n", nama);
      printf(1, "\n");
      printf(1, "Size\t\t: %d\n", stt.size);
      printf(1, "Type\t\t: Directory\n");
      printf(1, "Device\t\t: %d\n", stt.dev);
      printf(1, "File Inode\t: %d\n", stt.ino);
      printf(1, "Links\t\t: %d\n", stt.nlink);
    }
  else if(stt.type == T_FILE){
      printf(1, ">>> Informasi direktori dari %s <<<\n", nama);
      printf(1, "\n");
      printf(1, "Size\t\t: %d\n", stt.size);
      printf(1, "Type\t\t: File\n");
      printf(1, "Device\t\t: %d\n", stt.dev);
      printf(1, "File Inode\t: %d\n", stt.ino);
      printf(1, "Links\t\t: %d\n", stt.nlink);
  }
  else if(stt.type == T_DEV){
      printf(1, ">>> Informasi direktori dari %s <<<\n", nama);
      printf(1, "\n");
      printf(1, "Size\t\t: %d\n", stt.size);
      printf(1, "Type\t\t: Device\n");
      printf(1, "Device\t\t: %d\n", stt.dev);
      printf(1, "File Inode\t: %d\n", stt.ino);
      printf(1, "Links\t\t: %d\n", stt.nlink);
    }
}

int main(int argc, char *argv[]){
  int fd, i;
  char *nama = argv[1];

  if(argc < 2){
    printf(1, "Missing File Operand\n");
  }

  if((fd = open(nama, 0)) < 0){
    printf(2, "Stat: cannot open %s\n", nama);
  }
  
  if(fstat(fd, &stt) < 0){
    printf(2, "Stat: cannot stat %s : No such file or directory\n", nama);
    close(fd);
  }
  
  for(i=1; i<argc; i++){
    info(argv[i]);
  }

  exit();
}
