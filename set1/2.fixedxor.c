#include <stdio.h>
#include <string.h>

/* This strategy comes courtesy of Orwellophile on stackoverflow. Source:
 * stackoverflow.com/questions/10324/convert-a-hexaeecimal-string-to-an-integer-efficiently-in-c
 * This allows direct lookups, speeding up processing time.
 */
static const unsigned char hex_to_dec[] = {
  [0 ... 255] = 0,
  ['0'] = 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
  ['A'] = 10, 11, 12, 13, 14, 15,
  ['a'] = 10, 11, 12, 13, 14, 15
};
static const unsigned char dec_to_hex[] = {
  [0] = '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  'a', 'b', 'c', 'd', 'e', 'f'
};

/**
 * This takes two hexadecimal strings, decodes them into numbers, then XORS them.
 */
int main (int argc, char* argv[]) {

  int curarg = 1;
  int status = 0;
  //1440 bits of information + terminating null
  char bufferA[361] = "";
  char bufferB[361] = "";
  char bufferF[256] = "";
  char output[361] = "";
  char *curA = bufferA;
  char *curB = bufferB;
  //hexA and hexB are the numeric representations of a character from strings
  //bufferA and bufferB, respectively.
  unsigned char hexA = 0;
  unsigned char hexB = 0;
  unsigned char hexout = 0;
  unsigned int outindex = 0;

  //Either take input data as an argument, file containing input data as argument, or look for input through stdin.
  if (argc >= 3 && strncmp(argv[curarg], "-", 1)) {
    //If there are no '-' switches and two arguments, treat them as the two inputs.
    strncpy(bufferA, argv[curarg++], sizeof(bufferA) - 1);
    strncpy(bufferB, argv[curarg], sizeof(bufferB) - 1);
  } else if (argc >= 3 && !strcmp(argv[curarg], "-f")) {
    curarg++;

    strncpy(bufferF, argv[curarg], sizeof(bufferF) - 1);
    FILE * fp = fopen(bufferF, "r");
    
    if (fp != NULL) {
      if(fgets(bufferA, sizeof(bufferA), fp) == NULL) {
        status = 1;
        return status;
      }
      if (fgets(bufferB, sizeof(bufferB), fp) == NULL) {
        status = 1;
        return status;
      }
    } else {
      fprintf(stderr, "Error reading from %s.\n", bufferF);
    }
    fclose(fp);
  } else if (fgets(bufferA, sizeof(bufferA), stdin) != NULL) {
    if (fgets(bufferB, sizeof(bufferB), stdin) == NULL) {
      status = 1;
      return status;
    }
  } else {
    status = 1;
    return status;
  }

  //Cleanup:
  char *p;
  if ((p = strchr(bufferA, '\n')) != NULL && *(p + 1) == '\0') {
    *p = '\0';
  }
  if ((p = strchr(bufferB, '\n')) != NULL && *(p + 1) == '\0') {
    *p = '\0';
  }

  //While both of the characters wwe're looking at aren't null:
  while (*curA && *curB) {
    //First we convert the characters to numbers.
    hexA = hex_to_dec[(unsigned) *curA];
    hexB = hex_to_dec[(unsigned) *curB];
    //Then we XOR them.
    hexout = hexA ^ hexB;
    //Third, we encode the output.
    output[outindex++] = dec_to_hex[hexout];
    //Finally, we increment our counters.
    curA++;
    curB++;
  }
  //Output is now ready to be served.
  printf("%s\n", output);

  return status;
}
