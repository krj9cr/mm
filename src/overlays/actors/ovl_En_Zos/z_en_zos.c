/*
 * File: z_en_zos.c
 * Overlay: ovl_En_Zos
 * Description: Zora Synthesizer - Evan
 */

#include "z_en_zos.h"

#define FLAGS (ACTOR_FLAG_1 | ACTOR_FLAG_8 | ACTOR_FLAG_2000000)

#define THIS ((EnZos*)thisx)

void EnZos_Init(Actor* thisx, PlayState* play);
void EnZos_Destroy(Actor* thisx, PlayState* play);
void EnZos_Update(Actor* thisx, PlayState* play);
void EnZos_Draw(Actor* thisx, PlayState* play);

void EnZos_ChangeAnim(EnZos* this, s16 animIndex, u8 animMode);
void func_80BBB2C4(EnZos* this, PlayState* play);
void func_80BBB354(EnZos* this, PlayState* play);
void func_80BBB4CC(EnZos* this, PlayState* play);
void func_80BBB574(EnZos* this, PlayState* play);
void func_80BBB718(EnZos* this, PlayState* play);
void func_80BBB8AC(EnZos* this, PlayState* play);
void func_80BBBB84(EnZos* this, PlayState* play);
void func_80BBBCBC(EnZos* this, PlayState* play);
void func_80BBBD5C(EnZos* this, PlayState* play);
void func_80BBBDE0(EnZos* this, PlayState* play);
void func_80BBC070(EnZos* this, PlayState* play);
void func_80BBC14C(EnZos* this, PlayState* play);
void func_80BBC22C(EnZos* this, PlayState* play);
void func_80BBC24C(EnZos* this, PlayState* play);
void func_80BBC298(EnZos* this, PlayState* play);
void func_80BBC37C(EnZos* this, PlayState* play);

typedef enum {
    /*  0 */ EN_ZOS_ANIM_LEAN_ON_KEYBOARD,
    /*  1 */ EN_ZOS_ANIM_LEAN_ON_KEYBOARD_AND_SIGH,
    /*  2 */ EN_ZOS_ANIM_HANDS_ON_HIPS,
    /*  3 */ EN_ZOS_ANIM_TALK_FOOT_TAP,
    /*  4 */ EN_ZOS_ANIM_TALK_LOOK_DOWN,
    /*  5 */ EN_ZOS_ANIM_TALK_ARMS_OUT,
    /*  6 */ EN_ZOS_ANIM_TALK_HANDS_ON_HIPS,
    /*  7 */ EN_ZOS_ANIM_PLAY_RIGHT,
    /*  8 */ EN_ZOS_ANIM_PLAY_LEFT,
    /*  9 */ EN_ZOS_ANIM_INSPIRED,
    /* 10 */ EN_ZOS_ANIM_SLOW_PLAY,
    /* 11 */ EN_ZOS_ANIM_PLAY_RIGHT_SHORTENED,
    /* 12 */ EN_ZOS_ANIM_PLAY_LEFT_SHORTENED,
    /* 13 */ EN_ZOS_ANIM_MAX
} EnZosAnimation;

const ActorInit En_Zos_InitVars = {
    ACTOR_EN_ZOS,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_ZOS,
    sizeof(EnZos),
    (ActorFunc)EnZos_Init,
    (ActorFunc)EnZos_Destroy,
    (ActorFunc)EnZos_Update,
    (ActorFunc)EnZos_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_ENEMY,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        TOUCH_NONE | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_ON,
    },
    { 60, 40, 0, { 0, 0, 0 } },
};

