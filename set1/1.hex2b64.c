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
  //The same information in base 64 takes 2/3 the space as in hexadecimal.
  //The terminating null is a fixed cost.
  char output[241] = "";
  char *curchar = buffer;
  //This is the same as 0b0011
  unsigned const char lowmask = 0x3;
  //This is the same as 0b1100
  unsigned const char topmask = 0xc;
  //Since there isn't a 1:1 matchup of input to output, we need to store a carry.
  unsigned char carry = 0;
  //threemode cycles between 0, 1, and 2 to keep track of how we deal with carry.
  unsigned char threemode = 0;
  unsigned int outindex = 0;
  unsigned char lookup = 0;
  unsigned char letter = 0;
  unsigned char curhex = 0;

  //input hex value as either an argument or stdin.
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

  //Edge case: negative number.
  if (buffer[0] == '-') {
    output[0] = '-';
    curchar ++;
    outindex++;
  }
  //Edge case: leading 0x.
  if (*curchar == '0' && *(curchar + 1) == 'x') {
    *(++curchar) = '0';
    curchar++;
  }

  //While the character we're looking at isn't null:
  while (*curchar) {
    //First we convert it to a number rather than a character.
    curhex = hextable[*curchar];
    //Then we figure out how far into a triplet we are.
    if (threemode == 0) {
      //The first number takes up the top four bits of the first output.
      //So we move it into the carry and shift it over 2.
      carry = curhex;
      carry <<= 2;
    } else if (threemode == 1) {
      //The second number has a lot of juggling, as it gets split in half.
      //First make the low bits of lookup the high bits of the current number.
      //Then we move in the current contents of carry.
      //Finally, we make the carry be the low bits of the current number, shifted to be the high bits.
      lookup = curhex & topmask;
      lookup >>= 2;
      lookup |= carry;
      carry = curhex & lowmask;
      carry <<= 4;
    } else {
      //The third is rather straightforward.
      //We put the current number into lookup, then add and reset carry.
      lookup = curhex;
      lookup |= carry;
      carry = 0;
    }
    //2 out of 3 input characters have an output character. As long as threemode isn't 0,
    //lookup is assigned and we can figure out the output.
    if (threemode) {
      letter = b64table[lookup];
      output[outindex++] = letter;
    }
    curchar++;
    threemode ++;
    threemode = threemode % 3;
  }
  //Edge case: input length is not divisible by 3.
  if (threemode == 1) {
    lookup = 0;
    lookup |= carry << 2;
    output[outindex++] = b64table[lookup];
  } else if (threemode == 2) {
    lookup = 0;
    lookup |= carry << 4;
    output[outindex++] = b64table[lookup];
  }
  //Output is now ready to be served.
  printf("%s\n", output);

  return status;
}
