/*
 * File: z_en_torch2.c
 * Overlay: ovl_En_Torch2
 * Description: Elegy of Emptiness Shell
 */

#include "z_en_torch2.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_10)

#define THIS ((EnTorch2*)thisx)

void EnTorch2_Init(Actor* thisx, PlayState* play);
void EnTorch2_Destroy(Actor* thisx, PlayState* play);
void EnTorch2_Update(Actor* thisx, PlayState* play);
void EnTorch2_Draw(Actor* thisx, PlayState* play2);

void EnTorch2_UpdateIdle(Actor* thisx, PlayState* play);
void EnTorch2_UpdateDeath(Actor* thisx, PlayState* play);

const ActorInit En_Torch2_InitVars = {
    ACTOR_EN_TORCH2,
    ACTORCAT_ITEMACTION,
    FLAGS,
    GAMEPLAY_KEEP,
    sizeof(EnTorch2),
    (ActorFunc)EnTorch2_Init,
    (ActorFunc)EnTorch2_Destroy,
    (ActorFunc)EnTorch2_Update,
    (ActorFunc)EnTorch2_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_METAL,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER | OC1_TYPE_1 | OC1_TYPE_2,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK2,
        { 0x00100000, 0, 0 },
        { 0xF7CFFFFF, 0, 0 },
        TOUCH_NONE,
        BUMP_ON | BUMP_HOOKABLE,
        OCELEM_ON,
    },
    { 20, 60, 0, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(colChkInfo.mass, MASS_IMMOVABLE, ICHAIN_STOP),
};

// Shells for each of Link's different forms
// (Playing elegy as Fierce Deity puts down a human shell)
static Gfx* sShellDLists[] = {
    gElegyShellHumanDL, gElegyShellGoronDL, gElegyShellZoraDL, gElegyShellDekuDL, gElegyShellHumanDL,
};

void EnTorch2_Init(Actor* thisx, PlayState* play) {
    EnTorch2* this = THIS;
    s16 params;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);

    // params: which form Link is in (e.g. human, deku, etc.)
    params = this->actor.params;
    if (params != TORCH2_PARAM_DEKU) {
        this->actor.flags |= ACTOR_FLAG_4000000; // Can press switch
        if (params == TORCH2_PARAM_GORON) {
            this->actor.flags |= ACTOR_FLAG_20000; // Can press heavy switches
        }
    }
    this->framesUntilNextState = 20;
}

void EnTorch2_Destroy(Actor* thisx, PlayState* play) {
    EnTorch2* this = THIS;

    Collider_DestroyCylinder(play, &this->collider);
    Play_SetRespawnData(&play->state, this->actor.params + RESPAWN_MODE_GORON - 1, 0xFF, 0, 0xBFF,
                        &this->actor.world.pos, this->actor.shape.rot.y);
    play->actorCtx.unk254[this->actor.params] = 0;
}

void EnTorch2_Update(Actor* thisx, PlayState* play) {
    EnTorch2* this = THIS;
    u16 targetAlpha;
    u16 remainingFrames;
    s32 pad[2];

    if (this->state == TORCH2_STATE_IDLE) {
        this->actor.update = EnTorch2_UpdateIdle;
        return;
    }

    this->actor.gravity = -1.0f;
    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, 20.0f, 70.0f, 0x05);

    if (this->framesUntilNextState == 0) {
        remainingFrames = 0;
    } else {
        remainingFrames = --this->framesUntilNextState;
    }

    if (remainingFrames == 0) {
        if (this->state == TORCH2_STATE_INITIALIZED) {
            // Spawn in
            if (this->alpha == 0) {
                Math_Vec3f_Copy(&this->actor.world.pos, &this->actor.home.pos);
                this->actor.shape.rot.y = this->actor.home.rot.y;
                this->state = TORCH2_STATE_FADING_IN;
            }
            targetAlpha = 0;
        } else if (this->state == TORCH2_STATE_FADING_IN) {
            // Stay semitransparent until the player moves away
            if ((this->actor.xzDistToPlayer > 32.0f) || (fabsf(this->actor.playerHeightRel) > 70.0f)) {
                this->state = TORCH2_STATE_SOLID;
            }
            targetAlpha = 60;
        } else {
            // Once the player has moved away, update collision and become opaque
            Collider_UpdateCylinder(&this->actor, &this->collider);
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
            targetAlpha = 255;
        }
        Math_StepToS(&this->alpha, targetAlpha, 8);
    }
}

void EnTorch2_UpdateIdle(Actor* thisx, PlayState* play) {
    EnTorch2* this = THIS;

    if (this->state == TORCH2_STATE_DYING) {
        // Start death animation
        this->actor.update = EnTorch2_UpdateDeath;
        this->actor.velocity.y = 0.0f;
    }
}

void EnTorch2_UpdateDeath(Actor* thisx, PlayState* play) {
    EnTorch2* this = THIS;

    // Fall down and become transparent, then delete once invisible
    if (Math_StepToS(&this->alpha, 0, 8)) {
        Actor_Kill(&this->actor);
        return;
    }

    this->actor.gravity = -1.0f;
    Actor_MoveWithGravity(&this->actor);
}

void EnTorch2_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnTorch2* this = THIS;
    Gfx* gfx = sShellDLists[this->actor.params];

    OPEN_DISPS(play->state.gfxCtx);
    if (this->alpha == 0xFF) {
        Scene_SetRenderModeXlu(play, 0, 0x01);
        gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);
        Gfx_DrawDListOpa(play, gfx);
    } else {
        Scene_SetRenderModeXlu(play, 1, 0x02);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, this->alpha);
        Gfx_DrawDListXlu(play, gfx);
    }
    CLOSE_DISPS(play->state.gfxCtx);
}
