# include "dgd.h"
# include "hash.h"

/*
 * Generic string hash table.
 */

/*
 * NAME:	hashtab->new()
 * DESCRIPTION:	create a hashtable of size "size", where "maxlen" characters
 *		of each string are significant
 */
hashtab *ht_new(size, maxlen)
register unsigned int size;
unsigned int maxlen;
{
    register hashtab *ht;

    ht = (hashtab *) ALLOC(char, sizeof(hashtab) + sizeof(hte*) * (size - 1));
    ht->size = size;
    ht->maxlen = maxlen;
    memset(ht->table, '\0', size * sizeof(hte*));

    return ht;
}

/*
 * NAME:	hashtab->del()
 * DESCRIPTION:	delete a hash table
 */
void ht_del(ht)
hashtab *ht;
{
    FREE(ht);
}

/*
 * NAME:	hashstr()
 * DESCRIPTION:	Hash string s, considering at most len characters. Return
 *		an unsigned modulo size.
 *		Based on Peter K. Pearson's article in CACM 33-6, pp 677.
 */
unsigned short hashstr(s, len)
register char *s;
register unsigned int len;
{
    static char table[] = {
	  1,  87,  49,  12, 176, 178, 102, 166,
	121, 193,   6,  84, 249, 230,  44, 163,
	 14, 197, 213, 181, 161,  85, 218,  80,
	 64, 239,  24, 226, 236, 142,  38, 200,
	110, 177, 104, 103, 141, 253, 255,  50,
	 77, 101,  81,  18,  45,  96,  31, 222,
	 25, 107, 190,  70,  86, 237, 240,  34,
	 72, 242,  20, 214, 244, 227, 149, 235,
	 97, 234,  57,  22,  60, 250,  82, 175,
	208,   5, 127, 199, 111,  62, 135, 248,
	174, 169, 211,  58,  66, 154, 106, 195,
	245, 171,  17, 187, 182, 179,   0, 243,
	132,  56, 148,  75, 128, 133, 158, 100,
	130, 126,  91,  13, 153, 246, 216, 219,
	119,  68, 223,  78,  83,  88, 201,  99,
	122,  11,  92,  32, 136, 114,  52,  10,
	138,  30,  48, 183, 156,  35,  61,  26,
	143,  74, 251,  94, 129, 162,  63, 152,
	170,   7, 115, 167, 241, 206,   3, 150,
	 55,  59, 151, 220,  90,  53,  23, 131,
	125, 173,  15, 238,  79,  95,  89,  16,
	105, 137, 225, 224, 217, 160,  37, 123,
	118,  73,   2, 157,  46, 116,   9, 145,
	134, 228, 207, 212, 202, 215,  69, 229,
	 27, 188,  67, 124, 168, 252,  42,   4,
	 29, 108,  21, 247,  19, 205,  39, 203,
	233,  40, 186, 147, 198, 192, 155,  33,
	164, 191,  98, 204, 165, 180, 117,  76,
	140,  36, 210, 172,  41,  54, 159,   8,
	185, 232, 113, 196, 231,  47, 146, 120,
	 51,  65,  28, 144, 254, 221,  93, 189,
	194, 139, 112,  43,  71, 109, 184, 209,
    };
    register char h, l;

    h = 0;
    l = 0;
    while (*s != '\0' && len > 0) {
	h = l;
	l = table[UCHAR(l ^ *s++)];
	--len;
    }
    return (unsigned short) ((UCHAR(h) << 8) | UCHAR(l));
}

/*
 * NAME:	hashtab->lookup()
 * DESCRIPTION:	lookup a name in a hashtable, return the address of the entry
 *		or &NULL if none found
 */
hte **ht_lookup(ht, name)
hashtab *ht;
register char *name;
{
    register hte **e;

    e = &(ht->table[hashstr(name, ht->maxlen) & (ht->size - 1)]);
    while (*e != (hte *) NULL && strcmp((*e)->name, name) != 0) {
	e = &((*e)->next);
    }
    return e;
}
