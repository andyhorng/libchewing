/**
 * hanyupinyin.c
 *
 * Copyright (c) 2005
 *	libchewing Core Team. See ChangeLog for details.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file.
 */

/* @(#)hanyupinyin.c
 */

#include <stdio.h>
#include <string.h>
#include "hanyupinyin.h"
#include "hash.h"
#include "private.h"

/*
  according to XCIN bims_pinyin.cin
 */
static keymap* keytable;
static int INIT_FLAG = 0;
static int N_TOTAL = 0;

static void FreeMap()
{ 
	free(keytable);
}
static void InitMap()
{
	int i;
	FILE *fd;
	INIT_FLAG = 1;
	if ( getenv( "HOME" ) ) {
		char *filedir = strcat( getenv( "HOME" ), CHEWING_HASH_PATH );
		char *filepath = strcat( filedir, "/pinyin.tab" );

		if (access(filepath, R_OK) == 0) {
			/* Use user-defined tables */
			fd = fopen( filepath, "r" );
		}
		else {
			return;
			/* Failback */
			fd = fopen( CHEWING_DATADIR "/pinyin.tab", "r");
		}
	
		if ( fd ) {
			addTerminateService( FreeMap );
			fscanf( fd, "%d", &N_TOTAL );
			keytable = ALC( keymap, N_TOTAL );
			for ( i = 0; i < N_TOTAL - 1; i++ ){
				memset(&keytable[i], 0, sizeof(keymap));
				fscanf( fd, "%s %s",
					keytable[ i ].pinyin,
					keytable[ i ].zuin );
			}
			fclose( fd );
			return;
		}	
	}
}
static int compkey(const void *k1, const void *k2) {
	keymap *key1 = (keymap *) k1;
	keymap *key2 = (keymap *) k2;
	return strcmp(key1->pinyin, key2->pinyin);
}
/*
  0: Success
  Non-Zero: Fail to fully convert
  1: Failed fo lookup initals
  2: Failed fo lookup finals
  Map pinyin key-sequence to Zuin key-sequence.
  Caller should allocate char zuin[4].
 */

int HanyuPinYinToZuin( char *pinyinKeySeq, char *zuinKeySeq )
{
	if ( ! INIT_FLAG )
		InitMap();
	/*
	 * pinyinKeySeq[] should have at most 6 letters (Shuang)
	 * zuinKeySeq[] has at most 3 letters.
	 */
	keymap key, *res;
	strcpy(key.pinyin, pinyinKeySeq);
	res = bsearch(&key, keytable, N_TOTAL, sizeof(keymap), compkey);
	if ( res != NULL)
		sprintf( zuinKeySeq, "%s\0", res->zuin);
	else
		strcpy( zuinKeySeq, "");
	return 0;
}