void EnZos_Init(Actor* thisx, PlayState* play) {
    EnZos* this = THIS;

    Actor_SetScale(&this->actor, 0.0115f);
    this->actionFunc = func_80BBBDE0;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    SkelAnime_InitFlex(play, &this->skelAnime, &gEvanSkel, &gEvanLeanOnKeyboardAnim, this->jointTable, this->morphTable,
                       EVAN_LIMB_MAX);
    Animation_PlayLoop(&this->skelAnime, &gEvanLeanOnKeyboardAnim);
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->unk_2B6 = 0;
    this->actor.terminalVelocity = -4.0f;
    this->actor.gravity = -4.0f;
    EnZos_ChangeAnim(this, EN_ZOS_ANIM_LEAN_ON_KEYBOARD, ANIMMODE_ONCE);

    switch (ENZOS_GET_F(&this->actor)) {
        case ENZOS_F_1:
            if (!(gSaveContext.save.weekEventReg[55] & 0x80)) {
                Actor_Kill(&this->actor);
            }

            if (gSaveContext.save.weekEventReg[78] & 1) {
                this->actionFunc = func_80BBC24C;
            } else {
                this->actionFunc = func_80BBC14C;
            }
            EnZos_ChangeAnim(this, EN_ZOS_ANIM_LEAN_ON_KEYBOARD, ANIMMODE_ONCE);
            break;

        case ENZOS_F_2:
            this->actionFunc = func_80BBC37C;
            EnZos_ChangeAnim(this, EN_ZOS_ANIM_PLAY_RIGHT, ANIMMODE_ONCE);
            this->unk_2BC = -1;
            this->unk_2B6 |= 0x40;
            break;

        default:
            if (gSaveContext.save.weekEventReg[55] & 0x80) {
                Actor_Kill(&this->actor);
            }
            this->actor.flags |= ACTOR_FLAG_10;
            break;
    }
}

void EnZos_Destroy(Actor* thisx, PlayState* play) {
    EnZos* this = THIS;

    gSaveContext.save.weekEventReg[52] &= (u8)~0x10;
}

static AnimationHeader* sAnimations[] = {
    &gEvanLeanOnKeyboardAnim,        // EN_ZOS_ANIM_LEAN_ON_KEYBOARD
    &gEvanLeanOnKeyboardAndSighAnim, // EN_ZOS_ANIM_LEAN_ON_KEYBOARD_AND_SIGH
    &gEvanHandsOnHipsAnim,           // EN_ZOS_ANIM_HANDS_ON_HIPS
    &gEeanTalkFootTapAnim,           // EN_ZOS_ANIM_TALK_FOOT_TAP
    &gEvanTalkLookDownAnim,          // EN_ZOS_ANIM_TALK_LOOK_DOWN
    &gEvanTalkArmsOutAnim,           // EN_ZOS_ANIM_TALK_ARMS_OUT
    &gEvanTalkHandsOnHipsAnim,       // EN_ZOS_ANIM_TALK_HANDS_ON_HIPS
    &gEvanPlayRightAnim,             // EN_ZOS_ANIM_PLAY_RIGHT
    &gEvanPlayLeftAnim,              // EN_ZOS_ANIM_PLAY_LEFT
    &gEvanInspiredAnim,              // EN_ZOS_ANIM_INSPIRED
    &gEvanSlowPlayAnim,              // EN_ZOS_ANIM_SLOW_PLAY
    &gEvanPlayRightAnim,             // EN_ZOS_ANIM_PLAY_RIGHT_SHORTENED
    &gEvanPlayLeftAnim,              // EN_ZOS_ANIM_PLAY_LEFT_SHORTENED
};

void EnZos_ChangeAnim(EnZos* this, s16 animIndex, u8 animMode) {
    f32 endFrame;

    if ((animIndex != this->animIndex) && (animIndex >= EN_ZOS_ANIM_LEAN_ON_KEYBOARD) &&
        (animIndex < EN_ZOS_ANIM_MAX)) {
        if (animIndex > EN_ZOS_ANIM_SLOW_PLAY) {
            endFrame = 29.0f;
        } else {
            endFrame = Animation_GetLastFrame(sAnimations[animIndex]);
        }
        Animation_Change(&this->skelAnime, sAnimations[animIndex], 1.0f, 0.0f, endFrame, animMode, -5.0f);
        this->animIndex = animIndex;
        this->unk_2B6 &= ~0x80;
    }
}

