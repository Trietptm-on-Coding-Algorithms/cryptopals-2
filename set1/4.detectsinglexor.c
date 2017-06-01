#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*courtesy of Orwellophile on stackoverflow
 * stackoverflow.com/questions/10324/convert-a-hexaeecimal-string-to-an-integer-efficiently-in-c
 */
static const unsigned char hex2dec[] = {
  [0 ... 255] = 0,
  ['0'] = 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
  ['A'] = 10, 11, 12, 13, 14, 15,
  ['a'] = 10, 11, 12, 13, 14, 15
};

//relative frequency lists courtesy of http://mdickens.me/typing/letter_frequency.html
static const unsigned char monograph[] =  " etaoinsrhldcumfgpyw\nb,.vk-\"_\'x)(;0j1q=2:"
                                          "z/*!?$35>49[]867\\+|&<%@#^`~";
static const unsigned char digraph[]   =  "th""he""in""er""an""re""on""at""en""nd""st"
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
 * This takes a hexadecimal number as input and prints its base64 representation.
 */
int main (int argc, char* argv[]) {

  int status = 0;
  char filename[361] = "";

  //If there is an argument passed, treat it as the name of the file to parse.
  //Otherwise, ask the user.
  //Note: the last char is reserved to ensure the string stays null-terminated.
  if (argc >= 2) {
    strncpy(filename, argv[1], sizeof(filename) - 1);
  } else {
    printf("Please enter the filename that you want parsed: \n");
    if (fgets(filename, sizeof(filename) - 1, stdin) != NULL) {
      char * p;
      //this next bit checks for the pattern "\n\0" and converts it to "\0\0".
      //I'm considering taking out the check for the succeeding null.
      if ((p = strchr(filename, '\n')) != NULL && *(p + 1) == '\0') {
        *p = '\0';
      }
    }
  }
 
  return status;
}
