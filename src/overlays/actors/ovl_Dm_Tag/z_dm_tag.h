#ifndef Z_DM_TAG_H
#define Z_DM_TAG_H

#include "global.h"

struct DmTag;

typedef void (*DmTagActionFunc)(struct DmTag*, PlayState*);
typedef s32 (*DmTagMsgEventFunc)(struct DmTag*, PlayState*);

typedef struct DmTag {
    /* 0x000 */ Actor actor;
    /* 0x144 */ UNK_TYPE1 pad144[0x44];
    /* 0x188 */ DmTagActionFunc actionFunc;
    /* 0x18C */ u16 unk_18C;
    /* 0x18E */ u8 unk_18E;
    /* 0x190 */ s32* msgEventScript;
    /* 0x194 */ s32 msgEventArg4;
    /* 0x198 */ UNK_TYPE1 pad198[0xC];
    /* 0x1A4 */ s16 unk_1A4;
    /* 0x1A8 */ DmTagMsgEventFunc msgEventCallback;
} DmTag; // size = 0x1AC

extern const ActorInit Dm_Tag_InitVars;

#endif // Z_DM_TAG_H
