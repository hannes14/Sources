#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <kernel/si_gmp.h>

#include <omalloc/omalloc.h>
#include <Singular/s_buff.h>
//struct s_buff_s
//{
//    char * buff; // buffer
//    int fd;      // file descrr.
//    int size;    // size of buff
//    int bp;      // current pos. in buff
//    int end;     // last position in buff
//};

//typedef struct s_buff_s * s_buff;

#define S_BUFF_LEN 128

s_buff s_open(int fd)
{
  s_buff F=(s_buff)omAlloc0(sizeof(*F));
  F->fd=fd;
  F->buff=(char*)omAlloc0(S_BUFF_LEN+2); /*debug*/
  return F;
}

s_buff s_open_by_name(const char *n)
{
  int fd=open(n,O_RDONLY);
  return s_open(fd);
}
int    s_close(s_buff &F)
{
  if (F!=NULL)
  {
    omFreeSize(F->buff,S_BUFF_LEN);
    int r=close(F->fd);
    omFreeSize(F,sizeof(*F));
    F=NULL;
    return r;
  }
  return 0;
}

int s_getc(s_buff F)
{
  if (F==NULL) printf("s_getc\n"); /*debug*/
  if (F->bp>=F->end)
  {
    memset(F->buff,0,S_BUFF_LEN); /*debug*/
    int r=read(F->fd,F->buff,S_BUFF_LEN);
    if (r<=0)
    {
      F->is_eof=1;
      return -1;
    }
    else
    {
      F->end=r-1;
      F->bp=0;
      return F->buff[0];
    }
  }
  /*else*/
  F->bp++;
  return F->buff[F->bp];
}
int s_isready(s_buff F)
{
  if (F==NULL) printf("s_isready\n"); /*debug*/
  if (F->bp>=F->end) return 0;
  int p=F->bp+1;
  while((p<F->end)&&(F->buff[p]<=' ')) p++;
  if (p>=F->end) return 0;
  printf("next char:%c(%d)\n",F->buff[p],F->buff[p]); /*debug*/
  return 1;
}

int s_ungetc(int c, s_buff F)
{
  if (F==NULL) printf("s_ungetc\n"); /*debug*/
  if (F->bp>=0)
  {
    F->buff[F->bp]=c;
    F->bp--;
  }
}

int s_readint(s_buff F)
{
  char c;
  int neg=1;
  int r=0;
  int digit=0;
  do
  {
    c=s_getc(F);
  } while((!F->is_eof) && (c<=' '));
  if (c=='-') { neg=-1; c=s_getc(F); }
  while(isdigit(c))
  {
    digit++;
    r=r*10+(c-'0');
    c=s_getc(F);
  }
  s_ungetc(c,F);
  if (digit==0) { printf("unknown char %c(%d)\n",c,c); /*debug*/
                  printf("buffer:%s\np=%d,e=%d\n",F->buff,F->bp,F->end);fflush(stdout); } /*debug*/
  return r*neg;
}

int s_readbytes(char *buff,int len, s_buff F)
{
  int i=0;
  while((!F->is_eof)&&(i<len))
  {
    buff[i]=s_getc(F);
    i++;
  }
  return i;
}

void s_readmpz(s_buff F, mpz_t a)
{
  mpz_set_ui(a,0);
  char c;
  int neg=1;
  do
  {
    c=s_getc(F);
  } while((!F->is_eof) && (c<=' '));
  if (c=='-') { neg=-1; c=s_getc(F); }
  while(isdigit(c))
  {
    mpz_mul_ui(a,a,10);
    mpz_add_ui(a,a,(c-'0'));
    c=s_getc(F);
  }
  s_ungetc(c,F);
  if (neg==-1) mpz_neg(a,a);
}

void s_readmpz_base(s_buff F, mpz_ptr a, int base)
{
  mpz_set_ui(a,0);
  char c;
  int neg=1;
  do
  {
    c=s_getc(F);
  } while((!F->is_eof) && (c<=' '));
  if (c=='-') { neg=-1; c=s_getc(F); }
  while(c>' ')
  {
    if (isdigit(c))
    {
      mpz_mul_ui(a,a,base);
      mpz_add_ui(a,a,(c-'0'));
    }
    else if ((c>='a') && (c<'z'))
    {
      mpz_mul_ui(a,a,base);
      mpz_add_ui(a,a,(c-'a'+10));
    }
    else if ((c>='A') && (c<'Z'))
    {
      mpz_mul_ui(a,a,base);
      mpz_add_ui(a,a,(c-'A'+10));
    }
    else
    {
      s_ungetc(c,F);
      break;
    }
    c=s_getc(F);
  }
  if (neg==-1) mpz_neg(a,a);
}
int s_iseof(s_buff F)
{
  if (F!=NULL) return F->is_eof;
  else         return 1;
}