s32 func_80BBAF5C(EnZos* this, PlayState* play) {
    if (Player_IsFacingActor(&this->actor, 0x3000, play) &&
        ((!Actor_IsFacingPlayer(&this->actor, 0x4000) && (this->actor.home.rot.y == this->actor.shape.rot.y)) ||
         (Actor_IsFacingPlayer(&this->actor, 0x3000) && (this->actor.home.rot.y != this->actor.shape.rot.y))) &&
        (this->actor.xzDistToPlayer < 100.0f)) {
        return true;
    }
    return false;
}

s32 func_80BBAFFC(EnZos* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        switch ((s16)Rand_ZeroFloat(4.0f)) {
            case 0:
                EnZos_ChangeAnim(this, EN_ZOS_ANIM_PLAY_RIGHT, ANIMMODE_ONCE);
                break;

            case 1:
                EnZos_ChangeAnim(this, EN_ZOS_ANIM_PLAY_LEFT, ANIMMODE_ONCE);
                break;

            case 2:
                EnZos_ChangeAnim(this, EN_ZOS_ANIM_PLAY_RIGHT_SHORTENED, ANIMMODE_ONCE);
                break;

            default:
                EnZos_ChangeAnim(this, EN_ZOS_ANIM_PLAY_LEFT_SHORTENED, ANIMMODE_ONCE);
                break;
        }
        return true;
    }
    return false;
}

void func_80BBB0D4(EnZos* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        if (Rand_ZeroFloat(1.0f) < 0.9f) {
            EnZos_ChangeAnim(this, EN_ZOS_ANIM_LEAN_ON_KEYBOARD, ANIMMODE_ONCE);
        } else {
            EnZos_ChangeAnim(this, EN_ZOS_ANIM_LEAN_ON_KEYBOARD_AND_SIGH, ANIMMODE_ONCE);
        }
        SkelAnime_Update(&this->skelAnime);
    }
}

void func_80BBB15C(EnZos* this, PlayState* play) {
    s32 textId;

    if (gSaveContext.save.playerForm == PLAYER_FORM_ZORA) {
        if (this->unk_2B6 & 8) {
            textId = 0x1235;
            EnZos_ChangeAnim(this, EN_ZOS_ANIM_TALK_HANDS_ON_HIPS, ANIMMODE_LOOP);
            this->unk_2B6 |= 2;
        } else if (this->unk_2B6 & 4) {
            textId = 0x123E;
            EnZos_ChangeAnim(this, EN_ZOS_ANIM_TALK_HANDS_ON_HIPS, ANIMMODE_LOOP);
            this->unk_2B6 |= 2;
        } else if (gSaveContext.save.weekEventReg[40] & 0x20) {
            textId = 0x1236;
            EnZos_ChangeAnim(this, EN_ZOS_ANIM_TALK_HANDS_ON_HIPS, ANIMMODE_LOOP);
            this->unk_2B6 |= 0x80;
        } else {
            textId = 0x1231;
            EnZos_ChangeAnim(this, EN_ZOS_ANIM_TALK_HANDS_ON_HIPS, ANIMMODE_LOOP);
            this->unk_2B6 |= 0x80;
        }
    } else {
        this->unk_2B6 &= ~2;
        if (gSaveContext.save.weekEventReg[39] & 0x10) {
            textId = 0x1243;
            EnZos_ChangeAnim(this, EN_ZOS_ANIM_TALK_HANDS_ON_HIPS, ANIMMODE_LOOP);
            this->unk_2B6 |= 0x80;
        } else {
            textId = 0x1244;
            gSaveContext.save.weekEventReg[39] |= 0x10;
            EnZos_ChangeAnim(this, EN_ZOS_ANIM_TALK_LOOK_DOWN, ANIMMODE_LOOP);
            this->unk_2B6 |= 0x10;
        }
    }
    Message_StartTextbox(play, textId, &this->actor);
}

