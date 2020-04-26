#pragma once
#include <string.h>

// 16 bytes == 128 bit digest
#define MD5_DIGEST_LENGTH 16  

// MD5 Hash
typedef struct
{
	unsigned		buf[4];
	unsigned		bits[2];
	unsigned char	in[64];
} MD5Context_t;

void MD5Init( MD5Context_t *context );
void MD5Update( MD5Context_t *context, unsigned char const *buf, unsigned len );
void MD5Final( unsigned char digest[ MD5_DIGEST_LENGTH ], MD5Context_t *context );

unsigned MD5_PseudoRandom(unsigned nSeed);