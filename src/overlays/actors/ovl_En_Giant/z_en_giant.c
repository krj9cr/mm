/*
 * File: z_en_giant.c
 * Overlay: ovl_En_Giant
 * Description: Giant
 */

#include "z_en_giant.h"

#define FLAGS (ACTOR_FLAG_10 | ACTOR_FLAG_20)

#define THIS ((EnGiant*)thisx)

void EnGiant_Init(Actor* thisx, PlayState* play);
void EnGiant_Destroy(Actor* thisx, PlayState* play);
void EnGiant_Update(Actor* thisx, PlayState* play);
void EnGiant_Draw(Actor* thisx, PlayState* play);

void EnGiant_PerformClockTowerSuccessActions(EnGiant* this, PlayState* play);
void EnGiant_PlayClockTowerFailureAnimation(EnGiant* this, PlayState* play);
void EnGiant_PerformCutsceneActions(EnGiant* this, PlayState* play);

#define GIANT_TYPE_IS_NOT_TERMINA_FIELD(type) (type > GIANT_TYPE_OCEAN_TERMINA_FIELD)
#define GIANT_TYPE_IS_TERMINA_FIELD(type) (type <= GIANT_TYPE_OCEAN_TERMINA_FIELD)
#define GIANT_TYPE_IS_CLOCK_TOWER_SUCCESS(type) \
    (type >= GIANT_TYPE_MOUNTAIN_CLOCK_TOWER_SUCCESS && type <= GIANT_TYPE_OCEAN_CLOCK_TOWER_SUCCESS)
#define GIANT_TYPE_IS_CHAMBER_OR_ENDING(type) \
    (type >= GIANT_TYPE_MOUNTAIN_GIANTS_CHAMBER_AND_ENDING && type <= GIANT_TYPE_OCEAN_GIANTS_CHAMBER_AND_ENDING)
#define GIANT_TYPE_IS_CLOCK_TOWER_FAILURE(type) \
    (type >= GIANT_TYPE_MOUNTAIN_CLOCK_TOWER_FAILURE && type <= GIANT_TYPE_OCEAN_CLOCK_TOWER_FAILURE)

/**
 * These values are used to index into sAnimations to pick the appropriate animation.
 */
typedef enum {
    /*  0 */ GIANT_ANIM_LOOK_UP_START,
    /*  1 */ GIANT_ANIM_LOOK_UP_LOOP,
    /*  2 */ GIANT_ANIM_FALLING_OVER,
    /*  3 */ GIANT_ANIM_RAISED_ARMS_START,
    /*  4 */ GIANT_ANIM_RAISED_ARMS_LOOP,
    /*  5 */ GIANT_ANIM_STRUGGLE_START,
    /*  6 */ GIANT_ANIM_STRUGGLE_LOOP,
    /*  7 */ GIANT_ANIM_IDLE_LOOP,
    /*  8 */ GIANT_ANIM_WALKING_LOOP,
    /*  9 */ GIANT_ANIM_BIG_CALL_START,
    /* 10 */ GIANT_ANIM_BIG_CALL_LOOP,
    /* 11 */ GIANT_ANIM_BIG_CALL_END,
    /* 12 */ GIANT_ANIM_SMALL_CALL_START,
    /* 13 */ GIANT_ANIM_SMALL_CALL_LOOP,
    /* 14 */ GIANT_ANIM_SMALL_CALL_END,
    /* 15 */ GIANT_ANIM_MAX
} GiantAnimation;

/**
 * Used as values for csAction. The UNKNOWN ones are never used in-game.
 */
typedef enum {
    /*  0 */ GIANT_CS_ACTION_NONE,
    /*  1 */ GIANT_CS_ACTION_IDLE,
    /*  2 */ GIANT_CS_ACTION_WALKING,
    /*  3 */ GIANT_CS_ACTION_LOOKING_UP,
    /*  4 */ GIANT_CS_ACTION_RAISING_ARMS,
    /*  5 */ GIANT_CS_ACTION_STRUGGLING,
    /*  6 */ GIANT_CS_ACTION_FALLING_OVER,
    /*  7 */ GIANT_CS_ACTION_IDLE_FADE_IN,
    /*  8 */ GIANT_CS_ACTION_TALKING,
    /*  9 */ GIANT_CS_ACTION_DONE_TALKING,
    /* 10 */ GIANT_CS_ACTION_TEACHING_OATH_TO_ORDER,
    /* 11 */ GIANT_CS_ACTION_PLAYER_LEARNED_OATH_TO_ORDER,
    /* 12 */ GIANT_CS_ACTION_UNKNOWN_12,
    /* 13 */ GIANT_CS_ACTION_UNKNOWN_13,
    /* 14 */ GIANT_CS_ACTION_UNKNOWN_14,
    /* 15 */ GIANT_CS_ACTION_HOLDING_UP_MOON_IN_CLOCK_TOWER
} GiantCsActionIndex;

