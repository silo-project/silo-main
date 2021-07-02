/* Synthesizer : Compile Blueprint and static Type Checking.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../include/simulator/simulator.h"
#include "../include/simulator/thread.h"
#include "../include/simulator/simulation_types.h"

enum TypesError {
	TYPERR_NONERROR = 0,
	TYPERR_MISMATCH,
	TYPERR_OUTBOUND,
	TYPERR_NULLTYPE
};

const int SYNTHFLAGMAX = 5;

enum SyntheticFlag {
	SF_cTypeMisMatch = 0,
	SF_cTypeInvalids,
	SF_cLinkOutOfBound,
	SF_cLinkConflict,
	SF_NonSynthetic, // Non make Chipwafer;
	SF_Warnings, // 0 == Ignore ALL warnings, 1 == Warning Enable, 2 == Warning To Error;
	SF_Errors,
	SF_Exception, // 0 == No Abort on errors, 1 == Abort on error;
	SF_
};

static int SynthFlagArray[SYNTHFLAGMAX];

static char ** DefNamesList;
static size_t DefNamesSize;
static int DefNamesLast, DefNamesNums;

static void DefNamesSort(void);
static bool SyntChkTypes(struct LinkWire *);

static bool warningToError = false;
static bool checkTypeMis = true;
static bool checkOOBound = false;
static bool checkTypeValid = true;


static int SyntChkTabSize(int newtid) { // need optimization
	if (newtid >= DefNamesSize / sizeof(char*)) {
		DefNamesSize += BASICMEM;
		void * p = realloc(DefNamesList, DefNamesSize);
		if (p) DefNamesList = p;
		else return -1;
	}
	return newtid;
}

FlowType_t SyntNewTypes(char * typename) {
	if (SyntChkTabSize(DefNamesLast) < 0) return -1;
	DefNamesList[DefNamesLast] = malloc(strlen(typename) + 1);
	if (!DefTypesList[DefNamesLast]) return -1;
	DefNamesList[DefNamesLast] = newtype;
	return DefNamesLast++;
}
int SyntDelTypes(FlowType_t typenum) {
	if (DefNamesList[typenum]) free(DefNamesList[typenum]);
	else return -1;
	DefNamesList[typenum] = NULL;
	return typenum;
}

int SyntheInit(void) {
	DefNamesSize = sizeof(char*) * BASICMEM;
	DefNamesList = malloc(DefNamesSize);
	if (!DefNamesList) return -1;
	DefNamesLast = DefNamesNums = 0;
	return 0;
}

static enum TypesError SyntChkTypes(struct LinkWire * lp) {
	struct GateNode *snode, *dnode;
	int swire, dport;
	int stype, dtype;

	snode = lp->Source, dnode = lp->Destin;
	swire = lp->IndexO, swire = lp->IndexI;

	// Check Out of Bound
	if (checkOOBound && (snode->WireNumber < swire || dnode->PortNumber < dport))
		return TYPERR_OUTBOUND;
	// If Check Success
	stype = snode->WireTypesPtr[swire], dtype = dnode->PortTypesPtr[dwire];
	// Check Type is Valid
	if (checkTypeValid && (stype > DefNamesLast || dtype > DefNamesLast) || (!DefNamesList[stype] || !DefNamesList[dtype]))
		return TYPERR_MISMATCH;
	// Check a Type Mismatching
	if (checkTypeMis && (stype != dtype))
		return TYPERR_MISMATCH;
	return TYPERR_NONERROR;
}

struct Chipwafer * SynthesizerBegin(struct Blueprint * bluep) {
	int i;
	struct Chipwafer * architecture = malloc(sizeof(struct Chipwafer));
	if (!architecture) goto EXCEPTION;
	struct LinkWire * CurrentLink;

	int warnings, errors;

	warnings = errors = 0;
	
	// Type Checking.
	if (SynthFlagArray[SF_cTypeMisMatch]) {
		for (i = 0; i < bluep->LinkNumber; i++) {
			if (SyntChkTypes(CurrentLink = bluep->LinkList[i]) != TYPERR_NONERROR) {
				struct GateNode *snode, *dnode;
				int snodeid, dnodeid;
				int swireid, dportid;
				FlowType_t stype, dtype;
				char *stypename, *dtypename;
				snode = CurrentLink->Source, dnode = CurrentLink->Destin;
				snodeid = snode->GateNodeID, dnodeid = dnode->GateNodeID;
				swireid = CurrentLink->IndexO, dwireid = CurrentLink->IndexI;
				stype = snode->WireTypesPtr[swireid], dtype = dnode->PortTypesPtr[dwireid];
				stypename = DefNamesList[stype], dtypename = DefNamesList[dtype];
	
				fprintf(stderr, "a GateNodeID(%d):WIRE[%d] as Type(%d) '%s', send to GateNodeID(%d):PORT[%d] as Type(%d) '%s'.\n",
					snodeid, swireid, stype, stypename,
					dnodeid, dportid, dtype, dtypename
				);
				warnings++;
				if (SynthFlagArray[SF_Exception]) {
					errors++;
					goto EXCEPTION;
				}
			}
			// no typemismatch
		}
	}
	goto TERMINATE;
EXCEPTION:
	if (architecture)
		free(architecture);
	architecture = NULL;
TERMINATE:
	return architecture;
}
