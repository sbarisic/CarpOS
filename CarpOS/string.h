#pragma once
#include "CarpSDK.h"

void * memchr( const void * s, int c, size_t n );
int memcmp( const void * s1, const void * s2, size_t n );
void * memcpy( void * s1, const void * s2, size_t n );
void * memmove( void * s1, const void * s2, size_t n );
void * memset( void * s, int c, size_t n );
int strcmp( const char * s1, const char * s2 );