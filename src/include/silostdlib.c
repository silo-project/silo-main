#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#include "define.h"
#include "memalloc.h"

void * ReAllocate(void ** t, size_t s) {
	void * p = realloc(*t, s);
	if (!p) return NULL; else return *t = p;
}

inline Bitfield_t* BitfieldAddress(Bitfield_t * b, Bitindex_t i) { return &b[(i&~(UINTBITCOUNT-1))>>UINTBITSHIFT]; }
inline unsigned    BitfieldCalcula(Bitindex_t i) { return 0x1<<i%UINTBITCOUNT; }

inline bool 	 BitfieldGet(Bitfield_t * b, Bitindex_t i) { return *BitfieldAddress(b, i) &   BitfieldCalcula(i); }
inline bool 	 BitfieldSet(Bitfield_t * b, Bitindex_t i) { return *BitfieldAddress(b, i) |=  BitfieldCalcula(i); }
inline bool 	 BitfieldClr(Bitfield_t * b, Bitindex_t i) { return *BitfieldAddress(b, i) &= ~BitfieldCalcula(i); }
inline bool BitfieldSetValue(Bitfield_t * b, Bitindex_t i, bool v) { if (v) return BitfieldSet(b, i); else return BitfieldClr(b, i); }
