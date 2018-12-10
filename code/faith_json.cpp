/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Alexander Udov $
   $Notice: (C) Copyright 2015 by Shadowbiz, Inc. All Rights Reserved. $
   ======================================================================== */

#include "faith_json.h"

#define JSON_STRICT
#define TOKEN_PRINT(t)                                 \
	printf("start: %d, end: %d, type: %d, size: %d\n", \
		   (t).Start, (t).End, (t).Type, (t).Size)

#define TOKEN_PRINT_STRING(t, c) \
	printf("- %.*s\n", t.End - t.Start, c + t.Start)

internal JsonToken *
JsonAllocateToken(JsonParser *parser, JsonToken *tokens, size_t tokensCount)
{
	JsonToken *result;

	if (parser->NextToken >= tokensCount)
	{
		return 0;
	}
	result = &tokens[parser->NextToken++];
	result->Start = result->End = -1;
	result->Size = 0;

	return result;
}

internal void
JsonFillToken(JsonToken *token, JsonType type, int start, int end)
{
	token->Type = type;
	token->Start = start;
	token->End = end;
	token->Size = 0;
}

internal JsonError
JsonParsePrimitive(JsonParser *parser, const char *js,
				   size_t len, JsonToken *tokens, size_t tokensCount)
{
	JsonToken *token;
	int start;

	//start = parser->Position;

	for (start = parser->Position;
		 parser->Position < len && js[parser->Position] != '\0';
		 parser->Position++)
	{
		switch (js[parser->Position])
		{
#ifndef JSON_STRICT
		case ':':
#endif
		case '\t':
		case '\r':
		case '\n':
		case ' ':
		case ',':
		case ']':
		case '}':
			goto found;
		}
		if (js[parser->Position] < 32 || js[parser->Position] >= 127)
		{
			parser->Position = start;
			return JSON_ERROR_INVAL;
		}
	}
#ifdef JSON_STRICT
	parser->Position = start;
	return JSON_ERROR_PART;
#endif

found:
	if (tokens == 0)
	{
		parser->Position--;
		return JSON_ERROR_NOERR;
	}

	token = JsonAllocateToken(parser, tokens, tokensCount);
	if (token == 0)
	{
		parser->Position = start;
		return JSON_ERROR_NOMEM;
	}
	JsonFillToken(token, JSON_PRIMITIVE, start, parser->Position);

#ifdef JSON_PARENT_LINKS
	token->parent = parser->ParentToken;
#endif
	parser->Position--;
	return JSON_ERROR_NOERR;
}

internal JsonError
JsonParseString(JsonParser *parser, const char *js,
				size_t len, JsonToken *tokens, size_t tokensCount)
{
	JsonToken *token;

	int start = parser->Position;

	parser->Position++;

	for (; parser->Position < len && js[parser->Position] != '\0'; parser->Position++)
	{
		char c = js[parser->Position];

		if (c == '\"')
		{
			if (tokens == NULL)
			{
				return JSON_ERROR_NOERR;
			}
			token = JsonAllocateToken(parser, tokens, tokensCount);
			if (token == NULL)
			{
				parser->Position = start;
				return JSON_ERROR_NOMEM;
			}
			JsonFillToken(token, JSON_STRING, start + 1, parser->Position);
#ifdef JSON_PARENT_LINKS
			token->parent = parser->ParentToken;
#endif
			return JSON_ERROR_NOERR;
		}

		if (c == '\\' && parser->Position + 1 < len)
		{
			int i;
			parser->Position++;
			switch (js[parser->Position])
			{
			case '\"':
			case '/':
			case '\\':
			case 'b':
			case 'f':
			case 'r':
			case 'n':
			case 't':
				break;

			case 'u':
				parser->Position++;
				for (i = 0; i < 4 && parser->Position < len && js[parser->Position] != '\0'; i++)
				{
					// If it isn't a hex character we have an error
					if (!((js[parser->Position] >= 48 && js[parser->Position] <= 57) || // 0-9
						  (js[parser->Position] >= 65 && js[parser->Position] <= 70) || // A-F
						  (js[parser->Position] >= 97 && js[parser->Position] <= 102)))
					{
						// a-f
						parser->Position = start;
						return JSON_ERROR_INVAL;
					}
					parser->Position++;
				}
				parser->Position--;
				break;
			// Unexpected symbol
			default:
				parser->Position = start;
				return JSON_ERROR_INVAL;
			}
		}
	}
	parser->Position = start;
	return JSON_ERROR_PART;
}

