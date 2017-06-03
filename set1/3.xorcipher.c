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
static const unsigned char monograph[] =  " etaoinsrhldcumfgpyw\nb,.vk-\"_\'x)(;0j1q=2:"
                                          "z/*!?$35>49[]867\\+|&<%@#^`~";
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

//Returns 0 if every character before the first \000 is printable, -1 otherwise.
signed int unprintable_score(size_t size, char* chararray) {
  for (int i = 0; chararray[i] && i < size; i ++) {
    unsigned char current = (unsigned) chararray[i];
    if (current <= 31 || current > 127) {
      return -1;
    }
  }
  return 0;
}

/*
 * Scoring function for various judgments on how 'English' a string is.
 * Assuming unprintable doesn't disqualify it, this goes through every letter,
 *  then every pair of letters, then finally every trio of letters.
 * Each time it adds to the score based on how far down the relative list the given
 *  combination of letter(s) falls.
 */
signed int score (size_t size, char* chararray) {
  signed int result = unprintable_score(size, chararray);
  if (result < 0) {
    return result;
  }


  char* lower_copy = malloc(size * sizeof(char));
  for (size_t i = 0; chararray[i] && i < size; i++) {
    lower_copy[i] = chararray[i];
  }
  capslock(1, size, lower_copy);

  if (size > 2) {
    for (size_t i = 0; chararray[i + 2] && (i + 2) < size; i++) {
      for (size_t j = 0, score = sizeof(trigraph); j < sizeof(trigraph); j += 3, score -= 3) {
        if (lower_copy[i] == trigraph[j] && lower_copy[i + 1] == trigraph[j + 1] && lower_copy[i + 2] == trigraph[j + 2]) {
          result += (score);
        }
      }
    }
  }

  if (size > 1) {
    for (size_t i = 0; chararray[i + 1] && (i + 1) < size; i++) {
      for (size_t j = 0, score = sizeof(digraph); j < sizeof(digraph); j += 2, score -= 2) {
        if (lower_copy[i] == digraph[j] && lower_copy[i + 1] == digraph[j + 1]) {
          result += (sizeof(digraph) - j);
        }
      }
    }
  }

  for (size_t i = 0; chararray[i] && i < size; i++) {
    for (size_t j = 0, score = sizeof(monograph); j < sizeof(monograph); j++, score--) {
      if (lower_copy[i] == monograph[j]) {
        result += (sizeof(monograph) - j);
      }
    }
  }

  free(lower_copy);
  return result;
}

/**
 * This takes a hexadecimal string XOR'd by a single character and guesses what the key is.
 */
int main (int argc, char* argv[]) {

  int status = 0;
  //1440 bits of information + terminating null
  char buffer[361] = "";
  char* cur = buffer;
  //Hex stores the numeric value of every pair of hexadecimal characters in the given string.
  //Plus the fixed cost for the terminating null, of course.
  unsigned char hex[181] = "";
  unsigned char* curhex = hex;
  //Since the key is a character, it could theoretically be anything from 0 to 255.
  //While it is more likely to be a printable character, and even more likely to be
  //  an alphanumeric character, we shouldn't make assumptions.
  char* output[256];
  char temp = 0;
  //This initializes all of the scores to be 0.
  signed int scores[256] = {0};
  unsigned char counter = 0;
  //Now we run through every possible value of a character and make space for its output.
  do {
    output[counter] = malloc(sizeof(hex));
    counter ++;
  } while (counter != 0);
  counter = 0;

  //Then we handle input. As 1 and 2 have done, this takes input as either an argument or expects it from stdin.
  if (argc >= 2) {
    strncpy(buffer, argv[1], sizeof(buffer) - 1);
  } else if (fgets(buffer, sizeof(buffer) - 1, stdin) != NULL) {
    //remove trailing newline if one exists
    char *p;
    if ((p = strchr(buffer, '\n')) != NULL && *(p + 1) == '\0') {
      *p = '\0';
    }
  } else {
    status = 1;
    return status;
  }
  //What follows is the logic for converting every pair of hexadecimal characters to a single char.
  while (*cur && *cur + 1) {
    temp = hex2dec[(unsigned) *cur++] << 4;
    temp |= hex2dec[(unsigned) *cur++];
    *curhex++ = temp;
    temp = 0;
    }
  //Edge case: odd number of characters
  if (*cur) {
    temp = hex2dec[(unsigned) *cur++] << 4;
    *curhex = temp;
  }

  //MAIN LOOPS
  //First, generating every possible plaintext that could generate the cipher text we were given.
  //We can loop through every value of the counter variable through a do-while loop,
  //  checking for when the counter becomes its starting value.
  do {
    //output[counter] = hex ^ counter
    curhex = &hex[0];
    int i = 0;
    while (*curhex) {
      output[counter][i] = (*curhex++) ^ counter;
      i++;
    }
    counter ++;
  } while (counter != 0);

  //We then score every generated plaintext, using the same loop construction as above.
  counter = 0;
  do {
    scores[counter] = score(sizeof(hex) - 1, output[counter]);
    counter ++;
  } while (counter != 0);

  //Now we need to find the best result.
  unsigned char result = 0;
  signed int maxscore = -1;
  counter = 0;
  //Using the same loop construction as above, we compare each score to our stored
  //  maximum score, replacing it and the stored key each time it is succeeded.
  do {
    if (scores[counter] >= 0 && scores[counter] > maxscore) {
      result = counter;
      maxscore = scores[counter];
    }
    counter ++;
  } while (counter != 0);

  printf("%c(%x) with a score of %d: %s\n", result, result, scores[result], output[result]);

  //Very important step: freeing every resource that's been allocated after use.
  for (int i = 0; i < sizeof(scores)/sizeof(scores[0]) - 1; i++) {
    free(output[i]);
  }

  return status;
}
