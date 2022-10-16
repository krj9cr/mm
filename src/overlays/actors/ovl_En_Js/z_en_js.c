/*
 * File: z_en_js.c
 * Overlay: ovl_En_Js
 * Description: Moon Child
 */

#include "z_en_js.h"

#define FLAGS (ACTOR_FLAG_1 | ACTOR_FLAG_8 | ACTOR_FLAG_10)

#define THIS ((EnJs*)thisx)

void EnJs_Init(Actor* thisx, PlayState* play);
void EnJs_Destroy(Actor* thisx, PlayState* play);
void EnJs_Update(Actor* thisx, PlayState* play);
void EnJs_Draw(Actor* thisx, PlayState* play);

s32 func_80968B8C(EnJs* this, PlayState* play);
s32 func_809692A8(s32 arg0);
void func_80968A5C(EnJs* this);
void func_80969400(s32 arg0);
void func_8096971C(EnJs* this, PlayState* play);
void func_80969898(EnJs* this, PlayState* play);
void func_80969B5C(EnJs* this, PlayState* play);
void func_80969DA4(EnJs* this, PlayState* play);
void func_8096A104(EnJs* this, PlayState* play);
void func_8096A38C(EnJs* this, PlayState* play);
void func_8096A6F4(EnJs* this, PlayState* play);

const ActorInit En_Js_InitVars = {
    ACTOR_EN_JS,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_OB,
    sizeof(EnJs),
    (ActorFunc)EnJs_Init,
    (ActorFunc)EnJs_Destroy,
    (ActorFunc)EnJs_Update,
    (ActorFunc)EnJs_Draw,
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
    { 20, 40, 0, { 0, 0, 0 } },
};

static Gfx* D_8096ABCC[] = {
    gMoonChildMajorasMaskDL, gMoonChildOdalwasMaskDL,   gMoonChildGohtsMaskDL,
    gMoonChildGyorgsMaskDL,  gMoonChildTwinmoldsMaskDL,
};

f32 D_8096ABE0[] = { 1.0f, 0.5f, 0.5f, 0.48f, 0.45f };

f32 D_8096ABF4[] = { 60.0f, 0.0f, 0.0f, 0.0f, 0.0f };

f32 D_8096AC08[] = { -450.0f, 1400.0f, 1470.0f, 1670.0f, 1470.0f };

f32 D_8096AC1C[] = { 0.0f, 700.0f, 900.0f, 900.0f, 900.0f };

Vec3f D_8096AC30 = { 500.0f, -500.0f, 0.0f };

void EnJs_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    s16 cs;
    s32 i;
    EnJs* this = THIS;

    Actor_SetScale(&this->actor, 0.01f);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 20.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gMoonChildSkel, &gMoonChildStandingAnim, this->jointTable,
                       this->morphTable, MOONCHILD_LIMB_MAX);
    Animation_PlayLoop(&this->skelAnime, &gMoonChildStandingAnim);
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);

    this->actionFunc = func_8096971C;
    this->maskType = 0;
    this->actor.terminalVelocity = -9.0f;
    this->actor.gravity = -1.0f;

    cs = this->actor.cutscene;

    for (i = 0; i < ARRAY_COUNT(this->cutscenes); i++) {
        this->cutscenes[i] = cs;
        if (cs != -1) {
            this->actor.cutscene = cs;
            cs = ActorCutscene_GetAdditionalCutscene(this->actor.cutscene);
        }
    }

    this->cutsceneIndex = -1;

    switch (ENJS_GET_TYPE(&this->actor)) {
        case 0:
            this->maskType = 0;
            this->actionFunc = func_8096A6F4;

            Animation_PlayLoop(&this->skelAnime, &gMoonChildSittingAnim);
            func_8016566C(0x3C);

            if (gSaveContext.save.weekEventReg[84] & 0x20) {
                Inventory_DeleteItem(ITEM_MASK_FIERCE_DEITY, SLOT(ITEM_MASK_FIERCE_DEITY));
                gSaveContext.save.weekEventReg[84] &= (u8)~0x20;
            }
            break;
        case 1:
        case 2:
        case 3:
        case 4:
            this->maskType = ENJS_GET_TYPE(&this->actor);
            func_80968B8C(this, play);
            this->actionFunc = func_80969B5C;
            func_80968A5C(this);
            if (func_809692A8(ENJS_GET_TYPE(&this->actor) + 4)) {
                Actor_Kill(&this->actor);
                return;
            }
            break;
        case 5:
        case 6:
        case 7:
        case 8:
            this->maskType = ENJS_GET_TYPE(&this->actor) - 4;
            this->actionFunc = func_8096A104;
            break;
        default:
            break;
    }
}

