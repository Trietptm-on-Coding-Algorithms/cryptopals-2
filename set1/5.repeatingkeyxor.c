#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * This takes a hexadecimal string XOR'd by a single character and guesses what the key is.
 *
 * Expected arguments: 5.repeatingxor key plaintext_filename
 */
int main (int argc, char* argv[]) {
  int status = 0;
  //Sized for 255 chars of content + 1 '\0'
  char key[256] = {0};
  size_t index = 0;
  char plaintext_filename[256] = {0};

  //Here we handle input. As previous exercises have done, this takes input as either an argument or expects it from stdin.
  if (argc >= 3) {
    strncpy(key, argv[1], sizeof(key) - 1);
    strncpy(plaintext_filename, argv[2], sizeof(plaintext_filename) - 1);
  } else if ((fgets(key, sizeof(key) - 1, stdin) != NULL)
      && (fgets(plaintext_filename, sizeof(plaintext_filename) - 1, stdin) != NULL)) {
    //remove trailing newline if one exists
    char *p;
    if ((p = strchr(key, '\n')) != NULL && *(p + 1) == '\0') {
      *p = '\0';
    }
    if ((p = strchr(plaintext_filename, '\n')) != NULL && *(p + 1) == '\0') {
      *p = '\0';
    }
  } else {
    //TODO: add stderr messages about missing input.
    status = 1;
    return status;
  }

  FILE * fp = fopen(plaintext_filename, "r");

  char next_char = 0;
  //No sense doing multiple passes is we can get by with just one.
  //We can assume we'll get at least one char, otherwise we aren't doing anything anyways.
  next_char = getc(fp);
  //getc returns -1 on EOF
  while (next_char >= 0) {
    //Since this is character-by character, the logic is simple enough to not bother with a function call.
    char cipher = next_char ^ key[index];
    if (!key[++index]) {
      index = 0;
    }
    next_char = getc(fp);
    //I expect branch prediction to make this go reasonably fast for every character but the last.
    if (next_char >= 0) {
      printf("%02x", cipher);
    }
  }

  //Final \n to be polite to the user.
  printf("\n");
  fclose(fp);
  return status;
}