const ActorInit En_Giant_InitVars = {
    ACTOR_EN_GIANT,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_GIANT,
    sizeof(EnGiant),
    (ActorFunc)EnGiant_Init,
    (ActorFunc)EnGiant_Destroy,
    (ActorFunc)EnGiant_Update,
    (ActorFunc)EnGiant_Draw,
};

static AnimationHeader* sAnimations[] = {
    &gGiantLookUpStartAnim,    &gGiantLookUpLoopAnim,    &gGiantFallingOverAnim,  &gGiantRaisedArmsStartAnim,
    &gGiantRaisedArmsLoopAnim, &gGiantStruggleStartAnim, &gGiantStruggleLoopAnim, &gGiantIdleAnim,
    &gGiantWalkingAnim,        &gGiantBigCallStartAnim,  &gGiantBigCallLoopAnim,  &gGiantBigCallEndAnim,
    &gGiantSmallCallStartAnim, &gGiantSmallCallLoopAnim, &gGiantSmallCallEndAnim,
};

void EnGiant_ChangeAnim(EnGiant* this, s16 animIndex) {
    if (animIndex >= GIANT_ANIM_LOOK_UP_START && animIndex < GIANT_ANIM_MAX) {
        if (((this->animIndex == GIANT_ANIM_WALKING_LOOP) && (animIndex != GIANT_ANIM_WALKING_LOOP)) ||
            ((animIndex == GIANT_ANIM_WALKING_LOOP) && (this->animIndex != GIANT_ANIM_WALKING_LOOP))) {
            Animation_Change(&this->skelAnime, sAnimations[animIndex], 1.0f, 0.0f,
                             Animation_GetLastFrame(&sAnimations[animIndex]->common), ANIMMODE_ONCE, 10.0f);
        } else {
            Animation_PlayOnce(&this->skelAnime, sAnimations[animIndex]);
        }
        this->animIndex = animIndex;
    }
}

s32 EnGiant_IsImprisoned(EnGiant* this) {
    switch (GIANT_TYPE(&this->actor)) {
        case GIANT_TYPE_SWAMP_TERMINA_FIELD:
        case GIANT_TYPE_SWAMP_CLOCK_TOWER_SUCCESS:
        case GIANT_TYPE_SWAMP_GIANTS_CHAMBER_AND_ENDING:
        case GIANT_TYPE_SWAMP_CLOCK_TOWER_FAILURE:
            if (!CHECK_QUEST_ITEM(QUEST_REMAINS_ODOLWA)) {
                return true;
            }
            break;
        case GIANT_TYPE_MOUNTAIN_TERMINA_FIELD:
        case GIANT_TYPE_MOUNTAIN_CLOCK_TOWER_SUCCESS:
        case GIANT_TYPE_MOUNTAIN_GIANTS_CHAMBER_AND_ENDING:
        case GIANT_TYPE_MOUNTAIN_CLOCK_TOWER_FAILURE:
            if (!CHECK_QUEST_ITEM(QUEST_REMAINS_GOHT)) {
                return true;
            }
            break;
        case GIANT_TYPE_OCEAN_TERMINA_FIELD:
        case GIANT_TYPE_OCEAN_CLOCK_TOWER_SUCCESS:
        case GIANT_TYPE_OCEAN_GIANTS_CHAMBER_AND_ENDING:
        case GIANT_TYPE_OCEAN_CLOCK_TOWER_FAILURE:
            if (!CHECK_QUEST_ITEM(QUEST_REMAINS_GYORG)) {
                return true;
            }
            break;
        case GIANT_TYPE_CANYON_TERMINA_FIELD:
        case GIANT_TYPE_CANYON_CLOCK_TOWER_SUCCESS:
        case GIANT_TYPE_CANYON_GIANTS_CHAMBER_AND_ENDING:
        case GIANT_TYPE_CANYON_CLOCK_TOWER_FAILURE:
            if (!CHECK_QUEST_ITEM(QUEST_REMAINS_TWINMOLD)) {
                return true;
            }
            break;
    }

    return false;
}

