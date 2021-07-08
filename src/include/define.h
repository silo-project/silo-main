/*
	Name: SILO defines
	Copyright: SILO Project
	Author: see AUTHOR file
	Date: 10-10-20 09:14
	Description: basic SILO defines
*/
#ifndef SILO_DEFINE_HEAD
#define SILO_DEFINE_HEAD

#include <stdbool.h>
#include <stddef.h>

// default processing size
#define SILO_WORD long long
#define SILO_ADDR long long

typedef unsigned SILO_WORD DEFT_WORD;
typedef unsigned SILO_ADDR DEFT_ADDR;

typedef DEFT_WORD SIGNALSIZE;
typedef unsigned int NODEID;
typedef unsigned int PORTID;

typedef unsigned int nindex_t;
typedef unsigned int pindex_t;

typedef unsigned Bitfield_t;
typedef unsigned Bitindex_t;

struct DataVector {
	void * Base;
	size_t Size;
};
struct VectorStruct {
	size_t Size;
	char   Base[];
};

#define BASICMEM 4096 // 4kiB = 1Page
#define UINTBITCOUNT 32
#define UINTBITSHIFT 5 // 1<<5 == 32;

Bitfield_t* BitfieldAddress(Bitfield_t * b, Bitindex_t i);
unsigned    BitfieldCalcula(Bitindex_t i);

bool BitfieldGet(Bitfield_t * b, Bitindex_t i);
bool BitfieldSet(Bitfield_t * b, Bitindex_t i);
bool BitfieldClr(Bitfield_t * b, Bitindex_t i);
bool BitfieldSetValue(Bitfield_t * b, Bitindex_t i, bool v);

// don't use, if using then the world is collapse;
#ifdef SILO_IF_KARYL_HAD_BETRAYED
typedef union Burst {
	void VOID;
} GrimBurst;
Void = sizeof(GrimBurst);
#endif

#endif
