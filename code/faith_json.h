#ifndef __FAITH_JSON_H__
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Alexander Udov $
   $Notice: (C) Copyright 2015 by Shadowbiz, Inc. All Rights Reserved. $
   ======================================================================== */

#define __FAITH_JSON_H__

//#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

	enum JsonType
	{
		JSON_PRIMITIVE = 0,
		JSON_OBJECT = 1,
		JSON_ARRAY = 2,
		JSON_STRING = 3
	};

	enum JsonError
	{
		JSON_ERROR_NOERR = 0,
		JSON_ERROR_NOMEM = -1, // Not enough tokens were provided
		JSON_ERROR_INVAL = -2, // Invalid character inside JSON string
		JSON_ERROR_PART = -3   // The string is not a full JSON packet, more bytes expected
	};

	struct JsonToken
	{
		JsonType Type;
		int32 Start;
		int32 End;
		int32 Size;
	};

	struct JsonParser
	{
		uint32 Position;   /* offset in the JSON string */
		uint32 NextToken;  //toknext; /* next token to allocate */
		int32 ParentToken; /* superior token node, e.g parent object or array */
	};

	void JsonInit(JsonParser *parser);

	int32 JsonParse(JsonParser *parser, const char *js, size_t len,
					JsonToken *tokens, uint32 tokensCount);

	bool32 JsonEqual(const char *json, JsonToken *tok, const char *s);

	int32 JsonGetint(char *jsonString, JsonToken *tok);

	char *JsonGetString(char *jsonString, JsonToken *token);

#ifdef __cplusplus
}
#endif

#endif
