#include<stdio.h>
#include"rxm.h"

int main(int argc, char *argv[]){

  if (argc != 2){
    printf("Usage: %s <filename>\n", argv[0]);
    return 1;  
  }

  FILE *file = fopen(argv[1], "rb");
  if (file == 0){
    printf("Could not open '%s'\n", argv[1]);
    return 1;
  }
  
  //get file size
  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fseek(file, 0, SEEK_SET);

  //allocate buffer for the file
  uint8_t *buffer = malloc(size);
  fread(buffer, 1, size, file);
  fclose(file);

  // print all bytes as hex
  for (long i = 0; i < size; i++) {
    printf("%02x ", buffer[i]);
    if ((i + 1) % 16 == 0) printf("\n");
  }
 
  printf("\n");  
  free(buffer);

  return 0;
}
