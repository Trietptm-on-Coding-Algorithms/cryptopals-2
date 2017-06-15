#include <stdio.h>
#include <stdlib.h>
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

//Relative frequency lists courtesy of http://mdickens.me/typing/letter_frequency.html
//Plans for expansion: replace with a lookup table. This gets complicated for n-grams.
static const unsigned char monograph[] =  " etaoinsrhldcumgyfpwb.,vk0-'x)(1j2:q\"/5!?z346879%[]*=+|_;\\>$#^&@<~{}`";
static const unsigned char digraph[] =    "th""he""in""er""an""re""on""at""en""nd""st"
                                          "or""te""es""is""ha""ou""it""to""ed""ti""ng"
                                          "ar""se""al""nt""as""le""ve""of""me""hi""ea"
                                          "ne""de""co""ro""ll""ri""li""ra""io""be""el"
                                          "ch""ic""ce""ta""ma""ur""om""ho""et""no""ut"
                                          "si""ca""la""il""fo""us""pe""ot""ec""lo""di"
                                          "ns""ge""ly""ac""wi""wh""tr""ee""so""un""rs"
                                          "wa""ow""id""ad""ai""ss""pr""ct""we""mo""ol"
                                          "em""nc""rt""sh""po""ie""ul""im""ts""am""ir"
                                          "yo""fi""os""pa""ni""ld""sa""ay""ke""mi""na"
                                          "oo""su""do""ig""ev""gh""bl""if""tu""av""pl"
                                          "wo""ry""bu";
static const unsigned char trigraph[]  =  "the""ing""and""ion""ent""hat""her""tio"
                                          "tha""for""ter""ere""his""you""thi""ate"
                                          "ver""all""ati""ith""rea""con""wit""are"
                                          "ers""int""nce""sta""not""eve""res""ist"
                                          "ted""ons""ess""ave""ear""out""ill""was"
                                          "our""men""pro""com""est""ome""one""ect"
                                          "ive""tin""hin""hav""ght""but""igh""ore"
                                          "ain""str""oul""per""sti""ine""uld""ste"
                                          "tur""man""oth""oun""rom""ble""nte""ove"
                                          "ind""han""hou""whi""fro""use""der""ame"
                                          "ide""ort""und""rin""cti""ant""hen""end"
                                          "tho""art""red""lin";

static int debug = 0;
static int verbose = 0;

/**
 * This converts the given character string to lower case if shift is 0 and upper case otherwise.
 */
void capslock (char shift, size_t size, char* chararray) {
  for (char* cur = chararray; *cur && cur < chararray + size; cur++) {
    if (*cur >= 'A' && *cur <= 'Z') {
      if (shift) {
        *cur += ('a' - 'A');
      } else {
        *cur -= ('a' - 'A');
      }
    }
  }
  return;
}

//Returns 0 if every character in [0, size), before the first \0, is printable, -1 otherwise.
//This assumes that the plaintext answer does not end early with '\0'.
signed int unprintable_score(size_t size, char* chararray) {
  for (int i = 0; chararray[i] && i < size; i ++) {
    unsigned char current = (unsigned) chararray[i];
    if (current <= 31 || current >= 127) {
      return -1;
    }
  }
  return 0;
}

//Separated out the XORing functionality for reuse.
void stringXOR(size_t size, char* source, char* dest, char key) {
  for (int i = 0; i < size; i++) {
    dest[i] = source[i] ^ key;
  }
}

/*
 * Scoring function for various judgments on how 'English' a string is.
 * Assuming unprintable doesn't disqualify it, this goes through every letter,
 *  then every pair of letters, then finally every trio of letters.
 * Each time it adds to the score based on how far down the relative list the given
 *  combination of letter(s) falls.
 */
signed int plaintext_score (size_t size, char* chararray) {
  signed int result = unprintable_score(size, chararray);
  if (result < 0) {
    return result;
  }

  //Rather than try to judge the relative frequencies of upper case vs lower case,
  //  we just make a copy with every upper case letter shifted to its lower case counterpart.
  char* lower_copy = malloc(size * sizeof(char));
  for (size_t i = 0; chararray[i] && i < size; i++) {
    lower_copy[i] = chararray[i];
  }
  capslock(1, size, lower_copy);

  //Only check against trigraphs if the string has at least 3 characters
  if (size > 2) {
    for (size_t i = 0; chararray[i + 2] && (i + 2) < size; i++) {
      for (size_t j = 0, score = sizeof(trigraph); j < sizeof(trigraph); j += 3, score -= 3) {
        if (lower_copy[i] == trigraph[j] && lower_copy[i + 1] == trigraph[j + 1] && lower_copy[i + 2] == trigraph[j + 2]) {
          result += score;
        }
      }
    }
  }

  //Only check against digraphs if the string has at least 2 characters
  if (size > 1) {
    for (size_t i = 0; chararray[i + 1] && (i + 1) < size; i++) {
      for (size_t j = 0, score = sizeof(digraph); j < sizeof(digraph); j += 2, score -= 2) {
        if (lower_copy[i] == digraph[j] && lower_copy[i + 1] == digraph[j + 1]) {
          result += score;
        }
      }
    }
  }

  //Always check against monographs.
  for (size_t i = 0; chararray[i] && i < size; i++) {
    for (size_t j = 0, score = sizeof(monograph); j < sizeof(monograph); j++, score--) {
      if (lower_copy[i] == monograph[j]) {
        result += score;
      }
    }
  }

  //DEBUG: This prints every candidate plaintext that has not been disqualified for the presence of non-printing characters.
  if (debug) {
    printf("DEBUG: \tScore=%4d\tText=%s\n", result, lower_copy);
  }

  free(lower_copy);
  return result;
}

