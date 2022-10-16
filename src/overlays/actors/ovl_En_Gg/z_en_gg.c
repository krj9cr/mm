/*
 * File: z_en_gg.c
 * Overlay: ovl_En_Gg
 * Description: Darmani's Ghost
 */

#include "z_en_gg.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_gg/object_gg.h"

#define FLAGS (ACTOR_FLAG_1 | ACTOR_FLAG_8 | ACTOR_FLAG_80)

#define THIS ((EnGg*)thisx)

void EnGg_Init(Actor* thisx, PlayState* play);
void EnGg_Destroy(Actor* thisx, PlayState* play);
void EnGg_Update(Actor* thisx, PlayState* play);
void EnGg_Draw(Actor* thisx, PlayState* play);

void func_80B352A4(EnGg* this, PlayState* play);
void func_80B35450(EnGg* this, PlayState* play);
void func_80B3556C(EnGg* this, PlayState* play);
u16 func_80B357F0(EnGg* this);
void func_80B358D8(EnGg* this, PlayState* play);
void func_80B359DC(EnGg* this, PlayState* play);
void func_80B363E8(EnGgStruct* ptr, PlayState* play, Vec3f* arg1, Vec3f* arg2, Vec3f* arg3);
void func_80B364D4(EnGgStruct* ptr, PlayState* play);

