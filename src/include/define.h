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

// default processing size
#define SILO_WORD long long
#define SILO_ADDR long long

typedef unsigned SILO_WORD DEFT_WORD;
typedef unsigned SILO_ADDR DEFT_ADDR;

typedef DEFT_WORD SIGNALSIZE;
typedef unsigned int NODEID;
typedef unsigned int PORTID;

#define BASICMEM 4096 // 4kiB = 1Page

#endif
