#include <stdio.h>
#include <string.h>

/* This strategy comes courtesy of Orwellophile on stackoverflow. Source:
 * stackoverflow.com/questions/10324/convert-a-hexaeecimal-string-to-an-integer-efficiently-in-c
 * This allows direct lookups, speeding up processing time.
 */
static const unsigned char hextable[] = {
  [0 ... 255] = 0,
  ['0'] = 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
  ['A'] = 10, 11, 12, 13, 14, 15,
  ['a'] = 10, 11, 12, 13, 14, 15
};
static const unsigned char dec2hex[] = {
  [0] = '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  'a', 'b', 'c', 'd', 'e', 'f'
};

/**
 * This takes two hexadecimal strings, decodes them into numbers, then XORS them.
 */
int main (int argc, char* argv[]) {

  int status = 0;
  //1440 bits of information + terminating null
  char bufferA[361] = "";
  char bufferB[361] = "";
  char output[361] = "";
  char *curA = bufferA;
  char *curB = bufferB;
  //hexA and hexB are the numeric representations of a character from strings
  //bufferA and bufferB, respectively.
  unsigned char hexA = 0;
  unsigned char hexB = 0;
  unsigned char hexout = 0;
  unsigned int outindex = 0;

  //input hex value as either an argument or stdin.
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

  //While both of the characters wwe're looking at aren't null:
  while (*curA && *curB) {
    //First we convert the characters to numbers.
    hexA = hex2dec[*curA];
    hexB = hex2dec[*curB];
    //Then we XOR them.
    hexout = hexA ^ hexB;
    //Third, we encode the output.
    output[outindex++] = dec2hex[hexout];
    //Finally, we increment our counters.
    curA++;
    curB++;
  }
  //Output is now ready to be served.
  printf("%s\n", output);

  return status;
}
