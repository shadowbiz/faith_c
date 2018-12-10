/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Alexander Udov $
   $Notice: (C) Copyright 2015 by Shadowbiz, Inc. All Rights Reserved. $
   ======================================================================== */

internal void
CulturePrint(Culture *culture)
{
    printf("----");
    printf("Culture:\n");
    printf("------");
    printf("%s, ", culture->Name);
    printf("Population: %I64u\n", culture->PopulationCount);
    printf("------");
    printf("Social:\n");
    for (uint32 i = 0; i != SOCIAL_CLASSES_COUNT; ++i)
    {
        printf("        ");
        SocialClass *social = &culture->Classes[i];
        printf("%s, Population: %I64u\n", social->Name, social->PopulationCount);
    }
    printf("      ");
    printf("Religion:\n");
    printf("        ");
    printf(" %s\n", culture->Religion->Name);

}

internal void
SettlementPrint(Settlement *city)
{
    printf("************* \n");
    printf("--");
    switch (city->Type)
    {
        case SETTLEMENT_TOWN:
            printf("Town ");
            break;
        case SETTLEMENT_VILLAGE:
            printf("Village ");
            break;
        case SETTLEMENT_CITY:
            printf("City ");
            break;
    }

    printf("%s ", city->Name);
    printf("Population: %I64u\n", city->PopulationCount);

    printf("----");
    printf("Features: ");
    for (uint32 i = 0; i != city->FeaturesCount; ++i)
    {
        switch (city->Features[i])
        {
            case SETTLEMENT_CAPITAL:
                printf("CAPITAL, ");
                break;
            case SETTLEMENT_PORT:
                printf("PORT, ");
                break;
            case SETTLEMENT_ROUTE_CENTER:
                printf("ROUTE CENTER, ");
                break;
            case SETTLEMENT_CULTURAL_CENTER:
                printf("CULTURAL CENTER, ");
                break;
            case SETTLEMENT_MANUFACTURING:
                printf("MANUFACTURING, ");
                break;
            case SETTLEMENT_MINING:
                printf("MINING, ");
                break;
            case SETTLEMENT_TOURIST_RESORT:
                printf("TOURIST RESORT, ");
                break;
            case SETTLEMENT_MARKET_TOWN:
                printf("MARKET TOWN, ");
                break;
            case SETTLEMENT_AGROCULTURAL:
                printf("AGROCULTURAL, ");
                break;
        }
    }
    printf ("\n");
    CulturePrint(&city->BaseCulture);
    //CulturePrint(&city->UserCulture);
    printf("************* \n");
}

internal void
CountryPrint(Country *country)
{
    printf("*********************************\n");
    printf("Country: %s, Population: %I64u, Settlements: %d\n", country->Name, country->PopulationCount, country->SettlementsCount);
    for (uint32 i = 0; i != country->SettlementsCount; ++i)
    {
        Settlement *city = country->Settlements + i;
        SettlementPrint(city);
    }
    printf("----");
    printf("Neigbours (%d): ", country->NeigboursCount);
    for (uint32 i = 0; i != country->NeigboursCount; ++i)
    {
        Country *neigbour = country->Neigbours[i];
        printf("%s", neigbour->Name);
    }
    printf("\n*********************************\n");
}

inline int32
WorldJsonParse(JsonParser *parser, const char *js, size_t len,
               JsonToken *tokens, uint32 tokensCount)
{
    int32 result;
    result = JsonParse(parser, js, len, tokens, tokensCount);
    if (result < 0)
    {
        printf("ERROR PARSING JSON\n");
        switch (tokensCount)
        {
            case JSON_ERROR_NOMEM:
                printf("JSON_ERROR_NOMEM\n");
                break;
            case JSON_ERROR_INVAL:
                printf("JSON_ERROR_INVAL\n");
                break;
            case JSON_ERROR_PART:
                printf("JSON_ERROR_PART\n");
                break;
            default:
                printf("Unknown\n");
                break;
        }
    }
    return result;
}

typedef enum
{
    START, KEY, PRINT, SKIP, STOP
} ParseState;