struct best_key_data{
  unsigned char key;
  signed int score;
  char* result_string;
};

/**
 * Scoring function that takes cipher text and returns the best key and score.
 * This will cycle through all possible keys, deciphering the chararray and then scoring it.
 * To avoid the copy cost of structs as return types, we take a pointer instead.
 *
 * returns 1 if malloc fails.
 */
int cipher_score(size_t size, char* chararray, struct best_key_data *result) {
  //Since the key is a character, it could theoretically be anything from 0 to 255.
  //While it is more likely to be a printable character, and even more likely to be
  //  an alphanumeric character, we shouldn't make assumptions.
  unsigned char counter = 0;
  //This time, we'll only store the generated strings long enough to score them.
  //Last time, they were stored separately for debugging purposes.
  char* buffer = (char *) malloc(size);
  char* best_string = (char *) malloc(size);
  //Check if malloc failed. If so, return error status.
  if (buffer == NULL || best_string == NULL) {
    return 1;
  }

  int current_score = 0;
  signed int max_score = -2;
  unsigned char best_key = 0;

  do {
    //Generate the XOR'ed string
    stringXOR(size, chararray, buffer, counter);
    //Calculate the score of the generated string
    current_score = plaintext_score(size, buffer);
    counter ++;

    if (current_score >= max_score) {
      max_score = current_score;
      best_key = counter;
      memcpy(best_string, buffer, size + 1);
    }
  } while (counter != 0);
  free(buffer);

  //Package our results for transport.
  result->key = best_key;
  result->score = max_score;
  memcpy(result->result_string, best_string, size + 1);
  free(best_string);
  return 0;
}

/**
 * This function converts a string of hexadecimal-encoded characters and decodes it.
 * It is assumed that sizeof(source) > size and sizeof(dest) > size / 2
 */
void hex2char(size_t size, char* source, char* dest) {
  for (int i = 0, j = 0; i < size; i += 2, j ++) {
    //Each character is 8 bits
    //Each hexadecimal digit is 4
    //source[x] accesses the ASCII encoding of the hexadecimal digit,
    //  so we convert it to numbers with the lookup table at the top.
    //The unsigned cast is to make it safe to use in array indexing.
    char temp = hextable[(unsigned) source[i]] << 4;
    temp |= hextable[(unsigned) source[i + 1]];
    dest[j] = temp;
  }
  return;
}

/**
 * This takes a hexadecimal string XOR'd by a single character and guesses what the key is.
 */
int main (int argc, char* argv[]) {
  int status = 0;
  char buffer[255] = {0};
  //Here we handle input. As 1 and 2 have done, this takes input as either an argument or expects it from stdin.
  for (int curarg = 1; curarg < argc && strcmp(argv[curarg], "-"); curarg ++) {
    if (!strcmp(argv[curarg], "-d")) {
      debug = 1;
    } else if (!strcmp(argv[curarg], "-f")) {
      strncpy(buffer, argv[++curarg], sizeof(buffer) - 1);
    } else if (!strcmp(argv[curarg], "-v")) {
      verbose = 1;
    }
  }
  
  int line_size, line_num, max_line_size = 0;
  FILE * fp;
  if (buffer[0]) {
    fp = fopen(buffer, "r");
  } else {
    fp = stdin;
  }

  //First step is to read the file to find out both how long each line is and how many lines there are.
  //In the real world, either talk to the people who will give you the data, or have an ini file, or bake in numbers from any samples you recieve.
  //Here though, the file is small, I want to keep this (theoretically) capable of scaling up, and I have plenty of cache.

  char temp;
  do {
    temp = fgetc(fp);
    if (temp == '\n') {
      line_num ++;
      if (line_size > max_line_size) {
        max_line_size = line_size;
      }
      line_size = 0;
    } else if (temp != EOF) {
      line_size ++;
    }
  } while (temp != EOF);
  //If the file does not end on \n, then the last line was counted but not added or compared.
  if (line_size) {
    line_num ++;
    if (line_size > max_line_size) {
        max_line_size = line_size;
      }
  }
  rewind(fp);

  //Next, we need to allocate space for our internal workings and output.
  int text_length = max_line_size / 2;
  char * hex_input = (char *) malloc(max_line_size + 1);
  char * char_input = (char *) malloc(text_length + 1);
  char * result_string = (char *) malloc(text_length + 1);
  struct best_key_data result;
  result.result_string = result_string;
  signed int best_score = -2;
  char * best_string = (char *) malloc (text_length + 1);

  //Now comes the main loop, where we go through each line of input
  //  and check if its best deciphering is better than our current.
  int current_line = 0;
  int best_line = 0;
  unsigned char best_key = 0;
  //Read error or just EOF results in a null pointer.
  while (fgets(hex_input, max_line_size + 1, fp) != NULL) {
    //But if the file ends in a \n, then we get a ghost line.
    if (hex_input[0] != '\n') {
      hex2char(max_line_size, hex_input, char_input);
      cipher_score(text_length, char_input, &result);
      if (result.score > best_score) {
        best_score = result.score;
        best_key = result.key;
        memcpy(best_string, result.result_string, text_length + 1);
        best_line = current_line;
      }
      current_line ++;
    }
  }
  
  if (verbose) {
    printf("Best line was %d XORed with %x, with a score of %d.\n", best_line, best_key, best_score);
  }
  printf("%s\n", best_string);

  free (result_string);
  free (best_string);
  free (char_input);
  free (hex_input);
  fclose(fp);
  return status;
}