void EnJs_Destroy(Actor* thisx, PlayState* play) {
    u32 paramsF;
    EnJs* this = THIS;

    Collider_DestroyCylinder(play, &this->collider);

    paramsF = ENJS_GET_TYPE(&this->actor);
    switch (paramsF) {
        case 0:
            func_80165690();
            break;
        case 5:
        case 6:
        case 7:
        case 8:
            if (!func_809692A8(paramsF)) {
                func_80969400(ENJS_GET_TYPE(&this->actor));
            }
            break;
        default:
            break;
    }
}

void func_80968A5C(EnJs* this) {
    if (Rand_ZeroOne() < 0.5f) {
        Animation_MorphToLoop(&this->skelAnime, &gMoonChildRunningAnim, -10.0f);
        this->unk_2B4 = 5.0f;
    } else {
        Animation_MorphToLoop(&this->skelAnime, &gMoonChildWalkingAnim, -10.0f);
        this->unk_2B4 = 2.5f;
    }
    this->unk_2B8 &= ~4;
    this->unk_2BC = Rand_ZeroFloat(40.0f) + 80.0f;
}

void func_80968B18(EnJs* this) {
    Animation_MorphToLoop(&this->skelAnime, &gMoonChildStandingAnim, -10.0f);
    this->unk_2B4 = 0.0f;
    this->unk_2BC = Rand_ZeroFloat(20.0f) + 40.0f;
    this->unk_2B8 |= 4;
}

s32 func_80968B8C(EnJs* this, PlayState* play) {
    Path* path;
    f32 temp_f0;
    f32 temp_f2;
    s32 pathIndex = ENJS_GET_PATH_INDEX(&this->actor);
    s32 i;
    f32 sp18 = 0.0f;
    Vec3s* points;

    if (pathIndex != 0x3F) {
        this->path = &play->setupPathList[pathIndex];
        if (this->path != NULL) {
            path = this->path;

            points = Lib_SegmentedToVirtual(path->points);

            for (i = 0; i < path->count; i++, points++) {
                temp_f0 = points->x - this->actor.world.pos.x;
                temp_f2 = points->z - this->actor.world.pos.z;
                temp_f0 = SQ(temp_f0) + SQ(temp_f2);
                if (i == 0) {
                    sp18 = temp_f0;
                    this->unk_2B0 = 0;
                } else if (temp_f0 < sp18) {
                    sp18 = temp_f0;
                    this->unk_2B0 = i;
                }
            }

            this->unk_2B0++;
            if (this->unk_2B0 >= path->count) {
                this->unk_2B0 = 0;
            }
        }
    } else {
        this->path = NULL;
        this->unk_2B0 = 0;
    }

    return false;
}

s32 func_80968CB8(EnJs* this) {
    Path* path;
    s32 pad;
    f32 temp_fa0;
    f32 temp_fa1;
    Vec3s* temp_v1;

    if (this->path == NULL) {
        return true;
    }

    path = this->path;
    temp_v1 = &((Vec3s*)Lib_SegmentedToVirtual(path->points))[this->unk_2B0];
    temp_fa0 = temp_v1->x - this->actor.world.pos.x;
    temp_fa1 = temp_v1->z - this->actor.world.pos.z;
    this->actor.world.rot.y = Math_Atan2S(temp_fa0, temp_fa1);

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.world.rot.y, 2, 0x7D0, 0xC8);

    if ((SQ(temp_fa0) + SQ(temp_fa1)) < SQ(10.0f)) {
        this->unk_2B0++;
        if (this->unk_2B0 >= path->count) {
            this->unk_2B0 = 0;
        }
        return true;
    }

    Math_StepToF(&this->actor.speedXZ, this->unk_2B4, 0.5f);

    return false;
}

