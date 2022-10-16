/*
 * File: z_en_kakasi.c
 * Overlay: ovl_En_Kakasi
 * Description: Pierre the Scarecorw
 */

#include "prevent_bss_reordering.h"
#include "z_en_kakasi.h"
#include "objects/object_ka/object_ka.h"

#define FLAGS (ACTOR_FLAG_1 | ACTOR_FLAG_8 | ACTOR_FLAG_10 | ACTOR_FLAG_2000000)

#define THIS ((EnKakasi*)thisx)

void EnKakasi_Init(Actor* thisx, PlayState* play);
void EnKakasi_Destroy(Actor* thisx, PlayState* play);
void EnKakasi_Update(Actor* thisx, PlayState* play);
void EnKakasi_Draw(Actor* thisx, PlayState* play);

void EnKakasi_SetupPostSongLearnDialogue(EnKakasi* this, PlayState* play);
void EnKakasi_InitTimeSkipDialogue(EnKakasi* this);
void EnKakasi_SetupIdleStanding(EnKakasi* this);

s32 EnKakasi_ValidatePictograph(PlayState* play, Actor* thisx);

void EnKakasi_TimeSkipDialogue(EnKakasi* this, PlayState* play);
void EnKakasi_IdleStanding(EnKakasi* this, PlayState* play);
void EnKakasi_OcarinaRemark(EnKakasi* this, PlayState* play);
void EnKakasi_TeachingSong(EnKakasi* this, PlayState* play);
void EnKakasi_PostSongLearnTwirl(EnKakasi* this, PlayState* play);
void EnKakasi_PostSongLearnDialogue(EnKakasi* this, PlayState* play);
void EnKakasi_SetupDigAway(EnKakasi* this);
void EnKakasi_DiggingAway(EnKakasi* this, PlayState* play);
void EnKakasi_RisenDialogue(EnKakasi* this, PlayState* play);
void EnKakasi_SetupIdleUnderground(EnKakasi* this);
void EnKakasi_IdleUnderground(EnKakasi* this, PlayState* play);
void EnKakasi_SetupIdleRisen(EnKakasi* this);
void EnKakasi_IdleRisen(EnKakasi* this, PlayState* play);
void EnKakasi_SetupRiseOutOfGround(EnKakasi* this, PlayState* play);
void EnKakasi_RisingOutOfGround(EnKakasi* this, PlayState* play);
void EnKakasi_DancingRemark(EnKakasi* this, PlayState* play);
void EnKakasi_SetupDanceNightAway(EnKakasi* this);
void EnKakasi_DancingNightAway(EnKakasi* this, PlayState* play);
void EnKakasi_DoNothing(EnKakasi* this, PlayState* play);
void EnKakasi_RegularDialogue(EnKakasi* this, PlayState* play);
void EnKakasi_SetupSongTeach(EnKakasi* this, PlayState* play);

void EnKakasi_SetupDialogue(EnKakasi* this);

static ColliderCylinderInit D_80971D80 = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xF7CFFFFF, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        TOUCH_NONE | TOUCH_SFX_NORMAL,
        BUMP_ON | BUMP_HOOKABLE,
        OCELEM_ON,
    },
    { 20, 70, 0, { 0, 0, 0 } },
};

const ActorInit En_Kakasi_InitVars = {
    ACTOR_EN_KAKASI,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_KA,
    sizeof(EnKakasi),
    (ActorFunc)EnKakasi_Init,
    (ActorFunc)EnKakasi_Destroy,
    (ActorFunc)EnKakasi_Update,
    (ActorFunc)EnKakasi_Draw,
};

Vec3f D_80971DCC[] = {
    { 0.0f, 60.0f, 60.0f },   { 40.0f, 40.0f, 50.0f },   { -40.0f, 40.0f, 50.0f },
    { 40.0f, 20.0f, 110.0f }, { -40.0f, 20.0f, 110.0f }, { 0.0f, 80.0f, 60.0f },
    { 50.0f, 40.0f, -30.0f }, { -50.0f, 40.0f, -30.0f }, { 0.0f, 50.0f, 60.0f },
};
Vec3f D_80971E38[] = {
    { 0.0f, 55.0f, 60.0f },   { 0.0f, 55.0f, 60.0f },   { -80.0f, 35.0f, 15.0f }, { 80.0f, 45.0f, 15.0f },
    { -10.0f, 35.0f, 65.0f }, { 0.0f, 110.0f, 180.0f }, { 0.0f, 60.0f, 70.0f },   { 0.0f, 60.0f, 70.0f },
    { -80.0f, 50.0f, 40.0f }, { 0.0f, -10.0f, 50.0f },  { 80.0f, 50.0f, 40.0f },  { 0.0f, 120.0f, 40.0f },
    { 0.0f, 28.0f, -50.0f },  { 0.0f, 60.0f, 50.0f },   { 0.0f, 80.0f, 160.0f },
};

Vec3f D_80971EEC[] = {
    { 12.0f, 1.0f, 2.0f },    { 12.0f, 1.0f, 2.0f },  { 2.0f, -6.0f, 0.0f },  { 12.0f, -6.0f, -10.0f },
    { -88.0f, 14.0, -10.0f }, { 0.0f, 0.0f, 0.0f },   { 0.0f, 0.0f, 0.0f },   { 0.0f, 0.0f, 0.0f },
    { 0.0f, -10.0f, 0.0f },   { 0.0f, -10.0f, 0.0f }, { 0.0f, -10.0f, 0.0f }, { 0.0f, -10.0f, 0.0f },
    { 0.0f, -10.0f, 0.0f },   { 0.0f, 0.0f, 0.0f },   { 0.0f, 0.0f, 0.0f },
};

Vec3f D_80971FA0[] = {
    { -50.0f, 160.0f, -40.0f }, { -50.0f, 160.0f, -40.0f }, { -20.0f, 80.0f, 60.0f },
    { 57.0f, 60.0f, 120.0f },   { 27.0f, 80.0f, 60.0f },    { -3.0f, 50.0f, 70.0f },
};

Vec3f D_80971FE8[] = {
    { 0.0f, 0.0f, 0.0f },    { 0.0f, 0.0f, 0.0f },  { -10.0f, -5.0f, 0.0f },
    { 10.0f, -10.0f, 7.0f }, { 20.0f, 5.0f, 2.0f }, { 10.0f, 5.0f, 2.0f },
};

// both use in digging away function
Vec3f D_80972030 = { -3.0f, 50.0f, 90.0f };
Vec3f D_8097203C = { 10.0f, -15.0f, 2.0f };

