#ifndef Z_EN_FU_H
#define Z_EN_FU_H

#include "global.h"
#include "overlays/actors/ovl_Bg_Fu_Mizu/z_bg_fu_mizu.h"

struct EnFu;

typedef void (*EnFuActionFunc)(struct EnFu*, PlayState*);

#define ENFU_GET_FF00(thisx) (((thisx)->params >> 8) & 0xFF)

typedef struct EnFuHeartEffect {
    /* 0x00 */ f32 scale;
    /* 0x04 */ UNK_TYPE1 unk_04[0x4];
    /* 0x08 */ Vec3f pos;
    /* 0x14 */ Vec3f accel;
    /* 0x20 */ Vec3f velocity;
    /* 0x2C */ s16 angle;
    /* 0x2E */ UNK_TYPE1 unk_2E[0x8];
    /* 0x36 */ u8 isEnabled;
    /* 0x37 */ u8 timer;
} EnFuHeartEffect; // size = 0x38

typedef struct EnFu {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider;
    /* 0x190 */ SkelAnime skelAnime;
    /* 0x1D4 */ EnFuActionFunc actionFunc;
    /* 0x1D8 */ Vec3s jointTable[21];
    /* 0x256 */ Vec3s morphTable[21];
    /* 0x2D4 */ BgFuMizu* bgFuMizu;
    /* 0x2D8 */ EnFuHeartEffect effects[10];
    /* 0x508 */ Vec3f unk_508;
    /* 0x514 */ Vec3f unk_514;
    /* 0x520 */ s32 unk_520;
    /* 0x524 */ s16 unk_524[3];
    /* 0x52A */ s16 unk_52A[3];
    /* 0x530 */ UNK_TYPE1 unk530[0x8];
    /* 0x538 */ Vec3s* unk_538;
    /* 0x53C */ s16 unk_53C;
    /* 0x53E */ s16 unk_53E;
    /* 0x540 */ s16 unk_540;
    /* 0x542 */ s16 unk_542;
    /* 0x544 */ s16 unk_544;
    /* 0x546 */ s16 unk_546;
    /* 0x548 */ s16 unk_548;
    /* 0x54A */ s16 unk_54A;
    /* 0x54C */ s16 unk_54C;
    /* 0x54E */ s16 spawnHeartTimer;
    /* 0x550 */ s16 unk_550;
    /* 0x552 */ u16 unk_552;
} EnFu; // size = 0x554

extern const ActorInit En_Fu_InitVars;

#endif // Z_EN_FU_H