s32 func_80968DD0(EnJs* this, PlayState* play) {
    if ((this->actor.xzDistToPlayer < 100.0f) && Player_IsFacingActor(&this->actor, 0x3000, play) &&
        Actor_IsFacingPlayer(&this->actor, 0x3000)) {
        return true;
    }

    return false;
}

s32 func_80968E38(s32 arg0) {
    s32 i;
    s32 count;
    u8 mask;
    u8* masksGivenOnMoon = gSaveContext.masksGivenOnMoon;

    if (((arg0 < 0) || (arg0 >= 9))) {
        return 0;
    }

    count = 0;

    arg0 *= 3;
    for (mask = 1, i = 0; i < 8; i++, mask <<= 1) {
        if (masksGivenOnMoon[arg0] & mask) {
            count++;
        }
    }

    arg0++;
    for (mask = 1, i = 0; i < 8; i++, mask <<= 1) {
        if (masksGivenOnMoon[arg0] & mask) {
            count++;
        }
    }

    arg0++;
    for (mask = 1, i = 0; i < 5; i++, mask <<= 1) {
        if (masksGivenOnMoon[arg0] & mask) {
            count++;
        }
    }

    return count;
}

s32 EnJs_GetRemainingMasks(void) {
    s32 count = 0;

    if (INV_CONTENT(ITEM_MASK_TRUTH) == ITEM_MASK_TRUTH) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_KAFEIS_MASK) == ITEM_MASK_KAFEIS_MASK) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_ALL_NIGHT) == ITEM_MASK_ALL_NIGHT) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_BUNNY) == ITEM_MASK_BUNNY) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_KEATON) == ITEM_MASK_KEATON) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_GARO) == ITEM_MASK_GARO) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_ROMANI) == ITEM_MASK_ROMANI) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_CIRCUS_LEADER) == ITEM_MASK_CIRCUS_LEADER) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_POSTMAN) == ITEM_MASK_POSTMAN) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_COUPLE) == ITEM_MASK_COUPLE) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_GREAT_FAIRY) == ITEM_MASK_GREAT_FAIRY) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_GIBDO) == ITEM_MASK_GIBDO) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_DON_GERO) == ITEM_MASK_DON_GERO) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_KAMARO) == ITEM_MASK_KAMARO) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_CAPTAIN) == ITEM_MASK_CAPTAIN) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_STONE) == ITEM_MASK_STONE) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_BREMEN) == ITEM_MASK_BREMEN) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_BLAST) == ITEM_MASK_BLAST) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_SCENTS) == ITEM_MASK_SCENTS) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_GIANT) == ITEM_MASK_GIANT) {
        count++;
    }

    count -= func_80968E38(0);
    return count;
}

void EnJs_TakeMask(s32 actionParams, s32 childType) {
    u8* masksGivenOnMoon = gSaveContext.masksGivenOnMoon;
    s32 temp = 0;

    if ((childType >= 0) && (childType < 9)) {
        actionParams -= PLAYER_AP_MASK_TRUTH;
        childType *= 3;
        if (actionParams < 8) {
            masksGivenOnMoon[childType] |= 1 << actionParams;
            masksGivenOnMoon[temp] |= 1 << actionParams;
            return;
        }

        actionParams -= 8;
        childType++;
        temp++;
        if (actionParams < 8) {
            masksGivenOnMoon[childType] |= 1 << actionParams;
            masksGivenOnMoon[temp] |= 1 << actionParams;
            return;
        }

        actionParams -= 8;
        childType++;
        temp++;
        if (actionParams < 6) {
            masksGivenOnMoon[childType] |= 1 << actionParams;
            masksGivenOnMoon[temp] |= 1 << actionParams;
        }
    }
}

s32 func_809692A8(s32 arg0) {
    switch (arg0) {
        case 1:
        case 2:
        case 3:
        case 4:
            if (func_80968E38(arg0) < arg0) {
                return false;
            }
            return true;
        case 5:
        case 6:
        case 7:
        case 8:
            if (func_80968E38(arg0) < (arg0 - 4)) {
                return false;
            }
            return true;
        default:
            return false;
    }
}

