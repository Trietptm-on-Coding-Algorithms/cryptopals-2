This is the collection of my answers to the [**Cryptopals** challenges](https://cryptopals.com/).
If you haven't tried them and think you might, you probably shouldn't read the rest of the repository.
The implementations are abnormal as I am also trying to practice writing small and fast code.

STATUS:

1.  1.hex2b64.c  
    Synopsis  
    >*executable* [-f input_filename] | [raw_input] | [-]  
    If input has not been recieved as an argument, it will look to stdin for input.

2.  2.fixedxor.c  
    Synopsis  
    >*executable* [-f filename] | [raw_input_1 raw_input_2] | [-]  
    If input has not been recieved as an argument, it will look to stdin for input.

3.  3.xorcipher.c  
    Synopsis  
    >*executable* [-f filename] | [raw_input] | [-]  
    If input has not been recieved as an argument, it will look to stdin for input.

4.  4.detectsinglexor.c  
    Does not work. Correctly parses sample data from #3, but does not show any English-like strings in the sample #4's sample data.

5.  5.repeatingkeyxor.c  
    Synopsis
    >*executable* [-k key] [-f filename]  
    If key has not been provided as an argument, the program will interpret the first line as the key.  
    If filename has not been provided as the argument, the program will listen to stdin for the input.

6.  6.breakrepeatingxor.c  
    Work in Progress. Hamming distance calculation works. Nothing else is implemented yet.
