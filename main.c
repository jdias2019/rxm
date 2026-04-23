#include"rxm.h"

void decode_modrm(uint8_t *buffer, long i){

  uint8_t modrm = buffer[i + 1];
  uint8_t mod = (modrm >> 6) & 0x03;
  uint8_t reg = (modrm >> 3) & 0x07;
  uint8_t rm  =  modrm       & 0x07;

  if (mod == 3){
    printf(" %s, %s", reg_names[reg], reg_names[rm]);
  } else if (mod == 0){
    printf(" %s, [%s]", reg_names[reg], reg_names[rm]);
  } else {
    printf("???");
  }
}

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

  long i = 0;
  while (i < size){
    uint8_t opcode = buffer[i];
    OpcodeInfo *info = &opcode_table[opcode];
    printf("0x%08lx   ", i); //offset
    printf("%02x   ", opcode); // raw byte

    // mnemonic 
    if (info->mnemonic[0] != '\0'){
      printf("%s", info->mnemonic);
    } else{
      printf("???");
    }

  
    if (info->imm_size > 0){
      if (info->imm_size == 4){
        uint32_t imm = (uint32_t)buffer[i+1] 
                     | (uint32_t)buffer[i+2] << 8
                     | (uint32_t)buffer[i+3] << 16
                     | (uint32_t)buffer[i+4] << 24;
        printf(" 0x%08X", imm);
      } else if (info->imm_size == 1){
        uint32_t imm = buffer[i+1];
        printf(" 0x%02X", imm);
      }
      
    }

    if (info->has_modrm == 1){
      decode_modrm(buffer, i);
      i += 1 + 1 + info->imm_size;
    } else{
      i += 1 + info->imm_size;
    }

    printf("\n");
  }

  free(buffer);

  return 0;
}
