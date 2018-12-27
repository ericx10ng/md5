//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "md5.h"

#if defined(__GNUC__)
#include <assert.h>
#else
#define assert(x)
#endif

static const uint32_t X[4][2] = {{0, 1}, {1, 5}, {5, 3}, {0, 7}};
static const uint32_t S[4][4] = {{ 7, 12, 17, 22 },{ 5, 9 , 14, 20 },{ 4, 11, 16, 23 },{ 6, 10, 15, 21 }};

uint32_t chain[4];
uint32_t state[4];

static uint32_t F( uint32_t X, uint32_t Y, uint32_t Z )
{
    return ( X & Y ) | ( ~X & Z );
}
static uint32_t G( uint32_t X, uint32_t Y, uint32_t Z )
{
    return ( X & Z ) | ( Y & ~Z );
}
static uint32_t H( uint32_t X, uint32_t Y, uint32_t Z )
{
    return X ^ Y ^ Z;
}
static uint32_t I( uint32_t X, uint32_t Y, uint32_t Z )
{
    return Y ^ ( X | ~Z );
}
// rotates x left s bits.
static uint32_t rotate_left( uint32_t x, uint32_t s )
{
    return ( x << s ) | ( x >> ( 32 - s ) );
}
// Pre-processin
static uint32_t count_padding_bits ( uint32_t length )
{
//	uint32_t div = length * BITS / BLOCK_SIZE;
    uint32_t mod = length * BITS % BLOCK_SIZE;
    uint32_t c_bits;
    if ( mod == 0 )
    {
        c_bits = MOD_SIZE;
    }
    else
    {
        c_bits = ( MOD_SIZE + BLOCK_SIZE - mod ) % BLOCK_SIZE;
    }
    return c_bits / BITS;
}

int32_t md5_start (void)
{
    chain[0] = A;
    chain[1] = B;
    chain[2] = C;
    chain[3] = D;
    return 0;
}

int32_t md5_update (uint32_t *w, int len)
{
    uint32_t ( *auxi[ 4 ])( uint32_t, uint32_t, uint32_t ) = { F, G, H, I };
    int sIdx;
    int wIdx;
    for (uint32_t j = 0; j < len; j += BLOCK_SIZE / BITS)
    {
        memmove ( state, chain, sizeof(chain) );
        for ( uint8_t roundIdx = 0; roundIdx < 4; roundIdx++ )
        {
            wIdx = X[ roundIdx ][ 0 ];
            sIdx = 0;
            for (uint8_t i = 0; i < 16; i++ )
            {
                state[sIdx] = state [(sIdx + 1)%4] + rotate_left( state[sIdx] +(*auxi[ roundIdx])( state[(sIdx+1) % 4],
                                                                                                   state[(sIdx+2) % 4],
                                                                                                   state[(sIdx+3) % 4]) + w[ wIdx ] + (uint32_t)floor((1ULL << 32) * fabs(sin( roundIdx * 16 + i + 1 )) ),
                                                                  S[ roundIdx ][ i % 4 ]);
                sIdx = ( sIdx + 3 ) % 4;
                wIdx = ( wIdx + X[ roundIdx ][ 1 ] ) & 0xF;
            }
        }
        chain[ 0 ] += state[ 0 ];
        chain[ 1 ] += state[ 1 ];
        chain[ 2 ] += state[ 2 ];
        chain[ 3 ] += state[ 3 ];
    }
    return EXIT_SUCCESS;
}

uint32_t md5_padding(uint8_t*data, uint32_t len, uint32_t msg_length)
{
    uint32_t bit_length = count_padding_bits ( msg_length );
    uint64_t app_length = msg_length * BITS;
    memset ( data + len, 0, bit_length );
    data [ len ] = SINGLE_ONE_BIT;
    memmove ( data + len + bit_length, (char *)&app_length, sizeof( uint64_t ) );
    return len + bit_length + sizeof( uint64_t );
}

int32_t md5_end (uint8_t *md5_32)
{
    memmove ( md5_32 + 0, (char *)&chain[0], sizeof(uint32_t) );
    memmove ( md5_32 + 4, (char *)&chain[1], sizeof(uint32_t) );
    memmove ( md5_32 + 8, (char *)&chain[2], sizeof(uint32_t) );
    memmove ( md5_32 + 12, (char *)&chain[3], sizeof(uint32_t) );
}

int md5_demo(char* data, int msg_len, uint8_t* result) {
    //
    int blockN = 0;
    uint32_t w[32];

    //
    md5_start();

    int bflag = 0;
    int remain = msg_len % (BLOCK_SIZE/BITS);
    if(remain == 0) {
        bflag = 1;
    }

    for(blockN = 0; blockN < msg_len/(BLOCK_SIZE/BITS) - bflag; blockN++) {
        memcpy ( (char *)w, data + blockN*(BLOCK_SIZE / BITS), BLOCK_SIZE / BITS );
        md5_update(w, BLOCK_SIZE/BITS);
    }

    int data_len = BLOCK_SIZE/BITS;
    if(remain > 0) {
        data_len = remain;
    }

    memcpy ( (char *)w, data + blockN*(BLOCK_SIZE/BITS), data_len);
    data_len = md5_padding((uint8_t *)w, data_len, msg_len);

    int n = data_len / (BLOCK_SIZE/BITS);
    assert((n == 1) || (n == 2));
    assert((data_len % (BLOCK_SIZE/BITS)) == 0);

    for(int i = 0; i < n; i++) {
        md5_update((uint32_t*)((char*)w + i*(BLOCK_SIZE/BITS)), BLOCK_SIZE/BITS);
    }

    md5_end(result);

    return 0;
}