void func_80BBB2C4(EnZos* this, PlayState* play) {
    if (Actor_ProcessTalkRequest(&this->actor, &play->state)) {
        Message_StartTextbox(play, 0x124F, &this->actor);
        this->actionFunc = func_80BBB8AC;
        this->actor.flags &= ~ACTOR_FLAG_10000;
    } else {
        func_800B8500(&this->actor, play, 1000.0f, 1000.0f, PLAYER_AP_MINUS1);
    }
}

void func_80BBB354(EnZos* this, PlayState* play) {
    s32 getItemId;

    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actionFunc = func_80BBB2C4;
        gSaveContext.save.weekEventReg[39] |= 0x20;
        this->actor.flags |= ACTOR_FLAG_10000;
        func_800B8500(&this->actor, play, 1000.0f, 1000.0f, PLAYER_AP_MINUS1);
    } else {
        if (gSaveContext.save.weekEventReg[39] & 0x20) {
            getItemId = GI_RUPEE_PURPLE;
        } else {
            getItemId = GI_HEART_PIECE;
        }
        Actor_PickUp(&this->actor, play, getItemId, 10000.0f, 50.0f);
    }
}

void func_80BBB414(EnZos* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (Cutscene_CheckActorAction(play, 501)) {
        s16 action = play->csCtx.actorActions[Cutscene_GetActorActionIndex(play, 501)]->action;

        if (action != this->unk_2BA) {
            this->unk_2BA = action;

            switch (action) {
                case 1:
                    EnZos_ChangeAnim(this, EN_ZOS_ANIM_LEAN_ON_KEYBOARD_AND_SIGH, ANIMMODE_LOOP);
                    break;

                case 2:
                    EnZos_ChangeAnim(this, EN_ZOS_ANIM_SLOW_PLAY, ANIMMODE_LOOP);
                    break;
            }
        }
    }
}

void func_80BBB4CC(EnZos* this, PlayState* play) {
    func_80BBB414(this, play);

    if ((this->actor.cutscene != -1) && (ActorCutscene_GetCurrentIndex() != this->actor.cutscene)) {
        if (ActorCutscene_GetCurrentIndex() == 0x7C) {
            ActorCutscene_Stop(0x7C);
            ActorCutscene_SetIntentToPlay(this->actor.cutscene);
        } else if (ActorCutscene_GetCanPlayNext(this->actor.cutscene)) {
            ActorCutscene_Start(this->actor.cutscene, &this->actor);
            this->actor.cutscene = -1;
        } else {
            ActorCutscene_SetIntentToPlay(this->actor.cutscene);
        }
    }
}

void func_80BBB574(EnZos* this, PlayState* play) {
    if (!(this->unk_2B6 & 0x10)) {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0x1000, 0x200);
        this->actor.world.rot.y = this->actor.shape.rot.y;
    }

    if (func_80BBAFFC(this, play)) {
        if (this->unk_2B6 & 0x20) {
            this->unk_2B6 &= ~0x20;
            func_80151938(play, 0x124C);
        }
    }

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_5) && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.currentTextId) {
            case 0x124B:
                if (this->animIndex == EN_ZOS_ANIM_INSPIRED) {
                    play->msgCtx.msgLength = 0;
                    this->unk_2B6 |= 0x20;
                } else {
                    func_80151938(play, 0x124C);
                }
                break;

            case 0x124C:
                play->msgCtx.msgLength = 0;
                this->actionFunc = func_80BBB4CC;
                EnZos_ChangeAnim(this, EN_ZOS_ANIM_SLOW_PLAY, ANIMMODE_LOOP);
                break;

            case 0x124D:
                this->unk_2B6 &= ~0x10;
                EnZos_ChangeAnim(this, EN_ZOS_ANIM_TALK_HANDS_ON_HIPS, ANIMMODE_LOOP);
                func_80151938(play, 0x124E);
                break;

            case 0x124E:
                func_801477B4(play);
                this->actionFunc = func_80BBB354;
                func_80BBB354(this, play);
                break;
        }
    }
}