s32 func_8096933C(s32 arg0) {
    s32 sp1C;

    switch (arg0) {
        case 1:
        case 2:
        case 3:
        case 4:
            sp1C = EnJs_GetRemainingMasks();
            if ((func_80968E38(arg0) + sp1C) < arg0) {
                return false;
            } else {
                return true;
            }
        case 5:
        case 6:
        case 7:
        case 8:
            sp1C = EnJs_GetRemainingMasks();
            if ((func_80968E38(arg0) + sp1C) < (arg0 - 4)) {
                return false;
            } else {
                return true;
            }
        default:
            return false;
    }
}

void func_80969400(s32 arg0) {
    u8* masksGivenOnMoon = gSaveContext.masksGivenOnMoon;
    u8* temp_v0 = &gSaveContext.masksGivenOnMoon[arg0 * 3];

    if ((arg0 >= 0) && (arg0 < 9)) {
        masksGivenOnMoon[0] &= ~temp_v0[0];
        masksGivenOnMoon[1] &= ~temp_v0[1];

        temp_v0[2] &= 0xF;
        masksGivenOnMoon[2] &= ~temp_v0[2];

        temp_v0[0] = 0;
        temp_v0[1] = 0;
        temp_v0[2] = 0;
    }
}

void func_80969494(EnJs* this, PlayState* play) {
    func_80968A5C(this);
    func_801477B4(play);
    this->actor.flags &= ~ACTOR_FLAG_100;
    this->actionFunc = func_80969B5C;
}

void func_809694E8(EnJs* this, PlayState* play) {
    func_801477B4(play);
    this->actor.flags &= ~ACTOR_FLAG_100;
    this->actionFunc = func_8096A104;
}

void func_80969530(EnJs* this, PlayState* play) {
    func_801477B4(play);
    this->actor.flags &= ~ACTOR_FLAG_100;
    this->actionFunc = func_8096A6F4;
    if ((this->actor.home.rot.y == this->actor.shape.rot.y) && (this->unk_2B8 & 0x10)) {
        Animation_Change(&this->skelAnime, &gMoonChildGettingUpAnim, -1.0f,
                         Animation_GetLastFrame(&gMoonChildGettingUpAnim) - 1.0f, 0.0f, ANIMMODE_ONCE, -10.0f);
        this->unk_2B8 &= ~0x10;
        this->unk_2B8 |= 8;
    }
}

s32 func_809695FC(EnJs* this, PlayState* play) {
    if (ENJS_GET_EXIT_INDEX(&this->actor) == 0x3F) {
        return false;
    }
    play->transitionTrigger = TRANS_TRIGGER_START;
    play->nextEntrance = play->setupExitList[ENJS_GET_EXIT_INDEX(&this->actor)];
    this->actionFunc = func_8096971C;
    play->msgCtx.msgLength = 0;
    gSaveContext.respawnFlag = -2;
    return true;
}

void func_80969688(EnJs* this) {
    if (this->cutsceneIndex != -1) {
        if (ActorCutscene_GetCurrentIndex() == this->cutscenes[this->cutsceneIndex]) {
            ActorCutscene_Stop(this->cutscenes[this->cutsceneIndex]);
        }
        this->cutsceneIndex = -1;
    }
}

void func_809696EC(EnJs* this, s16 arg1) {
    func_80969688(this);
    this->cutsceneIndex = arg1;
}

void func_8096971C(EnJs* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
}

void func_80969748(EnJs* this, PlayState* play) {
    s32 itemActionParam;
    Player* player = GET_PLAYER(play);

    SkelAnime_Update(&this->skelAnime);
    Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 6, 0x1838, 0x64);
    this->actor.shape.rot.y = this->actor.world.rot.y;
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_16) {
        itemActionParam = func_80123810(play);
        if (itemActionParam != PLAYER_AP_NONE) {
            this->actionFunc = func_80969898;
        }
        if (itemActionParam > PLAYER_AP_NONE) {
            func_801477B4(play);
            if ((itemActionParam >= PLAYER_AP_MASK_TRUTH) && (itemActionParam <= PLAYER_AP_MASK_GIANT)) {
                EnJs_TakeMask(itemActionParam, ENJS_GET_TYPE(&this->actor));
                Inventory_UnequipItem(itemActionParam - 4);
                if (!func_809692A8(ENJS_GET_TYPE(&this->actor))) {
                    player->actor.textId = 0x2212;
                } else {
                    player->actor.textId = 0x2213;
                }
            } else if ((itemActionParam >= PLAYER_AP_MASK_FIERCE_DEITY) && (itemActionParam <= PLAYER_AP_MASK_DEKU)) {
                player->actor.textId = 0x2211;
            } else {
                player->actor.textId = 0x2210;
            }
        }
        if (itemActionParam <= PLAYER_AP_MINUS1) {
            func_80151938(play, 0x2216);
        }
    }
}

