/*
 * There are a few things you need to do to get this working.
 * Firstly I suggest putting the prototype for the cprintf()'s in
 * your main headerfile. Add something like this.
 *
 *   int cprintf    args (( char *buf, char *ptr, ... ));
 *   int cnprintf   args (( char *buf, int maxlen, char *ptr, ... ));
 *
 * Secondly, you may want to add cprintf.c to your Makefile.
 *
 * And finally, you may want to take a look at the ansi tags
 * found in collen() and string_restrict(), and make sure
 * they fit what you use. Currently it's set to '#' as the
 * ansi pretag, and r,R,g,G,etc as the valid ansi tags. Just
 * change this to fit what you currently use.
 *
 * cprintf.c by Brian Graversen
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>
/*
 * should also include your main headerfile,
 * I assume it's merc.h. We need this for stuff
 * like TRUE, FALSE, MAX_STRING_LENGTH, etc.
 */
#include "merc.h"

/* edit these values so they fit your mud */
#define  _NO_STRING_LIMIT_      -1
#define  COLOR_TAG              '{'

/*
 * local procedures
 */
char *string_restrict  ( char *str, int size );
int   collen           ( const char *str );
int  _cprintf          ( char *buf, int maxlen, char *ptr, va_list ap );

/*
 * Acts like sprintf(), but doesn't break alignment
 * due to colors. It only supports %d and %s. Returns
 * the amount of chars copied.
 */
int cprintf(char *buf, char *ptr, ...)
{
  va_list ap;

  va_start(ap, ptr);

  return _cprintf(buf, _NO_STRING_LIMIT_, ptr, ap);
}

/*
 * Just as cprintf(), but safer, since you can restrict
 * the maximum amount of copied chars. It will return
 * the amount of copied chars, unless the output was
 * truncated due to reaching maxlen before it was done
 * copying the entire string, in which case it will return -1.
 */
int cnprintf(char *buf, int maxlen, char *ptr, ...)
{
  va_list ap;

  va_start(ap, ptr);

  return _cprintf(buf, maxlen, ptr, ap);
}

int _cprintf(char *buf, int maxlen, char *ptr, va_list ap)
{
  char dirty[100];
  char *s;
  int i, copied = 0;
  bool bEnd;

  while(*ptr != '\0')
  {
    bool reverse = FALSE;
    int size = 0, max_size = 0, j = 0;
    bEnd = FALSE;

    switch(*ptr)
    {
      default:
	*buf++ = *ptr++;

	if (++copied == maxlen)
	  goto done_copied;

	break;
      case '%':

	/* should we align this in reverse ? */
	if (*(ptr + 1) == '-')
	{
	  ptr++;
	  reverse = TRUE;
	}

	/* get the size, if any */
	while (isdigit(*(ptr + 1)))
	{
	  size *= 10;
	  size += *(++ptr) - '0';
	}

	/* any size restrictions ? */
	if (*(ptr + 1) == '.')
	{
	  ptr++;
	  while (isdigit(*(ptr + 1)))
          {
	    max_size *= 10;
	    max_size += *(++ptr) - '0';
          }
	}

	switch(*(++ptr))
	{
          default:
	    *buf++ = '%';

            if (++copied == maxlen)
	      goto done_copied;

	    break;
          case 's':
	    s = va_arg(ap, char *);
	    s = string_restrict(s, max_size);

	    size -= collen(s);

	    if (!reverse)
	    {
	      while(size-- > 0)
	      {
		*buf++ = ' ';

                if (++copied == maxlen)
	          goto done_copied;
	      }
	    }
	    while(*s != '\0')
	    {
	      *buf++ = *s++;

	      if (!reverse && *s == '\0')
		bEnd = TRUE;

              if (++copied == maxlen)
                goto done_copied;
	    }
	    if (reverse)
	    {
	      while(size-- > 0)
	      {
		*buf++ = ' ';

                if (size == 0)
	          bEnd = TRUE;

                if (++copied == maxlen)
	          goto done_copied;
	      }
	    }
	    ptr++;
	    break;
          case 'd':
	    i = va_arg(ap, int);

	    /* a little trick to see how long the number is */
	    sprintf(dirty, "%d", i);
	    size -= strlen(dirty);

	    if (!reverse)
	    {
	      while(size-- > 0)
	      {
		*buf++ = ' ';

                if (++copied == maxlen)
	          goto done_copied;
	      }
	    }

	    while (dirty[j] != '\0')
	    {
	      *buf++ = dirty[j++];

	      if (!reverse && dirty[j] == '\0')
		bEnd = TRUE;

              if (++copied == maxlen)
                goto done_copied;
	    }

	    if (reverse)
	    {
	      while(size-- > 0)
	      {
		*buf++ = ' ';

                if (size == 0)
	          bEnd = TRUE;

                if (++copied == maxlen)
	          goto done_copied;
	      }
	    }
	    ptr++;
	    break;
	}
	break;
    }
  }

  /*
   * this is our jumppoint, we use a goto for cleaner code,
   * some people may argue that one should never use goto's
   * while others will argue that refusing to use goto's no
   * matter what, can result in code that's horrible to read.
   */
 done_copied:
  *buf = '\0';

  /* if the output was truncated, we return -1 */
  if (*ptr != '\0' && (*(++ptr) != '\0' || !bEnd))
    copied = -1;

  /* clean up */
  va_end(ap);

  /* return how much we copied */
  return copied;
}

