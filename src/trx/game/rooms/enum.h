typedef enum {
    // clang-format off
    LADDER_NONE    = 0,
    LADDER_NORTH   = 1 << 0,
    LADDER_EAST    = 1 << 1,
    LADDER_SOUTH   = 1 << 2,
    LADDER_WEST    = 1 << 3,
    LADDER_NORTH_EAST = 1 << 4,
    LADDER_SOUTH_EAST = 1 << 5,
    LADDER_SOUTH_WEST = 1 << 6,
    LADDER_NORTH_WEST = 1 << 7,
    LADDER_CEILING = 1 << 8,
    // clang-format on
} LADDER_DIRECTION;