int32 JsonParse(JsonParser *parser, const char *js, size_t len,
				JsonToken *tokens, uint32 tokensCount)
{
	JsonError r;
	int i;
	JsonToken *token;
	int count = 0;

	for (; parser->Position < len && js[parser->Position] != '\0'; parser->Position++)
	{
		char c;
		JsonType type;

		c = js[parser->Position];
		switch (c)
		{
		case '{':
		case '[':
			count++;
			if (tokens == 0)
			{
				break;
			}

			token = JsonAllocateToken(parser, tokens, tokensCount);
			if (token == 0)
			{
				return JSON_ERROR_NOMEM;
			}

			if (parser->ParentToken != -1)
			{
				tokens[parser->ParentToken].Size++;
#ifdef JSON_PARENT_LINKS
				token->parent = parser->ParentToken;
#endif
			}
			token->Type = (c == '{' ? JSON_OBJECT : JSON_ARRAY);
			token->Start = parser->Position;
			parser->ParentToken = parser->NextToken - 1;
			break;

		case '}':
		case ']':
			if (tokens == 0)
			{
				break;
			}
			type = (c == '}' ? JSON_OBJECT : JSON_ARRAY);
#ifdef JSON_PARENT_LINKS
			if (parser->NextToken < 1)
			{
				return JSON_ERROR_INVAL;
			}

			token = &tokens[parser->NextToken - 1];
			while (1)
			{
				if (token->Start != -1 && token->End == -1)
				{
					if (token->Type != type)
					{
						return JSON_ERROR_INVAL;
					}
					token->End = parser->Position + 1;
					parser->ParentToken = token->parent;
					break;
				}

				if (token->parent == -1)
				{
					break;
				}
				token = &tokens[token->parent];
			}
#else
			for (i = parser->NextToken - 1; i >= 0; i--)
			{
				token = &tokens[i];
				if (token->Start != -1 && token->End == -1)
				{
					if (token->Type != type)
					{
						return JSON_ERROR_INVAL;
					}
					parser->ParentToken = -1;
					token->End = parser->Position + 1;
					break;
				}
			}
			if (i == -1)
			{
				return JSON_ERROR_INVAL;
			}
			for (; i >= 0; i--)
			{
				token = &tokens[i];
				if (token->Start != -1 && token->End == -1)
				{
					parser->ParentToken = i;
					break;
				}
			}
#endif
			break;

		case '\"':
			r = JsonParseString(parser, js, len, tokens, tokensCount);
			if (r < 0)
			{
				return r;
			}
			count++;
			if (parser->ParentToken != -1 && tokens != 0)
			{
				tokens[parser->ParentToken].Size++;
			}
			break;

		case '\t':
		case '\r':
		case '\n':
		case ' ':
			break;

		case ':':
			parser->ParentToken = parser->NextToken - 1;
			break;

		case ',':
			if (tokens != 0 &&
				tokens[parser->ParentToken].Type != JSON_ARRAY &&
				tokens[parser->ParentToken].Type != JSON_OBJECT)
			{
#ifdef JSON_PARENT_LINKS
				parser->ParentToken = tokens[parser->ParentToken].parent;
#else
				for (i = parser->NextToken - 1; i >= 0; i--)
				{
					if (tokens[i].Type == JSON_ARRAY ||
						tokens[i].Type == JSON_OBJECT)
					{
						if (tokens[i].Start != -1 &&
							tokens[i].End == -1)
						{
							parser->ParentToken = i;
							break;
						}
					}
				}
#endif
			}
			break;

#ifdef JSON_STRICT
		case '-':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 't':
		case 'f':
		case 'n':
			if (tokens != 0)
			{
				JsonToken *t = &tokens[parser->ParentToken];
				if (t->Type == JSON_OBJECT ||
					(t->Type == JSON_STRING && t->Size != 0))
				{
					return JSON_ERROR_INVAL;
				}
			}
#else
		default:
#endif
			r = JsonParsePrimitive(parser, js, len, tokens, tokensCount);
			if (r < 0)
			{
				return r;
			}

			count++;
			if (parser->ParentToken != -1 && tokens != 0)
			{
				tokens[parser->ParentToken].Size++;
			}
			break;

#ifdef JSON_STRICT
		default:
			return JSON_ERROR_INVAL;
#endif
		}
	}

	for (i = parser->NextToken - 1; i >= 0; i--)
	{
		if (tokens[i].Start != -1 && tokens[i].End == -1)
		{
			return JSON_ERROR_PART;
		}
	}
	return count;
}

void JsonInit(JsonParser *parser)
{
	parser->Position = 0;
	parser->NextToken = 0;
	parser->ParentToken = -1;
}

bool32
JsonEqual(const char *jsonString, JsonToken *tok, const char *s)
{
	size_t stringLength = StringLength(s);
	if (tok->Type == JSON_STRING &&
		stringLength == tok->End - tok->Start &&
		StringCompare(jsonString + tok->Start, s))
	{
		return true;
	}
	return false;
}

int32 JsonGetInt(char *jsonString, JsonToken *token)
{
	char *tmp = JsonGetString(jsonString, token);
	int32 result = StringToInt(tmp);
	return result;
}

char *
JsonGetString(char *jsonString, JsonToken *token)
{
	jsonString[token->End] = '\0';
	char *result = jsonString + token->Start;
	return result;
}