typedef enum {
    /* 0x0 */ ENKAKASI_ANIM_ARMS_CROSSED_ROCKING, // arms crossed and rocking back and forth
    /* 0x1 */ ENKAKASI_ANIM_SIDEWAYS_SHAKING,
    /* 0x2 */ ENKAKASI_ANIM_HOPPING_REGULAR,    // bounce bounce bounce (repeat)
    /* 0x3 */ ENKAKASI_ANIM_SPIN_REACH_OFFER,   // spin and then reaching toward player
    /* 0x4 */ ENKAKASI_ANIM_TWIRL,              // slower cinimatic twirl
    /* 0x5 */ ENKAKASI_ANIM_ARMS_CROSSED_STILL, // arms crossed but still, also some noise sfx
    /* 0x6 */ ENKAKASI_ANIM_WAVE,               // "wave" short sideways shake, stops early, partial? unused?
    /* 0x7 */ ENKAKASI_ANIM_SLOWROLL,           // partial bounch, ends looking left, OFFER anim takes over
    /* 0x8 */ ENKAKASI_ANIM_IDLE,               // slow stretching wiggle, ends in regular position
} EnKakasiAnimation;

static AnimationHeader* sAnimations[] = {
    &object_ka_Anim_007444, &object_ka_Anim_00686C, &object_ka_Anim_0081A4,
    &object_ka_Anim_007B90, &object_ka_Anim_0071EC, &object_ka_Anim_007444,
    &object_ka_Anim_00686C, &object_ka_Anim_0081A4, &object_ka_Anim_000214,
};

static u8 sAnimationModes[] = {
    ANIMMODE_LOOP, ANIMMODE_LOOP, ANIMMODE_LOOP, ANIMMODE_ONCE, ANIMMODE_ONCE,
    ANIMMODE_ONCE, ANIMMODE_ONCE, ANIMMODE_ONCE, ANIMMODE_ONCE,
};

void EnKakasi_Destroy(Actor* thisx, PlayState* play) {
    EnKakasi* this = THIS;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnKakasi_Init(Actor* thisx, PlayState* play) {
    EnKakasi* this = THIS;
    s32 tempCutscene;
    s32 i;

    Collider_InitAndSetCylinder(play, &this->collider, &this->picto.actor, &D_80971D80);
    SkelAnime_InitFlex(play, &this->skelanime, &object_ka_Skel_0065B0, &object_ka_Anim_000214, 0, 0, 0);

    this->songSummonDist = KAKASI_GET_SUMMON_DISTANCE(&this->picto.actor) * 20.0f;
    if (this->songSummonDist < 40.0f) {
        this->songSummonDist = 40.0f;
    }

    this->unkHeight = (this->picto.actor.world.rot.z * 20.0f) + 60.0f;
    this->picto.actor.world.rot.z = 0;
    this->picto.actor.targetMode = 0;
    if (this->picto.actor.world.rot.x > 0 && this->picto.actor.world.rot.x < 8) {
        this->picto.actor.targetMode = KAKASI_GET_TARGETMODE(thisx);
    }
    this->picto.actor.shape.rot.y = this->picto.actor.world.rot.y;

    this->aboveGroundStatus = KAKASI_GET_ABOVE_GROUND(&this->picto.actor);
    this->picto.actor.world.rot.x = 0;
    this->picto.actor.flags |= ACTOR_FLAG_400;
    this->picto.actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_SetScale(&this->picto.actor, 0.01f);

    i = 0;
    tempCutscene = this->picto.actor.cutscene;
    while (tempCutscene != -1) {
        //! FAKE:
        tempCutscene = ActorCutscene_GetAdditionalCutscene(this->actorCutscenes[i] = tempCutscene);
        i++;
    }

    if (this->aboveGroundStatus != 0) {
        if (gSaveContext.save.weekEventReg[79] & 8) {
            this->aboveGroundStatus = ENKAKASI_ABOVE_GROUND_TYPE;
            this->songSummonDist = 80.0f;
            EnKakasi_SetupIdleUnderground(this);
        } else {
            Actor_SetFocus(&this->picto.actor, 60.0f);
            this->picto.validationFunc = EnKakasi_ValidatePictograph;
            if (gSaveContext.save.weekEventReg[83] & 1) {
                EnKakasi_InitTimeSkipDialogue(this);
            } else {
                EnKakasi_SetupIdleStanding(this);
            }
        }
    } else {
        EnKakasi_SetupIdleUnderground(this);
    }
}

void EnKakasi_ChangeAnim(EnKakasi* this, s32 animIndex) {
    this->animIndex = animIndex;
    this->animeFrameCount = Animation_GetLastFrame(&sAnimations[this->animIndex]->common);
    // 1: regular playback speed, 0: starting frame
    Animation_Change(&this->skelanime, sAnimations[this->animIndex], 1.0f, 0.0f, this->animeFrameCount,
                     sAnimationModes[this->animIndex], -4.0f);
}

s32 EnKakasi_ValidatePictograph(PlayState* play, Actor* thisx) {
    return Snap_ValidatePictograph(play, thisx, 0x7, &thisx->focus.pos, &thisx->shape.rot, 280.0f, 1800.0f, -1);
}

void EnKakasi_CheckAnimationSfx(EnKakasi* this) {
    if (this->animIndex == ENKAKASI_ANIM_SIDEWAYS_SHAKING || this->animIndex == ENKAKASI_ANIM_ARMS_CROSSED_STILL) {
        if (Animation_OnFrame(&this->skelanime, 1.0f) || Animation_OnFrame(&this->skelanime, 8.0f)) {
            Actor_PlaySfxAtPos(&this->picto.actor, NA_SE_EV_KAKASHI_SWING);
        }
    }
    if (this->animIndex == ENKAKASI_ANIM_HOPPING_REGULAR || this->animIndex == ENKAKASI_ANIM_SLOWROLL) {
        if (Animation_OnFrame(&this->skelanime, 4.0f) || Animation_OnFrame(&this->skelanime, 8.0f)) {
            Actor_PlaySfxAtPos(&this->picto.actor, NA_SE_EV_KAKASHI_SWING);
        }
        if (Animation_OnFrame(&this->skelanime, 1.0f) || Animation_OnFrame(&this->skelanime, 9.0f) ||
            Animation_OnFrame(&this->skelanime, 16.0f)) {
            Actor_PlaySfxAtPos(&this->picto.actor, NA_SE_IT_KAKASHI_JUMP);
        }
        if (Animation_OnFrame(&this->skelanime, 18.0f)) {
            Actor_PlaySfxAtPos(&this->picto.actor, NA_SE_EV_KAKASHI_ROLL);
        }
    }
    if (this->animIndex == ENKAKASI_ANIM_SPIN_REACH_OFFER || this->animIndex == ENKAKASI_ANIM_TWIRL) {
        if (Animation_OnFrame(&this->skelanime, 1.0f)) {
            Actor_PlaySfxAtPos(&this->picto.actor, NA_SE_EV_KAKASH_LONGI_ROLL);
        }
    }
}

/*
 * moves the player's position relative to scarecrow during song teach, also each frame of dance the night away
 */
void EnKakasi_CheckPlayerPosition(EnKakasi* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (play->sceneId == SCENE_8ITEMSHOP) {
        player->actor.world.pos.x = -50.0f;
        player->actor.world.pos.z = 155.0f;
    } else if (play->sceneId == SCENE_TENMON_DAI) {
        player->actor.world.pos.x = 60.0f;
        player->actor.world.pos.z = -190.0f;
    }

    Math_SmoothStepToS(&player->actor.shape.rot.y, (this->picto.actor.yawTowardsPlayer + 0x8000), 5, 1000, 0);
}

/*
 * this goes off every frame of dancing the night away, and song teaching
 * something to do with cutscene camera?
 */
void func_8096FAAC(EnKakasi* this, PlayState* play) {
    if (this->subCamId != SUB_CAM_ID_DONE) {
        Math_ApproachF(&this->subCamEye.x, this->subCamEyeNext.x, 0.4f, 4.0f);
        Math_ApproachF(&this->subCamEye.y, this->subCamEyeNext.y, 0.4f, 4.0f);
        Math_ApproachF(&this->subCamEye.z, this->subCamEyeNext.z, 0.4f, 4.0f);

        Math_ApproachF(&this->subCamAt.x, this->subCamAtNext.x, 0.4f, 4.0f);
        Math_ApproachF(&this->subCamAt.y, this->subCamAtNext.y, 0.4f, 4.0f);
        Math_ApproachF(&this->subCamAt.z, this->subCamAtNext.z, 0.4f, 4.0f);

        Math_ApproachF(&this->subCamFov, this->subCamFovNext, 0.3f, 10.0f);

        Play_SetCameraAtEye(play, this->subCamId, &this->subCamAt, &this->subCamEye);
        Play_SetCameraFov(play, this->subCamId, this->subCamFov);
    }
}

/*
 * goes off every frame of song teach, but... doing what?
 */
void func_8096FBB8(EnKakasi* this, PlayState* play) {
    if (play->msgCtx.unk12048 == 0 || play->msgCtx.unk12048 == 1 || play->msgCtx.unk12048 == 2 ||
        play->msgCtx.unk12048 == 3 || play->msgCtx.unk12048 == 4) {
        // why not 0 < x < 4? fewer branches
        this->unk190++;
    }
    if (this->unk190 != 0 && this->animIndex != ENKAKASI_ANIM_SIDEWAYS_SHAKING) {
        EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_SIDEWAYS_SHAKING);
    }
    if (this->unk190 > 8) {
        this->unk190 = 8;
    }
    if (this->unk190 != 0) {
        Math_ApproachF(&this->skelanime.playSpeed, 1.0f, 0.1f, 0.2f);
        this->picto.actor.shape.rot.y += 0x800;
    }
}