void func_80BBB718(EnZos* this, PlayState* play) {
    s32 sp24;
    Player* player = GET_PLAYER(play);

    SkelAnime_Update(&this->skelAnime);

    if (Message_GetState(&play->msgCtx) == TEXT_STATE_16) {
        sp24 = func_80123810(play);
        if (sp24 > 0) {
            func_801477B4(play);

            if (sp24 == 25) {
                player->actor.textId = 0x1232;
                EnZos_ChangeAnim(this, EN_ZOS_ANIM_TALK_ARMS_OUT, ANIMMODE_LOOP);
                this->unk_2B6 |= 8;
                gSaveContext.save.weekEventReg[40] |= 0x20;
            } else if (gSaveContext.save.weekEventReg[39] & 8) {
                player->actor.textId = 0x1241;
            } else {
                player->actor.textId = 0x1237;
                gSaveContext.save.weekEventReg[39] |= 8;
                EnZos_ChangeAnim(this, EN_ZOS_ANIM_TALK_LOOK_DOWN, ANIMMODE_LOOP);
                this->unk_2B6 |= 4;
            }
            this->actionFunc = func_80BBB8AC;
        } else if (sp24 < 0) {
            if (gSaveContext.save.weekEventReg[39] & 8) {
                func_80151938(play, 0x1241);
            } else {
                func_80151938(play, 0x1237);
                gSaveContext.save.weekEventReg[39] |= 8;
                EnZos_ChangeAnim(this, EN_ZOS_ANIM_TALK_LOOK_DOWN, ANIMMODE_LOOP);
                this->unk_2B6 |= 4;
            }
            this->actionFunc = func_80BBB8AC;
        }
    }
}

void func_80BBB8AC(EnZos* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);

    SkelAnime_Update(&this->skelAnime);

    if (!(this->unk_2B6 & 0x10)) {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0x1000, 0x200);
        this->actor.world.rot.y = this->actor.shape.rot.y;
        if ((this->unk_2B6 & 0x80) && (this->actor.yawTowardsPlayer == this->actor.shape.rot.y)) {
            EnZos_ChangeAnim(this, EN_ZOS_ANIM_TALK_FOOT_TAP, ANIMMODE_LOOP);
        }
    }

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_5) && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.currentTextId) {
            case 0x1237:
                player->exchangeItemId = 0;

            case 0x1238:
            case 0x123A:
            case 0x123B:
            case 0x123C:
            case 0x123E:
            case 0x123F:
                func_80151938(play, play->msgCtx.currentTextId + 1);
                break;

            case 0x1244:
                this->unk_2B6 &= ~0x10;
                EnZos_ChangeAnim(this, EN_ZOS_ANIM_TALK_HANDS_ON_HIPS, ANIMMODE_LOOP);
                func_80151938(play, play->msgCtx.currentTextId + 1);
                break;

            case 0x1232:
            case 0x1241:
                player->exchangeItemId = 0;

            case 0x1239:
            case 0x1246:
                EnZos_ChangeAnim(this, EN_ZOS_ANIM_TALK_HANDS_ON_HIPS, ANIMMODE_LOOP);
                func_80151938(play, play->msgCtx.currentTextId + 1);
                break;

            case 0x1233:
                EnZos_ChangeAnim(this, EN_ZOS_ANIM_TALK_ARMS_OUT, ANIMMODE_LOOP);
                func_80151938(play, play->msgCtx.currentTextId + 1);
                break;

            case 0x1245:
            case 0x1248:
                EnZos_ChangeAnim(this, EN_ZOS_ANIM_TALK_FOOT_TAP, ANIMMODE_LOOP);
                func_80151938(play, play->msgCtx.currentTextId + 1);
                break;

            case 0x1231:
                func_80151938(play, 0xFF);
                this->actionFunc = func_80BBB718;
                break;

            case 0x1243:
            case 0x1249:
                EnZos_ChangeAnim(this, EN_ZOS_ANIM_HANDS_ON_HIPS, ANIMMODE_LOOP);
                func_801477B4(play);
                this->actionFunc = func_80BBBDE0;
                this->unk_2B6 |= 1;
                break;

            case 0x1234:
            case 0x123D:
            case 0x1242:
                EnZos_ChangeAnim(this, EN_ZOS_ANIM_HANDS_ON_HIPS, ANIMMODE_LOOP);
                Actor_ProcessTalkRequest(&this->actor, &play->state);
                func_801477B4(play);
                this->actionFunc = func_80BBBDE0;
                this->unk_2B6 |= 1;
                break;

            case 0x1236:
                EnZos_ChangeAnim(this, EN_ZOS_ANIM_HANDS_ON_HIPS, ANIMMODE_LOOP);
                func_801477B4(play);
                this->actionFunc = func_80BBBDE0;
                this->unk_2B6 |= 1;
                break;

            default:
                if (0) {}
                func_801477B4(play);
                this->actionFunc = func_80BBBDE0;
                this->unk_2B6 |= 1;
                break;
        }
    }
}

