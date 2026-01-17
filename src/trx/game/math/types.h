#pragma once

#include <stdint.h>

#pragma pack(push, 1)

typedef struct {
    int32_t x;
    int32_t z;
} XZ_32;

typedef struct {
    int16_t x;
    int16_t z;
} XZ_16;

typedef struct {
    int32_t x;
    int32_t y;
    int32_t z;
} XYZ_32;

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} XYZ_16;

typedef struct {
    bool x;
    bool y;
    bool z;
} XYZ_BOOL;

typedef struct {
    float x, y, z;
} XYZ_F;

typedef struct {
    float x, y, z, w;
} XYZW_F;

typedef enum {
    DIR_UNKNOWN = -1,
    DIR_NORTH = 0,
    DIR_EAST = 1,
    DIR_SOUTH = 2,
    DIR_WEST = 3,
} DIRECTION;

typedef enum {
    DIR8_UNKNOWN = -1,
    DIR8_NORTH = 0,
    DIR8_NORTHEAST = 1,
    DIR8_EAST = 2,
    DIR8_SOUTHEAST = 3,
    DIR8_SOUTH = 4,
    DIR8_SOUTHWEST = 5,
    DIR8_WEST = 6,
    DIR8_NORTHWEST = 7,
} DIRECTION_8;

typedef struct {
    XYZ_16 min;
    XYZ_16 max;
} BOUNDS_16;

typedef struct {
    XYZ_32 min;
    XYZ_32 max;
} BOUNDS_32;

#pragma pack(pop)
