#include <trx/core/math/func.h>

uint32_t Math_Sqrt(uint32_t n)
{
    uint32_t result = 0;
    uint32_t bit = 1u << 30;

    while (bit > n) {
        bit >>= 2;
    }

    while (bit != 0) {
        if (n >= result + bit) {
            n -= result + bit;
            result = (result >> 1) + bit;
        } else {
            result >>= 1;
        }
        bit >>= 2;
    }

    return result;
}

uint64_t Math_Sqrt64(uint64_t n)
{
    uint64_t result = 0;
    uint64_t bit = 1ULL << 62;

    while (bit > n) {
        bit >>= 2;
    }

    while (bit != 0) {
        if (n >= result + bit) {
            n -= result + bit;
            result = (result >> 1) + bit;
        } else {
            result >>= 1;
        }
        bit >>= 2;
    }

    return result;
}

void Math_GetVectorAngles(
    const int32_t x, const int32_t y, const int32_t z, int16_t *const dest)
{
    dest[0] = XYZ_32_GetYaw((XYZ_32) { x, y, z });
    dest[1] = XYZ_32_GetPitch((XYZ_32) { x, y, z });
}

int32_t Math_AngleInCone(int32_t angle1, int32_t angle2, int32_t cone)
{
    const int32_t diff = ((int)(angle1 - angle2 + DEG_180)) % DEG_360 - DEG_180;
    return ABS(diff) < cone;
}

DIRECTION Math_GetDirection(const int16_t angle)
{
    return (uint16_t)(angle + DEG_45) / DEG_90;
}

DIRECTION Math_GetDirection8(const int16_t angle)
{
    int16_t normalized = angle % DEG_360;
    if (normalized < 0) {
        normalized += DEG_360;
    }

    if (normalized < DEG_45) {
        return DIR_NORTH;
    }
    if (normalized < DEG_90) {
        return DIR_NORTH_EAST;
    }
    if (normalized < DEG_135) {
        return DIR_EAST;
    }
    if (normalized < DEG_180) {
        return DIR_SOUTH_EAST;
    }
    if (normalized < DEG_225) {
        return DIR_SOUTH;
    }
    if (normalized < DEG_270) {
        return DIR_SOUTH_WEST;
    }
    if (normalized < DEG_315) {
        return DIR_WEST;
    }
    return DIR_NORTH_WEST;
}

DIRECTION Math_GetDirectionCone(const int16_t angle, const int16_t cone)
{
    if (angle >= -cone && angle <= cone) {
        return DIR_NORTH;
    } else if (angle >= DEG_90 - cone && angle <= DEG_90 + cone) {
        return DIR_EAST;
    } else if (angle >= DEG_180 - cone || angle <= -DEG_180 + cone) {
        return DIR_SOUTH;
    } else if (angle >= -DEG_90 - cone && angle <= -DEG_90 + cone) {
        return DIR_WEST;
    }
    return DIR_UNKNOWN;
}

int16_t Math_DirectionToAngle(const DIRECTION dir)
{
    switch (dir) {
    case DIR_NORTH:
        return 0;
    case DIR_NORTH_EAST:
        return DEG_45;
    case DIR_EAST:
        return DEG_90;
    case DIR_SOUTH_EAST:
        return DEG_135;
    case DIR_SOUTH:
        return -DEG_180;
    case DIR_SOUTH_WEST:
        return -DEG_135;
    case DIR_WEST:
        return -DEG_90;
    case DIR_NORTH_WEST:
        return -DEG_45;
    default:
        return 0;
    }
}

int32_t Math_AngleMean(int32_t angle1, int32_t angle2, double ratio)
{
    int32_t diff = angle2 - angle1;

    if (diff > DEG_180) {
        diff -= DEG_360;
    } else if (diff < -DEG_180) {
        diff += DEG_360;
    }

    int32_t result = angle1 + diff * ratio;

    result %= DEG_360;
    if (result < 0) {
        result += DEG_360;
    }

    return result;
}

int32_t Math_FloorDiv(const int32_t x, const int32_t divisor)
{
    return x / divisor;
}

int32_t Math_GCD(int32_t a, int32_t b)
{
    while (b != 0) {
        const int32_t t = a % b;
        a = b;
        b = t;
    }
    return ABS(a);
}

int32_t Math_ApplyPercent(int32_t value, float scale)
{
    return (int32_t)ceilf(value * scale);
}