void func_80BBBB84(EnZos* this, PlayState* play) {
    if (Actor_ProcessTalkRequest(&this->actor, &play->state)) {
        this->actor.flags &= ~ACTOR_FLAG_10000;
        if (gSaveContext.save.playerForm == PLAYER_FORM_ZORA) {
            Message_StartTextbox(play, 0x1248, &this->actor);
            this->actionFunc = func_80BBB8AC;
            EnZos_ChangeAnim(this, EN_ZOS_ANIM_TALK_HANDS_ON_HIPS, ANIMMODE_LOOP);
            this->unk_2B6 |= 2;
        } else if (gSaveContext.save.weekEventReg[41] & 0x10) {
            Message_StartTextbox(play, 0x124A, &this->actor);
            this->actionFunc = func_80BBB8AC;
            EnZos_ChangeAnim(this, EN_ZOS_ANIM_TALK_HANDS_ON_HIPS, ANIMMODE_LOOP);
        } else {
            gSaveContext.save.weekEventReg[41] |= 0x10;
            Message_StartTextbox(play, 0x124B, &this->actor);
            this->actionFunc = func_80BBB574;
            EnZos_ChangeAnim(this, EN_ZOS_ANIM_INSPIRED, ANIMMODE_ONCE);
            this->unk_2B6 |= 0x10;
        }
    } else {
        func_800B8614(&this->actor, play, 300.0f);
    }
}

void func_80BBBCBC(EnZos* this, PlayState* play) {
    if (Actor_ProcessTalkRequest(&this->actor, &play->state)) {
        this->actor.flags &= ~ACTOR_FLAG_10000;
        EnZos_ChangeAnim(this, EN_ZOS_ANIM_TALK_ARMS_OUT, ANIMMODE_LOOP);
        Message_StartTextbox(play, 0x124D, &this->actor);
        this->actionFunc = func_80BBB574;
    } else {
        func_800B8500(&this->actor, play, 1000.0f, 1000.0f, PLAYER_AP_MINUS1);
    }
}

void func_80BBBD5C(EnZos* this, PlayState* play) {
    func_80BBB414(this, play);
    if (!Cutscene_CheckActorAction(play, 0x1F5)) {
        this->actionFunc = func_80BBBCBC;
        this->actor.flags |= ACTOR_FLAG_10000;
        func_800B8500(&this->actor, play, 1000.0f, 1000.0f, PLAYER_AP_MINUS1);
    }
}