void EnKakasi_InitTimeSkipDialogue(EnKakasi* this) {
    EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_SLOWROLL);
    this->actionFunc = EnKakasi_TimeSkipDialogue;
}

void EnKakasi_TimeSkipDialogue(EnKakasi* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (gSaveContext.respawnFlag != -4 && gSaveContext.respawnFlag != -8) {
        if (gSaveContext.save.time != CLOCK_TIME(6, 0) && gSaveContext.save.time != CLOCK_TIME(18, 0) &&
            !(gSaveContext.eventInf[1] & 0x80)) {

            if (this->picto.actor.textId == 0) {
                // dialogue after skipped time 'did you feel that? went by in an instant'
                this->picto.actor.textId = 0x1653;
                gSaveContext.save.weekEventReg[83] &= (u8)~1;
                this->talkState = TEXT_STATE_5;
                player->stateFlags1 |= PLAYER_STATE1_20;
                this->picto.actor.flags |= ACTOR_FLAG_10000;
            }

            if (Actor_ProcessTalkRequest(&this->picto.actor, &play->state)) {
                player->stateFlags1 &= ~PLAYER_STATE1_20;
                this->unkState196 = 2;
                this->picto.actor.flags &= ~ACTOR_FLAG_10000;
                this->actionFunc = EnKakasi_RegularDialogue;
            } else {
                func_800B8500(&this->picto.actor, play, 9999.9f, 9999.9f, PLAYER_AP_MINUS1);
            }
        }
    }
}

void EnKakasi_SetupIdleStanding(EnKakasi* this) {
    this->unkState196 = 0;
    this->actionFunc = EnKakasi_IdleStanding;
}

void EnKakasi_IdleStanding(EnKakasi* this, PlayState* play) {
    u32 saveContextDay = gSaveContext.save.day;
    s16 x;
    s16 y;

    // first talk to scarecrow dialogue
    this->picto.actor.textId = 0x1644;
    if (func_800B8718(&this->picto.actor, &play->state)) {
        this->skelanime.playSpeed = 1.0f;
        EnKakasi_SetupSongTeach(this, play);
        return;
    }
    if (Actor_ProcessTalkRequest(&this->picto.actor, &play->state)) {
        this->skelanime.playSpeed = 1.0f;
        EnKakasi_SetupDialogue(this);
        return;
    }
    if (play->actorCtx.flags & ACTORCTX_FLAG_2) {
        Actor_GetScreenPos(play, &this->picto.actor, &x, &y);
        if (this->picto.actor.projectedPos.z > -20.0f && x > 0 && x < SCREEN_WIDTH && y > 0 && y < SCREEN_HEIGHT &&
            this->animIndex != ENKAKASI_ANIM_SIDEWAYS_SHAKING) {
            // faster shaking
            EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_SIDEWAYS_SHAKING);
            this->skelanime.playSpeed = 2.0f;
        }
    } else if (Player_GetMask(play) == PLAYER_MASK_KAMARO) {
        if (this->animIndex != ENKAKASI_ANIM_SIDEWAYS_SHAKING) {
            EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_SIDEWAYS_SHAKING);
            this->skelanime.playSpeed = 2.0f;
        }
    } else if (saveContextDay == 3 && gSaveContext.save.isNight) {
        this->skelanime.playSpeed = 1.0f;
        if (this->animIndex != ENKAKASI_ANIM_SIDEWAYS_SHAKING) {
            EnKakasi_ChangeAnim(this, 1);
        }
    } else if (this->animIndex != ENKAKASI_ANIM_IDLE) {
        EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_IDLE);
    }
    if (this->picto.actor.xzDistToPlayer < 120.0f) {
        func_800B8614(&this->picto.actor, play, 100.0f);
        func_800B874C(&this->picto.actor, play, 100.0f, 80.0f);
    }
}

void EnKakasi_SetupDialogue(EnKakasi* this) {
    // bug? starts one animation then changes it a few lines down?
    if (this->animIndex != ENKAKASI_ANIM_SIDEWAYS_SHAKING) {
        EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_SIDEWAYS_SHAKING);
    }

    this->talkState = TEXT_STATE_5;
    this->unkState196 = 1;
    EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_SPIN_REACH_OFFER);
    this->actionFunc = EnKakasi_RegularDialogue;
}