void EnGiant_Init(Actor* thisx, PlayState* play) {
    EnGiant* this = THIS;
    s32 type = GIANT_TYPE(thisx);

    this->actor.uncullZoneForward = 4000.0f;
    this->actor.uncullZoneScale = 2000.0f;
    this->actor.uncullZoneDownward = 2400.0f;
    Actor_SetScale(&this->actor, 0.32f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gGiantSkel, &gGiantLargeStrideAnim, this->jointTable, this->morphTable,
                       GIANT_LIMB_MAX);
    EnGiant_ChangeAnim(this, GIANT_ANIM_IDLE_LOOP);
    this->csAction = GIANT_CS_ACTION_NONE;
    this->actionFunc = EnGiant_PerformCutsceneActions;
    this->actor.draw = NULL;
    this->alpha = 0;
    this->actor.velocity.y = -10.0f;
    this->actor.terminalVelocity = -10.0f;
    this->actor.gravity = -5.0f;
    switch (type) {
        case GIANT_TYPE_CANYON_TERMINA_FIELD:
        case GIANT_TYPE_CANYON_CLOCK_TOWER_SUCCESS:
        case GIANT_TYPE_CANYON_GIANTS_CHAMBER_AND_ENDING:
            this->actorActionCommand = 454;
            break;
        case GIANT_TYPE_SWAMP_TERMINA_FIELD:
        case GIANT_TYPE_SWAMP_CLOCK_TOWER_SUCCESS:
        case GIANT_TYPE_SWAMP_GIANTS_CHAMBER_AND_ENDING:
            this->actorActionCommand = 455;
            break;
        case GIANT_TYPE_OCEAN_TERMINA_FIELD:
        case GIANT_TYPE_OCEAN_CLOCK_TOWER_SUCCESS:
        case GIANT_TYPE_OCEAN_GIANTS_CHAMBER_AND_ENDING:
            this->actorActionCommand = 456;
            break;
        default:
            this->actorActionCommand = 453;
            break;
    }

    if (GIANT_TYPE_IS_CLOCK_TOWER_SUCCESS(type)) {
        if (!(gSaveContext.save.weekEventReg[25] & 2)) {
            Actor_Kill(&this->actor);
            return;
        }

        this->actorActionCommand = 0x1C5;
        Actor_SetScale(&this->actor, 0.32f);
        this->actionFunc = EnGiant_PerformClockTowerSuccessActions;
        Animation_Change(&this->skelAnime, &gGiantRaisedArmsStartAnim, 0.0f,
                         Animation_GetLastFrame(&gGiantRaisedArmsStartAnim) - 1.0f,
                         Animation_GetLastFrame(&gGiantRaisedArmsStartAnim), ANIMMODE_ONCE, 0.0f);
        this->actor.draw = EnGiant_Draw;
        this->actor.velocity.y = 0.0f;
        this->actor.terminalVelocity = 0.0f;
        this->actor.gravity = 0.0f;
    }

    if (GIANT_TYPE_IS_CLOCK_TOWER_FAILURE(type)) {
        Actor_SetScale(&this->actor, 0.32f);
        this->actionFunc = EnGiant_PlayClockTowerFailureAnimation;
        Animation_Change(&this->skelAnime, &gGiantStruggleLoopAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gGiantStruggleStartAnim), ANIMMODE_LOOP, 0.0f);
        this->actor.draw = EnGiant_Draw;
        this->actor.velocity.y = 0.0f;
        this->actor.terminalVelocity = 0.0f;
        this->actor.gravity = 0.0f;
        if (EnGiant_IsImprisoned(this)) {
            Actor_Kill(&this->actor);
        }
    }

    if (GIANT_TYPE_IS_NOT_TERMINA_FIELD(type)) {
        this->alpha = 255;
    }

    this->sfxId = 0xFFFF;
    if (GIANT_TYPE_IS_CHAMBER_OR_ENDING(type)) {
        switch (gSaveContext.sceneLayer) {
            case 0:
            case 10:
                this->sfxId = NA_SE_EV_KYOJIN_GRATITUDE2 - SFX_FLAG;
                break;
            case 1:
            case 2:
            case 3:
                this->sfxId = NA_SE_EV_KYOJIN_GRATITUDE1 - SFX_FLAG;
                break;
            default:
                this->sfxId = NA_SE_EV_KYOJIN_GRATITUDE0 - SFX_FLAG;
                break;
        }
    }
}

