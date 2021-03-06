#include <gccore.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "mystring.h"

/* not case sensible functions */

char *ms_AllocCopy (char *source, int addbytes) // basically remove/add \n\r, source buffer must contain enought space
	{
	if (!source || *source == '\0') return NULL;
	
	char *p;
	int l = strlen (source);
	
	p = malloc (l + 1 + addbytes);
	strcpy (p, source);
	return p;
	}

void ms_strtoupper(char *str)
	{
	if (!str) return;
    for(; *str; str++)
		if(*str>='a' && *str<='z')
			*str = *str-('a'-'A');
}
 
void ms_strtolower(char *str)
	{
	if (!str) return;
	for(;*str;str++)
		if(*str>='A' && *str<='Z')
			*str = *str+('a'-'A');
	}

//taken from http://www.daniweb.com/software-development/c/code/216564/strings-case-insensitive-strstr
char *ms_strstr(char *str1, char *str2)
	{
	if ( !str2 || !*str2 )
		{
		return NULL;
		}

	for ( ; *str1; ++str1 )
		{
		if ( toupper((int)*str1) == toupper((int)*str2) )
			{
			/*
			* Matched starting char -- loop through remaining chars.
			*/
			const char *h, *n;
			for ( h = str1, n = str2; *h && *n; ++h, ++n )
				{
				if ( toupper((int)*h) != toupper((int)*n) )
					{
					break;
					}
				}
			if ( !*n ) /* matched all of 'str2' to null termination */
				{
				return str1; /* return the start of the match */
				}
			}
		}
	return NULL;
	}

int ms_strcmp(const char *s1, const char *s2) 
	{
	int ret = 0;
	while (!(ret = toupper(*(unsigned char *) s1) - toupper(*(unsigned char *) s2)) && *s2) ++s1, ++s2;

	if (ret < 0) 
		{
		ret = -1;
		} 	
	else if (ret > 0) 
		{
		ret = 1 ;
		}

	return ret;
	}


int ms_isequal(char *str1, char *str2)
	{
	if (!str1 || !str2 || !*str1 || !*str2)
		return 0;
		
	int l = strlen(str1);
	
	if (l != strlen(str2)) return 0; // not equal
	
	while (*str1)
		{
		if (toupper((int)*str1) != toupper((int)*str2))
			{
			return 0;
			}
		
		str1++;
		str2++;
		}
	return 1;
	}

// Very simple wrapper for some utf8
char *ms_utf8_to_ascii (char *string)
	{
	char *buff = calloc (1, strlen(string));
	
	int i;
	int j = 0;
	
	for (i = 0; i < strlen(string); i++)
		{
		if (string[i] != 0xC3)
			{
			buff[j] = string[i];
			}
		else
			{
			i++;
			char c = '?';
			
			if (string[i] >= 0xA8 && string[i] <= 0xAB) c = 'e';
			if (string[i] >= 0xA0 && string[i] <= 0xA6) c = 'a';
			if (string[i] >= 0xAC && string[i] <= 0xAF) c = 'i';
			if (string[i] >= 0xB2 && string[i] <= 0xB6) c = 'o';

			buff[j] = c;
			}
		j++;
		}
 
	return buff;
	}
	
u8 *ms_FindStringInBuffer (u8 *buffer, size_t size, char *string)
	{
	int stringLen = strlen (string);
	
	if (size < stringLen) return NULL;
	
	int i, j;
	for (i = 0; i < size-stringLen; i++)
		{
		for (j = 0; j < stringLen; j++)
			{
			if (buffer[i + j] != string[j]) break;
			}
		if (j == stringLen) return buffer + i;
		}
	
	return NULL;
	}
	
// This function will return a (mallocated) section of delimited text, ex. "section0;section1;....;sectionn"
char *ms_GetDelimitedString (char *string, char sep, int idx)
	{
	char *p = string;
	char *psep;
	char *buff;
	int i;
	
	if (!string)
		return NULL;
	
	if (!strlen (string))
		return NULL;
		
	for (i = 0; i <= idx; i++)
		{
		//Debug ("%d %s", idx, p);
		
		psep = strchr (p, sep);
		
		if (psep == NULL && i == idx)
			{
			psep = p + strlen(p);
			}
			
		if (i == idx)
			{
			if (psep-p == 0) return NULL;
			
			buff = malloc ((psep-p)+1);
			strncpy (buff, p, (psep-p));
			buff[(psep-p)] = 0;
			return buff;
			}
		
		if (psep == NULL)
			{
			break;
			}
			
		p = psep+1;
		}
	
	return NULL;
	}


// search "tofind" and replace with "replace" in "string" (slow)
void ms_Subst (char *string, char *tofind, char *replace)
	{
	char * found;
	char * s1;
	char * s2;
	
	do
		{
		found = strstr (string, tofind);
		if (found)
			{
			s1 = malloc (strlen(string)+strlen(replace));
			s2 = malloc (strlen(string)+strlen(replace));
			found[0] = 0;
			strcpy (s1, string);
			strcpy (s2, &found[strlen(tofind)]);
		
			sprintf(string, "%s%s%s", s1, replace, s2);
			free (s1);
			free (s2);
			}
		}
	while (found != NULL);
	}

int ms_isnumeric(char *str)
	{
	while(*str)
		{
		if(!isdigit((unsigned char)*str))
		return 0;
		str++;
		}
	return 1;
	}