void func_80969898(EnJs* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);

    SkelAnime_Update(&this->skelAnime);
    Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 6, 0x1838, 0x64);
    this->actor.shape.rot.y = this->actor.world.rot.y;
    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_CHOICE:
            if (Message_ShouldAdvance(play) && (play->msgCtx.currentTextId == 0x2215)) {
                switch (play->msgCtx.choiceIndex) {
                    case 0:
                        func_8019F208();
                        func_80151938(play, 0x2217);
                        break;
                    case 1:
                        func_8019F230();
                        func_80151938(play, 0x2216);
                        break;
                }
            }
            break;
        case TEXT_STATE_5:
            if (Message_ShouldAdvance(play)) {
                switch (play->msgCtx.currentTextId) {
                    case 0x220C:
                        this->unk_2B8 |= 1;
                        if (!func_8096933C(ENJS_GET_TYPE(&this->actor))) {
                            func_80151938(play, 0x220F);
                            break;
                        }
                        func_80151938(play, 0x220D);
                        break;
                    case 0x220D:
                    case 0x2213:
                        func_80151938(play, play->msgCtx.currentTextId + 1);
                        break;
                    case 0x220E:
                        func_80151938(play, 0xFF);
                        this->actionFunc = func_80969748;
                        break;
                    case 0x2210:
                    case 0x2211:
                    case 0x2212:
                        player->exchangeItemId = PLAYER_AP_NONE;
                        func_80151938(play, 0xFF);
                        this->actionFunc = func_80969748;
                        break;
                    case 0x2214:
                    case 0x2217:
                        if (!func_809695FC(this, play)) {
                            func_80969494(this, play);
                            break;
                        }
                        break;
                    default:
                        func_80969494(this, play);
                        break;
                }
            }
            break;
    }
}

void func_80969AA0(EnJs* this, PlayState* play) {
    u16 textId;

    if (gSaveContext.save.playerForm != PLAYER_FORM_HUMAN) {
        textId = 0x220B;
    } else {
        textId = 0x2215;
        if (!func_809692A8(ENJS_GET_TYPE(&this->actor))) {
            if (this->unk_2B8 & 1) {
                if (!func_8096933C(ENJS_GET_TYPE(&this->actor))) {
                    textId = 0x220F;
                } else {
                    textId = 0x220E;
                }
            } else {
                textId = 0x220C;
            }
        } else {
            textId = 0x2215;
        }
    }
    Animation_MorphToLoop(&this->skelAnime, &gMoonChildStandingAnim, -10.0f);
    Message_StartTextbox(play, textId, &this->actor);
}

void func_80969B5C(EnJs* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->unk_2BC > 0) {
        this->unk_2BC--;
        if (this->unk_2BC == 0) {
            if (this->unk_2B8 & 4) {
                func_80968A5C(this);
            } else {
                func_80968B18(this);
            }
        }
    }
    if (Actor_ProcessTalkRequest(&this->actor, &play->state)) {
        this->actionFunc = func_80969898;
        this->actor.speedXZ = 0.0f;
        this->unk_2B4 = 0.0f;
        func_80969AA0(this, play);
    } else if ((this->actor.xzDistToPlayer < 100.0f) && Player_IsFacingActor(&this->actor, 0x3000, play)) {
        func_800B8614(&this->actor, play, 120.0f);
    }
    func_80968CB8(this);
}