inline void *
memcpy(void* dest, const void* src, size_t count)
{
    uint8* dst8 = (uint8*)dest;
    uint8* src8 = (uint8*)src;

    if (count & 1) {
        dst8[0] = src8[0];
        dst8++;
        src8++;
    }

    count /= 2;
    while (count--)
    {
        dst8[0] = src8[0];
        dst8[1] = src8[1];

        dst8 += 2;
        src8 += 2;
    }
    return dest;
}

internal void
WorldInitialize(GameMemory *memory, GameState* gameState)
{
    ThreadContext threadContext = {};
    debug_read_file_result file;
    char *worldData;

    int32 tokensCount;
    JsonParser parser = {};
    JsonToken tokens[512] = {};

    JsonInit(&parser);

    file = memory->DEBUGPlatformReadEntireFile(&threadContext, "world_temp.json");
    worldData = (char*)file.Contents;

    tokensCount = WorldJsonParse(&parser, worldData, StringLength(worldData), tokens, ArrayCount(tokens));
    if (tokensCount < 0)
    {
        return;
    }


    gameState->World = PushSize(&gameState->WorldArena, World);
    World *world = gameState->World;
    world->IDCounter = 0;
    world->ID = world->IDCounter++;
    world->TotalSettlementsCount = 0;
    world->TotalCountriesCount = 0;
    world->TotalReligionsCount = 0;
    world->TotalCulturesCount = 0;

    int32 index = 0;

    ParseState state = START;
    size_t objectTokens = 0;

    for (size_t i = 0, j = 1; j > 0; i++, j--)
    {
        JsonToken *t = &tokens[i];

        if (t->Type == JSON_ARRAY || t->Type == JSON_OBJECT)
        {
            j += t->Size;
        }

        switch (state)
        {
            case START:
                if (t->Type != JSON_OBJECT)
                {
                    printf("Root element must be an object.\n");
                    return;
                }

                state = KEY;
                objectTokens = t->Size;

                printf("objectTokens %zd\n", objectTokens);
                TOKEN_PRINT(*t);

                for (size_t o = i + 1; o < i + t->Size + 1; ++o)
                {
                    JsonToken *key = &tokens[o];
                    if (key->Type == JSON_STRING)
                    {
                        char *keyString = JsonGetString(worldData, key);
                        printf("%s ", keyString);
                    }
                    else
                    {
                        char *keyString = JsonGetString(worldData, key);
                        printf("%s ", keyString);
                    }
                    if (o % 2 == 0) printf("\n");
                }

                if (objectTokens == 0)
                {
                    state = STOP;
                }

                break;
        }
    }

    printf("**********************************\n");

    while (index < tokensCount)
    {
        if (JsonEqual(worldData, &tokens[index++], "Country"))
        {
            world->TotalCountriesCount++;
        }
    }
    printf("Countries count %d\n", world->TotalCountriesCount);

    bool32 isWorldToken = false;
    bool32 isCountryToken = false;
    bool32 isSettlementToken = false;
    bool32 isFeaturesToken = false;
    bool32 countryNeedsTobeSaved = false;

    uint32 countriesCount = world->TotalCountriesCount;
    world->Countries = PushArray(&gameState->WorldArena, countriesCount, Country);
    uint32 countryIndex = 0;

    //Settlement settlements[128];

    Country *country = world->Countries;

    index = 0;

    while (index++ < tokensCount)
    {
        if (JsonEqual(worldData, &tokens[index], "Name"))
        {
            JsonToken *key = &tokens[++index];
            world->Name = JsonGetString(worldData, key);
            break;
        }
    }

    while (index++ < tokensCount)
    {
        if (JsonEqual(worldData, &tokens[index], "Country"))
        {
            country = world->Countries + countryIndex++;

            int32 countryNameIndex = index;
            while (countryNameIndex++ < tokensCount)
            {
                if (JsonEqual(worldData, &tokens[countryNameIndex], "Country"))
                {
                    break;
                }
                else if (JsonEqual(worldData, &tokens[countryNameIndex], "Settlement"))
                {
                    break;
                }
                else if (JsonEqual(worldData, &tokens[countryNameIndex], "Name"))
                {
                    country->ID = world->IDCounter++;
                    JsonToken *key = &tokens[++countryNameIndex];
                    country->Name = JsonGetString(worldData, key);

                    printf("Country %s\n", country->Name);

                    break;

                }
            }

            int32 settlementIndex = index + 1;
            uint32 tempSettlementsCount = 0;

            while (settlementIndex < tokensCount)
            {
                if (JsonEqual(worldData, &tokens[settlementIndex], "Country"))
                {
                    break;

                }
                if (JsonEqual(worldData, &tokens[settlementIndex], "Settlement"))
                {
                    tempSettlementsCount++;
                    world->TotalSettlementsCount++;
                }
                ++settlementIndex;
            }

            country->Settlements = PushArray(&gameState->WorldArena,
                                             tempSettlementsCount,
                                             Settlement);

            country->SettlementsCount = tempSettlementsCount;

            Settlement *settlement = country->Settlements;
            uint32 currentSettlementIndex = 0;
            settlementIndex = index + 1;

            while (settlementIndex++ < tokensCount)
            {
                if (JsonEqual(worldData, &tokens[settlementIndex], "Country"))
                {
                    break;

                }
                else if (JsonEqual(worldData, &tokens[settlementIndex], "Settlement"))
                {
                    settlement = country->Settlements + currentSettlementIndex++;

                }
                else if (JsonEqual(worldData, &tokens[settlementIndex], "Features"))
                {

                }
                else if (JsonEqual(worldData, &tokens[settlementIndex], "SETTLEMENT_CAPITAL"))
                {
                    settlement->Features[settlement->FeaturesCount++] = SETTLEMENT_CAPITAL;

                }
                else if (JsonEqual(worldData, &tokens[settlementIndex], "Name"))
                {
                    settlement->ID = world->IDCounter++;
                    settlement->Name = JsonGetString(worldData, &tokens[++settlementIndex]);
                }
                else if (JsonEqual(worldData, &tokens[settlementIndex], "Population"))
                {
                    int32 population = JsonGetInt(worldData, &tokens[++settlementIndex]);
                    if (population > 0)
                    {
                        settlement->PopulationCount = population;
                    }

                }
                else if (JsonEqual(worldData, &tokens[settlementIndex], "Type"))
                {
                    ++index;
                    if (JsonEqual(worldData, &tokens[settlementIndex], "SETTLEMENT_VILLAGE"))
                    {
                        settlement->Type = SETTLEMENT_VILLAGE;
                    }
                    else if (JsonEqual(worldData, &tokens[settlementIndex], "SETTLEMENT_TOWN"))
                    {
                        settlement->Type = SETTLEMENT_TOWN;
                    }
                    else if (JsonEqual(worldData, &tokens[settlementIndex], "SETTLEMENT_CITY"))
                    {
                        settlement->Type = SETTLEMENT_CITY;
                    }       

                }
                else if (JsonEqual(worldData, &tokens[settlementIndex], "Area"))
                {
                    int32 area = JsonGetInt(worldData, &tokens[++settlementIndex]);
                    if (area > 0)
                    {
                        settlement->Area = (real32)area;
                        settlement->PopulationDensity = settlement->PopulationCount / settlement->Area;
                    }
                }

            }
        }
    }

    SocialClass baseSocialClasses[3];
    //TODO Load from file
    baseSocialClasses[0] = { 0, "Lower class", SOCIAL_LOWER_CLASS };
    baseSocialClasses[0].PopulationCount = 0;
    baseSocialClasses[0].Education = 0.1f;
    baseSocialClasses[0].Health = 0.3f;
    baseSocialClasses[0].Employment = 0.1f;
    baseSocialClasses[0].Happiness = 0.2f;
    baseSocialClasses[0].Agression = 0.3f;

    baseSocialClasses[1] = { 1, "Middle class", SOCIAL_MIDDLE_CLASS };
    baseSocialClasses[1].PopulationCount = 0;
    baseSocialClasses[1].Education = 0.45f;
    baseSocialClasses[1].Health = 0.5f;
    baseSocialClasses[1].Employment = 0.65f;
    baseSocialClasses[1].Happiness = 0.55f;
    baseSocialClasses[1].Agression = 0.4f;

    baseSocialClasses[2] = { 2, "Upper class", SOCIAL_UPPER_CLASS };
    baseSocialClasses[2].PopulationCount = 0;
    baseSocialClasses[2].Education = 0.85f;
    baseSocialClasses[2].Health = 0.7f;
    baseSocialClasses[2].Employment = 0.85f;
    baseSocialClasses[2].Happiness = 0.75f;
    baseSocialClasses[2].Agression = 0.7f;

    world->UserReligion = { world->TotalReligionsCount++, "User religion", RELIGION_USER };
    world->UserReligion.Tolerance = 1;
    world->UserReligion.FeaturesCount = 0;

    //TODO Generate base religions for all base cultures based on each culture's legacy and population social status
    Religion tempReligion = { world->TotalReligionsCount++, "Atheism", RELIGION_CUSTOM };
    tempReligion.Tolerance = 1;
    tempReligion.FeaturesCount = 0;

    Culture tempUserCulture = { world->TotalCulturesCount++, "User culture" };
    tempUserCulture.PopulationCount = 0;
    tempUserCulture.Religion = &world->UserReligion;

    //TODO Load from file
    Culture baseCultures[2];

    baseCultures[0] = { world->TotalCulturesCount++, "Culture 1" };
    baseCultures[0].PopulationCount = 0;
    baseCultures[0].Religion = PushSize(&gameState->WorldArena, Religion);
    *baseCultures[0].Religion = tempReligion;

    baseCultures[1] = { world->TotalCulturesCount++, "Culture 2" };
    baseCultures[1].PopulationCount = 0;
    baseCultures[1].Religion = PushSize(&gameState->WorldArena, Religion);
    *baseCultures[1].Religion = tempReligion;

    for (int i = 0 ; i != SOCIAL_CLASSES_COUNT; ++i)
    {
        baseCultures[0].Classes[i] = baseSocialClasses[i];
        baseCultures[1].Classes[i] = baseSocialClasses[i];
        tempUserCulture.Classes[i] = baseSocialClasses[i];
    }

    // TODO Load countries from file or generate based on worldmap data
    for (uint32 cIndex  = 0; cIndex != countriesCount; ++cIndex)
    {
        Country *currentCountry = &world->Countries[cIndex];
        uint32 citiesCount = currentCountry->SettlementsCount;

        for (uint32 c = 0; c != citiesCount; ++c)
        {
            Settlement *settlement = &currentCountry->Settlements[c];
            currentCountry->PopulationCount += settlement->PopulationCount;

            settlement->BaseCulture = baseCultures[GetRandom(2)];
            settlement->UserCulture = tempUserCulture;

            Culture *baseCulture = &settlement->BaseCulture;
            baseCulture->PopulationCount = settlement->PopulationCount;

            real32 populationDistribution[SETTLEMENT_TYPE_COUNT][SOCIAL_CLASSES_COUNT] =
                SOCIAL_CLASS_POPULATION_DISTRIBUTION;

            for (uint32 i = 0; i != SOCIAL_CLASSES_COUNT; ++i)
            {
                baseCulture->Classes[i].PopulationCount =
                    (uint64)(baseCulture->PopulationCount *
                             populationDistribution[(uint32)settlement->Type][i]);
            }

            settlement->Churches = 0;
            settlement->Neigbours = 0;
        }
    }

    //TODO Read neigbours from world map data
    for (uint32 cIndex = 0; cIndex != countriesCount; ++cIndex)
    {
        Country *tempNeigbours[128];
        uint32 neigboursCount = 0;

        Country *currentCountry = world->Countries + cIndex;
        if (cIndex > 0)
        {
            Country *neigbour = world->Countries + (cIndex - 1);
            tempNeigbours[neigboursCount++] = neigbour;
        }
        if (cIndex < countriesCount - 1)
        {
            Country *neigbour = world->Countries + (cIndex + 1);
            tempNeigbours[neigboursCount++] = neigbour;
        }

        currentCountry->Neigbours = PushArray(&gameState->WorldArena, neigboursCount, Country*);
        currentCountry->NeigboursCount = neigboursCount;

        for (uint32 i = 0; i != neigboursCount; ++i)
        {
            currentCountry->Neigbours[i] = tempNeigbours[i];
        }
    }

    printf("World: %s\n", world->Name);
    for (uint32 i = 0; i != world->TotalCountriesCount; ++i)
    {
        Country *currentCountry = world->Countries + i;
        CountryPrint(currentCountry);
    }

    size_t left = gameState->WorldArena.Size - gameState->WorldArena.Used;
    printf("Memory left: %zdkB of %zdkB\n", ToKilobytes(left), ToKilobytes(gameState->WorldArena.Size));

    index = 0;





}