void EnKakasi_RegularDialogue(EnKakasi* this, PlayState* play) {
    u32 saveContextDay = gSaveContext.save.day;
    f32 currentAnimeFrame = this->skelanime.curFrame;

    Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.yawTowardsPlayer, 5, 2000, 0);
    // if first dialogue
    if (this->picto.actor.textId != 0x1644 && this->animeFrameCount <= currentAnimeFrame &&
        this->animIndex == ENKAKASI_ANIM_SLOWROLL) {

        EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_SPIN_REACH_OFFER);
        this->unkCounter1A4 = 0;
    }

    // if dialogue: oh sorry come back again
    if (this->picto.actor.textId == 0x1651 || this->picto.actor.textId == 0x1659) {
        if (this->animeFrameCount <= currentAnimeFrame && this->animIndex != ENKAKASI_ANIM_SPIN_REACH_OFFER) {
            if (++this->unkCounter1A4 >= 2) {
                this->unkCounter1A4 = 0;
                EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_SPIN_REACH_OFFER);
            }
        }
    }

    if (this->unkState1A8 == 2 && this->unkState196 == 2) {
        func_800B7298(play, &this->picto.actor, 0x49);
        this->unkState1A8 = 0;
    }

    if ((this->talkState == Message_GetState(&play->msgCtx)) && Message_ShouldAdvance(play)) {
        func_801477B4(play);
        if (this->talkState == TEXT_STATE_5) {
            // bad song input
            if (this->unkState196 == 2 && this->picto.actor.textId == 0x1647) {
                func_800B7298(play, &this->picto.actor, 6);
            }

            // after timeskip
            if (this->picto.actor.textId == 0x1653) {
                u32 saveContextDay2 = gSaveContext.save.day;

                if (this->animIndex != ENKAKASI_ANIM_SIDEWAYS_SHAKING) {
                    EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_SIDEWAYS_SHAKING);
                }

                if (saveContextDay2 == 3 && gSaveContext.save.isNight) {
                    // text: dangerous outside
                    this->picto.actor.textId = 0x164F;
                } else if (gSaveContext.save.isNight) {
                    // text: would you like to skip time?
                    this->picto.actor.textId = 0x164E;
                } else {
                    // text: would you like to skip time? 2
                    this->picto.actor.textId = 0x1645;
                }

                func_80151938(play, this->picto.actor.textId);
                return;

            } else if (this->picto.actor.textId == 0x165D || this->picto.actor.textId == 0x165F ||
                       this->picto.actor.textId == 0x1660 || this->picto.actor.textId == 0x1652) {
                func_800B7298(play, &this->picto.actor, 4);
                if (ActorCutscene_GetCurrentIndex() == 0x7C) {
                    ActorCutscene_Stop(0x7C);
                    ActorCutscene_SetIntentToPlay(this->actorCutscenes[0]);
                    this->actionFunc = EnKakasi_DancingRemark;
                } else {
                    if (!ActorCutscene_GetCanPlayNext(this->actorCutscenes[0])) {
                        ActorCutscene_SetIntentToPlay(this->actorCutscenes[0]);
                        this->actionFunc = EnKakasi_DancingRemark;
                    } else {
                        ActorCutscene_StartAndSetUnkLinkFields(this->actorCutscenes[0], &this->picto.actor);
                        this->subCamId = ActorCutscene_GetCurrentSubCamId(this->picto.actor.cutscene);
                        this->actionFunc = EnKakasi_DancingRemark;
                    }
                }
                return;

            } else if (this->picto.actor.textId == 0x1645 || this->picto.actor.textId == 0x164E) {
                this->picto.actor.textId = 0x1650; // "Shall we dance?  No Yes"
                if (this->animIndex != ENKAKASI_ANIM_SIDEWAYS_SHAKING) {
                    EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_SIDEWAYS_SHAKING);
                }
                this->talkState = TEXT_STATE_CHOICE;

            } else if (this->picto.actor.textId == 0x1644) {
                if (this->animIndex != ENKAKASI_ANIM_SIDEWAYS_SHAKING) {
                    EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_SIDEWAYS_SHAKING);
                }
                if (gSaveContext.save.isNight) {
                    this->picto.actor.textId = 0x164E;
                } else {
                    this->picto.actor.textId = 0x1645;
                }

            } else if (this->picto.actor.textId == 0x164F) {
                if (this->animIndex != ENKAKASI_ANIM_SIDEWAYS_SHAKING) {
                    EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_SIDEWAYS_SHAKING);
                }
                this->picto.actor.textId = 0x165A;
            } else if (this->picto.actor.textId == 0x1651) {
                if (this->animIndex != ENKAKASI_ANIM_SIDEWAYS_SHAKING) {
                    EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_SIDEWAYS_SHAKING);
                }
                this->picto.actor.textId = 0x1654;
            } else if (this->picto.actor.textId == 0x1654) {
                this->picto.actor.textId = 0x1655;
            } else if (this->picto.actor.textId == 0x1655) {
                this->picto.actor.textId = 0x1656;
                this->talkState = TEXT_STATE_CHOICE;
            } else if (this->picto.actor.textId == 0x1658) {
                this->picto.actor.textId = 0x1659;
            } else if (this->picto.actor.textId == 0x165A) {
                this->picto.actor.textId = 0x165B;
            } else if (this->picto.actor.textId == 0x165B) {
                this->picto.actor.textId = 0x165C;
                this->talkState = TEXT_STATE_CHOICE;

            } else if (this->picto.actor.textId == 0x165E) {
                this->picto.actor.textId = 0x165F;
            } else {
                EnKakasi_SetupIdleStanding(this);
                return;
            }
        } else {
            this->talkState = TEXT_STATE_5;

            if (play->msgCtx.choiceIndex == 1) {
                func_8019F208();
                if (this->picto.actor.textId == 0x1656) {
                    this->picto.actor.textId = 0x1658;
                } else if (this->picto.actor.textId == 0x165C) {
                    this->picto.actor.textId = 0x165E;
                } else if (saveContextDay == 3 && gSaveContext.save.isNight) {
                    this->picto.actor.textId = 0x164F;
                } else {
                    this->picto.actor.textId = 0x1652;
                }
                EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_HOPPING_REGULAR);
            } else {
                func_8019F230();
                if (this->picto.actor.textId == 0x1656) { // would you like to learn a song? yes/no
                    this->picto.actor.textId = 0x1657;
                } else if (this->picto.actor.textId == 0x165C) { // would you like to learn a song? yes/no
                    this->picto.actor.textId = 0x165D;
                } else {
                    this->picto.actor.textId = 0x1651;
                }
                this->unkCounter1A4 = 0;
                if (this->animIndex != ENKAKASI_ANIM_ARMS_CROSSED_ROCKING) {
                    EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_ARMS_CROSSED_ROCKING);
                }
            }
        }
        func_80151938(play, this->picto.actor.textId);
    }
}

