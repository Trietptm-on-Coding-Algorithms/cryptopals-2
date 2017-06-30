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
 *  -d          = debug;          print debug information during run
 *  -e stage    = execute stage;  run only the given stage. Valid stages are "hamming", "b64", "keylength", "histogram"
 *  -f infilename = any file input. If this is not present input is read from stdin.
 *  -h          = hamming;        print the hamming distance between two lines of text.
 *  -i format   = input format;   If format is "hex" or "hexadecimal" it will skip the initial b64-> hex conversion.
 *  -k          = keep;           keeps intermediate file(s) rather than deleting them.
 *  -o infilename = output;         writes output to the specified file; assumes -k as well.
 *  -v          = verbose;        print extra information on completion
 *  -           = end of arguments
 */
int main (int argc, char* argv[]) {
  int status = 0;
  int hamming_only = 0;
  int b64_only = 0;
  int keylength_only = 0;
  int histogram_only = 0;
  int keep = 0;
  int skip_b64 = 0;
  init();

  char infilename[256] = "";
  char outfilename[256] = "";

  for (int curarg = 1; curarg < argc && strcmp(argv[curarg], "-"); curarg++) {
    if (!strcmp(argv[curarg], "-d")) {
      debug = 1;
    } else if (!strcmp(argv[curarg], "-v")) {
      verbose = 1;
    } else if (!strcmp(argv[curarg], "-h")) {
      hamming_only = 1;
    } else if (!strcmp(argv[curarg], "-f")) {
      strncpy(infilename, argv[++curarg], sizeof(infilename) - 1);
    } else if (!strcmp(argv[curarg], "-e")) {
      if (!strcmp(argv[++curarg], "hamming")) {
        hamming_only = 1;
      } else if (!strcmp(argv[curarg], "b64")) {
        b64_only = 1;
      } else if (!strcmp(argv[curarg], "keylength")) {
        keylength_only = 1;
      } else if (!strcmp(argv[curarg], "histogram")) {
        histogram_only = 1;
      }
    } else if (!strcmp(argv[curarg], "-k")) {
      keep = 1;
    } else if (!strcmp(argv[curarg], "-o")) {
      strncpy(infilename, argv[++curarg], sizeof(infilename) - 1);
    } else if (!strcmp(argv[curarg], "-i")) {
      if (!strcmp(argv[++curarg], "hex") || !strcmp(argv[++curarg], "hexadecimal")) {
        skip_b64 = 1;
      }
    }
  }

  FILE * fp;
  if (infilename[0]) {
    fp = fopen(infilename, "r");
  } else {
    fp = stdin;
  }

  if (hamming_only) {
    char bufferA[256] = "";
    char bufferB[256] = "";
    char * pointerA = fgets(bufferA, sizeof(bufferA), fp);
    char * pointerB = fgets(bufferB, sizeof(bufferB), fp);
    if (pointerA == NULL || pointerB == NULL) {
      fprintf(stderr, "Error reading from %s.\n", infilename);
      status = 1;
    } else {
      printf("%d\n", hamming(sizeof(bufferA), bufferA, bufferB));
    }
  }



  fclose(fp);
  return status;
}