void func_80969C54(EnJs* this, PlayState* play) {
    s32 itemActionParam;
    Player* player = GET_PLAYER(play);

    SkelAnime_Update(&this->skelAnime);
    Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 6, 0x1838, 0x64);
    this->actor.shape.rot.y = this->actor.world.rot.y;
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_16) {
        itemActionParam = func_80123810(play);
        if (itemActionParam != PLAYER_AP_NONE) {
            this->actionFunc = func_80969DA4;
        }
        if (itemActionParam > PLAYER_AP_NONE) {
            func_801477B4(play);
            if ((itemActionParam >= PLAYER_AP_MASK_TRUTH) && (itemActionParam <= PLAYER_AP_MASK_GIANT)) {
                EnJs_TakeMask(itemActionParam, ENJS_GET_TYPE(&this->actor));
                Inventory_UnequipItem(itemActionParam - 4);
                if (!func_809692A8(ENJS_GET_TYPE(&this->actor))) {
                    player->actor.textId = 0x2221;
                } else {
                    player->actor.textId = 0x2222;
                }
            } else if ((itemActionParam >= PLAYER_AP_MASK_FIERCE_DEITY) && (itemActionParam <= PLAYER_AP_MASK_DEKU)) {
                player->actor.textId = 0x2220;
            } else {
                player->actor.textId = 0x221D;
            }
        }
        if (itemActionParam <= PLAYER_AP_MINUS1) {
            func_80151938(play, 0x221E);
        }
    }
}

void func_80969DA4(EnJs* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);

    SkelAnime_Update(&this->skelAnime);
    Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 6, 0x1838, 0x64);
    this->actor.shape.rot.y = this->actor.world.rot.y;
    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_CHOICE:
            if (Message_ShouldAdvance(play) &&
                ((play->msgCtx.currentTextId == 0x2219) || (play->msgCtx.currentTextId == 0x221E))) {
                switch (play->msgCtx.choiceIndex) {
                    case 0:
                        func_8019F208();
                        if (!func_809695FC(this, play)) {
                            func_809694E8(this, play);
                            break;
                        }
                        break;
                    case 1:
                        func_8019F230();
                        func_80151938(play, play->msgCtx.currentTextId + 1);
                        break;
                }
            }
            break;
        case TEXT_STATE_5:
            if (Message_ShouldAdvance(play)) {
                switch (play->msgCtx.currentTextId) {
                    case 0x221B:
                        if (!func_8096933C(ENJS_GET_TYPE(&this->actor))) {
                            func_80151938(play, 0x2219);
                            break;
                        }
                        func_80151938(play, 0x221C);
                        break;
                    case 0x2224:
                    case 0x2226:
                    case 0x2228:
                    case 0x222A:
                        func_80151938(play, play->msgCtx.currentTextId + 1);
                        break;
                    case 0x2225:
                    case 0x2227:
                    case 0x2229:
                    case 0x222B:
                        if (!func_809695FC(this, play)) {
                            func_809694E8(this, play);
                            break;
                        }
                        break;
                    case 0x2222:
                        player->exchangeItemId = PLAYER_AP_NONE;
                        func_80151938(play, play->msgCtx.currentTextId + 1);
                        break;
                    case 0x2223:
                        switch (ENJS_GET_TYPE(&this->actor)) {
                            case 5:
                                func_80151938(play, 0x2224);
                                break;
                            case 6:
                                func_80151938(play, 0x2226);
                                break;
                            case 7:
                                func_80151938(play, 0x2228);
                                break;
                            case 8:
                                func_80151938(play, 0x222A);
                                break;
                        }
                        break;
                    case 0x221C:
                        func_80151938(play, 0xFF);
                        this->actionFunc = func_80969C54;
                        break;
                    case 0x221D:
                    case 0x2220:
                    case 0x2221:
                        player->exchangeItemId = PLAYER_AP_NONE;
                        func_80151938(play, 0xFF);
                        this->actionFunc = func_80969C54;
                        break;
                    default:
                        func_809694E8(this, play);
                        break;
                }
            }
            break;
    }
}

void func_8096A080(EnJs* this, PlayState* play) {
    u16 textId;

    if (gSaveContext.save.playerForm != PLAYER_FORM_HUMAN) {
        textId = 0x2218;
    } else {
        textId = 0x221B;
        if (this->unk_2B8 & 1) {
            if (!func_8096933C(ENJS_GET_TYPE(&this->actor))) {
                textId = 0x2219;
            } else {
                textId = 0x221C;
            }
        } else {
            this->unk_2B8 |= 1;
        }
    }
    Message_StartTextbox(play, textId, &this->actor);
}

