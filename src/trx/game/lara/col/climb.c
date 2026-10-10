#include <trx/config.h>
#include <trx/game/input.h>
#include <trx/game/lara.h>
#include <trx/game/lara/util.h>
#include <trx/game/rooms.h>
#include <trx/game/rooms/geometry.h>
#include <trx/version.h>

// clang-format off
#define M_CLIMB_SHIFT            70
#define M_CLIMB_HANG             900
#define M_CLIMB_WIDTH_LEFT       (g_TRVersion >= 4 ? 120 : 80)
#define M_CLIMB_WIDTH_RIGHT      120
#define M_CLIMB_HEIGHT           (WALL_L / 2) // = 512
#define M_LF_HANG                21
#define M_LF_STOP_HANG           9
#define M_LF_CLIMB_L_SHIFT_START 28
#define M_LF_CLIMB_L_SHIFT_END   29
#define M_LF_CLIMB_R_SHIFT       57
#define M_LEDGE_JUMP_PUSH_HEIGHT (STEP_L - 16)                    // = 240
#define M_LEDGE_JUMP_HEIGHT_UP   (LARA_HEIGHT + (STEP_L * 3) / 8) // = 858
#define M_LEDGE_JUMP_HEIGHT_BACK (LARA_HEIGHT - (STEP_L * 5) / 4) // = 442
#define M_HANG_SHIFT             (g_TRVersion >= 3 ? 4 : 2)
#define M_CLIMB_HANG_SHIFT       (g_TRVersion < 4 ? M_HANG_SHIFT : STEP_L)
#define M_CLIMB_WIDTH_CORNER     120
#define M_CORNER_SIDE_SHIFT      16
#define M_CORNER_FRONT_DIST      (LARA_RADIUS + M_CORNER_SIDE_SHIFT) // = 116
#define M_CORNER_MAX_DROP        (STEP_L * 3)                        // = 768
// clang-format on

typedef enum {
    // clang-format off
    CLIMB_RESULT_CRAWL = -2,
    CLIMB_RESULT_NEG  = -1,
    CLIMB_RESULT_NONE = 0,
    CLIMB_RESULT_POS  = 1,
    // clang-format on
} M_CLIMB_RESULT;

static bool M_IsTriangleClimbable(const ITEM *const item, const XYZ_32 pos)
{
    int16_t room_num = item->room_num;
    const SECTOR *const sector = Room_GetSector(pos, &room_num);
    if (sector == nullptr || !sector->floor.is_split) {
        return false;
    }

    const ROOM_HEIGHT_CTX ctx = { .fix_tilts = true };
    const int32_t center = Room_GetHeightEx(sector, pos, &ctx);
    if (center == NO_HEIGHT) {
        return false;
    }

    const int32_t sample = WALL_L / 4;
    const int32_t h_x = Room_GetHeightEx(
        sector, (XYZ_32) { pos.x + sample, pos.y, pos.z }, &ctx);
    const int32_t h_z = Room_GetHeightEx(
        sector, (XYZ_32) { pos.x, pos.y, pos.z + sample }, &ctx);
    if (h_x == NO_HEIGHT || h_z == NO_HEIGHT) {
        return false;
    }

    return ABS(h_x - center) < STEP_L * 2 || ABS(h_z - center) < STEP_L * 2;
}

static int32_t M_GetDestinationClearance(const ITEM *const item)
{
    XYZ_32 pos = {
        .x = 0,
        .y = -M_CLIMB_HEIGHT,
        .z = 0,
    };
    Lara_GetJointAbsPosition(&pos, LM_HAND_R);
    int16_t room_num = item->room_num;
    Room_GetSector(pos, &room_num);

    pos = XYZ_32_OffsetYaw(pos, item->rot.y, LARA_RADIUS * 2);
    const SECTOR *const sector = Room_GetSector(pos, &room_num);
    const int32_t height = Room_GetHeightEx(
        sector, pos, &(ROOM_HEIGHT_CTX) { .fix_tilts = true });
    const int32_t ceiling = Room_GetCeilingEx(
        sector, pos, &(ROOM_HEIGHT_CTX) { .fix_tilts = true });

    if (height == NO_HEIGHT || ceiling == NO_HEIGHT) {
        return NO_HEIGHT;
    }
    return ABS(height - ceiling);
}

