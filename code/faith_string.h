#ifndef __FAITH_STRING_H__
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Alexander Udov $
   $Notice: (C) Copyright 2015 by Shadowbiz, Inc. All Rights Reserved. $
   ======================================================================== */

size_t StringLength(const char *string);
char* StringCopy(char *dest, const char* source);
char* StringCopy(char *dest, const char* source, size_t count);
void StringsCat(size_t sourceACount, char *sourceA, size_t sourceBCount, char *sourceB, size_t destCount, char *dest);
int32 StringToInt(const char *string);

#define __FAITH_STRING_H__
#endif
