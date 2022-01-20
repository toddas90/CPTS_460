#define kstrcmp strcmp
#define kstrlen strlen

int kstrcmp(const char *s1, const char *s2)
{
  while ((*s1 && *s2) && (*s1==*s2)){
      s1++; s2++;
  }
  if (*s1==0 && *s2==0)
    return 0;
  
  return *s1 - *s2;
}

int kstrlen(const char *s) {
    unsigned int n = 0;
    while (*s++)
	++n;
    return n;
}
