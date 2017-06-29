#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//This gets procedurally filled out in the init() function.
static unsigned char b64_to_num[] = {
  [0 ... 255] = 0
};

static int debug = 0;
static int verbose = 0;

void init() {
  for (unsigned char c = 'A', v = 0; c <= 'Z'; c++, v++) {
    b64_to_num[c] = v;
  }
  for (unsigned char c = 'a', v = 26; c <= 'z'; c++, v++) {
    b64_to_num[c] = v;
  }
  for (unsigned char c = '0', v = 52; c <= '9'; c++, v++) {
    b64_to_num[c] = v;
  }
  b64_to_num['+'] = 62;
  b64_to_num['/'] = 63;
}

int hamming(size_t size, const char* lhs, const char* rhs) {
  int result = 0;

  //For every character:
  //  get the difference in byte-sized blocks by XORing the characters
  //  For every bit in difference:
  //    add the lowest bit to the result
  //    shift difference 1 to the right for the next loop
  for (size_t i = 0; i < size && (lhs[i] || rhs[i]); i++) {
    char difference = lhs[i] ^ rhs[i];
    //This could probably get better performance by parallelizing.
    //Instead of shifting difference, shift the mask by the process id.
    for (int j = 0; j < 8; j++) {
      result += difference & 1;
      difference >>= 1;
    }
  }

  return result;
}

/*
 * Flags:
 *  -f filename = any file input. If this is not present input is read from stdin.
 *  -h          = hamming;        print the hamming distance between two lines of text.
 *  -d          = debug;          print debug information during run
 *  -v          = verbose;        print extra information on completion
 *  -           = end of arguments
 */
int main (int argc, char* argv[]) {
  int status = 0;
  init();

  int hamming_only = 0;
  char filename[256] = "";

  for (int curarg = 1; curarg < argc && strcmp(argv[curarg], "-"); curarg++) {
    if (!strcmp(argv[curarg], "-d")) {
      debug = 1;
    } else if (!strcmp(argv[curarg], "-v")) {
      verbose = 1;
    } else if (!strcmp(argv[curarg], "-h")) {
      hamming_only = 1;
    } else if (!strcmp(argv[curarg], "-f")) {
      strncpy(filename, argv[++curarg], sizeof(filename) - 1);
    }
  }

  FILE * fp;
  if (filename[0]) {
    fp = fopen(filename, "r");
  } else {
    fp = stdin;
  }

  if (hamming_only) {
    char bufferA[256] = "";
    char bufferB[256] = "";
    char * pointerA = fgets(bufferA, sizeof(bufferA), fp);
    char * pointerB = fgets(bufferB, sizeof(bufferB), fp);
    if (pointerA == NULL || pointerB == NULL) {
      fprintf(stderr, "Error reading from %s.\n", filename);
      status = 1;
    } else {
      printf("%d\n", hamming(sizeof(bufferA), bufferA, bufferB));
    }
  } else {
    //Here we find the normalized hamming distances between the first 4 blocks for a range of potential key sizes.
    size_t keysize;
    char block_A[12] = "";
    char block_B[12] = "";
    char block_C[12] = "";
    char block_D[12] = "";

    for (keysize = 2; keysize < sizeof(block_A); keysize++) {

    }
  }

  fclose(fp);
  return status;
}
