#include <stdio.h>
#include <string.h>

static const unsigned char hex2dec[] = {
  [0 ... 255] = 0,
  ['0'] = 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
  ['A'] = 10, 11, 12, 13, 14, 15,
  ['a'] = 10, 11, 12, 13, 14, 15
}; 
/*courtesy of Orwellophile on stackoverflow
 * stackoverflow.com/questions/10324/convert-a-hexaeecimal-string-to-an-integer-efficiently-in-c
 */

static const unsigned char dec2hex[] = {
  [0] = '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  'a', 'b', 'c', 'd', 'e', 'f'
};

/**
 * This takes a hexadecimal number as input and prints its base64 representation.
 */
int main (int argc, char* argv[]) {

  int status = 0;
  //1440 bits of information + terminating null
  char bufferA[361] = "";
  char bufferB[361] = "";
  char output[361] = "";
  char *curA = bufferA;
  char *curB = bufferB;
  unsigned char hexA = 0;
  unsigned char hexB = 0;
  unsigned char hexout = 0;
  unsigned int outindex = 0;

  //input hex value
  if (argc >= 3) {
    strncpy(bufferA, argv[1], sizeof(bufferA));
    bufferA[sizeof(bufferA) - 1] = '\0';
    strncpy(bufferB, argv[2], sizeof(bufferB));
    bufferB[sizeof(bufferB) - 1] = '\0';
  } else if (fgets(bufferA, sizeof(bufferA), stdin) != NULL) {
    //remove trailing newline if one exists
    char *p;
    if ((p = strchr(bufferA, '\n')) != NULL && *(p + 1) == '\0') {
      *p = '\0';
    }
    if (fgets(bufferB, sizeof(bufferB), stdin) != NULL) {
      //remove trailing newline if one exists
      if ((p = strchr(bufferB, '\n')) != NULL && *(p + 1) == '\0') {
        *p = '\0';
      }
    } else {
      status = 1;
      return status;
    }
  } else {
    status = 1;
    return status;
  }
  
  int breakpoint = 0;
  while (*curA && *curB) {
    hexA = hex2dec[*curA];
    hexB = hex2dec[*curB];
    hexout = hexA ^ hexB;
    output[outindex++] = dec2hex[hexout];
    curA++;
    curB++;
  } 
  //print result
  printf("%s\n", output);

  return status;
}