void func_8096A104(EnJs* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (Actor_ProcessTalkRequest(&this->actor, &play->state)) {
        this->actionFunc = func_80969DA4;
        func_8096A080(this, play);
    } else if (func_80968DD0(this, play)) {
        func_800B8614(&this->actor, play, 120.0f);
    }
}

void func_8096A184(EnJs* this, PlayState* play) {
    u16 textId;

    if (gSaveContext.save.playerForm != PLAYER_FORM_HUMAN) {
        textId = 0x220B;
    } else if (func_80968E38(0) >= 20) {
        textId = 0x2202;
    } else {
        textId = 0x21FC;
    }
    Message_StartTextbox(play, textId, &this->actor);
}

void func_8096A1E8(EnJs* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        Animation_MorphToLoop(&this->skelAnime, &gMoonChildStandingAnim, 0.0f);
    }
    if (Actor_ProcessTalkRequest(&this->actor, &play->state)) {
        this->actor.flags &= ~ACTOR_FLAG_10000;
        this->actionFunc = func_8096A38C;
        Message_StartTextbox(play, 0x2208, &this->actor);
        gSaveContext.save.weekEventReg[84] |= 0x20;
        func_809696EC(this, 0);
    } else {
        func_800B8500(&this->actor, play, 1000.0f, 1000.0f, PLAYER_AP_MINUS1);
    }
}

void func_8096A2C0(EnJs* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        Animation_MorphToLoop(&this->skelAnime, &gMoonChildStandingAnim, 0.0f);
    }
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actor.flags |= ACTOR_FLAG_10000;
        this->actionFunc = func_8096A1E8;
        func_800B8500(&this->actor, play, 1000.0f, 1000.0f, PLAYER_AP_MINUS1);
    } else {
        Actor_PickUp(&this->actor, play, GI_MASK_FIERCE_DEITY, 10000.0f, 1000.0f);
    }
}

void func_8096A38C(EnJs* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        Animation_MorphToLoop(&this->skelAnime, &gMoonChildStandingAnim, 0.0f);
        this->unk_2B8 |= 2;
    }
    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_CHOICE:
            if (Message_ShouldAdvance(play)) {
                switch (play->msgCtx.choiceIndex) {
                    case 0:
                        func_8019F208();
                        break;

                    case 1:
                        func_8019F230();
                        break;
                }

                switch (play->msgCtx.currentTextId) {
                    case 0x21FC:
                        switch (play->msgCtx.choiceIndex) {
                            case 0:
                                func_80151938(play, 0x21FE);
                                Animation_MorphToPlayOnce(&this->skelAnime, &gMoonChildGettingUpAnim, -5.0f);
                                this->unk_2B8 |= 0x10;
                                break;
                            case 1:
                                func_80151938(play, 0x21FD);
                                break;
                        }
                        break;

                    case 0x21FE:
                        switch (play->msgCtx.choiceIndex) {
                            case 0:
                                func_80151938(play, 0x2200);
                                func_809696EC(this, 0);
                                break;
                            case 1:
                                func_80151938(play, 0x21FF);
                                break;
                        }
                        break;

                    case 0x2203:
                        switch (play->msgCtx.choiceIndex) {
                            case 0:
                                func_80151938(play, 0x2205);
                                Animation_MorphToPlayOnce(&this->skelAnime, &gMoonChildGettingUpAnim, -5.0f);
                                this->unk_2B8 |= 0x10;
                                break;
                            case 1:
                                func_80151938(play, 0x2204);
                                break;
                        }
                }
            }
            break;
        case TEXT_STATE_5:
            if (Message_ShouldAdvance(play)) {
                switch (play->msgCtx.currentTextId) {
                    case 0x2202:
                    case 0x2205:
                    case 0x2206:
                    case 0x2209:
                        func_80151938(play, play->msgCtx.currentTextId + 1);
                        break;

                    case 0x2200:
                    case 0x2208:
                        func_809696EC(this, 1);
                        func_80151938(play, play->msgCtx.currentTextId + 1);
                        break;

                    case 0x2207:
                        if (INV_CONTENT(ITEM_MASK_FIERCE_DEITY) == ITEM_MASK_FIERCE_DEITY) {
                            func_80151938(play, 0x2208);
                            func_809696EC(this, 0);
                        } else {
                            func_801477B4(play);
                            this->actionFunc = func_8096A2C0;
                            func_8096A2C0(this, play);
                        }
                        break;

                    case 0x2201:

                    case 0x220A:
                        if (!func_809695FC(this, play)) {
                            func_80969530(this, play);
                        }
                        break;

                    default:
                        func_80969530(this, play);
                        break;
                }
            }
            break;
    }
}