void EnGiant_Destroy(Actor* thisx, PlayState* play) {
}

/**
 * The animations in sAnimations are organized such that looping animations
 * appear immediately after their respective starting animations. The point of
 * this function is to play the requested start animation if it has not been
 * played yet and play the respetive looping animation otherwise.
 */
void EnGiant_ChangeToStartOrLoopAnimation(EnGiant* this, s16 requestedAnimIndex) {
    s16 nextAnimIndex = requestedAnimIndex + 1;

    if (this->animIndex != nextAnimIndex) {
        if (this->animIndex != requestedAnimIndex) {
            EnGiant_ChangeAnim(this, requestedAnimIndex);
        } else {
            EnGiant_ChangeAnim(this, nextAnimIndex);
        }
    }
}

/**
 * Immediately switches to the specified animation for this cutscene action.
 */
void EnGiant_ChangeAnimBasedOnCsAction(EnGiant* this) {
    switch (this->csAction) {
        case GIANT_CS_ACTION_IDLE:
            EnGiant_ChangeAnim(this, GIANT_ANIM_IDLE_LOOP);
            break;
        case GIANT_CS_ACTION_WALKING:
            EnGiant_ChangeAnim(this, GIANT_ANIM_WALKING_LOOP);
            break;
        case GIANT_CS_ACTION_STRUGGLING:
            EnGiant_ChangeAnim(this, GIANT_ANIM_STRUGGLE_START);
            break;
        case GIANT_CS_ACTION_FALLING_OVER:
            EnGiant_ChangeAnim(this, GIANT_ANIM_FALLING_OVER);
            break;
        case GIANT_CS_ACTION_IDLE_FADE_IN:
            EnGiant_ChangeAnim(this, GIANT_ANIM_IDLE_LOOP);
            this->alpha = 0;
            break;
        case GIANT_CS_ACTION_TALKING:
            EnGiant_ChangeAnim(this, GIANT_ANIM_BIG_CALL_START);
            break;
        case GIANT_CS_ACTION_DONE_TALKING:
            EnGiant_ChangeAnim(this, GIANT_ANIM_BIG_CALL_END);
            break;
        case GIANT_CS_ACTION_TEACHING_OATH_TO_ORDER:
            EnGiant_ChangeAnim(this, GIANT_ANIM_SMALL_CALL_START);
            break;
        case GIANT_CS_ACTION_PLAYER_LEARNED_OATH_TO_ORDER:
            EnGiant_ChangeAnim(this, GIANT_ANIM_SMALL_CALL_END);
            break;
        case GIANT_CS_ACTION_UNKNOWN_12:
            EnGiant_ChangeAnim(this, GIANT_ANIM_IDLE_LOOP);
            break;
        case GIANT_CS_ACTION_UNKNOWN_13:
            EnGiant_ChangeAnim(this, GIANT_ANIM_WALKING_LOOP);
            break;
        case GIANT_CS_ACTION_UNKNOWN_14:
            if (this->animIndex != GIANT_ANIM_WALKING_LOOP) {
                EnGiant_ChangeAnim(this, GIANT_ANIM_WALKING_LOOP);
            }
            break;
        case GIANT_CS_ACTION_HOLDING_UP_MOON_IN_CLOCK_TOWER:
            Animation_Change(&this->skelAnime, &gGiantRaisedArmsStartAnim, 0.0f,
                             Animation_GetLastFrame(&gGiantRaisedArmsStartAnim) - 1.0f,
                             Animation_GetLastFrame(&gGiantRaisedArmsStartAnim), ANIMMODE_ONCE, 0.0f);
            break;
    }
}

void EnGiant_UpdateAlpha(EnGiant* this) {
    switch (this->csAction) {
        case GIANT_CS_ACTION_FALLING_OVER:
            if (this->skelAnime.curFrame >= 90.0f && this->alpha > 0) {
                this->alpha -= 12;
            }
            break;
        case GIANT_CS_ACTION_UNKNOWN_14:
            this->alpha -= 12;
            break;
        default:
            if (this->alpha < 255) {
                this->alpha += 8;
            }
            break;
    }
}