void func_80BBBDE0(EnZos* this, PlayState* play) {
    Actor* thisx = &this->actor;
    Vec3f sp28;

    if (this->unk_2B6 & 1) {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.home.rot.y, 2, 0x1000, 0x200);
        this->actor.world.rot.y = thisx->shape.rot.y;
        if (this->actor.home.rot.y == thisx->shape.rot.y) {
            EnZos_ChangeAnim(this, EN_ZOS_ANIM_LEAN_ON_KEYBOARD, ANIMMODE_ONCE);
            this->unk_2B6 &= ~1;
        }
    }

    func_80BBB0D4(this, play);

    if (play->msgCtx.ocarinaMode == 0x2A) {
        play->msgCtx.ocarinaMode = 4;
        this->actionFunc = func_80BBBB84;
        this->actor.flags |= ACTOR_FLAG_10000;
        func_800B8614(&this->actor, play, 120.0f);
        return;
    }

    if (Actor_ProcessTalkRequest(&this->actor, &play->state)) {
        this->actionFunc = func_80BBB8AC;
        func_80BBB15C(this, play);
    } else if (Cutscene_CheckActorAction(play, 0x1F5)) {
        this->actionFunc = func_80BBBD5C;
    } else if (func_80BBAF5C(this, play)) {
        func_800B8614(&this->actor, play, 120.0f);
    }

    if (!Actor_IsFacingPlayer(&this->actor, 0x4000) && (this->actor.xzDistToPlayer < 100.0f)) {
        gSaveContext.save.weekEventReg[52] |= 0x10;
    } else {
        gSaveContext.save.weekEventReg[52] &= (u8)~0x10;
    }

    sp28.x = this->actor.projectedPos.x;
    sp28.y = this->actor.projectedPos.y;
    sp28.z = this->actor.projectedPos.z;
    func_801A1FB4(3, &sp28, 110, 1000.0f);
}

void func_80BBBFBC(EnZos* this, PlayState* play) {
    u16 textId;

    if (gSaveContext.save.playerForm == PLAYER_FORM_ZORA) {
        if (gSaveContext.save.weekEventReg[79] & 1) {
            textId = 0x125B;
        } else if (gSaveContext.save.weekEventReg[78] & 0x80) {
            textId = 0x125A;
        } else {
            textId = 0x1259;
            gSaveContext.save.weekEventReg[78] |= 0x80;
        }
        EnZos_ChangeAnim(this, EN_ZOS_ANIM_TALK_ARMS_OUT, ANIMMODE_LOOP);
    } else {
        textId = 0x1258;
        EnZos_ChangeAnim(this, EN_ZOS_ANIM_TALK_HANDS_ON_HIPS, ANIMMODE_LOOP);
    }
    Message_StartTextbox(play, textId, &this->actor);
}

void func_80BBC070(EnZos* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0x1000, 0x200);
    this->actor.world.rot.y = this->actor.shape.rot.y;

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_5:
            if (Message_ShouldAdvance(play)) {
                EnZos_ChangeAnim(this, EN_ZOS_ANIM_HANDS_ON_HIPS, ANIMMODE_LOOP);
                func_801477B4(play);
                this->actionFunc = func_80BBC14C;
                this->unk_2B6 |= 1;
            }
            break;

        case TEXT_STATE_CLOSING:
            EnZos_ChangeAnim(this, EN_ZOS_ANIM_HANDS_ON_HIPS, ANIMMODE_LOOP);
            this->actionFunc = func_80BBC14C;
            this->unk_2B6 |= 1;
            break;
    }
}

void func_80BBC14C(EnZos* this, PlayState* play) {
    Actor* thisx = &this->actor;

    if (this->unk_2B6 & 1) {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.home.rot.y, 2, 0x1000, 0x200);
        this->actor.world.rot.y = thisx->shape.rot.y;
        if (this->actor.home.rot.y == this->actor.shape.rot.y) {
            EnZos_ChangeAnim(this, EN_ZOS_ANIM_LEAN_ON_KEYBOARD, ANIMMODE_ONCE);
            this->unk_2B6 &= ~1;
        }
    }

    func_80BBB0D4(this, play);

    if (Actor_ProcessTalkRequest(&this->actor, &play->state)) {
        this->actionFunc = func_80BBC070;
        func_80BBBFBC(this, play);
    } else if (func_80BBAF5C(this, play)) {
        func_800B8614(&this->actor, play, 120.0f);
    }
}

