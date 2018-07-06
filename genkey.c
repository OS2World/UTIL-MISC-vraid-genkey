/*
 * $Source: /home/vitus/Data/Source/driver/raid/Setup/RCS/genkey.c,v $
 * $Revision: 1.4 $
 * $Date: 2008/03/04 22:29:27 $
 * $Locker:  $
 *
 *	Generates user key for VRAID.flt
 *
 * $Log: genkey.c,v $
 * Revision 1.4  2008/03/04 22:29:27  vitus
 * - changes to make source compilable under linux
 *
 * Revision 1.3  1999/06/06 20:04:42  vitus
 * - changed key calculation to use XOR and include previous characters
 *
 * Revision 1.2  1998/12/13 23:04:24  vitus
 * - changed as global headers moved to /source/Lib
 *
 * Revision 1.1  1998/12/07 02:14:36  vitus
 * Initial revision
 * -------------------------------------------
 * This code is Copyright Vitus Jensen 1998-99
 */
static char const id[]="$Id: genkey.c,v 1.4 2008/03/04 22:29:27 vitus Exp $";

#include <stdio.h>
#include <string.h>

#if defined __OS2__
# define INCL_BASE
# include <os2.h>
#else
typedef void		VOID;
typedef unsigned char	UCHAR;
typedef unsigned int	UINT;
#endif

#include "../../../lib/defines.h"



#define REG_MAGIC	0x53A1B7F9
#define REG_KEYLEN	40

UCHAR const	szProduct[]=		"Visual RAID Filter";
UCHAR		szUser[REG_KEYLEN];
UCHAR		szKey[REG_KEYLEN];
UINT		uLevel;				/* 1: up to RAID0,
						   2: plus RAID1
						   3: plus RAID4+5 */



PRIVATE VOID
GenKey(VOID)
{
    UCHAR	c = szProduct[0];		/* start value */
    int		len;
    UCHAR const	*p;
    UCHAR const *k;

    memset(szKey, 0, REG_KEYLEN);
    for( len = 0, p = szUser, k = szProduct;
	 *p != '\0';
	 ++p, ++len, k = (*(k+1) == '\0' ? szProduct : k+1) )
    {
	c += (UCHAR)((*p & 0xf0) >> 2 ^ (*p & 0x0f));
	c *= *k + uLevel;
	c += *k;
	szKey[len] = c;
    }

    {
	int	i;

	printf("Key: |");
	for( i = 0; i < len; ++i )
	{
	    printf("%02x", szKey[i]);
	}
	printf("|\n");
    }
    return;
}




PRIVATE VOID
Help(void)
{
    printf("usage: genkey <level> <user name>\n");
    return;
}




PUBLIC int
main(int argc,char *argv[])
{
    int	i;

    if( argc != 3 )
    {
	Help();
	return -1;
    }

    sscanf(argv[1], " %u", &uLevel);
    if( uLevel < 1  ||  uLevel > 3 )
    {
	fprintf(stderr, "Invalid level %u\n", uLevel);
	return -2;
    }

    if( argv[2][0] == '"' )
	i = 1;
    else
	i = 0;

    strncpy(szUser, &argv[2][i], REG_KEYLEN);
    if( szUser[strlen(szUser)-1] == '"' )
	szUser[strlen(szUser)-1] = '\0';

    printf("Generating key for %s\nUser: |%s|\nLevel: %u\n\n",
	   szProduct, szUser, uLevel);

    GenKey();

    return 0;
}