static M_CLIMB_RESULT M_TestClimbPos(
    const ITEM *const item, const int32_t front, const int32_t right,
    const int32_t origin, const int32_t item_height, int32_t *const shift)
{
    const int32_t y = item->pos.y + origin;
    int32_t x;
    int32_t z;
    int32_t x_front = 0;
    int32_t z_front = 0;

    switch (Math_GetDirection(item->rot.y)) {
    case DIR_NORTH:
        x = item->pos.x + right;
        z = item->pos.z + front;
        z_front = M_CLIMB_HANG_SHIFT;
        break;

    case DIR_EAST:
        x = item->pos.x + front;
        z = item->pos.z - right;
        x_front = M_CLIMB_HANG_SHIFT;
        break;

    case DIR_SOUTH:
        x = item->pos.x - right;
        z = item->pos.z - front;
        z_front = -M_CLIMB_HANG_SHIFT;
        break;

    case DIR_WEST:
        x = item->pos.x - front;
        z = item->pos.z + right;
        x_front = -M_CLIMB_HANG_SHIFT;
        break;

    default:
        x = front;
        z = front;
        break;
    }

    *shift = 0;
    bool hang = true;

    int16_t room_num = item->room_num;
    XYZ_32 sample_pos = { x, y - 128, z };
    const SECTOR *sector = Room_GetSector(sample_pos, &room_num);
    const bool triangle_climb = M_IsTriangleClimbable(item, (XYZ_32) { x, y, z });
    if (!Lara_GetLaraInfo()->climb_status && !triangle_climb) {
        return CLIMB_RESULT_NONE;
    }

    sample_pos.y = y;
    int32_t height = Room_GetHeightEx(
        sector, sample_pos, &(ROOM_HEIGHT_CTX) { .fix_tilts = true });
    if (height == NO_HEIGHT) {
        return CLIMB_RESULT_NONE;
    }

    height -= y + item_height + STEP_L / 2;
    if (height < -M_CLIMB_SHIFT) {
        return CLIMB_RESULT_NONE;
    }
    if (height < 0) {
        *shift = height;
    }

    int32_t ceiling = Room_GetCeilingEx(
        sector, sample_pos, &(ROOM_HEIGHT_CTX) { .fix_tilts = true }) - y;
    if (ceiling > M_CLIMB_SHIFT) {
        return CLIMB_RESULT_NONE;
    }
    if (ceiling > 0) {
        if (*shift) {
            return CLIMB_RESULT_NONE;
        }
        *shift = ceiling;
    }

    if (item_height + height < M_CLIMB_HANG) {
        hang = false;
    }

    const int32_t x2 = x + x_front;
    const int32_t z2 = z + z_front;
    sample_pos.x = x2;
    sample_pos.y = y;
    sample_pos.z = z2;
    sector = Room_GetSector(sample_pos, &room_num);
    height = Room_GetHeightEx(
        sector, sample_pos, &(ROOM_HEIGHT_CTX) { .fix_tilts = true });
    if (height != NO_HEIGHT) {
        height -= y;
    }

    if (height > M_CLIMB_SHIFT) {
        ceiling = Room_GetCeilingEx(
            sector, sample_pos, &(ROOM_HEIGHT_CTX) { .fix_tilts = true }) - y;
        if (ceiling >= M_CLIMB_HEIGHT) {
            return CLIMB_RESULT_POS;
        }

        if (ceiling > M_CLIMB_HEIGHT - M_CLIMB_SHIFT) {
            if (*shift > 0) {
                return hang ? CLIMB_RESULT_NEG : CLIMB_RESULT_NONE;
            }
            *shift = ceiling - M_CLIMB_HEIGHT;
            return CLIMB_RESULT_POS;
        }

        if (ceiling > 0) {
            return hang ? CLIMB_RESULT_NEG : CLIMB_RESULT_NONE;
        }

        if (ceiling > -M_CLIMB_SHIFT && hang && *shift <= 0) {
            if (*shift > ceiling) {
                *shift = ceiling;
            }

            return CLIMB_RESULT_NEG;
        }

        return CLIMB_RESULT_NONE;
    }

    if (height > 0) {
        if (*shift < 0) {
            return CLIMB_RESULT_NONE;
        }
        if (height > *shift) {
            *shift = height;
        }
    }

    room_num = item->room_num;
    sample_pos = (XYZ_32) { x, item_height + y, z };
    Room_GetSector(sample_pos, &room_num);
    sample_pos.x = x2;
    sample_pos.z = z2;
    sector = Room_GetSector(sample_pos, &room_num);
    ceiling = Room_GetCeilingEx(
        sector, sample_pos, &(ROOM_HEIGHT_CTX) { .fix_tilts = true });
    if (ceiling == NO_HEIGHT) {
        return CLIMB_RESULT_POS;
    }

    ceiling -= y;
    if (ceiling <= height) {
        return CLIMB_RESULT_POS;
    }

    if (ceiling >= M_CLIMB_HEIGHT) {
        return CLIMB_RESULT_POS;
    }

    if (ceiling > M_CLIMB_HEIGHT - M_CLIMB_SHIFT) {
        if (*shift > 0) {
            return hang ? CLIMB_RESULT_NEG : CLIMB_RESULT_NONE;
        }
        *shift = ceiling - M_CLIMB_HEIGHT;
        return CLIMB_RESULT_POS;
    }

    return hang ? CLIMB_RESULT_NEG : CLIMB_RESULT_NONE;
}