void func_8096A6F4(EnJs* this, PlayState* play) {
    s32 pad;

    if (this->actor.home.rot.y != this->actor.shape.rot.y) {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.home.rot.y, 2, 0x400, 0x100);
        if ((this->actor.home.rot.y == this->actor.shape.rot.y) && (this->unk_2B8 & 0x10)) {
            Animation_Change(&this->skelAnime, &gMoonChildGettingUpAnim, -1.0f,
                             Animation_GetLastFrame(&gMoonChildGettingUpAnim) - 1.0f, 0.0f, ANIMMODE_ONCE, -10.0f);
            this->unk_2B8 &= ~0x10;
            this->unk_2B8 |= 8;
        }
        this->actor.world.rot.y = this->actor.shape.rot.y;
    }
    if (SkelAnime_Update(&this->skelAnime)) {
        Animation_MorphToLoop(&this->skelAnime, &gMoonChildSittingAnim, -10.0f);
        this->unk_2B8 &= ~8;
    }
    if (Actor_ProcessTalkRequest(&this->actor, &play->state)) {
        this->actionFunc = func_8096A38C;
        this->unk_2B8 &= ~2;
        func_8096A184(this, play);
        return;
    }
    if (!(this->unk_2B8 & 8) && (this->actor.xzDistToPlayer < 100.0f) &&
        Player_IsFacingActor(&this->actor, 0x3000, play) && Actor_IsFacingPlayer(&this->actor, 0x1000)) {
        func_800B8614(&this->actor, play, 120.0f);
    }
}

void EnJs_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnJs* this = THIS;

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);

    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 40.0f, 25.0f, 40.0f, 5);

    this->actionFunc(this, play);

    if ((this->cutsceneIndex != -1) && (ActorCutscene_GetCurrentIndex() != this->cutscenes[this->cutsceneIndex])) {
        if (ActorCutscene_GetCurrentIndex() == 0x7C) {
            ActorCutscene_Stop(0x7C);
            ActorCutscene_SetIntentToPlay(this->cutscenes[this->cutsceneIndex]);
        } else if (ActorCutscene_GetCanPlayNext(this->cutscenes[this->cutsceneIndex])) {
            ActorCutscene_Start(this->cutscenes[this->cutsceneIndex], &this->actor);
        } else {
            ActorCutscene_SetIntentToPlay(this->cutscenes[this->cutsceneIndex]);
        }
    }
}

void func_8096A9F4(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    s32 pad;
    EnJs* this = THIS;

    if (limbIndex == MOONCHILD_LIMB_HEAD) {
        Matrix_MultVec3f(&D_8096AC30, &thisx->focus.pos);
        if ((this->maskType >= 0) && (this->maskType < 5)) {
            OPEN_DISPS(play->state.gfxCtx);

            if (this->maskType != 0) {
                Matrix_Scale(D_8096ABE0[this->maskType], D_8096ABE0[this->maskType], D_8096ABE0[this->maskType],
                             MTXMODE_APPLY);
                Matrix_RotateZYX(0, -0x4000, -0x36B0, MTXMODE_APPLY);
            }
            Matrix_Translate(D_8096ABF4[this->maskType], D_8096AC08[this->maskType], D_8096AC1C[this->maskType],
                             MTXMODE_APPLY);
            gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_OPA_DISP++, D_8096ABCC[this->maskType]);

            CLOSE_DISPS(play->state.gfxCtx);
        }
    }
}

void EnJs_Draw(Actor* thisx, PlayState* play) {
    EnJs* this = THIS;

    func_8012C28C(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          func_8096A9F4, &this->actor);
}