/**
 * Plays the currently specified animation and, if the animation is done playing,
 * switches to the animation appropriate for the current cutscene action. This
 * function can be used to play the appropriate starting or looping animation for
 * a given cutscene action, depending on what animation has already been played.
 */
void EnGiant_PlayAndUpdateAnimation(EnGiant* this) {
    if (SkelAnime_Update(&this->skelAnime) &&
        (this->animIndex != GIANT_ANIM_FALLING_OVER || this->csAction != GIANT_CS_ACTION_FALLING_OVER)) {
        EnGiant_ChangeAnim(this, this->animIndex);
        switch (this->csAction) {
            case GIANT_CS_ACTION_LOOKING_UP:
                EnGiant_ChangeToStartOrLoopAnimation(this, GIANT_ANIM_LOOK_UP_START);
                break;
            case GIANT_CS_ACTION_RAISING_ARMS:
                EnGiant_ChangeToStartOrLoopAnimation(this, GIANT_ANIM_RAISED_ARMS_START);
                break;
            case GIANT_CS_ACTION_STRUGGLING:
                EnGiant_ChangeToStartOrLoopAnimation(this, GIANT_ANIM_STRUGGLE_START);
                break;
            case GIANT_CS_ACTION_FALLING_OVER:
                // Unused
                EnGiant_ChangeToStartOrLoopAnimation(this, GIANT_ANIM_FALLING_OVER);
                break;
            case GIANT_CS_ACTION_TALKING:
                EnGiant_ChangeAnim(this, GIANT_ANIM_BIG_CALL_LOOP);
                break;
            case GIANT_CS_ACTION_DONE_TALKING:
            case GIANT_CS_ACTION_PLAYER_LEARNED_OATH_TO_ORDER:
                EnGiant_ChangeAnim(this, GIANT_ANIM_IDLE_LOOP);
                break;
            case GIANT_CS_ACTION_TEACHING_OATH_TO_ORDER:
                EnGiant_ChangeAnim(this, GIANT_ANIM_SMALL_CALL_LOOP);
                break;
        }
        SkelAnime_Update(&this->skelAnime);
    }
}

void EnGiant_PlaySound(EnGiant* this) {
    if (this->actor.draw != NULL && this->alpha > 0) {
        if (this->animIndex == GIANT_ANIM_WALKING_LOOP &&
            (Animation_OnFrame(&this->skelAnime, 40.0f) || Animation_OnFrame(&this->skelAnime, 100.0f))) {
            Actor_PlaySfxAtPos(&this->actor, NA_SE_EV_KYOJIN_WALK);
        }
        if (this->animIndex == GIANT_ANIM_FALLING_OVER && Animation_OnFrame(&this->skelAnime, 40.0f)) {
            Actor_PlaySfxAtPos(&this->actor, NA_SE_EV_KYOJIN_VOICE_FAIL);
        }
        if (this->sfxId != 0xFFFF &&
            ((this->animIndex == GIANT_ANIM_BIG_CALL_START && this->skelAnime.curFrame >= 18.0f) ||
             this->animIndex == GIANT_ANIM_BIG_CALL_LOOP)) {
            func_800B9010(&this->actor, this->sfxId);
        }
        if ((this->animIndex == GIANT_ANIM_SMALL_CALL_START && this->skelAnime.curFrame >= 18.0f) ||
            this->animIndex == GIANT_ANIM_SMALL_CALL_LOOP) {
            func_800B9010(&this->actor, NA_SE_EV_KYOJIN_SIGN - SFX_FLAG);
        }
    }
}

void EnGiant_UpdatePosition(EnGiant* this, PlayState* play, u32 actionIndex) {
    CsCmdActorAction* actorAction = play->csCtx.actorActions[actionIndex];
    f32 startPosY = actorAction->startPos.y;
    s32 pad[2];
    f32 endPosY = actorAction->endPos.y;
    f32 scale = Environment_LerpWeight(actorAction->endFrame, actorAction->startFrame, play->csCtx.frames);

    this->actor.world.pos.y = ((endPosY - startPosY) * scale) + startPosY;
}

