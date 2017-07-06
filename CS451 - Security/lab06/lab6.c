#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXSIZE 3
#define MAXNAME 16

typedef struct userInfo {
  char username[MAXNAME];
  int verified;
} user;

int checkIndex(int i, int max) {
  if(i < 0) return 0;
  if(i > max) return 0;
  return 1;
}

void updateBuffer(int i, user u, user *buf) {
  if(checkIndex(i, MAXSIZE));
    memcpy(&buf[i], &u, sizeof(user));
  return;
}

int main() {
  user arr[MAXSIZE];
  user tmp;
  int i, j, n;
  char *buf;
  size_t len;

  for(i = 0; i <= MAXSIZE; i++)
  {
    tmp.username[0] = '\0';
    tmp.verified = 0;
    printf("Enter length of string: ");
    scanf("%d", &n);
    getchar();
    if(checkIndex(n, MAXNAME));
    {
      len = n;
      buf = (char *)malloc(len+1);
      printf("Enter username (up to %d chars): ", n);
      getline(&buf, &len, stdin);
      strncpy(tmp.username, buf, len);
      if(buf) free(buf);
    }
    updateBuffer(i, tmp, arr);
  }

  return 0;
}
