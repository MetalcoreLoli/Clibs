#ifndef LOLI_STRING_H
#define LOLI_STRING_H
#include <string.h> // for memcpy
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

#define LS_NOT_IMPLEMENTED() assert("not implemented" == 0)
#define LS_EMPTY_CHECK(ls) assert ((ls) != NULL || (ls)->mData != NULL)

typedef struct LString LString_t;
typedef _Bool(*LStringFunc_t)(const LString_t* ls);
typedef _Bool(*LSSymFunc_t)(const char symbol);

struct LString {
    size_t mCount;
    const char* mData;
};

LString_t CreateLSFrom(const char* value);
const char * lsAsCStr(const LString_t* ls);
size_t lsCountOfSym(const LString_t* ls, char sym);

LString_t lsSubFrom (const LString_t* ls, size_t start, size_t end);
LString_t lsSkip(const LString_t* ls, size_t count);
LString_t lsAppend (const LString_t* ls, const LString_t* ls1);
LString_t lsAppendSym (const LString_t* ls, char sym);
LString_t lsAppendCStr (const LString_t* ls, const char* ls1);

LString_t lsSelectWhere(const LString_t* ls, LSSymFunc_t predicate);
LString_t lsSelectSub(const LString_t* ls, LStringFunc_t predicate);

LString_t* lsSplit (const LString_t* ls, char delimeter);

_Bool lsEqToCStr(const LString_t*, const char* value);
_Bool lsEqTo(const LString_t*, const LString_t* value);

#ifdef LOLI_STRING_IMPL
LString_t CreateLSFrom(const char* value) {
    assert (value != NULL);
    size_t count = 0;
    for (; value[count] != '\0'; count++); 

    LString_t ls = {
        .mCount = count,
        .mData  = value
    };

    return ls;
}

const char * lsAsCStr(const LString_t* ls) {
    assert(ls->mData != NULL); 
    return ls->mData;
}

size_t lsCountOfSym(const LString_t* ls, char sym) {
    size_t count = 0;
    for (size_t i = 0; i < ls->mCount; i++) {
        if (ls->mData[i] == sym)
            count++;
    }
    return count;
}

LString_t lsSubFrom (const LString_t* ls, size_t start, size_t end) {
    assert (start < end || ls->mCount < end);
    size_t len = (end - start);
    char * value = (char*)malloc(sizeof(char) * len);

    for (size_t i = 0; i < len; i++) {
        value[i] = ls->mData[start + i];
    }
    value[len] = '\0';
    LString_t out =  {
        .mData = value,
        .mCount = len - 1
    };
    return out;
}

LString_t lsAppend (const LString_t* ls, const LString_t* ls1) {
    assert (ls != NULL);
    assert (ls1 != NULL);
    size_t len = ls->mCount + ls1->mCount;
    char* value = (char*)malloc(sizeof(char) * len);
    
    memcpy (value, ls->mData, sizeof(char) * ls->mCount);
    for (size_t i = 0; i <= ls1->mCount; i++) {
        value[ls->mCount + i] = ls1->mData[i];
    }
    value[len] = '\0';
    LString_t out = {
        .mData = value,
        .mCount = len,
    };
    return out;
}

LString_t lsAppendCStr (const LString_t* ls, const char* ls1) {
    size_t lenOfCStr = sizeof(ls1) / sizeof(ls1[0]);
    size_t len = ls->mCount + lenOfCStr; 
    char* value = (char*)malloc (sizeof(char) * len);

    memcpy (value, ls->mData, sizeof (ls->mData));
    for (size_t i = 0; i < lenOfCStr; i++) {
        value[ls->mCount + i] = ls1[i];
    }
    value [len] = '\0';
    LString_t out = {
        .mData  = value,
        .mCount = len
    };
    return out;
}

LString_t lsAppendSym (const LString_t* ls, char sym) {
    assert (ls != NULL);
    size_t len = ls->mCount + 1;
    char * value = (char*)malloc (sizeof(char) * len);

    memcpy(value, ls->mData, sizeof(ls->mData));
    value[len - 1] = sym;
    value[len] = '\0';

    LString_t out = {
        .mCount = len,
        .mData  = value, 
    };
    return out;
}

LString_t* lsSplit (const LString_t* ls, char delimeter) {
    LString_t s = CreateLSFrom("");
    size_t countOfElems = lsCountOfSym (ls, delimeter) + 1;

    LString_t *out =(LString_t*)malloc(sizeof(LString_t) * countOfElems);
    assert (out != NULL);
    size_t currentElem = 0;

    for (size_t i = 0; i <= ls->mCount; i++) {
        char current = ls->mData[i];
        if (current != delimeter) {
            s = lsAppendSym(&s, current);
        } else {
            LString_t tmp = CreateLSFrom(s.mData);
            out[currentElem++] = tmp;
            s = CreateLSFrom("");
        }
    }
    out[currentElem] = s;
    return out;
    //LS_NOT_IMPLEMENTED();
}

LString_t lsSelectWhere(const LString_t* ls, LSSymFunc_t predicate) {
    LS_EMPTY_CHECK(ls);
    LString_t out = CreateLSFrom ("");
    for (size_t i = 0; i < ls->mCount; i++) {
        char current = ls->mData[i];
        if (predicate(current)) {
            out = lsAppendSym(&out, current);
        }
    }

    return out;
}

LString_t lsSelectSub(const LString_t* ls, LStringFunc_t predicate) {
    LS_EMPTY_CHECK (ls);
    LS_NOT_IMPLEMENTED();
    return CreateLSFrom("");
}

LString_t lsSkip(const LString_t* ls, size_t count) {
    assert (ls != NULL);
    LString_t out = lsSubFrom (ls, count, ls->mCount);
    return out;
}

_Bool lsEqToCStr(const LString_t* ls, const char* value) {
    LS_EMPTY_CHECK(ls);
    size_t countOfElems = sizeof(value) / sizeof(value[0]) - 1;
    if (countOfElems != ls->mCount) {
        return 0;
    }

    for (size_t i =0; i < countOfElems; i++) {
        char current = ls->mData[i];
        if (current != value[i])
            return 0;
    }
    return 1;
}

_Bool lsEqTo(const LString_t* ls, const LString_t* value){
    return lsEqToCStr(ls, value->mData);
}


#endif
#endif