void EnKakasi_SetupSongTeach(EnKakasi* this, PlayState* play) {
    this->picto.actor.textId = 0x1646;
    Message_StartTextbox(play, this->picto.actor.textId, &this->picto.actor);
    this->subCamId = SUB_CAM_ID_DONE;
    this->subCamFov = 0.0f;
    this->subCamFovNext = 60.0f;
    EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_TWIRL);
    this->unkState196 = 2;
    this->actionFunc = EnKakasi_OcarinaRemark;
}

/*
 * you took out ocarina hes talking about how he wants to hear you play him a song
 * before actually teaching
 */
void EnKakasi_OcarinaRemark(EnKakasi* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_5) && Message_ShouldAdvance(play)) {
        func_80152434(play, 0x35);
        this->unkState1A8 = 0;
        if (ActorCutscene_GetCurrentIndex() == 0x7C) {
            ActorCutscene_Stop(0x7C);
            ActorCutscene_SetIntentToPlay(this->actorCutscenes[0]);
            this->actionFunc = EnKakasi_TeachingSong;

        } else if (ActorCutscene_GetCanPlayNext(this->actorCutscenes[0]) == 0) {
            ActorCutscene_SetIntentToPlay(this->actorCutscenes[0]);
            this->actionFunc = EnKakasi_TeachingSong;

        } else {
            this->unkState1A8 = 1;
            ActorCutscene_StartAndSetUnkLinkFields(this->actorCutscenes[0], &this->picto.actor);
            this->subCamId = ActorCutscene_GetCurrentSubCamId(this->picto.actor.cutscene);
            Math_Vec3f_Copy(&this->unk22C, &this->picto.actor.home.pos);
            this->actionFunc = EnKakasi_TeachingSong;
        }
    }
}

void EnKakasi_TeachingSong(EnKakasi* this, PlayState* play) {
    Vec3f tempVec;

    EnKakasi_CheckPlayerPosition(this, play);
    Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.home.rot.y, 1, 3000, 0);
    if (this->unkState1A8 == 0) {
        if (ActorCutscene_GetCurrentIndex() == 0x7C) {
            ActorCutscene_Stop(0x7C);
            ActorCutscene_SetIntentToPlay(this->actorCutscenes[0]);
            return;
        }
        if (ActorCutscene_GetCanPlayNext(this->actorCutscenes[0]) == 0) {
            ActorCutscene_SetIntentToPlay(this->actorCutscenes[0]);
            return;
        }
        ActorCutscene_StartAndSetUnkLinkFields(this->actorCutscenes[0], &this->picto.actor);
        this->subCamId = ActorCutscene_GetCurrentSubCamId(this->picto.actor.cutscene);
        Math_Vec3f_Copy(&this->unk22C, &this->picto.actor.home.pos);
        this->unkState1A8 = 1;
        this->unkState1A8 = 1;
    }

    if (this->unkState1A8 == 1) {
        this->unk22C.y = this->picto.actor.home.pos.y + 50.0f;
        this->subCamEyeNext.x = D_80971DCC[this->unk190].x;
        this->subCamEyeNext.y = D_80971DCC[this->unk190].y;
        this->subCamEyeNext.z = D_80971DCC[this->unk190].z;

        Math_Vec3f_Copy(&tempVec, &this->subCamEyeNext);
        OLib_DbCameraVec3fSum(&this->picto.actor.home, &tempVec, &this->subCamEyeNext, 1);
        Math_Vec3f_Copy(&this->subCamAtNext, &this->unk22C);
        Math_Vec3f_Copy(&this->subCamEye, &this->subCamEyeNext);
        Math_Vec3f_Copy(&this->subCamAt, &this->subCamAtNext);
        func_8096FAAC(this, play);
        func_8096FBB8(this, play);

        if (play->msgCtx.ocarinaMode == 4) { // song failed
            this->unk190 = 0;
            this->unkCounter1A4 = 0;
            ActorCutscene_Stop(this->actorCutscenes[0]);
            Actor_PlaySfxAtPos(&this->picto.actor, NA_SE_EN_YASE_DEAD);
            if (this) {}
            this->unkState196 = 2;
            this->subCamId = SUB_CAM_ID_DONE;
            this->picto.actor.textId = 0x1647;
            this->unkState1A8 = 2;
            this->talkState = TEXT_STATE_5;
            EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_ARMS_CROSSED_ROCKING);
            this->actionFunc = EnKakasi_RegularDialogue;

        } else if (play->msgCtx.ocarinaMode == 3) { // song success
            this->postTeachTimer = 30;
            this->skelanime.playSpeed = 2.0f;
            EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_HOPPING_REGULAR);
            this->actionFunc = EnKakasi_PostSongLearnTwirl;
        }
    }
}

void EnKakasi_PostSongLearnTwirl(EnKakasi* this, PlayState* play) {
    f32 animeFrame = this->skelanime.curFrame;

    if (this->postTeachTimer == 0 && this->animIndex != ENKAKASI_ANIM_TWIRL) {
        EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_TWIRL);
        this->skelanime.playSpeed = 2.0f;
    }
    if (this->postTeachTimer == 0 && this->animIndex == ENKAKASI_ANIM_TWIRL && this->animeFrameCount <= animeFrame) {
        EnKakasi_SetupPostSongLearnDialogue(this, play);
    }
}

void EnKakasi_SetupPostSongLearnDialogue(EnKakasi* this, PlayState* play) {
    ActorCutscene_Stop(this->actorCutscenes[0]);
    play->msgCtx.ocarinaMode = 4;
    this->unk190 = 0;
    this->unkCounter1A4 = 0;
    EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_HOPPING_REGULAR);
    this->subCamId = SUB_CAM_ID_DONE;
    this->talkState = TEXT_STATE_5;
    this->unkState1A8 = 1;
    this->actionFunc = EnKakasi_PostSongLearnDialogue;
    this->subCamFov = 0.0f;
    this->subCamFovNext = 60.0f;
}

