/*
 * File: z_dm_ah.c
 * Overlay: ovl_Dm_Ah
 * Description: Anju's Mother (cutscene)
 */

#include "z_dm_ah.h"

#define FLAGS (ACTOR_FLAG_1 | ACTOR_FLAG_8)

#define THIS ((DmAh*)thisx)

void DmAh_Init(Actor* thisx, PlayState* play);
void DmAh_Destroy(Actor* thisx, PlayState* play);
void DmAh_Update(Actor* thisx, PlayState* play);
void DmAh_Draw(Actor* thisx, PlayState* play);

const ActorInit Dm_Ah_InitVars = {
    ACTOR_DM_AH,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_AH,
    sizeof(DmAh),
    (ActorFunc)DmAh_Init,
    (ActorFunc)DmAh_Destroy,
    (ActorFunc)DmAh_Update,
    (ActorFunc)DmAh_Draw,
};

static AnimationInfoS sAnimations[] = {
    { &object_ah_Anim_001860, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },
    { &object_ah_Anim_000DDC, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },
};

s32 func_80C1D410(DmAh* this, s32 animationIndex) {
    s32 ret = false;

    if (animationIndex != this->animationIndex) {
        this->animationIndex = animationIndex;
        ret = SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimations, animationIndex);
    }
    return ret;
}

void func_80C1D458(DmAh* this) {
    if (DECR(this->unk_286) == 0) {
        this->unk_284++;
        if (this->unk_284 >= 4) {
            this->unk_286 = Rand_S16Offset(30, 30);
            this->unk_284 = 0;
        }
    }
}

s32 func_80C1D4D0(DmAh* this, s32 arg1) {
    s32 pad;
    Vec3f sp40;
    Vec3f sp34;
    s16 temp_v0;

    Math_Vec3f_Copy(&sp40, &this->unk_280->world.pos);
    Math_Vec3f_Copy(&sp34, &this->actor.world.pos);
    temp_v0 = Math_Vec3f_Yaw(&sp34, &sp40);

    Math_ApproachS(&this->unk_28C, (temp_v0 - this->unk_290) - this->actor.shape.rot.y, 4, 0x2AA8);
    this->unk_28C = CLAMP(this->unk_28C, -0x1FFE, 0x1FFE);

    Math_ApproachS(&this->unk_290, (temp_v0 - this->unk_28C) - this->actor.shape.rot.y, 4, 0x2AA8);
    this->unk_290 = CLAMP(this->unk_290, -0x1C70, 0x1C70);

    if (this->unk_280->id == ACTOR_PLAYER) {
        sp40.y = ((Player*)this->unk_280)->bodyPartsPos[7].y + 3.0f;
    } else {
        Math_Vec3f_Copy(&sp40, &this->unk_280->focus.pos);
    }

    Math_Vec3f_Copy(&sp34, &this->actor.focus.pos);
    Math_ApproachS(&this->unk_28A, Math_Vec3f_Pitch(&sp34, &sp40) - this->unk_28E, 4, 0x2AA8);
    this->unk_28A = CLAMP(this->unk_28A, -0x1C70, 0x1C70);

    Math_ApproachS(&this->unk_28E, Math_Vec3f_Pitch(&sp34, &sp40) - this->unk_28A, 4, 0x2AA8);
    this->unk_28E = CLAMP(this->unk_28E, -0x1C70, 0x1C70);
    return true;
}

s32 func_80C1D6E0(DmAh* this, PlayState* play) {
    if (this->unk_280 != NULL) {
        func_80C1D4D0(this, play);
        this->unk_27C &= ~1;
        this->unk_27C |= 2;
    } else if (this->unk_27C & 2) {
        this->unk_27C &= ~2;
        this->unk_28A = 0;
        this->unk_28C = 0;
        this->unk_28E = 0;
        this->unk_290 = 0;
        this->unk_288 = 20;
    } else if (DECR(this->unk_288) == 0) {
        this->unk_27C |= 1;
    }
    return true;
}

Actor* func_80C1D78C(PlayState* play) {
    Actor* tempActor;
    Actor* foundActor = NULL;

    while (true) {
        foundActor = SubS_FindActor(play, foundActor, ACTORCAT_NPC, ACTOR_DM_AN);

        if ((foundActor == NULL) || (foundActor->update != NULL)) {
            break;
        }

        tempActor = foundActor->next;
        if (tempActor == NULL || false) {
            foundActor = NULL;
            break;
        }
        foundActor = tempActor;
    }
    return foundActor;
}

void func_80C1D7FC(DmAh* this, PlayState* play) {
    s32 D_80C1DE00[] = { 0, 0, 0, 0, 0 };
    u16 csAction;
    s32 actionIndex;

    if (play->csCtx.state != 0) {
        if (!this->unk_29C) {
            this->action = 0xFF;
            this->unk_29C = true;
            this->animationIndex2 = this->animationIndex;
        }
        if (Cutscene_CheckActorAction(play, 0x232)) {
            actionIndex = Cutscene_GetActorActionIndex(play, 0x232);
            csAction = play->csCtx.actorActions[actionIndex]->action;
            if (this->action != (u8)csAction) {
                this->action = csAction;
                func_80C1D410(this, D_80C1DE00[csAction]);
            }
            Cutscene_ActorTranslateAndYaw(&this->actor, play, actionIndex);
        }
    } else if (this->unk_29C) {
        this->unk_29C = false;
        func_80C1D410(this, this->animationIndex2);
    }
}

