/*
 * File: z_en_bombal.c
 * Overlay: ovl_En_Bombal
 * Description: Bombers - Majora Balloon
 */

#include "z_en_bombal.h"
#include "assets/objects/object_fusen/object_fusen.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_10)

#define THIS ((EnBombal*)thisx)

void EnBombal_Init(Actor* thisx, PlayState* play);
void EnBombal_Destroy(Actor* thisx, PlayState* play);
void EnBombal_Update(Actor* thisx, PlayState* play);
void EnBombal_Draw(Actor* thisx, PlayState* play);

void func_80C05B3C(EnBombal* this, PlayState* play);
void func_80C05C44(EnBombal* this, PlayState* play);
void func_80C05DE8(EnBombal* this, PlayState* play);
void func_80C05B24(EnBombal* this);
void EnBombal_InitEffects(EnBombal* this, Vec3f* pos, s16 fadeDelay);
void EnBombal_UpdateEffects(EnBombal* this, PlayState* play);
void EnBombal_DrawEffects(EnBombal*, PlayState*);

const ActorInit En_Bombal_InitVars = {
    ACTOR_EN_BOMBAL,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_FUSEN,
    sizeof(EnBombal),
    (ActorFunc)EnBombal_Init,
    (ActorFunc)EnBombal_Destroy,
    (ActorFunc)EnBombal_Update,
    (ActorFunc)EnBombal_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x004138B0, 0x00, 0x00 },
        TOUCH_NONE | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 60, 90, -50, { 0, 0, 0 } },
};

void EnBombal_Init(Actor* thisx, PlayState* play) {
    EnBombal* this = THIS;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 25.0f);
    this->actor.colChkInfo.mass = 0;
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->actor.targetMode = 6;
    this->actor.colChkInfo.health = 1;
    this->scale = 0.1f;
    this->cutscene = this->actor.cutscene;
    func_80C05B24(this);
}

void EnBombal_Destroy(Actor* thisx, PlayState* play) {
    EnBombal* this = THIS;

    Collider_DestroyCylinder(play, &this->collider);
}

void func_80C05B24(EnBombal* this) {
    this->isPopped = false;
    this->actionFunc = func_80C05B3C;
}

void func_80C05B3C(EnBombal* this, PlayState* play) {
    Player* player;

    this->oscillationAngle += 1500.0f;
    this->actor.velocity.y = Math_SinS(this->oscillationAngle);
    Math_ApproachF(&this->scale, 0.1f, 0.3f, 0.01f);

    if (play->msgCtx.msgLength == 0) {
        if (this->collider.base.acFlags & AC_HIT) {
            player = GET_PLAYER(play);
            this->collider.base.acFlags &= ~AC_HIT;
            if (!(gSaveContext.save.weekEventReg[75] & 0x40) && !(gSaveContext.save.weekEventReg[73] & 0x10) &&
                !(gSaveContext.save.weekEventReg[85] & 2)) {
                player->stateFlags1 |= ACTOR_FLAG_20;
                this->actor.flags |= ACTOR_FLAG_100000;
            }
            this->actionFunc = func_80C05C44;
        }
    }
}

void func_80C05C44(EnBombal* this, PlayState* play) {
    s32 phi_s0 = false;
    s32 i;
    Vec3f pos;

    if (!(gSaveContext.save.weekEventReg[75] & 0x40) && !(gSaveContext.save.weekEventReg[73] & 0x10) &&
        !(gSaveContext.save.weekEventReg[85] & 2)) {
        if (ActorCutscene_GetCurrentIndex() == 0x7C) {
            ActorCutscene_Stop(0x7C);
            ActorCutscene_SetIntentToPlay(this->cutscene);
            return;
        }

        if (!ActorCutscene_GetCanPlayNext(this->cutscene)) {
            ActorCutscene_SetIntentToPlay(this->cutscene);
        } else {
            ActorCutscene_StartAndSetUnkLinkFields(this->cutscene, &this->actor);
            phi_s0 = true;
        }
    } else {
        phi_s0 = true;
    }

    if (phi_s0) {
        Math_Vec3f_Copy(&pos, &this->actor.world.pos);
        pos.y += 60.0f;
        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, pos.x, pos.y, pos.z, 255, 255, 200,
                    CLEAR_TAG_LARGE_EXPLOSION);

        for (i = 0; i < 100; i++) {
            EnBombal_InitEffects(this, &pos, 10);
        }

        gSaveContext.save.weekEventReg[83] |= 4;
        Actor_PlaySfxAtPos(&this->actor, NA_SE_EV_MUJURA_BALLOON_BROKEN);
        this->timer = 30;
        this->isPopped = true;
        this->actionFunc = func_80C05DE8;
    }
}

