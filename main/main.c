#include <stdio.h>
#include <stdlib.h>
#include<math.h>

void xorshift32(unsigned int seed,int n,unsigned int v[])
{
    unsigned int i,r;
    r=seed;
    for(i=0; i<n; i++)
    {
        r=r^r<<13;
        r=r^r>>17;
        r=r^r<<5;
        v[i]=r;
    }
}
void Durstenfeld_algorithm (int n,unsigned int *p,int seed,int dim,unsigned int *v)
{
    unsigned int r,k,aux,i=0;
    xorshift32(seed,dim,v);
    for(k=0; k<n; k++)
        p[k]=k;
    for(k=n-1; k>=1; k--)
    {
        r=v[i]%(k+1);
        aux=p[r];
        p[r]=p[k];
        p[k]=aux;
        i++;
    }
}
int image_width(char *name,FILE *f)
{
    int x=0;
    f= fopen(name , "rb");
    fseek(f, 18, SEEK_SET);
    fread(&x , 1 , 2 , f);
    return x;
    fclose(f);
}
int image_height(char *name,FILE *f)
{
    int x=0;
    f= fopen(name , "rb");
    fseek(f, 22, SEEK_SET);
    fread(&x , 1 , 2 , f);
    return x;
    fclose(f);
}
void linearization(char *nume,int w,int h,unsigned char *L)
{
    int i,j,n=0;
    unsigned char pRGB[3];
    FILE *f= fopen(nume , "rb");
    fseek(f,54,SEEK_SET);
    int padding;
    if(w % 4 != 0)
        padding = 4 - (3 * w) % 4;
    else
        padding = 0;
    for(i=0; i<w; i++)
    {
        for(j=0; j<h; j++)
        {
            fread(pRGB, 3, 1, f);
            L[3*n]=pRGB[0];
            L[3*n+1]=pRGB[1];
            L[3*n+2]=pRGB[2];
            n++;
        }
        fseek(f,padding,SEEK_CUR);
    }
    fclose(f);
}
void linearized_image(char *nume1,char *nume2,unsigned char *L,int w,int h)
{

    FILE *fin = fopen(nume1 , "rb");
    FILE *fout = fopen(nume2 , "wb");
    int i,x,j;
    for(i = 0; i < 54; i++)
    {
        fread(&x , 1 , 1 , fin);
        fwrite(&x , 1 , 1 , fout);
    }
    fseek(fout, 54, SEEK_SET);
    for(i = 0; i < w*h; i++)
    {
        for(j=0; j<3; j++)
            fwrite(&L[3*i+j], 1, 1, fout);
    }
    fclose(fin);
    fclose(fout);
}
void image_permuted(char *nume1,char *nume2,unsigned char *L,int w,int h,unsigned int *p,unsigned char *P)
{
    FILE *fin = fopen(nume1 , "rb");
    FILE   *f = fopen(nume2 , "wb");
    int i,x,j;
    for(i = 0; i < 54; i++)
    {
        fread(&x , 1 , 1 , fin);
        fwrite(&x , 1 , 1 , f);
    }
    fseek(f, 54, SEEK_SET);
    for(i = 0; i <w*h; i++)
    {
        for(j=0; j<3; j++)
        {
            P[3*i+j]=L[j+3*p[i]];
            fwrite(&P[j+3*i], 1, 1, f);
        }
    }
    fclose(fin);
    fclose(f);
}
union inttochar
{
    unsigned int x;
    unsigned char s[4];
};
void image_encrypted(char *nume1,char *nume2,unsigned int sv,unsigned char *C,unsigned char *P,int w,int h,unsigned int *v)
{
    FILE *fin = fopen(nume1 , "rb");
    FILE *f = fopen(nume2 , "wb");
    unsigned int n,x,i,j;
    n=2*w*h-1;
    xorshift32(sv,n,v);
    for(i = 0; i < 54; i++)
    {
        fread(&x , 1 , 1 , fin);
        fwrite(&x , 1 , 1 , f);
    }
    fseek(f, 54, SEEK_SET);
    union inttochar a,b;
    for(j=0; j<3; j++)
    {
        a.x=sv;
        b.x=v[w*h];
        C[j]=a.s[j]^P[j]^b.s[j];
        fwrite(&C[j],1,1,f);
    }
    for(i = 1; i < w*h; i++)
    {
        for(j=0; j<3; j++)
        {
            union inttochar a;
            a.x=v[w*h+i];
            C[3*i+j]=C[3*(i-1)+j]^P[3*i+j]^a.s[j];
            fwrite(&C[j+3*i], 1, 1, f);
        }
    }
    fclose(fin);
    fclose(f);
}
void image_permuted_back(char *nume1,char *nume2,unsigned int sv,unsigned char *K,unsigned char *C,int w,int h,unsigned int *v)
{
    FILE *fin = fopen(nume1 , "rb");
    FILE *f = fopen(nume2 , "wb");
    unsigned int n,x,i,j;
    n=2*w*h;
    xorshift32(sv,n,v);
    for(i = 0; i < 54; i++)
    {
        fread(&x , 1 , 1 , fin);
        fwrite(&x , 1 , 1 , f);
    }
    fseek(f, 54, SEEK_SET);
    union inttochar a,b;
    for(j=0; j<3; j++)
    {
        a.x=sv;
        b.x=v[w*h];
        K[j]=a.s[j]^C[j]^b.s[j];
        fwrite(&K[j],1,1,f);
    }
    for(i = 1; i < w*h; i++)
    {
        for(j=0; j<3; j++)
        {
            union inttochar a;
            a.x=v[w*h+i];
            K[3*i+j]=C[3*(i-1)+j]^C[3*i+j]^a.s[j];
            fwrite(&K[j+3*i], 1, 1, f);
        }
    }
    fclose(fin);
    fclose(f);
}
unsigned int* permutation_reverse (unsigned int *P,int w,int h)
{
    int k;
    unsigned int *a=malloc(w*h*sizeof(unsigned int));
    for(k=0; k<w*h; k++)
        a[P[k]]=k;
    return a;
}
void image_decrypted(char *nume1,char *nume2,unsigned char *K,unsigned char *D,int w,int h,unsigned int *p)
{
    FILE *fin = fopen(nume1, "rb");
    FILE   *f = fopen(nume2 , "wb");
    int i,x,j;
    for(i = 0; i < 54; i++)
    {
        fread(&x , 1 , 1 , fin);
        fwrite(&x , 1 , 1 , f);
    }
    fseek(f, 54, SEEK_SET);
    unsigned    int *a=permutation_reverse(p,w,h);
    for(i = 0; i <w*h; i++)
    {
        for(j=0; j<3; j++)
        {
            D[3*i+j]=K[j+3*a[i]];
            fwrite(&D[j+3*i], 1, 1, f);
        }
    }
    fclose(fin);
    fclose(f);
}
void chi_square (unsigned char *L,int j,int w,int h)
{
    int i,*fr;
    double chi=0;
    fr=calloc(256,sizeof(int));
    for(i = 0; i < w*h; i++)
        fr[L[3*i+j]]++;
    double f_barat=w*h/256;
    for(j=0; j<256; j++)
        chi=chi+(double)(fr[j]-f_barat)*(double)(fr[j]-f_barat)/f_barat;
    printf("%lf ",chi);
    printf("\n");
}
void print_chi(unsigned char *L,unsigned char *C,int inaltime,int latime)
{
    printf("Chi-squared test on RGB channels for peppers.bmp:");
    printf("\n");
    printf("R: ");
    chi_square(L,2,inaltime,latime);
    printf("G: ");
    chi_square(L,1,inaltime,latime);
    printf("B: ");
    chi_square(L,0,inaltime,latime);
    printf("\n");
    printf("Chi-squared test on RGB channels for peppers_criptat.bmp:");
    printf("\n");
    printf("R: ");
    chi_square(C,2,inaltime,latime);
    printf("G: ");
    chi_square(C,1,inaltime,latime);
    printf("B: ");
    chi_square(C,0,inaltime,latime);
}
void project()
{
    FILE *fin;
    unsigned int sv=123456789;
    fin = fopen("peppers.bmp" , "rb");
    int latime=image_width("peppers.bmp",fin);
    printf("%d ",latime);
    int inaltime=image_height("peppers.bmp",fin);
    printf("%d",inaltime);
    int dim=2*latime*inaltime;
    unsigned int *v=malloc(dim*sizeof(int));
    int n=latime*inaltime;
    unsigned int *p=malloc(n*sizeof(int));
    Durstenfeld_algorithm(n,p,123456789,dim,v);
    unsigned char *L=malloc(3*latime*inaltime);
    linearization("peppers.bmp",latime,inaltime,L);
    linearized_image("peppers.bmp","peppers_linearized.bmp",L,latime,inaltime);
    unsigned char *P=malloc(3*latime*inaltime);
    image_permuted("peppers.bmp","peppers_permuted.bmp",L,latime,inaltime,p,P);
    unsigned char *C=malloc(3*latime*inaltime);
    image_encrypted("peppers.bmp","peppers_encrypted.bmp",sv,C,P,latime,inaltime,v);
    unsigned char *K=malloc(3*latime*inaltime);
    free(P);
    image_permuted_back("peppers.bmp","peppers_permuted_back.bmp",sv,K,C,latime,inaltime,v);
    unsigned char *D=malloc(3*latime*inaltime);
    image_decrypted("peppers.bmp","peppers_decrypted.bmp",K,D,latime,inaltime,p);
    printf("\n");
    print_chi(L,C,latime,inaltime);
    free(v);
    free(p);
    free(L);
    free(C);
    free(K);
    free(D);
}


int main()
{
    project();
    return 0;
}
