#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

void display_help(const char *program_name) {
    printf("Usage: %s [-n] <filename>\n", program_name);
    printf("Options:\n");
    printf("  -n              Display line number\n");
    printf("  -b              Display line numbers but do not number blank lines\n");
    printf("  -s              When multiple blank lines are encountered, only one blank line is displayed\n");
    printf("  -h              Display this help message\n");
    printf("  -v              Display version information\n");
    printf("  --help          Display this help message\n");
    printf("  --version       Display version information\n");
}

void display_version() {
    printf("ElonCat 1.0\n");
    printf("License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.\n");
    printf("This is free software: you are free to change and redistribute it.\n");
    printf("There is NO WARRANTY, to the extent permitted by law.\n");
    printf("Written by Elon Li.\n");
}

void create_null_file(const char * filename) {
  FILE *file = fopen(filename, "r");

  if (!file) {
    file = fopen(filename, "w");
    if (!file) {
      printf("Error: Unable to create file '%s'\n", filename);
    }
    fclose(file);
  }

}

void cat_file(const char *filename, int flag) {
  FILE *file = fopen(filename, "r");
  char buffer[1024];
  int line_number = 1;
  if (flag == 0) {
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
      printf("%s", buffer);
    }
  }
  else if(flag == 1){
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
      printf("%d: %s", line_number++, buffer);
    }
  }
  else if(flag == 2){
    while (fgets(buffer, sizeof(buffer), file) !=NULL) {
      // 检查是否是空行
      if (strcmp(buffer, "\n") == 0) {
        printf("%s", buffer);
      } else {
        printf("%d: %s", line_number++, buffer);
      }
    }
  }
  else if(flag == 3){
    int prev_empty_line = 0;
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
      // 检查是否是空行
      if (strcmp(buffer, "\n") == 0) {
        // 如果上一个行不是空行，则输出一个空行
        if (!prev_empty_line) {
          printf("%s", buffer);
          prev_empty_line = 1;
        }
      } else {
        // 输出行号和内容
        printf("%d: %s", line_number++, buffer);
        prev_empty_line = 0;
      }
    }
  }

  fclose(file);
}


void cat_directory(const char *dirname, int flag) {
  DIR *dir;
  struct dirent *entry;

  // 打开目录
  dir = opendir(dirname);
  if (!dir) {
    printf("Error: Unable to open directory '%s'\n", dirname);
    return;
  }

  // 遍历目录中的所有文件
  while ((entry = readdir(dir)) != NULL) {
    // 忽略 . 和 .. 目录
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }

    // 构建文件的完整路径
    char fullpath[1024];
    snprintf(fullpath, sizeof(fullpath), "%s/%s", dirname, entry->d_name);

    // 根据文件类型调用相应的函数
    struct stat st;
    if (stat(fullpath, &st) != 0) {
      perror("Error");
      continue;
    }
    if (S_ISREG(st.st_mode)) {
      cat_file(fullpath, flag);
      printf("\n"); // 在文件内容之间输出一个空行
    } else if (S_ISDIR(st.st_mode)) {
      printf("=== Directory: %s ===\n", fullpath);
      cat_directory(fullpath, flag);
    }
  }

  // 关闭目录
  closedir(dir);
}


int is_directory(const char *path) {
  struct stat path_stat;
  if (stat(path, &path_stat) != 0) {
    perror("Error");
    return -1; // 返回-1表示出错
  }
  if (S_ISDIR(path_stat.st_mode))
    return 1; // 返回1表示是目录
  else
    return 0; // 返回0表示是文件
}

int main(int argc, char *argv[]) {
  char *filename = NULL;
  int flag = 0;
  if (argc == 3) {
    // 如果 argc 等于 3，则假设存在 "-n" 选项
    if (strcmp(argv[1], "-n") == 0) {
      flag = 1;
    }
    if (strcmp(argv[1], "-b") == 0){
      flag = 2;
    }
    if (strcmp(argv[1], "-s") == 0){
      flag = 3;
    }
    filename = argv[2];
  }
  else if (argc == 2) {
    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
      display_help(argv[0]);
      return 0;
    } else if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
      display_version();
      return 0;
    } else {
      filename = argv[1];
    }
  }

  int result = is_directory(filename);
  if (result == 1) {
    printf("%s is a directory.\n", filename);
    cat_directory(filename, flag);
  }
  else if (result == 0){
    printf("%s is a file.\n", filename);
    cat_file(filename, flag);
  }
  else{
    printf("create a %s file.\n", filename);
    create_null_file(argv[1]);
  }

  return 0;
}
