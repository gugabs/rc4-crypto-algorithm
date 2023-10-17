#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void read_line(FILE* credentials, unsigned char** target);
void swap(unsigned char* a, unsigned char* b);
void KSA(unsigned char* key, unsigned char* S);
long int read_hex_byte(unsigned char* hex_content, int byte_index);
void PRGA(unsigned char* S, unsigned char* hex_content, unsigned char* inverse_content);

int main() {
  FILE* credentials;

  unsigned char* key = NULL;
  unsigned char* hex_content = NULL;

  unsigned char S[256];
  unsigned char* inverse_content;

  credentials = fopen("credentials.txt", "r");

  if (credentials == NULL) {
    perror("File can't be opened\n");

    return 1;
  }

  read_line(credentials, &key);
  read_line(credentials, &hex_content);

  printf("Key: %s\n", key);
  printf("HEX input: %s\n", hex_content);

  inverse_content = malloc(sizeof(unsigned char) * strlen(hex_content) / 2);

  KSA(key, S);
  PRGA(S, hex_content, inverse_content);

  printf("HEX output: ");

  for (int i = 0; i < strlen(inverse_content); i++)
    printf("%02hhX", inverse_content[i]);

  printf("\n");

  printf("Text output: %s\n", inverse_content);

  free(key);
  free(hex_content);
  free(inverse_content);

  fclose(credentials);

  return 0;
}

void read_line(FILE* credentials, unsigned char** target) {
  char* line;

  char ch_buffer;

  int line_size = 1;
  int ch_index = 0;

  line = malloc(sizeof(char));

  while ((ch_buffer = getc(credentials)) != '\n' && ch_buffer != EOF) {
    line = realloc(line, line_size + 1);
    line[ch_index] = ch_buffer;

    line_size++;
    ch_index++;
  }

  line[ch_index] = '\0';

  *target = realloc(*target, line_size);
  strcpy(*target, line);

  free(line);
}

void swap(unsigned char* a, unsigned char* b) {
  int tmp = *a;

  *a = *b;
  *b = tmp;
}

void KSA(unsigned char* key, unsigned char* S) {
  int T[256];
  int key_length = strlen(key);

  int j = 0;

  for (int i = 0; i < 256; i++) {
    S[i] = i;
    T[i] = key[i % key_length] % 256;
  }

  for (int i = 0; i < 256; i++) {
    j = (j + S[i] + T[i]) % 256;
    swap(&S[i], &S[j]);
  }
}

long int read_hex_byte(unsigned char* hex_content, int byte_index) {
  unsigned char hex_byte[2];

  hex_byte[0] = hex_content[byte_index * 2];
  hex_byte[1] = hex_content[(byte_index * 2) + 1];

  return strtol(hex_byte, NULL, 16);
}

void PRGA(unsigned char* S, unsigned char* hex_content, unsigned char* inverse_content) {
  int content_length = strlen(hex_content);

  int i = 0, j = 0, k = 0;

  for (int byte_index = 0; byte_index < content_length / 2; byte_index++) {
    i = (i + 1) % 256;
    j = (j + S[i]) % 256;

    swap(&S[i], &S[j]);

    k = S[(S[i] + S[j]) % 256];

    long int hex_byte = read_hex_byte(hex_content, byte_index);

    inverse_content[byte_index] = k ^ hex_byte;
  }
}