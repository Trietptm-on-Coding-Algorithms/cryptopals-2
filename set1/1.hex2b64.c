#include <stdio.h>
#include <string.h>

static const unsigned char hextable[] = {
  [0 ... 255] = 0,
  ['0'] = 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
  ['A'] = 10, 11, 12, 13, 14, 15,
  ['a'] = 10, 11, 12, 13, 14, 15
}; 
/*courtesy of Orwellophile on stackoverflow
 * stackoverflow.com/questions/10324/convert-a-hexaeecimal-string-to-an-integer-efficiently-in-c
 */
static const unsigned char b64table[] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
  'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
  'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
};

/**
 * This takes a hexadecimal number as input and prints its base64 representation.
 */
int main (int argc, char* argv[]) {

  int status = 0;
  //1440 bits of information + terminating null
  char buffer[361] = "";
  char output[241] = "";
  char *curchar = buffer;
  unsigned const char lowmask = 0x3;
  unsigned const char topmask = 0xc;
  unsigned char carry = 0;
  unsigned char threechar = 0;
  unsigned int outindex = 0;
  unsigned char lookup = 0;
  unsigned char letter = 0;
  unsigned char curhex = 0;

  //input hex value
  if (argc >= 2) {
    strncpy(buffer, argv[1], sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';
  } else if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
    //remove trailing newline if one exists
    char *p;
    if ((p = strchr(buffer, '\n')) != NULL && *(p + 1) == '\0') {
      *p = '\0';
    }
  } else {
    status = 1;
    return status;
  }
  
  // edge case fixes
  //deal with negative number
  if (buffer[0] == '-') {
    output[0] = '-';
    curchar ++;
    outindex++;
  }
  //strip 0x prefix if it exists, 0 out the 'x' just in case, and move the pointer forward.
  if (*curchar == '0' && *(curchar + 1) == 'x') {
    *(++curchar) = '0';
    curchar++;
  }
  
  int breakpoint = 0;
  //printf("DEBUG: LOOP ENTRY\n");
  while (*curchar) {
    curhex = hextable[*curchar];
    /*
    printf("DEBUG:\tLOOP = %c\n", *curchar);
    printf("\toutput index = %u\n", outindex);
    printf("\tinput mod 3 = %u\n", threechar);
    printf("\tincoming carry = %#02o\n", carry);
    printf("\tcurrent hex = %#02x\n", curhex);
    */
    if (threechar == 0) {
      carry = curhex;
      carry <<= 2;
    } else if (threechar == 1) {
      lookup = curhex & topmask;
      lookup >>= 2;
      lookup |= carry;
      carry = curhex & lowmask;
      carry <<= 4;
    } else {
      lookup = curhex;
      lookup |= carry;
      carry = 0;
    }
    if (threechar) {
      //printf("\tlookup number = %u\n", lookup);
      letter = b64table[lookup];
      //printf("\tresult letter = %c\n", letter);
      output[outindex++] = letter;
    }
    curchar++;
    threechar ++;
    threechar = threechar % 3;
  }
  if (threechar == 1) {
    lookup = 0;
    lookup |= carry << 2;
    output[outindex++] = b64table[lookup];
  } else if (threechar == 2) {
    lookup = 0;
    lookup |= carry << 4;
    output[outindex++] = b64table[lookup];
  }
  //print result
  printf("%s\n", output);

  return status;
}
