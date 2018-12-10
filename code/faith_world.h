#ifndef __FAITH_WORLD_H__
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Alexander Udov $
   $Notice: (C) Copyright 2015 by Shadowbiz, Inc. All Rights Reserved. $
   ======================================================================== */

#define MAX_RELIGIONS_PER_CULTURE 2
#define MAX_CULTURES_PER_COUNTRY 1
#define MAX_NAME_LENGTH 128
#define BASE_CULTURES_COUNT 2

#define BASE_VILLAGE_MIN 300
#define BASE_VILLAGE_MAX 2000
#define BASE_TOWN_MAX 50000
#define BASE_CITY_MAX 100000

enum ReligionType
{
    RELIGION_USER,
    RELIGION_CUSTOM,
    RELIGIONS_COUNT
};

enum SocialClassType
{
    SOCIAL_LOWER_CLASS,
    SOCIAL_MIDDLE_CLASS,
    SOCIAL_UPPER_CLASS,
    SOCIAL_CLASSES_COUNT
};

#define SOCIAL_CLASS_POPULATION_DISTRIBUTION                                \
    {                                                                       \
        {0.85f, 0.13f, 0.02f}, {0.7f, 0.25f, 0.05f}, { 0.65f, 0.3f, 0.05f } \
    }

enum SettlementType
{
    SETTLEMENT_VILLAGE,
    SETTLEMENT_TOWN,
    SETTLEMENT_CITY,
    SETTLEMENT_TYPE_COUNT
};

enum SettlementFeature
{
    SETTLEMENT_CAPITAL,
    SETTLEMENT_PORT,
    SETTLEMENT_ROUTE_CENTER,
    SETTLEMENT_CULTURAL_CENTER,
    SETTLEMENT_MANUFACTURING,
    SETTLEMENT_MINING,
    SETTLEMENT_TOURIST_RESORT,
    SETTLEMENT_MARKET_TOWN,
    SETTLEMENT_AGROCULTURAL,
    SETTLEMENT_TRAITS_COUNT
};

enum CountryFeatureType
{
    COUNTRY_FEAUTURES_COUNT,
};

enum ReligionFeature
{
    RELIGION_FEATURES_COUNT
};

struct Religion
{
    uint32 ID;
    char Name[MAX_NAME_LENGTH];
    ReligionType Type;

    ReligionFeature Features[128];
    uint32 FeaturesCount;

    real32 Tolerance;

    //real32 SpreadSpeedInsideCulture;
    //real32 SpreadSpeedBetweenCultures;

    //real32 SpreadSpeedInsideClass[SOCIAL_CLASSES_COUNT];
    //real32 SpreadSpeedBetweenClasses;
};

struct SocialClass
{
    uint32 ID;
    char Name[MAX_NAME_LENGTH];

    SocialClassType Type;

    uint64 PopulationCount;

    real32 Education;
    real32 Health;
    real32 Employment;

    real32 Happiness;
    real32 Agression;
};

struct Culture
{
    uint32 ID;
    char Name[MAX_NAME_LENGTH];

    uint64 PopulationCount;

    // For now only one pagan religion per culture
    Religion *Religion;

    SocialClass Classes[SOCIAL_CLASSES_COUNT];
};

struct Church
{
    uint32 ID;

    uint64 CongregationCount;
    uint64 CongregationCapacity;
    real32 Strength;

    Church *Next;
};

struct Sect
{
    uint32 ID;

    Religion *Religion;

    uint64 CongregationCount;
    real32 Strength;
    real32 Danger;
};

struct Settlement
{
    uint32 ID;
    char *Name;

    uint64 PopulationCount;

    real32 Area;
    real32 PopulationDensity;

    Culture BaseCulture;
    Culture UserCulture;

    SettlementType Type;
    SettlementFeature Features[128];
    uint32 FeaturesCount;

    Church *Churches;
    Sect *Sects;

    Settlement **Neigbours;
    uint32 NeigboursCount;
};

struct Country
{
    uint32 ID;
    char *Name;

    uint64 PopulationCount;

    Settlement *Settlements;
    Country **Neigbours;

    uint32 SettlementsCount;
    uint32 NeigboursCount;
};

struct World
{
    uint32 ID;
    char *Name;
    uint32 CurrentYear;

    Religion UserReligion;

    Country *Countries;

    uint32 TotalCountriesCount;
    uint32 TotalSettlementsCount;
    uint32 TotalReligionsCount;
    uint32 TotalCulturesCount;

    uint32 IDCounter;
};

#define __FAITH_WORLD_H__
#endif
