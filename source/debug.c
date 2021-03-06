#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ogc/usbgecko.h>
#include <ogc/exi.h>
#include <unistd.h>
/*

Debug, will write debug information to sd and/or gecko.... as debug file is open/closed it will be VERY SLOW

*/

//#define DEBUGDISABLED

#define DEBUG_MAXCACHE 32
static char dbgfile[64];
static char *cache[DEBUG_MAXCACHE];

static int started = 0;
static int filelog = 0;
static int geckolog = 0;

s32 DebugStart (bool gecko, char *fn)
	{
#ifdef DEBUGDISABLED
	return;
#else
	filelog = 0;
	started = 0;
	
	sprintf (dbgfile, fn);

	geckolog = usb_isgeckoalive (EXI_CHANNEL_1);
	
	// check if the file exist
	FILE * f = NULL;
	f = fopen(dbgfile, "rb");
	if (f) 
		{
		filelog = 1;
		fclose (f);
		}
	
	memset (cache, 0, sizeof(cache));
	
	if (filelog || geckolog)
		started = 1;
	
	return started;
#endif	
	}
	
void DebugStop (void)
	{
#ifdef DEBUGDISABLED
	return;
#endif

	filelog = 0;
	started = 2;
	}

void Debug (const char *text, ...)
	{
#ifdef DEBUGDISABLED
	return;
#else	
	if (!started || text == NULL) return;
		
	int i;
	char mex[2048];
	FILE * f = NULL;

	va_list argp;
	va_start (argp, text);
	vsprintf (mex, text, argp);
	va_end (argp);
	
	strcat (mex, "\r\n");

	if (geckolog)
		{
		usb_sendbuffer( EXI_CHANNEL_1, mex, strlen(mex) );
		usb_flush(EXI_CHANNEL_1);
		usleep (500);
		}
	if (started == 2) return;
	if (filelog == 0) return;
	
	// If a message start with '@', do not open... it will be cached
	if (mex[0] != '@')
		f = fopen(dbgfile, "ab");

	//if file cannot be opened, cannot open the file, maybe filesystem unmounted or nand emu active... use cache
	if (f)
		{
		for (i = 0; i < DEBUG_MAXCACHE; i++)
			{
			if (cache[i] != NULL)
				{
				fwrite (cache[i], 1, strlen(mex), f );
				free (cache[i]);
				cache[i] = NULL;
				}
			}
		fwrite (mex, 1, strlen(mex), f );
		fclose(f);
		}
	else
		{
		for (i = 0; i < DEBUG_MAXCACHE; i++)
			{
			if (cache[i] == NULL)
				{
				cache[i] = calloc (strlen(mex) + 1, 1);
				strcpy (cache[i], mex);
				break;
				}
			}
		}
#endif		
	}

static char ascii(char s)
{
    if (s < 0x20) return '.';
    if (s > 0x7E) return '.';
    return s;
}

void gprintf (const char *format, ...)
{
#ifdef DEBUGDISABLED
	return;
#else
	char * tmp = NULL;
	va_list va;
	va_start(va, format);
	
	if((vasprintf(&tmp, format, va) >= 0) && tmp)
	{
        usb_sendbuffer(EXI_CHANNEL_1, tmp, strlen(tmp));
	}
	va_end(va);

	if(tmp)
        free(tmp);
#endif
}

void Debug_hexdump (void *d, int len)
{
#ifdef DEBUGDISABLED
	return;
#else
    u8 *data;
    int i, off;
    data = (u8*) d;

    gprintf("\n       0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  0123456789ABCDEF");
    gprintf("\n====  ===============================================  ================\n");

    for (off = 0; off < len; off += 16)
    {
        gprintf("%04x  ", off);
        for (i = 0; i < 16; i++)
            if ((i + off) >= len)
                gprintf("   ");
            else gprintf("%02x ", data[off + i]);

        gprintf(" ");
        for (i = 0; i < 16; i++)
            if ((i + off) >= len)
                gprintf(" ");
            else gprintf("%c", ascii(data[off + i]));
        gprintf("\n");
    }
#endif	
} 
void Debug_hexdumplog (void *d, int len)
{
#ifdef DEBUGDISABLED
	return;
#else
    u8 *data;
    int i, off;
	char b[2048] = {0};
	char bb[256];
		
    data = (u8*) d;

    sprintf(bb, "\n       0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  0123456789ABCDEF");
	strcat(b, bb);
    sprintf(bb, "\n====  ===============================================  ================\n");
	strcat(b, bb);

    for (off = 0; off < len; off += 16)
    {
        sprintf(bb, "%04x  ", off);
		strcat(b, bb);
        for (i = 0; i < 16; i++)
            if ((i + off) >= len)
				{
                sprintf(bb, "   ");
				strcat(b, bb);
				}
            else 
				{
				sprintf(bb, "%02x ", data[off + i]);
				strcat(b, bb);
				}

        sprintf(bb, " ");
		strcat(b, bb);
		
        for (i = 0; i < 16; i++)
            if ((i + off) >= len)
				{
                sprintf(bb, " ");
				strcat(b, bb);
				}
            else 
				{
				sprintf(bb, "%c", ascii(data[off + i]));
				strcat(b, bb);
				}
				
        sprintf(bb, "\n");
		strcat(b, bb);
    }
	Debug (b);
#endif	
} 