void EnKakasi_PostSongLearnDialogue(EnKakasi* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 tempAnimFrame = this->skelanime.curFrame;
    Vec3f vec3fCopy;

    Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.home.rot.y, 1, 3000, 0);
    Math_SmoothStepToS(&player->actor.shape.rot.y, this->picto.actor.yawTowardsPlayer + 0x8000, 5, 1000, 0);

    if (this->unk190 == 0) {
        func_801477B4(play);
        func_800B7298(play, &this->picto.actor, 0x56);
        this->picto.actor.textId = 0x1648;
        Message_StartTextbox(play, (this->picto.actor.textId), &this->picto.actor);
        this->unkState1A8 = 0;
        this->unk190 = 1;
    }

    if (this->picto.actor.textId == 0x1648 && this->animIndex == ENKAKASI_ANIM_HOPPING_REGULAR &&
        this->animeFrameCount <= tempAnimFrame) {
        this->unkCounter1A4++;
        if (this->unkCounter1A4 >= 2) {
            EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_ARMS_CROSSED_ROCKING);
        }
    }

    if (this->picto.actor.textId == 0x164B && this->animIndex == ENKAKASI_ANIM_ARMS_CROSSED_ROCKING &&
        this->animeFrameCount <= tempAnimFrame) {
        this->unkCounter1A4++;
        if (this->unkCounter1A4 >= 2) {
            EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_SPIN_REACH_OFFER);
        }
    }

    if (this->unkState1A8 == 0) {
        if (ActorCutscene_GetCurrentIndex() == 0x7C) {
            ActorCutscene_Stop(0x7C);
            ActorCutscene_SetIntentToPlay(this->actorCutscenes[0]);
            return;
        }
        if (ActorCutscene_GetCanPlayNext(this->actorCutscenes[0]) == 0) {
            ActorCutscene_SetIntentToPlay(this->actorCutscenes[0]);
            return;
        }
        Math_Vec3f_Copy(&this->unk22C, &this->picto.actor.home.pos);
        ActorCutscene_StartAndSetUnkLinkFields(this->actorCutscenes[0], &this->picto.actor);
        this->subCamId = ActorCutscene_GetCurrentSubCamId(this->picto.actor.cutscene);
        func_800B7298(play, &this->picto.actor, 0x56);
        this->unkState1A8 = 1;
    }

    if (this->subCamId != SUB_CAM_ID_DONE) {
        this->unk22C.y = this->picto.actor.home.pos.y + 50.0f;
        EnKakasi_CheckPlayerPosition(this, play);
        this->subCamEyeNext.x = D_80971FA0[this->unk190].x;
        this->subCamEyeNext.y = D_80971FA0[this->unk190].y;
        this->subCamEyeNext.z = D_80971FA0[this->unk190].z;
        Math_Vec3f_Copy(&vec3fCopy, &this->subCamEyeNext);
        OLib_DbCameraVec3fSum(&this->picto.actor.home, &vec3fCopy, &this->subCamEyeNext, 1);
        this->subCamAtNext.x = D_80971FE8[this->unk190].x + this->unk22C.x;
        this->subCamAtNext.y = D_80971FE8[this->unk190].y + this->unk22C.y;
        this->subCamAtNext.z = D_80971FE8[this->unk190].z + this->unk22C.z;
        Math_Vec3f_Copy(&this->subCamEye, &this->subCamEyeNext);
        Math_Vec3f_Copy(&this->subCamAt, &this->subCamAtNext);
    }

    func_8096FAAC(this, play);

    if ((this->unkState1A8 != 0) && (Message_GetState(&play->msgCtx) == this->talkState) &&
        Message_ShouldAdvance(play)) {

        func_801477B4(play);

        if (this->talkState == TEXT_STATE_5) {
            this->unk190++;
            if (this->unk190 > 5) {
                this->unk190 = 5;
            }

            if (this->picto.actor.textId == 0x1648) {
                func_800B7298(play, &this->picto.actor, 7);
                this->picto.actor.textId = 0x1649;
                if (this->animIndex != ENKAKASI_ANIM_ARMS_CROSSED_ROCKING) {
                    EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_ARMS_CROSSED_ROCKING);
                }

            } else if (this->picto.actor.textId == 0x1649) {
                this->picto.actor.textId = 0x1660;
                this->talkState = TEXT_STATE_CHOICE;

            } else if (this->picto.actor.textId == 0x164A) {
                this->picto.actor.textId = 0x164B;

            } else if (this->picto.actor.textId == 0x164B) {
                this->picto.actor.textId = 0x164C;
                EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_TWIRL);

            } else {
                if (this->picto.actor.textId == 0x164C || this->picto.actor.textId == 0x1661) {
                    EnKakasi_SetupDigAway(this);
                    return;
                }
            }

        } else {
            this->talkState = TEXT_STATE_5;
            if (play->msgCtx.choiceIndex == 1) {
                func_8019F208(); // play 0x4808 sfx (decide) and calls AudioSfx_StopById
                this->picto.actor.textId = 0x164A;
            } else {
                func_8019F230(); // play 0x480A sfx (cancel) and calls AudioSfx_StopById
                this->picto.actor.textId = 0x1661;
            }
        }
        func_80151938(play, this->picto.actor.textId);
    }
}

/*
 * talking before dancing the night away, and cutscene setup
 */
void EnKakasi_DancingRemark(EnKakasi* this, PlayState* play) {
    u32 currentDay = gSaveContext.save.day;

    this->unkState196 = 3;
    if (ActorCutscene_GetCurrentIndex() == 0x7C) {
        ActorCutscene_Stop(0x7C);
        ActorCutscene_SetIntentToPlay(this->actorCutscenes[0]);
    } else if (ActorCutscene_GetCanPlayNext(this->actorCutscenes[0]) == 0) {
        ActorCutscene_SetIntentToPlay(this->actorCutscenes[0]);
    } else {
        ActorCutscene_StartAndSetUnkLinkFields(this->actorCutscenes[0], &this->picto.actor);
        this->subCamId = ActorCutscene_GetCurrentSubCamId(this->picto.actor.cutscene);
        if (currentDay == 3 && gSaveContext.save.isNight) {
            EnKakasi_SetupDigAway(this);
        } else {
            func_801A2BB8(NA_BGM_SARIAS_SONG);
            EnKakasi_SetupDanceNightAway(this);
        }
    }
}

void EnKakasi_SetupDanceNightAway(EnKakasi* this) {
    this->unk190 = 0;
    this->unkCounter1A4 = 0;
    this->subCamFov = 0.0f;
    this->subCamFovNext = 60.0f;
    EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_TWIRL);
    Math_Vec3f_Copy(&this->unk22C, &this->picto.actor.home.pos);
    func_8016566C(0xB4);
    this->actionFunc = EnKakasi_DancingNightAway;
}