void func_80C1D92C(DmAh* this, PlayState* play) {
}

void DmAh_Init(Actor* thisx, PlayState* play) {
    DmAh* this = THIS;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 20.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &object_ah_Skel_009E70, NULL, this->morphTable, this->jointTable,
                       OBJECT_AH_LIMB_MAX);
    this->animationIndex = -1;
    func_80C1D410(this, 0);
    this->actor.flags &= ~ACTOR_FLAG_1;
    Actor_SetScale(&this->actor, 0.01f);
    this->unk_27C |= 1;
    if ((play->sceneId == SCENE_YADOYA) && (play->curSpawn == 4)) {
        this->unk_280 = func_80C1D78C(play);
        func_80C1D410(this, 1);
        this->actionFunc = func_80C1D92C;
    } else {
        this->actionFunc = func_80C1D7FC;
    }
}

void DmAh_Destroy(Actor* thisx, PlayState* play) {
}

void DmAh_Update(Actor* thisx, PlayState* play) {
    DmAh* this = THIS;

    this->actionFunc(this, play);
    func_80C1D6E0(this, play);
    SkelAnime_Update(&this->skelAnime);
    func_80C1D458(this);
    Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, 12.0f, 0.0f, 4);
}

static Vec3f D_80C1DE14 = { 1000.0f, 0.0f, 0.0f };

void DmAh_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    if (limbIndex == OBJECT_AH_LIMB_07) {
        Matrix_MultVec3f(&D_80C1DE14, &thisx->focus.pos);
        Math_Vec3s_Copy(&thisx->focus.rot, &thisx->world.rot);
    }
}

void DmAh_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
    DmAh* this = THIS;
    s32 stepRot;
    s32 overrideRot;

    if (!(this->unk_27C & 1)) {
        if (this->unk_27C & 2) {
            overrideRot = true;
        } else {
            overrideRot = false;
        }
        stepRot = true;
    } else {
        stepRot = false;
        overrideRot = false;
    }

    if (limbIndex == OBJECT_AH_LIMB_07) {
        SubS_UpdateLimb(BINANG_ADD(this->unk_28A + this->unk_28E, 0x4000),
                        BINANG_ADD(this->unk_28C + this->unk_290 + this->actor.shape.rot.y, 0x4000), &this->unk_18C,
                        &this->unk_1A4, stepRot, overrideRot);
        Matrix_Pop();
        Matrix_Translate(this->unk_18C.x, this->unk_18C.y, this->unk_18C.z, MTXMODE_NEW);
        Matrix_Scale(thisx->scale.x, thisx->scale.y, thisx->scale.z, MTXMODE_APPLY);
        Matrix_RotateYS(this->unk_1A4.y, MTXMODE_APPLY);
        Matrix_RotateXS(this->unk_1A4.x, MTXMODE_APPLY);
        Matrix_RotateZS(this->unk_1A4.z, MTXMODE_APPLY);
        Matrix_Push();
    } else if (limbIndex == OBJECT_AH_LIMB_02) {
        SubS_UpdateLimb(BINANG_ADD(this->unk_28E, 0x4000), BINANG_ADD(this->unk_290 + this->actor.shape.rot.y, 0x4000),
                        &this->unk_198, &this->unk_1AA, stepRot, overrideRot);
        Matrix_Pop();
        Matrix_Translate(this->unk_198.x, this->unk_198.y, this->unk_198.z, MTXMODE_NEW);
        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
        Matrix_RotateYS(this->unk_1AA.y, MTXMODE_APPLY);
        Matrix_RotateXS(this->unk_1AA.x, MTXMODE_APPLY);
        Matrix_RotateZS(this->unk_1AA.z, MTXMODE_APPLY);
        Matrix_Push();
    }
}

static TexturePtr D_80C1DE20[] = { object_ah_Tex_008D70, object_ah_Tex_009570 };

static TexturePtr D_80C1DE28[] = {
    object_ah_Tex_006D70, object_ah_Tex_007570, object_ah_Tex_007D70, object_ah_Tex_007570, object_ah_Tex_008570,
};

void DmAh_Draw(Actor* thisx, PlayState* play) {
    DmAh* this = THIS;

    OPEN_DISPS(play->state.gfxCtx);

    func_8012C28C(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(D_80C1DE28[this->unk_284]));
    gSPSegment(POLY_OPA_DISP++, 0x09, Lib_SegmentedToVirtual(D_80C1DE20[0]));

    SkelAnime_DrawTransformFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   this->skelAnime.dListCount, NULL, DmAh_PostLimbDraw, DmAh_TransformLimbDraw,
                                   &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
