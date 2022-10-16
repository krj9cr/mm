/*
 * File: z_dm_ravine.c
 * Overlay: ovl_Dm_Ravine
 * Description: Lost Woods Cutscene - Tree Trunk
 */

#include "z_dm_ravine.h"

#define FLAGS (ACTOR_FLAG_10 | ACTOR_FLAG_20)

#define THIS ((DmRavine*)thisx)

void DmRavine_Init(Actor* thisx, PlayState* play);
void DmRavine_Destroy(Actor* thisx, PlayState* play);
void DmRavine_DoNothing(DmRavine* this, PlayState* play);
void DmRavine_Update(Actor* thisx, PlayState* play);
void DmRavine_Draw(Actor* thisx, PlayState* play);

const ActorInit Dm_Ravine_InitVars = {
    ACTOR_DM_RAVINE,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_KEIKOKU_DEMO,
    sizeof(DmRavine),
    (ActorFunc)DmRavine_Init,
    (ActorFunc)DmRavine_Destroy,
    (ActorFunc)DmRavine_Update,
    (ActorFunc)DmRavine_Draw,
};

void DmRavine_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    DmRavine* this = THIS;

    if (((((void)0, gSaveContext.save.weekEventReg[0]) & 0x10) | cREG(0)) != 0) {
        Actor_Kill(&this->actor);
        return;
    }

    this->isActive = false;
    play->roomCtx.unk7A[0] = 1;
    play->roomCtx.unk7A[1] = 0;
    this->state = 0;
    Actor_SetScale(&this->actor, 1.0f);
    this->actionFunc = DmRavine_DoNothing;
}

void DmRavine_Destroy(Actor* thisx, PlayState* play) {
}

void DmRavine_DoNothing(DmRavine* this, PlayState* play) {
}

void DmRavine_Update(Actor* thisx, PlayState* play) {
    DmRavine* this = THIS;
    RoomContext* roomCtx;

    switch ((DmRavineState)this->state) {
        case DM_RAVINE_STATE_INITIALIZED:
            return;
        case DM_RAVINE_STATE_ACTIVE:
            this->isActive = true;
            play->roomCtx.unk7A[1]++;
            if (play->roomCtx.unk7A[1] > 254) {
                play->roomCtx.unk7A[1] = 254;
                if (play->csCtx.frames > 700) {
                    play->roomCtx.unk7A[1] = 255;
                    play->roomCtx.unk7A[0] = 0;
                    this->state++; // -> DM_RAVINE_STATE_PENDING_DEATH
                }
            }
            break;
        case DM_RAVINE_STATE_PENDING_DEATH:
            Actor_Kill(&this->actor);
            break;
    }
}

void DmRavine_Draw(Actor* thisx, PlayState* play) {
}
