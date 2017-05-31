#include <stdio.h>
#include <stdlib.h>
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

//relative frequency list courtesy of http://mdickens.me/typing/letter_frequency.html
signed int score (size_t size, char* chararray) {
  signed int result = unprintable_score(size, chararray);
  if (result < 0) {
    return result;
  }
  const unsigned char monograph[] = " etaoinsrhldcumfgpyw\nb,.vk-\"_\'x)(;0j1q=2:"
                                    "z/*!?$35>49[]867\\+|&<%@#^`~";
  const unsigned char digraph[] = "th""he""in""er""an""re""on""at""en""nd""st"
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
  const unsigned char trigraph[] =  "the""ing""and""ion""ent""hat""her""tio"
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
 * This takes a hexadecimal number as input and prints its base64 representation.
 */
int main (int argc, char* argv[]) {

  int status = 0;
  //1440 bits of information + terminating null
  char buffer[361] = "";
  char* cur = buffer;
  unsigned char hex[181] = "";
  unsigned char* curhex = hex;
  char* output[256];
  char temp = 0;
  signed int scores[256] = {0};
  unsigned char counter = 0;
  do {
    output[counter] = malloc(sizeof(hex));
    counter ++;
  } while (counter != 0);
  counter = 0;

  //input hex value
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
  while (*cur && *cur + 1) {
    temp = hex2dec[(unsigned) *cur++] << 4;
    temp |= hex2dec[(unsigned) *cur++];
    *curhex++ = temp;
    temp = 0;
    }
  if (*cur) {
    temp = hex2dec[(unsigned) *cur++] << 4;
    *curhex = temp;
  }

  //MAIN LOOP
  do {
    //output[counter] = hex ^ counter
    curhex = &hex[0];
    int i = 0;
    while (*curhex) {
      output[counter][i] = (*curhex++) ^ counter;
      i++;
    }
    counter ++;
    //x = 0; do while x != 0 -> run through every possible value of x once and only once.
  } while (counter != 0);

  counter = 0;
  do {
    scores[counter] = score(sizeof(hex) - 1, output[counter]);
    counter ++;
  } while (counter != 0);

  unsigned char result = 0;
  signed int maxscore = -1;
  counter = 0;
  do {
    if (scores[counter] >= 0 && scores[counter] > maxscore) {
      result = counter;
      maxscore = scores[counter];
    }
    counter ++;
  } while (counter != 0);

  printf("%c(%x) with a score of %d: %s\n", result, result, scores[result], output[result]);
  /* Debug output:
  for (int i = 0; i < sizeof(scores)/sizeof(scores[0]); i++) {
    if (scores[i] >= 0) {
      printf("%x: %d\t(", i, scores[i]);
      for (int j = 0; hex[j] && j < sizeof(hex); j++) {
        if (output[i][j] >= 32 && output[i][j] <= 126) {
          printf("%c", output[i][j]);
        } else {
          printf("\\%.3o", output[i][j]);
        }
      }
      printf(")\n");
    }
  }
  */

  for (int i = 0; i < sizeof(scores)/sizeof(scores[0]) - 1; i++) {
    free(output[i]);
  }

  return status;
}
