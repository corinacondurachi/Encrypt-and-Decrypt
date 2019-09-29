# Encrypt and Decrypt

Encrypts an image using a cryptography algorithm. Then it decrypts the image by doing the same reversed operations.


# Files

 - Main.c
 - peppers.bmp (the image which is supposed to be encrypted )
 - peppers_linearized.bmp
 - peppers_permuted.bmp
 - peppers_encrypted.bmp
 - peppers_permuted_back.bmp
 - peppers_decrypted.bmp

## Describing the code
W=image's width
H=image's height

 - void xorshift32 (unsigned int seed,int n,unsigned int v[]): generates an array of n random numbers starting from the seed (a random value which will be 123456789)
 
 - void Durstenfeld_algorithm (int n,unsigned int *p,int seed,int dim,unsigned int * v): generates a random permutation having dimension of W * H
 - int image_width(FILE *f): returns the width of the image
 -  int image_height(FILE *f): returns the height of the image
 - void linearization(char *nume, int w, int h, unsigned char *L):  creates an unidimensional array L, containing every pixel of the image 
 - void linearized_image(char *nume1,char *nume2,unsigned char *L,int w,int h): creates a new image called peppers_linearized.bmp in order to verify if the linearization works
 - void image_permuted(unsigned char *L,int w,int h,unsigned int *p,unsigned char *P): permutes the image's pixels using the permutation resulted from  Durstenfeld_algorithm
 - void image_encrypted(unsigned char *C,unsigned char *P,int w,int h,unsigned int *v): applies XOR operation between pixels and the random numbers generated
 - void image_permuted_back(unsigned char *K,unsigned char *C,int w,int h,unsigned int *v): applies again XOR operation between pixels and the random numbers generated and generates the same image as the one permuted
 - unsigned int* permutation_reverse (unsigned int *P,int w,int h): generates the reverse of the permutation
 - void image_decrypted(unsigned char *K,unsigned char *D,int w,int h,unsigned int *p): permutes the image's pixels back using the reverse of the permutation
 - void chi_square (unsigned char *L,int j,int w,int h): calculates the values of chi_square test and then prints them