void EnKakasi_DancingNightAway(EnKakasi* this, PlayState* play) {
    f32 currentFrame;
    Vec3f localVec3f;
    Player* player;

    currentFrame = this->skelanime.curFrame;
    EnKakasi_CheckPlayerPosition(this, play);
    Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.home.rot.y, 1, 3000, 0);
    this->unk22C.y = this->picto.actor.home.pos.y + 50.0f;

    this->subCamEyeNext.x = D_80971E38[this->unk190].x;
    this->subCamEyeNext.y = D_80971E38[this->unk190].y;
    this->subCamEyeNext.z = D_80971E38[this->unk190].z;
    Math_Vec3f_Copy(&localVec3f, &this->subCamEyeNext);
    OLib_DbCameraVec3fSum(&this->picto.actor.home, &localVec3f, &this->subCamEyeNext, 1);

    if (1) {}
    this->subCamAtNext.x = D_80971EEC[this->unk190].x + this->unk22C.x;
    this->subCamAtNext.y = D_80971EEC[this->unk190].y + this->unk22C.y;
    this->subCamAtNext.z = D_80971EEC[this->unk190].z + this->unk22C.z;
    if (this->unk190 != 6 && this->unk190 != 7) {
        Math_Vec3f_Copy(&this->subCamEye, &this->subCamEyeNext);
        Math_Vec3f_Copy(&this->subCamAt, &this->subCamAtNext);
    }
    if (this->unk190 >= 7 && this->unk190 != 0xE) {
        this->picto.actor.shape.rot.y += 0x800;
    }

    func_8096FAAC(this, play);

    // switching through manual cutscene states
    switch (this->unk190) {
        case 0:
            // twirling
            this->unk204 = 0x28;
            this->unk190++;
            return;
        case 1:
            // end of twirl, start shaking back and forth
            if (this->unk204 == 0 && this->animeFrameCount <= currentFrame) {
                this->unk204 = 0x14;
                this->unk190++;
                EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_SIDEWAYS_SHAKING);
            }
            return;
        case 2:
            // camera view on the left
            if (this->unk204 == 0) {
                this->unk204 = 0x14;
                this->unk190++;
            }
            return;
        case 3:
            // camera view on the right
            if (this->unk204 == 0) {
                this->unk204 = 0x14;
                this->unk190++;
            }
            return;
        case 4:
            // camera view close up front
            if (this->unk204 == 0) {
                this->unk204 = 0x14;
                this->unk190++;
            }
            return;
        case 5:
            // camera view zoomed out front
            if (this->unk204 == 0) {
                this->unk204 = 0xF;
                this->unk190++;
            }
            return;
        case 6:
            // zoom in and sweep down in from case 5
            if (this->unk204 == 0) {
                this->unk204 = 0xF;
                this->unk190++;
            }
            return;
        case 7:
            // zoom in close from case 6
            if (this->unk204 == 0) {
                this->unk204 = 0xA;
                this->unk190++;
            }
            return;

        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
            // multiple quick angle cuts viewing from multiple sides
            if (this->unk204 == 0) {
                this->unk190++;
                this->unk204 = 0xA;
                if (this->unk190 == 0xE) {
                    func_800B7298(play, &this->picto.actor, 0x49);
                    func_80165690();
                    this->unk204 = 0x14;
                }
            }
            return;
        case 14:
            // goes off once for some camera changes,
            // otherwise it's the end when camera is back to normal and Player is confused
            Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.yawTowardsPlayer, 5, 1000, 0);
            if (this->unk204 == 0) {
                player = GET_PLAYER(play);

                Play_SetRespawnData(&play->state, RESPAWN_MODE_DOWN, Entrance_CreateFromSpawn(0), player->unk_3CE,
                                    0xBFF, &player->unk_3C0, player->unk_3CC);
                func_80169EFC(&play->state);

                if (0) {}
                if (gSaveContext.save.time > CLOCK_TIME(18, 0) || gSaveContext.save.time < CLOCK_TIME(6, 0)) {
                    gSaveContext.save.time = CLOCK_TIME(6, 0);
                    gSaveContext.respawnFlag = -4;
                    gSaveContext.eventInf[2] |= 0x80;
                } else {
                    gSaveContext.save.time = CLOCK_TIME(18, 0);
                    gSaveContext.respawnFlag = -8;
                }
                gSaveContext.save.weekEventReg[83] |= 1;
                this->unk190 = 0;
                this->actionFunc = EnKakasi_DoNothing;
            }
    }
}

void EnKakasi_DoNothing(EnKakasi* this, PlayState* play) {
}

void EnKakasi_SetupDigAway(EnKakasi* this) {
    if (this->animIndex != ENKAKASI_ANIM_SIDEWAYS_SHAKING) {
        EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_SIDEWAYS_SHAKING);
    }
    this->unk190 = 0;
    this->unkCounter1A4 = 0;
    this->subCamFovNext = 60.0f;
    this->subCamFov = 60.0f;
    Math_Vec3f_Copy(&this->unk22C, &this->picto.actor.home.pos);
    this->unkState196 = 4;
    this->actionFunc = EnKakasi_DiggingAway;
}

void EnKakasi_DiggingAway(EnKakasi* this, PlayState* play) {
    Vec3f tempunk238;
    Vec3f tempWorldPos;

    if (this->subCamId != SUB_CAM_ID_DONE) {
        this->unk22C.y = this->picto.actor.home.pos.y + 50.0f;
        this->subCamEyeNext.x = D_80972030.x;
        this->subCamEyeNext.y = D_80972030.y;
        this->subCamEyeNext.z = D_80972030.z;

        Math_Vec3f_Copy(&tempunk238, &this->subCamEyeNext);
        OLib_DbCameraVec3fSum(&this->picto.actor.home, &tempunk238, &this->subCamEyeNext, 1);
        this->subCamAtNext.x = ((f32)D_8097203C.x) + this->unk22C.x; // cast req
        this->subCamAtNext.y = ((f32)D_8097203C.y) + this->unk22C.y;
        this->subCamAtNext.z = ((f32)D_8097203C.z) + this->unk22C.z;
        Math_Vec3f_Copy(&this->subCamEye, &this->subCamEyeNext);
        Math_Vec3f_Copy(&this->subCamAt, &this->subCamAtNext);
        func_8096FAAC(this, play);
    }

    if (this->unkCounter1A4 < 15) {
        this->unkCounter1A4++;
        return;
    }

    this->picto.actor.shape.rot.y += 0x3000;
    Math_SmoothStepToS(&this->unk190, 500, 5, 50, 0);
    if ((play->gameplayFrames % 4) == 0) {
        Math_Vec3f_Copy(&tempWorldPos, &this->picto.actor.world.pos);
        tempWorldPos.y = this->picto.actor.floorHeight;
        tempWorldPos.x += randPlusMinusPoint5Scaled(2.0f);
        tempWorldPos.z += randPlusMinusPoint5Scaled(2.0f);

        if (play->sceneId == SCENE_8ITEMSHOP) {
            EffectSsGSplash_Spawn(play, &tempWorldPos, 0, 0, 0, randPlusMinusPoint5Scaled(100.0f) + 200.0f);
            SoundSource_PlaySfxAtFixedWorldPos(play, &tempWorldPos, 0x32, NA_SE_EV_BOMB_DROP_WATER);

        } else {
            Actor_SpawnFloorDustRing(play, &this->picto.actor, &this->picto.actor.world.pos,
                                     this->picto.actor.shape.shadowScale - 20.0f, 5, 4.0f, 200, 10, 1);
            Actor_PlaySfxAtPos(&this->picto.actor, NA_SE_EN_AKINDONUTS_HIDE);
        }
    }

    Math_ApproachF(&this->picto.actor.shape.yOffset, -6000.0f, 0.5f, 200.0f);
    if (fabsf(this->picto.actor.shape.yOffset + 6000.0f) < 10.0f) {
        gSaveContext.save.weekEventReg[79] |= 8;
        func_800B7298(play, &this->picto.actor, 6);
        ActorCutscene_Stop(this->actorCutscenes[0]);
        this->aboveGroundStatus = ENKAKASI_ABOVE_GROUND_TYPE;
        this->songSummonDist = 80.0f;
        EnKakasi_SetupIdleUnderground(this);
    }
}