const ActorInit En_Gg_InitVars = {
    ACTOR_EN_GG,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_GG,
    sizeof(EnGg),
    (ActorFunc)EnGg_Init,
    (ActorFunc)EnGg_Destroy,
    (ActorFunc)EnGg_Update,
    (ActorFunc)EnGg_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
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
    { 24, 72, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 24, 72, 0, MASS_IMMOVABLE };

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, 0x0),
    /* Deku Stick     */ DMG_ENTRY(0, 0x0),
    /* Horse trample  */ DMG_ENTRY(0, 0x0),
    /* Explosives     */ DMG_ENTRY(0, 0x0),
    /* Zora boomerang */ DMG_ENTRY(0, 0x0),
    /* Normal arrow   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, 0x0),
    /* Hookshot       */ DMG_ENTRY(0, 0x0),
    /* Goron punch    */ DMG_ENTRY(0, 0x0),
    /* Sword          */ DMG_ENTRY(0, 0x0),
    /* Goron pound    */ DMG_ENTRY(0, 0x0),
    /* Fire arrow     */ DMG_ENTRY(0, 0x0),
    /* Ice arrow      */ DMG_ENTRY(0, 0x0),
    /* Light arrow    */ DMG_ENTRY(0, 0x0),
    /* Goron spikes   */ DMG_ENTRY(0, 0x0),
    /* Deku spin      */ DMG_ENTRY(0, 0x0),
    /* Deku bubble    */ DMG_ENTRY(0, 0x0),
    /* Deku launch    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0x0),
    /* Zora barrier   */ DMG_ENTRY(0, 0x0),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0x0),
    /* Thrown object  */ DMG_ENTRY(0, 0x0),
    /* Zora punch     */ DMG_ENTRY(0, 0x0),
    /* Spin attack    */ DMG_ENTRY(0, 0x0),
    /* Sword beam     */ DMG_ENTRY(0, 0x0),
    /* Normal Roll    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(0, 0x0),
};

static AnimationInfo sAnimationInfo[] = {
    { &object_gg_Anim_00F578, 1.0f, 0.0f, 0.0f, 0, -10.0f }, { &object_gg_Anim_00D528, 1.0f, 0.0f, 0.0f, 2, -10.0f },
    { &object_gg_Anim_00D174, 1.0f, 0.0f, 0.0f, 2, -10.0f }, { &object_gg_Anim_00ECC0, 1.0f, 0.0f, 0.0f, 2, -10.0f },
    { &object_gg_Anim_00BAF0, 1.0f, 0.0f, 0.0f, 0, -10.0f }, { &object_gg_Anim_00AF40, 1.0f, 0.0f, 0.0f, 0, -10.0f },
    { &object_gg_Anim_00F578, 1.0f, 0.0f, 0.0f, 0, -10.0f }, { &object_gg_Anim_00AF40, 1.0f, 0.0f, 0.0f, 0, -10.0f },
    { &object_gg_Anim_00F578, 1.0f, 0.0f, 0.0f, 0, -10.0f }, { &object_gg_Anim_0100C8, 1.0f, 0.0f, 0.0f, 0, 0.0f },
    { &object_gg_Anim_00CDA0, 1.0f, 0.0f, 0.0f, 0, 0.0f },   { &object_gg_Anim_00B560, 1.0f, 0.0f, 0.0f, 0, 0.0f },
    { &object_gg_Anim_00A4B4, 1.0f, 0.0f, 0.0f, 2, 0.0f },   { &object_gg_Anim_00E86C, 1.0f, 0.0f, 0.0f, 2, 0.0f },
    { &object_gg_Anim_00D99C, 1.0f, 0.0f, 0.0f, 2, 0.0f },   { &object_gg_Anim_00E2A4, 1.0f, 0.0f, 0.0f, 0, 0.0f },
};

void func_80B34F70(EnGg* this) {
    this->actor.focus.pos.x = this->actor.world.pos.x;
    this->actor.focus.pos.y = this->actor.world.pos.y + 80.0f;
    this->actor.focus.pos.z = this->actor.world.pos.z;

    this->actor.focus.rot.x = this->actor.world.rot.x;
    this->actor.focus.rot.y = this->actor.world.rot.y;
    this->actor.focus.rot.z = this->actor.world.rot.z;
}

s32 func_80B34FB4(EnGg* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f sp40;
    Vec3f sp34;
    s16 pitch;

    sp40 = player->actor.world.pos;
    sp40.y = player->bodyPartsPos[7].y + 3.0f;
    sp34 = this->actor.world.pos;
    sp34.y += 70.0f;
    pitch = Math_Vec3f_Pitch(&sp34, &sp40);

    if ((this->actor.xzDistToPlayer < 250.0f) && (this->actor.xzDistToPlayer > 50.0f) &&
        (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_80) || (gSaveContext.save.weekEventReg[19] & 0x80))) {
        Math_SmoothStepToS(&this->unk_2E8, pitch, 4, 0x2AA8, 1);
    } else {
        Math_SmoothStepToS(&this->unk_2E8, 0, 4, 0x2AA8, 1);
    }
    this->unk_2E8 = CLAMP(this->unk_2E8, 0, 0x1C70);
    return true;
}

void func_80B35108(EnGg* this, PlayState* play) {
    this->collider.dim.pos.x = this->actor.world.pos.x;
    this->collider.dim.pos.y = this->actor.world.pos.y;
    this->collider.dim.pos.z = this->actor.world.pos.z;

    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 30.0f, 30.0f, 7);
}

void func_80B351A4(EnGg* this) {
    if ((this->unk_2E6 == 2) || (this->unk_2E6 == 3)) {
        this->unk_2E2 = 3;
    } else {
        s16 temp = this->unk_2E4 - 1;

        if (temp >= 3) {
            this->unk_2E2 = 0;
            this->unk_2E4 = temp;
        } else if (temp == 0) {
            this->unk_2E2 = 2;
            this->unk_2E4 = (s32)(Rand_ZeroOne() * 60.0f) + 20;
        } else {
            this->unk_2E2 = 1;
            this->unk_2E4 = temp;
        }
    }
}

void func_80B35250(EnGg* this) {
    this->unk_2E4 = 20;
    this->unk_2E2 = 0;
    this->unk_2E6 = 0;
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, 0);
    this->actionFunc = func_80B35450;
}

void func_80B352A4(EnGg* this, PlayState* play) {
    s16 sp26 = this->skelAnime.curFrame;
    s16 lastFrame = Animation_GetLastFrame(sAnimationInfo[this->unk_2E6].animation);

    if (sp26 == lastFrame) {
        switch (this->actor.textId) {
            case 0xCE5:
                this->unk_2E6 = 1;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, 1);
                break;

            case 0xCE6:
            case 0xCEC:
                this->unk_2E6 = 0;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, 0);
                break;

            case 0xCE8:
                this->unk_2E6 = 2;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, 2);
                break;

            case 0xCE9:
                this->unk_2E6 = 3;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, 3);
                break;

            case 0xCED:
            case 0xCEE:
                this->unk_2E6 = 4;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, 4);
                break;

            default:
                this->unk_2E6 = 0;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, 0);
                break;
        }
        gSaveContext.save.weekEventReg[19] |= 0x80;
        this->actionFunc = func_80B3556C;
    } else if ((this->unk_2E6 == 0) && ((this->actor.textId == 0xCED) || (this->actor.textId == 0xCEE))) {
        if (sp26 < (lastFrame - 1)) {
            this->skelAnime.playSpeed = 2.0f;
        } else {
            this->unk_2E6 = 4;
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, 4);
        }
    }
}

void func_80B35450(EnGg* this, PlayState* play) {
    if ((gSaveContext.save.weekEventReg[91] & 0x10) && (play->csCtx.state == 0)) {
        func_80B359DC(this, play);
    }

    if (Actor_ProcessTalkRequest(&this->actor, &play->state)) {
        if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_80)) {
            Actor_DeactivateLens(play);
        }
        this->unk_308 = 1;
        this->actionFunc = func_80B352A4;
    } else if ((this->actor.xzDistToPlayer < 200.0f) && (this->actor.xzDistToPlayer > 50.0f)) {
        if (gSaveContext.save.weekEventReg[19] & 0x80) {
            func_800B863C(&this->actor, play);
            this->actor.textId = 0xCEE;
        } else if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_80)) {
            func_800B863C(&this->actor, play);
            this->actor.textId = 0xCE5;
        }
    }
}

void func_80B3556C(EnGg* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_5) && Message_ShouldAdvance(play)) {
        if (this->unk_2E6 == 4) {
            play->msgCtx.msgMode = 0x43;
            play->msgCtx.stateTimer = 4;
            this->unk_308 = 0;
            this->actor.flags &= ~ACTOR_FLAG_80;
            func_80B35250(this);
        } else {
            this->actor.textId = func_80B357F0(this);
            Message_StartTextbox(play, this->actor.textId, &this->actor);
            this->actionFunc = func_80B352A4;
        }
    }
}

void func_80B35634(EnGg* this, PlayState* play) {
    s32 pad;
    s32 actionIndex;

    if (Cutscene_CheckActorAction(play, 119)) {
        actionIndex = Cutscene_GetActorActionIndex(play, 119);

        if (this->unk_2DB != play->csCtx.actorActions[actionIndex]->action) {
            this->unk_2DB = play->csCtx.actorActions[actionIndex]->action;

            switch (play->csCtx.actorActions[actionIndex]->action) {
                case 1:
                    this->unk_2DA = 0;
                    this->unk_2E6 = 0;
                    break;

                case 2:
                    this->unk_2DA = 9;
                    this->unk_2E6 = 9;
                    break;

                case 3:
                    this->unk_2DA = 10;
                    this->unk_2E6 = 10;
                    break;

                case 4:
                    do {
                        this->unk_344.unk_30 = this->unk_2DA = 11;
                    } while (0);
                    this->unk_2E6 = 11;
                    func_80B364D4(&this->unk_344, play);
                    break;

                case 5:
                    do {
                        this->unk_344.unk_30 = this->unk_2DA = 12;
                    } while (0);
                    this->unk_2E6 = 12;
                    break;

                case 6:
                    do {
                        this->unk_344.unk_30 = this->unk_2DA = 13;
                    } while (0);
                    this->unk_2E6 = 13;
                    break;

                case 7:
                    do {
                        this->unk_344.unk_30 = this->unk_2DA = 14;
                    } while (0);
                    this->unk_2E6 = 14;
                    func_80B364D4(&this->unk_344, play);
                    break;

                case 8:
                    Actor_Kill(&this->actor);
                    return;

                default:
                    this->unk_2DA = 0;
                    break;
            }
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, this->unk_2DA);
        }

        if (this->unk_2DA == 14) {
            func_80B358D8(this, play);
        }

        Cutscene_ActorTranslateAndYaw(&this->actor, play, actionIndex);
        this->actor.shape.yOffset = 0.0f;
    } else {
        this->unk_2DB = 99;
    }
}

u16 func_80B357F0(EnGg* this) {
    switch (this->actor.textId) {
        case 0xCE5:
            return 0xCE6;
        case 0xCE6:
            return 0xCE8;
        case 0xCE8:
            return 0xCE9;
        case 0xCE9:
            return 0xCEC;
        case 0xCEC:
            return 0xCED;
    }
    return 0;
}

void func_80B3584C(EnGg* this) {
    this->unk_30A += 0x72C;
    this->actor.shape.yOffset = Math_SinS(this->unk_30A) * 100.0f;
    this->actor.shape.shadowScale = 30.0f - (Math_SinS(this->unk_30A) * 5.0f);
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, 0x1000, 0x100);
    this->actor.world.rot.y = this->actor.shape.rot.y;
}

void func_80B358D8(EnGg* this, PlayState* play) {
    s16 sp1E = this->skelAnime.curFrame;
    s16 lastFrame = Animation_GetLastFrame(sAnimationInfo[this->unk_2DA].animation);

    if ((this->unk_2E6 == 14) && (sp1E == lastFrame)) {
        this->unk_2E6 = 15;
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, 15);
    }
}

void func_80B35968(EnGg* this, PlayState* play) {
    if (this->unk_344.unk_34 != NULL) {
        this->unk_344.unk_34(&this->unk_344, play);
    } else {
        if ((this->unk_2DA == 11) || (this->unk_2DA == 14)) {
            this->unk_344.unk_30 = this->unk_2DA;
        }
        func_80B363E8(&this->unk_344, play, &this->unk_320, &this->unk_32C, &this->unk_338);
    }
}

void func_80B359DC(EnGg* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->actor.xzDistToPlayer < 200.0f) {
        if (this->unk_306 == 0) {
            if (player->stateFlags2 & PLAYER_STATE2_8000000) {
                this->unk_306 = 1;
                play_sound(NA_SE_SY_TRE_BOX_APPEAR);
            }
        } else if (!(player->stateFlags2 & PLAYER_STATE2_8000000)) {
            this->unk_306 = 0;
        }

        if ((player->transformation == PLAYER_FORM_HUMAN) && (play->msgCtx.ocarinaMode == 3) &&
            (play->msgCtx.lastPlayedSong == OCARINA_SONG_HEALING)) {
            if (!(gSaveContext.save.weekEventReg[19] & 0x80)) {
                gSaveContext.save.weekEventReg[19] |= 0x80;
            }
            this->unk_307 = true;
        }

        if (ActorCutscene_GetCanPlayNext(this->unk_2DC)) {
            ActorCutscene_Start(this->unk_2DC, &this->actor);
            this->unk_307 = false;
        } else {
            if (ActorCutscene_GetCurrentIndex() == 0x7C) {
                ActorCutscene_Stop(0x7C);
            }

            if (this->unk_307) {
                ActorCutscene_SetIntentToPlay(this->unk_2DC);
            }
        }
    } else {
        this->unk_307 = false;
    }
}

void func_80B35B24(EnGgStruct* ptr, PlayState* play) {
    ptr->unk_34 = NULL;
    ptr->unk_38 = NULL;
    ptr->unk_3C = 0;
    ptr->unk_40 = 0;
    ptr->unk_44 = 0;
}

void func_80B35B44(EnGgStruct* ptr, PlayState* play) {
    if (ptr->unk_30 == 0xE) {
        ptr->unk_40++;
        if (ptr->unk_40 > 0x46) {
            ptr->unk_48 = 1;
            ptr->unk_40 = 0;
        }

        if (ptr->unk_40 != 0x46) {
            if (ptr->unk_48 != 0) {
                ptr->unk_44 = 0x37;
            } else {
                ptr->unk_44 = ptr->unk_40 - 15;
                if (ptr->unk_44 < 0) {
                    ptr->unk_44 = 0;
                }
            }
        } else {
            ptr->unk_44++;
            if (ptr->unk_44 > 0x37) {
                ptr->unk_44 = 0x37;
            }
        }
    } else if ((ptr->unk_30 == 11) || (ptr->unk_30 == 12)) {
        ptr->unk_40++;
        if (ptr->unk_40 > 0x46) {
            ptr->unk_48 = 1;
            ptr->unk_40 = 0;
        }
    } else if (ptr->unk_30 == 0xD) {
        ptr->unk_48 = 0;
        ptr->unk_40++;
        if (ptr->unk_40 > 0x46) {
            ptr->unk_40 = 0x46;
        }

        if (ptr->unk_40 != 0x46) {
            ptr->unk_44 = ptr->unk_40 - 15;
            if (ptr->unk_44 < 0) {
                ptr->unk_44 = 0;
            }
        } else {
            ptr->unk_44++;
            if (ptr->unk_44 > 0x37) {
                func_80B364D4(ptr, play);
            }
        }
    }
}

void func_80B35C84(EnGgStruct* ptr, PlayState* play) {
    s32 sp74;
    s32 phi_s7;
    f32 temp_f22;
    f32 temp_f24;
    f32 temp_f26;
    f32 temp_f20;
    s32 i;
    s32 temp = 10;

    if (ptr->unk_48 != 0) {
        sp74 = ptr->unk_40 % temp;
        ptr->unk_40 = sp74;
        phi_s7 = 0x46;
    } else if (ptr->unk_30 == 0xD) {
        sp74 = ptr->unk_40;
        phi_s7 = 0x46;
    } else {
        phi_s7 = ptr->unk_40 - ptr->unk_44;
        sp74 = ptr->unk_40 % temp;
    }

    if (phi_s7 <= 0) {
        return;
    }

    OPEN_DISPS(play->state.gfxCtx);

    Matrix_Push();

    for (i = sp74; i < phi_s7; i += temp) {
        temp_f20 = i * 0.14f;
        temp_f22 = ptr->unk_00.x + (ptr->unk_18.x * temp_f20) + (0.5f * ptr->unk_24.x * temp_f20 * temp_f20);
        temp_f24 = ptr->unk_00.y - Math_SinS((i * 0x27FFB) / 70);
        temp_f26 = ptr->unk_00.z + (ptr->unk_18.z * temp_f20) + (0.5f * ptr->unk_24.z * temp_f20 * temp_f20);
        temp_f20 = Rand_ZeroOne() * 0.003f;

        if (1) {}

        Matrix_Translate(temp_f22, temp_f24, temp_f26, MTXMODE_NEW);
        Matrix_Scale(temp_f20, temp_f20, temp_f20, MTXMODE_APPLY);

        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 255, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 150, 0, 255);

        func_8012C2DC(play->state.gfxCtx);
        Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);

        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gOwlStatueWhiteFlashDL);
    }

    Matrix_Pop();

    Matrix_Push();

    for (i = sp74; i < phi_s7; i += temp) {
        temp_f20 = i * 0.14f;
        temp_f22 = ptr->unk_0C.x + ((ptr->unk_18.x * temp_f20) + (0.5f * ptr->unk_24.x * temp_f20 * temp_f20));
        temp_f24 = ptr->unk_0C.y - Math_SinS((i * 0x27FFB) / 70);
        temp_f26 = ptr->unk_0C.z + (ptr->unk_18.z * temp_f20) + (0.5f * ptr->unk_24.z * temp_f20 * temp_f20);
        temp_f20 = Rand_ZeroOne() * 0.003f;

        Matrix_Translate(temp_f22, temp_f24, temp_f26, MTXMODE_NEW);
        Matrix_Scale(temp_f20, temp_f20, temp_f20, MTXMODE_APPLY);

        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 255, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 150, 0, 255);

        func_8012C2DC(play->state.gfxCtx);
        Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);

        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gOwlStatueWhiteFlashDL);
    }

    Matrix_Pop();

    gSPMatrix(POLY_XLU_DISP++, &gIdentityMtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80B3610C(EnGgStruct* ptr, PlayState* play) {
    f32 temp_f20;
    f32 temp_f24;
    f32 temp_f26;
    f32 temp_f28;
    f32 temp_f30;
    s32 phi_s4;
    f32 phi_f22;
    s32 i;

    if (ptr->unk_48 != 0) {
        phi_s4 = 0x46;
    } else {
        phi_s4 = ptr->unk_40;
    }

    if (phi_s4 > 0) {
        OPEN_DISPS(play->state.gfxCtx);

        Matrix_Push();

        for (i = 0; i < phi_s4; i++) {
            if (ptr->unk_48 != 0) {
                phi_f22 = phi_s4 / 70.0f;
            } else {
                phi_f22 = i / 70.0f;
            }
            temp_f24 = ptr->unk_0C.x + (ptr->unk_18.x * i) + (0.5f * ptr->unk_24.x * i * i);
            temp_f26 = ptr->unk_0C.y + (ptr->unk_18.y * i) + (0.5f * ptr->unk_24.y * i * i);
            temp_f28 = ptr->unk_0C.z + (ptr->unk_18.z * i) + (0.5f * ptr->unk_24.z * i * i);
            temp_f20 = Rand_ZeroOne() * 0.003f;

            Matrix_Translate((Rand_Centered() * (100.0f * phi_f22)) + temp_f24, temp_f26,
                             ((30.0f * phi_f22) * Rand_Centered()) + temp_f28, MTXMODE_NEW);
            Matrix_Scale(temp_f20, temp_f20, temp_f20, MTXMODE_APPLY);

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 255, 255);
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 150, 0, 255);

            func_8012C2DC(play->state.gfxCtx);
            Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);

            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gOwlStatueWhiteFlashDL);
        }

        Matrix_Pop();

        gSPMatrix(POLY_XLU_DISP++, &gIdentityMtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void func_80B363E8(EnGgStruct* ptr, PlayState* play, Vec3f* arg1, Vec3f* arg2, Vec3f* arg3) {
    ptr->unk_34 = func_80B35B44;
    ptr->unk_00 = *arg1;
    ptr->unk_0C = *arg1;
    ptr->unk_18 = *arg2;
    ptr->unk_24 = *arg3;
    ptr->unk_48 = 0;

    ptr->unk_00.x += -4.0f;
    ptr->unk_0C.x += 4.0f;

    if ((ptr->unk_30 == 11) || (ptr->unk_30 == 12) || (ptr->unk_30 == 13)) {
        ptr->unk_38 = func_80B35C84;
    } else if (ptr->unk_30 == 14) {
        ptr->unk_38 = func_80B3610C;
    }
}

void func_80B364D4(EnGgStruct* ptr, PlayState* play) {
    ptr->unk_34 = NULL;
    ptr->unk_38 = NULL;
    func_80B35B24(ptr, play);
}

void EnGg_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnGg* this = THIS;

    if (INV_CONTENT(ITEM_MASK_GORON) == ITEM_MASK_GORON) {
        Actor_Kill(&this->actor);
        return;
    }

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    this->actor.bgCheckFlags |= 0x400;
    SkelAnime_InitFlex(play, &this->skelAnime, &object_gg_Skel_00F6C0, &object_gg_Anim_00F578, this->jointTable,
                       this->morphTable, 20);

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);

    gSaveContext.save.weekEventReg[20] &= (u8)~4;
    gSaveContext.save.weekEventReg[20] &= (u8)~8;
    gSaveContext.save.weekEventReg[20] &= (u8)~0x10;
    this->actor.flags &= ~ACTOR_FLAG_80;
    this->unk_310 = this->actor.home.pos.y;
    this->unk_2DC = this->actor.cutscene;
    this->actor.flags |= ACTOR_FLAG_2000000;
    this->unk_308 = 0;
    this->unk_309 = 0;
    this->unk_304 = 0;
    this->unk_30A = 0;
    this->actor.flags |= ACTOR_FLAG_10;
    func_80B35B24(&this->unk_344, play);
    func_80B35250(this);
}

void EnGg_Destroy(Actor* thisx, PlayState* play) {
}

void EnGg_Update(Actor* thisx, PlayState* play) {
    EnGg* this = THIS;

    if (play->actorCtx.lensMaskSize == LENS_MASK_ACTIVE_SIZE) {
        this->actor.flags |= ACTOR_FLAG_80;
        this->actor.flags |= ACTOR_FLAG_1;
    } else {
        this->actor.flags &= ~ACTOR_FLAG_80;
        this->actor.flags &= ~ACTOR_FLAG_1;
    }

    if (gSaveContext.save.weekEventReg[19] & 0x80) {
        if (play->csCtx.state == 0) {
            this->actor.flags |= ACTOR_FLAG_1;
        } else {
            this->actor.flags &= ~ACTOR_FLAG_1;
        }
    }

    if ((play->csCtx.state == 0) &&
        ((this->unk_2DA != 14) && (this->unk_2DA != 11) && (this->unk_2DA != 12) && (this->unk_2DA != 13))) {
        func_80B364D4(&this->unk_344, play);
    }

    if (((this->unk_2DA == 14) || (this->unk_2DA == 11) || (this->unk_2DA == 12) || (this->unk_2DA == 13)) &&
        (this->unk_309 == 1)) {
        func_80B35968(this, play);
    }

    if (!(gSaveContext.save.weekEventReg[91] & 0x10) &&
        ((gSaveContext.save.weekEventReg[19] & 0x80) || CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_80) ||
         (this->unk_308 == 1))) {
        gSaveContext.save.weekEventReg[91] |= 0x10;
    }

    this->actionFunc(this, play);

    func_80B34F70(this);
    func_80B35108(this, play);
    func_80B34FB4(this, play);
    Actor_MoveWithoutGravity(&this->actor);
    SkelAnime_Update(&this->skelAnime);

    if (play->csCtx.state == 0) {
        func_80B3584C(this);
    } else {
        this->unk_2E8 = 0;
    }

    func_80B35634(this, play);
    Actor_TrackPlayer(play, &this->actor, &this->unk_1D8, &this->unk_1DE, this->actor.focus.pos);
    func_80B351A4(this);
}

s32 EnGg_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx,
                          Gfx** gfx) {
    EnGg* this = THIS;

    if (limbIndex == 2) {
        Matrix_RotateZS(this->unk_2E8, MTXMODE_APPLY);
    }
    return false;
}

void EnGg_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx, Gfx** gfx) {
    static Vec3f D_80B36DF0 = { 1800.0f, 300.0f, 200.0f };
    EnGg* this = THIS;
    Vec3f sp30 = { 0.0f, 0.0f, 0.0f };
    Vec3f sp24 = { 0.0f, 0.0f, 0.0f };

    if (this->unk_2DA == 14) {
        sp30.y = 3.0f;
        sp30.z = -1.0f;
        sp24.y = -0.07f;
        this->unk_309 = 1;
    } else if ((this->unk_2DA == 11) || (this->unk_2DA == 12) || (this->unk_2DA == 13)) {
        sp30.x = 3.0f;
        sp24.x = 0.5f;
        this->unk_309 = 1;
    } else {
        this->unk_309 = 0;
    }

    if (limbIndex == 4) {
        Matrix_MultVec3f(&D_80B36DF0, &this->unk_320);
        Matrix_Push();
        Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_NEW);
        Matrix_MultVec3f(&sp30, &this->unk_32C);
        Matrix_MultVec3f(&sp24, &this->unk_338);
        Matrix_Pop();
    }
}

TexturePtr D_80B36DFC[] = {
    object_gg_Tex_009260,
    object_gg_Tex_009660,
    object_gg_Tex_009A60,
    object_gg_Tex_009E60,
};

void EnGg_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnGg* this = THIS;

    OPEN_DISPS(play->state.gfxCtx);

    if (this->unk_344.unk_38 != 0) {
        this->unk_344.unk_38(&this->unk_344, play);
    }

    if (gSaveContext.save.weekEventReg[19] & 0x80) {
        func_8012C28C(play->state.gfxCtx);

        gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(D_80B36DFC[this->unk_2E2]));

        POLY_OPA_DISP =
            SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                               EnGg_OverrideLimbDraw, EnGg_PostLimbDraw, &this->actor, POLY_OPA_DISP);
    } else if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_80) || (this->unk_308 == 1)) {
        func_8012C2DC(play->state.gfxCtx);

        gSPSegment(POLY_XLU_DISP++, 0x08, Lib_SegmentedToVirtual(D_80B36DFC[this->unk_2E2]));

        POLY_XLU_DISP =
            SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                               EnGg_OverrideLimbDraw, EnGg_PostLimbDraw, &this->actor, POLY_XLU_DISP);
    }
    func_8012C2DC(play->state.gfxCtx);

    CLOSE_DISPS(play->state.gfxCtx);
}