void func_80C05DE8(EnBombal* this, PlayState* play) {
    if (this->timer == 0) {
        if (!(gSaveContext.save.weekEventReg[75] & 0x40) && !(gSaveContext.save.weekEventReg[73] & 0x10) &&
            !(gSaveContext.save.weekEventReg[85] & 2)) {
            ActorCutscene_Stop(this->cutscene);
        }
        Actor_Kill(&this->actor);
        return;
    }

    if (this->timer < 10) {
        this->actor.colChkInfo.health = 0;
    }
}

void EnBombal_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnBombal* this = THIS;

    if (this->timer != 0) {
        this->timer--;
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;

    Actor_SetFocus(&this->actor, 30.0f);
    Actor_SetScale(&this->actor, this->scale);

    this->actionFunc(this, play);

    Actor_MoveWithGravity(&this->actor);
    EnBombal_UpdateEffects(this, play);

    if (!this->isPopped) {
        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }
}

void EnBombal_Draw(Actor* thisx, PlayState* play) {
    EnBombal* this = THIS;

    if (this->isPopped != true) {
        Gfx_DrawDListOpa(play, gMajoraBalloonDL);
        Gfx_DrawDListOpa(play, gMajoraBalloonKnotDL);
    }
    EnBombal_DrawEffects(this, play);
}

void EnBombal_InitEffects(EnBombal* this, Vec3f* pos, s16 fadeDelay) {
    s16 i;
    EnBombalEffect* sPtr = this->effects;

    for (i = 0; i < ARRAY_COUNT(this->effects); i++, sPtr++) {
        if (!sPtr->isEnabled) {
            sPtr->isEnabled = true;
            sPtr->pos = *pos;
            sPtr->alphaFadeDelay = fadeDelay;
            sPtr->alpha = 255;

            sPtr->accel.x = (Rand_ZeroOne() - 0.5f) * 10.0f;
            sPtr->accel.y = (Rand_ZeroOne() - 0.5f) * 10.0f;
            sPtr->accel.z = (Rand_ZeroOne() - 0.5f) * 10.0f;

            sPtr->velocity.x = Rand_ZeroOne() - 0.5f;
            sPtr->velocity.y = Rand_ZeroOne() - 0.5f;
            sPtr->velocity.z = Rand_ZeroOne() - 0.5f;

            sPtr->scale = (Rand_ZeroFloat(1.0f) * 0.5f) + 2.0f;
            return;
        }
    }
}

void EnBombal_UpdateEffects(EnBombal* this, PlayState* play) {
    s32 i;
    EnBombalEffect* sPtr = this->effects;

    for (i = 0; i < ARRAY_COUNT(this->effects); i++, sPtr++) {
        if (sPtr->isEnabled) {
            sPtr->pos.x += sPtr->velocity.x;
            sPtr->pos.y += sPtr->velocity.y;
            sPtr->pos.z += sPtr->velocity.z;
            sPtr->velocity.x += sPtr->accel.x;
            sPtr->velocity.y += sPtr->accel.y;
            sPtr->velocity.z += sPtr->accel.z;

            if (sPtr->alphaFadeDelay != 0) {
                sPtr->alphaFadeDelay--;
            } else {
                sPtr->alpha -= 10;
                if (sPtr->alpha < 10) {
                    sPtr->isEnabled = 0;
                }
            }
        }
    }
}

void EnBombal_DrawEffects(EnBombal* this, PlayState* play) {
    s16 i;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    EnBombalEffect* sPtr = this->effects;

    OPEN_DISPS(gfxCtx);

    func_8012C28C(gfxCtx);
    func_8012C2DC(play->state.gfxCtx);

    for (i = 0; i < ARRAY_COUNT(this->effects); i++, sPtr++) {
        if (sPtr->isEnabled != 0) {
            Matrix_Translate(sPtr->pos.x, sPtr->pos.y, sPtr->pos.z, MTXMODE_NEW);
            Matrix_Scale(sPtr->scale, sPtr->scale, sPtr->scale, MTXMODE_APPLY);

            POLY_XLU_DISP = Gfx_CallSetupDL(POLY_XLU_DISP, 0x14);

            gSPSegment(POLY_XLU_DISP++, 0x08, Lib_SegmentedToVirtual(&gSun1Tex));

            gSPDisplayList(POLY_XLU_DISP++, &gSunSparkleMaterialDL);

            gDPPipeSync(POLY_XLU_DISP++);

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
            gDPSetEnvColor(POLY_XLU_DISP++, 250, 180, 255, sPtr->alpha);

            Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);
            Matrix_RotateZF(DEGF_TO_RADF(play->state.frames * 20.0f), MTXMODE_APPLY);

            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, &gSunSparkleModelDL);
        }
    }
    CLOSE_DISPS(gfxCtx);
}