void EnKakasi_SetupIdleUnderground(EnKakasi* this) {
    this->picto.actor.shape.yOffset = -7000.0;
    this->picto.actor.draw = NULL;
    this->picto.actor.flags |= ACTOR_FLAG_8000000;
    this->unkState196 = 5;
    this->actionFunc = EnKakasi_IdleUnderground;
}

void EnKakasi_IdleUnderground(EnKakasi* this, PlayState* play) {
    if ((gSaveContext.save.weekEventReg[79] & 8) && this->picto.actor.xzDistToPlayer < this->songSummonDist &&
        (BREG(1) != 0 || play->msgCtx.ocarinaMode == 0xD)) {
        this->picto.actor.flags &= ~ACTOR_FLAG_8000000;
        play->msgCtx.ocarinaMode = 4;
        this->actionFunc = EnKakasi_SetupRiseOutOfGround;
    }
}

void EnKakasi_SetupRiseOutOfGround(EnKakasi* this, PlayState* play) {
    s32 cutsceneIndex;

    cutsceneIndex = 0;
    if (this->aboveGroundStatus == ENKAKASI_ABOVE_GROUND_TYPE) {
        cutsceneIndex = 1;
    }

    if (ActorCutscene_GetCurrentIndex() == 0x7C) {
        ActorCutscene_Stop(0x7C);
        ActorCutscene_SetIntentToPlay(this->actorCutscenes[cutsceneIndex]);

    } else if (ActorCutscene_GetCanPlayNext(this->actorCutscenes[cutsceneIndex]) == 0) {
        ActorCutscene_SetIntentToPlay(this->actorCutscenes[cutsceneIndex]);

    } else {
        ActorCutscene_StartAndSetUnkLinkFields(this->actorCutscenes[cutsceneIndex], &this->picto.actor);
        Actor_PlaySfxAtPos(&this->picto.actor, NA_SE_EN_AKINDONUTS_HIDE);
        this->picto.actor.draw = EnKakasi_Draw;
        this->unkState196 = 6;
        this->actionFunc = EnKakasi_RisingOutOfGround;
    }
}

void EnKakasi_RisingOutOfGround(EnKakasi* this, PlayState* play) {
    this->picto.actor.shape.rot.y += 0x3000;

    if (this->animIndex != ENKAKASI_ANIM_SIDEWAYS_SHAKING) {
        EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_SIDEWAYS_SHAKING);
    }
    if (this->picto.actor.shape.yOffset < -10.0f) {
        if ((play->gameplayFrames % 8) == 0) {
            Actor_SpawnFloorDustRing(play, &this->picto.actor, &this->picto.actor.world.pos,
                                     this->picto.actor.shape.shadowScale - 20.0f, 10, 8.0f, 500, 10, 1);
            Actor_PlaySfxAtPos(&this->picto.actor, NA_SE_EN_AKINDONUTS_HIDE);
        }
        Math_ApproachF(&this->picto.actor.shape.yOffset, 0.0f, 0.5f, 200.0f);
    } else {
        EnKakasi_SetupIdleRisen(this);
    }
}

void EnKakasi_SetupIdleRisen(EnKakasi* this) {
    // text: great to see you again
    this->picto.actor.textId = 0x164D;
    this->unkState196 = 7;
    this->actionFunc = EnKakasi_IdleRisen;
    this->picto.actor.shape.yOffset = 0.0f;
}

void EnKakasi_IdleRisen(EnKakasi* this, PlayState* play) {
    Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.yawTowardsPlayer, 5, 1000, 0);
    if (Actor_ProcessTalkRequest(&this->picto.actor, &play->state)) {
        this->actionFunc = EnKakasi_RisenDialogue;
    } else {
        func_800B8614(&this->picto.actor, play, 70.0f);
    }
}

void EnKakasi_RisenDialogue(EnKakasi* this, PlayState* play) {
    Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.yawTowardsPlayer, 5, 1000, 0);

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_5) && Message_ShouldAdvance(play)) {
        func_801477B4(play);
        EnKakasi_SetupIdleRisen(this);
    }
}

void EnKakasi_Update(Actor* thisx, PlayState* play) {
    EnKakasi* this = THIS;
    s32 pad;

    SkelAnime_Update(&this->skelanime);
    if (this->picto.actor.draw != NULL) {
        EnKakasi_CheckAnimationSfx(this);
    }

    this->picto.actor.world.rot.y = this->picto.actor.shape.rot.y;
    DECR(this->postTeachTimer);
    DECR(this->unk204);

    if (this->unkState196 != 5) {
        if (this->unk1BC.x != 0.0f || this->unk1BC.z != 0.0f) {
            Math_Vec3f_Copy(&this->picto.actor.focus.pos, &this->unk1BC);
            this->picto.actor.focus.pos.y += 10.0f;
            if (this->subCamId == CAM_ID_MAIN) {
                Math_Vec3s_Copy(&this->picto.actor.focus.rot, &this->picto.actor.world.rot);
            } else {
                Math_Vec3s_Copy(&this->picto.actor.focus.rot, &this->picto.actor.home.rot);
            }
        }
    } else {
        Actor_SetFocus(&this->picto.actor, this->unkHeight);
    }

    this->actionFunc(this, play);
    Actor_MoveWithGravity(&this->picto.actor);
    Actor_UpdateBgCheckInfo(play, &this->picto.actor, 50.0f, 50.0f, 100.0f, 0x1C);
    if (this->picto.actor.draw != NULL) {
        Collider_UpdateCylinder(&this->picto.actor, &this->collider);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

void EnKakasi_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnKakasi* this = THIS;

    if (limbIndex == 4) {
        Matrix_MultVec3f(&gZeroVec3f, &this->unk1BC);
    }
}

void EnKakasi_Draw(Actor* thisx, PlayState* play) {
    EnKakasi* this = THIS;
    func_8012C28C(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelanime.skeleton, this->skelanime.jointTable, this->skelanime.dListCount, NULL,
                          EnKakasi_PostLimbDraw, &this->picto.actor);
}