void EnGiant_PerformClockTowerSuccessActions(EnGiant* this, PlayState* play) {
    if (Cutscene_CheckActorAction(play, this->actorActionCommand)) {
        EnGiant_UpdatePosition(this, play, Cutscene_GetActorActionIndex(play, this->actorActionCommand));
        if (this->csAction !=
            play->csCtx.actorActions[Cutscene_GetActorActionIndex(play, this->actorActionCommand)]->action) {
            this->csAction =
                play->csCtx.actorActions[Cutscene_GetActorActionIndex(play, this->actorActionCommand)]->action;
            EnGiant_ChangeAnimBasedOnCsAction(this);
        }
        EnGiant_UpdateAlpha(this);
    }

    EnGiant_PlaySound(this);
    if (this->csAction == GIANT_CS_ACTION_STRUGGLING) {
        func_800B9010(&this->actor, NA_SE_IT_KYOJIN_BEARING - SFX_FLAG);
    }
    EnGiant_PlayAndUpdateAnimation(this);
}

void EnGiant_PlayClockTowerFailureAnimation(EnGiant* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
}

void EnGiant_PerformCutsceneActions(EnGiant* this, PlayState* play) {
    this->actor.draw = EnGiant_Draw;

    if (Cutscene_CheckActorAction(play, this->actorActionCommand)) {
        Cutscene_ActorTranslateAndYaw(&this->actor, play, Cutscene_GetActorActionIndex(play, this->actorActionCommand));
        if (this->csAction !=
            play->csCtx.actorActions[Cutscene_GetActorActionIndex(play, this->actorActionCommand)]->action) {
            this->csAction =
                play->csCtx.actorActions[Cutscene_GetActorActionIndex(play, this->actorActionCommand)]->action;
            EnGiant_ChangeAnimBasedOnCsAction(this);
        }
        EnGiant_UpdateAlpha(this);
    }

    if (GIANT_TYPE_IS_TERMINA_FIELD(GIANT_TYPE(&this->actor)) && EnGiant_IsImprisoned(this)) {
        this->actor.draw = NULL;
    }

    EnGiant_PlaySound(this);
    EnGiant_PlayAndUpdateAnimation(this);
}

void EnGiant_Update(Actor* thisx, PlayState* play) {
    EnGiant* this = THIS;
    s32 blinkTimerTemp;

    this->actionFunc(this, play);
    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 4);

    if (this->blinkTimer == 0) {
        blinkTimerTemp = 0;
    } else {
        this->blinkTimer--;
        blinkTimerTemp = this->blinkTimer;
    }

    if (!blinkTimerTemp) {
        this->blinkTimer = Rand_S16Offset(60, 60);
    }
    this->faceIndex = this->blinkTimer;
    if (this->faceIndex >= 3) {
        this->faceIndex = 0;
    }
}

void EnGiant_PostLimbDrawOpa(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    if (limbIndex == GIANT_LIMB_HEAD) {
        OPEN_DISPS(play->state.gfxCtx);

        gSPDisplayList(POLY_OPA_DISP++, gGiantBeardDL);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void EnGiant_PostLimbDrawXlu(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx, Gfx** gfx) {
    EnGiant* this = THIS;

    if (limbIndex == GIANT_LIMB_HEAD) {
        Matrix_Get(&this->headDrawMtxF);
    }
}

void EnGiant_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnGiant* this = THIS;
    static TexturePtr sFaceTextures[] = { gGiantFaceEyeOpenTex, gGiantFaceEyeHalfTex, gGiantFaceEyeClosedTex };

    if (this->alpha > 0) {
        OPEN_DISPS(play->state.gfxCtx);

        if (this->alpha >= 255) {
            func_8012C28C(play->state.gfxCtx);
            gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sFaceTextures[this->faceIndex]));
            gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
            Scene_SetRenderModeXlu(play, 0, 1);
            SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                  this->skelAnime.dListCount, NULL, EnGiant_PostLimbDrawOpa, thisx);
        } else if (this->alpha > 0) {
            if (this->alpha >= 129) {
                func_8012C2B4(POLY_XLU_DISP++);
                Scene_SetRenderModeXlu(play, 2, 2);
            } else {
                func_8012C304(POLY_XLU_DISP++);
                Scene_SetRenderModeXlu(play, 1, 2);
            }
            gSPSegment(POLY_XLU_DISP++, 0x08, Lib_SegmentedToVirtual(sFaceTextures[this->faceIndex]));
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);
            POLY_XLU_DISP =
                SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   this->skelAnime.dListCount, NULL, EnGiant_PostLimbDrawXlu, thisx, POLY_XLU_DISP);
            Matrix_Mult(&this->headDrawMtxF, MTXMODE_NEW);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gGiantBeardDL);
        }

        CLOSE_DISPS(play->state.gfxCtx);
    }
}
