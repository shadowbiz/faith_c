/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Alexander Udov $
   $Notice: (C) Copyright 2015 by Shadowbiz, Inc. All Rights Reserved. $
   ======================================================================== */

size_t
StringLength(const char *string)
{
    int count = 0;
    while (*string++)
    {
        ++count;
    }
    return count;
}

bool32
StringCompare(const char *string1, const char *string2)
{
    while (*string1 && *string2)
    {
        if (*string1++ != *string2++)
        {
            return false;
        }
    }
    return true;
}

char *
StringCopy(char *dest, const char *source)
{
    size_t size = StringLength(source);
    //Assert(ArrayCount(dest) == ArrayCount(source));
    size_t i;
    for (i = 0; i < size && source[i] != '\0'; i++)
    {
        dest[i] = source[i];
    }
    for (; i < size; i++)
    {
        dest[i] = '\0';
    }

    return dest;
}

char *
StringCopy(char *dest, const char *source, size_t count)
{
    size_t size = count;
    //Assert(ArrayCount(dest) == ArrayCount(source));
    size_t i;
    for (i = 0; i < size && source[i] != '\0'; i++)
    {
        dest[i] = source[i];
    }
    for (; i < size; i++)
    {
        dest[i] = '\0';
    }

    return dest;
}

void StringsCat(size_t sourceACount, char *sourceA,
                size_t sourceBCount, char *sourceB,
                size_t destCount, char *dest)
{
    Assert((sourceACount + sourceBCount) <= destCount);

    for (int index = 0; index < sourceACount; ++index)
    {
        *dest++ = *sourceA++;
    }

    for (int index = 0; index < sourceBCount; ++index)
    {
        *dest++ = *sourceB++;
    }

    *dest++ = 0;
}

int32 StringToInt(const char *string)
{
    int32 result = (int32)strtol(string, (char **)NULL, 10);
    return result;
}