void func_80BBC22C(EnZos* this, PlayState* play) {
    func_80BBAFFC(this, play);
}

void func_80BBC24C(EnZos* this, PlayState* play) {
    func_80BBB0D4(this, play);
    if (gSaveContext.save.weekEventReg[79] & 1) {
        this->actionFunc = func_80BBC22C;
        EnZos_ChangeAnim(this, EN_ZOS_ANIM_PLAY_RIGHT, ANIMMODE_ONCE);
    }
}

void func_80BBC298(EnZos* this, PlayState* play) {
    func_80BBAFFC(this, play);

    if (this->unk_2BC < 799) {
        this->unk_2BC += 200;
    } else {
        this->unk_2BC += 30;
    }

    if (this->unk_2BC > 999) {
        this->unk_2BC = 999;
    }

    if (Cutscene_CheckActorAction(play, 515)) {
        if (play->csCtx.actorActions[Cutscene_GetActorActionIndex(play, 515)]->action == 1) {
            this->actionFunc = func_80BBC37C;
            this->unk_2BC = -1;
        }
    } else {
        this->actionFunc = func_80BBC37C;
        this->unk_2BC = -1;
    }
}

void func_80BBC37C(EnZos* this, PlayState* play) {
    func_80BBAFFC(this, play);
    if (Cutscene_CheckActorAction(play, 515) &&
        (play->csCtx.actorActions[Cutscene_GetActorActionIndex(play, 515)]->action == 3)) {
        this->actionFunc = func_80BBC298;
    }
}

void EnZos_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnZos* this = THIS;

    Actor_MoveWithGravity(&this->actor);
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    Actor_UpdateBgCheckInfo(play, &this->actor, 10.0f, 10.0f, 30.0f, 4);

    this->actionFunc(this, play);

    if (DECR(this->blinkTimer) == 0) {
        this->blinkTimer = Rand_S16Offset(60, 60);
    }

    this->eyeIndex = this->blinkTimer;
    if (this->eyeIndex >= 3) {
        this->eyeIndex = 0;
    }
}

s32 func_80BBC4E4(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    return false;
}

void func_80BBC500(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f D_80BBC750 = { 0.0f, 0.0f, 0.0f };

    if (limbIndex == EVAN_LIMB_HEAD) {
        Matrix_MultVec3f(&D_80BBC750, &thisx->focus.pos);
    }
}

void EnZos_Draw(Actor* thisx, PlayState* play) {
    static TexturePtr sEyeTextures[] = {
        gEvanEyeOpenTex,
        gEvanEyeHalfTex,
        gEvanEyeClosedTex,
    };
    EnZos* this = THIS;
    Gfx* gfx;

    OPEN_DISPS(play->state.gfxCtx);

    func_8012C28C(play->state.gfxCtx);

    if (this->unk_2B6 & 0x40) {
        POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, 0, 0, 0, 0, this->unk_2BC, 1000);
    }

    gfx = POLY_OPA_DISP;

    gSPSegment(&gfx[0], 0x08, Lib_SegmentedToVirtual(sEyeTextures[this->eyeIndex]));

    Matrix_Push();
    Matrix_RotateYS(this->actor.home.rot.y - this->actor.shape.rot.y, MTXMODE_APPLY);
    Matrix_Translate(0.0f, 0.0f, -974.4f, MTXMODE_APPLY);

    gSPMatrix(&gfx[1], Matrix_NewMtx(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(&gfx[2], gEvanUnknownWhiteTriangleDL);
    gSPDisplayList(&gfx[3], gEvanKeyboardDL);

    POLY_OPA_DISP = &gfx[4];

    Matrix_Pop();

    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          func_80BBC4E4, func_80BBC500, &this->actor);

    if (this->unk_2B6 & 0x40) {
        POLY_OPA_DISP = func_801660B8(play, POLY_OPA_DISP);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