/*
 * This nifty little function calculates the length of a
 * string without the color tags. If you use other tags
 * than those mentioned here, then you should add them.
 */
int collen(const char *str)
{
  int len = 0;

  while (*str != '\0')
  {
    switch(*str)
    {
      default:
	len++, str++;
	break;
      case COLOR_TAG:
	switch(*(++str))
	{
          default:
	    len++;
	    break;
          case 'A':  case 'a':
  	  case 'B':  case 'b':
  	  case 'C':  case 'c':
  	  case 'D':  case 'd':
          case 'E':  case 'e':
          case 'F':  case 'f':
  	  case 'G':  case 'g':
          case 'H':  case 'h':
          case 'I':  case 'i':
          case 'J':  case 'j':
          case 'K':  case 'k':
          case 'L':  case 'l':
          case 'M':  case 'm':
          case 'N':  case 'n':
          case 'O':  case 'o':
          case 'P':  case 'p':
          case 'Q':  case 'q':
  	  case 'R':  case 'r':
          case 'S':  case 's':
          case 'T':  case 't':
          case 'U':  case 'u':
          case 'V':  case 'v':
  	  case 'W':  case 'w':
          case 'X':  case 'x':
  	  case 'Y':  case 'y':
          case 'Z':  case 'z':
          case '1':  case '!':
	  case '2':  case '@':
	  case '3':  case '#':
	  case '4':  case '$':
	  case '5':  case '%':
	  case '6':  case '^':
	  case '7':  case '&':
	  case '8':  case '*':
          case '9':  case '(':
          case '0':  case ')':
	    str++;
	    break;
	}
	break;
    }
  }

  return len;
}

/*
 * This nifty little function will return the
 * longest possible prefix of 'str' that can
 * be displayed in 'size' characters on a mud
 * client. (ie. it ignores ansi chars).
 */
char *string_restrict(char *str, int size)
{
  static char buf[MAX_STRING_LENGTH] = { '\0' };
  char *ptr = buf;
  int len = 0;
  bool done = FALSE;

  /* no size restrictions, we just return the string */
  if (size == 0)
    return str;

  while (*str != '\0' && !done)
  {
    switch(*str)
    {
      default:
	if (++len > size)
	{
	  done = TRUE;
	  break;
	}
	*ptr++ = *str++;
	break;
      case COLOR_TAG:
	switch(*(++str))
	{
	  default:
            if (++len > size)
            {
              done = TRUE;
              break;
            }
	    *ptr++ = COLOR_TAG;
	    break;
          case 'A':  case 'a':
  	  case 'B':  case 'b':
  	  case 'C':  case 'c':
  	  case 'D':  case 'd':
          case 'E':  case 'e':
          case 'F':  case 'f':
  	  case 'G':  case 'g':
          case 'H':  case 'h':
          case 'I':  case 'i':
          case 'J':  case 'j':
          case 'K':  case 'k':
          case 'L':  case 'l':
          case 'M':  case 'm':
          case 'N':  case 'n':
          case 'O':  case 'o':
          case 'P':  case 'p':
          case 'Q':  case 'q':
  	  case 'R':  case 'r':
          case 'S':  case 's':
          case 'T':  case 't':
          case 'U':  case 'u':
          case 'V':  case 'v':
  	  case 'W':  case 'w':
          case 'X':  case 'x':
  	  case 'Y':  case 'y':
          case 'Z':  case 'z':
	  case '1':  case '!':
	  case '2':  case '@':
	  case '3':  case '#':
	  case '4':  case '$':
	  case '5':  case '%':
	  case '6':  case '^':
	  case '7':  case '&':
	  case '8':  case '*':
          case '9':  case '(':
          case '0':  case ')':
	    *ptr++ = COLOR_TAG;
	    *ptr++ = *str++;
	    break;
	}
	break;
    }
  }
  *ptr = '\0';

  return buf;
}
