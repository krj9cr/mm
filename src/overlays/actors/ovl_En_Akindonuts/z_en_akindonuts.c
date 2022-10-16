/*
 * File: z_en_akindonuts.c
 * Overlay: ovl_En_Akindonuts
 * Description: Trade quest Business Scrubs
 */

#include "z_en_akindonuts.h"
#include "objects/object_dnt/object_dnt.h"

#define FLAGS (ACTOR_FLAG_1 | ACTOR_FLAG_8 | ACTOR_FLAG_10 | ACTOR_FLAG_20)

#define THIS ((EnAkindonuts*)thisx)

void EnAkindonuts_Init(Actor* thisx, PlayState* play);
void EnAkindonuts_Destroy(Actor* thisx, PlayState* play);
void EnAkindonuts_Update(Actor* thisx, PlayState* play);
void EnAkindonuts_Draw(Actor* thisx, PlayState* play);

void func_80BEEDC0(EnAkindonuts* this, PlayState* play);
void func_80BEEE10(EnAkindonuts* this, PlayState* play);
void func_80BEEFA8(EnAkindonuts* this, PlayState* play);
void func_80BEF18C(EnAkindonuts* this, PlayState* play);
void func_80BEF20C(EnAkindonuts* this, PlayState* play);
void func_80BEF360(EnAkindonuts* this, PlayState* play);
void func_80BEF450(EnAkindonuts* this, PlayState* play);
void func_80BEF4B8(EnAkindonuts* this, PlayState* play);
void func_80BEF518(EnAkindonuts* this, PlayState* play);
void func_80BEF770(EnAkindonuts* this, PlayState* play);
void func_80BEF83C(EnAkindonuts* this, PlayState* play);
void func_80BEF9F0(EnAkindonuts* this, PlayState* play);
void func_80BEFAF0(EnAkindonuts* this, PlayState* play);
void func_80BEFD74(EnAkindonuts* this, PlayState* play);

const ActorInit En_Akindonuts_InitVars = {
    ACTOR_EN_AKINDONUTS,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_DNT,
    sizeof(EnAkindonuts),
    (ActorFunc)EnAkindonuts_Init,
    (ActorFunc)EnAkindonuts_Destroy,
    (ActorFunc)EnAkindonuts_Update,
    (ActorFunc)EnAkindonuts_Draw,
};

static ColliderCylinderInitType1 sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
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
    { 27, 32, 0, { 0, 0, 0 } },
};

static AnimationInfoS sAnimationInfo[] = {
    { &object_dnt_Anim_005488, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },
    { &object_dnt_Anim_00B0B4, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },
    { &object_dnt_Anim_004AA0, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },
    { &object_dnt_Anim_004E38, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },
    { &object_dnt_Anim_0029E8, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },
    { &object_dnt_Anim_005CA8, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },
    { &object_dnt_Anim_0038CC, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },
    { &object_dnt_Anim_003CC0, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },
    { &object_dnt_Anim_0012F4, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },
    { &object_dnt_Anim_004700, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },
    { &object_dnt_Anim_001BC8, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },
    { &object_dnt_Anim_003438, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },
    { &object_dnt_Anim_001E2C, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },
    { &object_dnt_Anim_000994, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },
    { &object_dnt_Anim_002268, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },
    { &object_dnt_Anim_002F08, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },
    { &object_dnt_Anim_00577C, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },
    { &object_dnt_Anim_0029E8, 1.0f, 8, -1, ANIMMODE_ONCE, 0 },
    { &object_dnt_Anim_0029E8, 1.0f, 4, -1, ANIMMODE_ONCE, -4 },
    { &object_dnt_Anim_0029E8, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },
    { &object_dnt_Anim_001BC8, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },
    { &object_dnt_Anim_0012F4, -1.0f, 0, -1, ANIMMODE_ONCE, 0 },
    { &object_dnt_Anim_002670, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },
};

static u16 D_80BF048C[] = {
    0x15E4,
    0x15F8,
    0x1609,
    0x161D,
};

static u16 D_80BF0494[] = {
    0x15E3,
    0x15F9,
    0x160A,
    0x161E,
};

static u16 D_80BF049C[] = {
    0x15E3,
    0x15F7,
    0x160C,
    0x161F,
};

static u16 D_80BF04A4[] = {
    0x15E3,
    0x15F7,
    0x160B,
    0x1621,
};

static u16 D_80BF04AC[] = {
    0x15E3,
    0x15F7,
    0x160B,
    0x1620,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 0, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 30, ICHAIN_STOP),
};

void func_80BECBE0(EnAkindonuts* this, s16 arg1) {
    f32 sp24 = Math_CosS(this->actor.world.rot.x) * this->actor.speedXZ;

    this->actor.velocity.x = Math_SinS(this->actor.world.rot.y) * sp24;
    this->actor.velocity.y = Math_SinS(this->actor.world.rot.x) * this->actor.speedXZ;
    this->actor.velocity.z = Math_CosS(this->actor.world.rot.y) * sp24;

    if (arg1) {
        this->actor.velocity.x = 0.0f;
        this->actor.velocity.z = 0.0f;
    }

    Actor_UpdatePos(&this->actor);
}

void func_80BECC7C(EnAkindonuts* this, PlayState* play) {
    if (this->unk_32C & 4) {
        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }

    if (this->unk_32C & 2) {
        Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 20.0f, 5);
    }
}

s32 func_80BECD10(EnAkindonuts* this, Path* path, s32 arg2) {
    Vec3s* sp5C = Lib_SegmentedToVirtual(path->points);
    s32 sp58 = path->count;
    s32 idx = arg2;
    s32 sp50 = false;
    f32 phi_f12;
    f32 phi_f14;
    f32 sp44;
    f32 sp40;
    f32 sp3C;
    Vec3f sp30;

    Math_Vec3s_ToVec3f(&sp30, &sp5C[idx]);

    if (idx == 0) {
        phi_f12 = sp5C[1].x - sp5C[0].x;
        phi_f14 = sp5C[1].z - sp5C[0].z;
    } else if (idx == (sp58 - 1)) {
        phi_f12 = sp5C[sp58 - 1].x - sp5C[sp58 - 2].x;
        phi_f14 = sp5C[sp58 - 1].z - sp5C[sp58 - 2].z;
    } else {
        phi_f12 = sp5C[idx + 1].x - sp5C[idx - 1].x;
        phi_f14 = sp5C[idx + 1].z - sp5C[idx - 1].z;
    }

    func_8017B7F8(&sp30, RADF_TO_BINANG(func_80086B30(phi_f12, phi_f14)), &sp44, &sp40, &sp3C);
    if (((this->actor.world.pos.x * sp44) + (sp40 * this->actor.world.pos.z) + sp3C) > 0.0f) {
        sp50 = true;
    }

    return sp50;
}

f32 func_80BECEAC(Path* path, s32 arg1, Vec3f* pos, Vec3s* arg3) {
    Vec3s* temp;
    Vec3f sp20;
    Vec3s* point;

    if (path != NULL) {
        temp = Lib_SegmentedToVirtual(path->points);
        point = &temp[arg1];

        sp20.x = point->x;
        sp20.y = point->y;
        sp20.z = point->z;
    }

    arg3->y = Math_Vec3f_Yaw(pos, &sp20);
    arg3->x = Math_Vec3f_Pitch(pos, &sp20);

    return sp20.y - pos->y;
}

s16 func_80BECF6C(Path* path) {
    Vec3s* sp34 = Lib_SegmentedToVirtual(path->points);
    Vec3f sp28;
    Vec3f sp1C;

    Math_Vec3s_ToVec3f(&sp28, &sp34[0]);
    Math_Vec3s_ToVec3f(&sp1C, &sp34[1]);

    return Math_Vec3f_Yaw(&sp28, &sp1C);
}

s32 func_80BECFBC(EnAkindonuts* this) {
    switch (ENAKINDONUTS_GET_3(&this->actor)) {
        case 0:
            return GI_DEED_SWAMP;

        case 1:
            return GI_DEED_MOUNTAIN;

        case 2:
            return GI_DEED_OCEAN;

        case 3:
            Inventory_DeleteItem(ITEM_DEED_OCEAN, SLOT(ITEM_DEED_OCEAN));
            return GI_RUPEE_HUGE;
    }
    return GI_NONE;
}

s32 func_80BED034(EnAkindonuts* this) {
    switch (ENAKINDONUTS_GET_3(&this->actor)) {
        case 0:
            return GI_MAGIC_BEANS;

        case 1:
            return GI_BOMB_BAG_40;

        case 2:
            return GI_POTION_GREEN;

        case 3:
            return GI_POTION_BLUE;
    }
    return GI_NONE;
}

void func_80BED090(PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (player->transformation == PLAYER_FORM_DEKU) {
        gSaveContext.save.weekEventReg[63] |= 8;
        gSaveContext.save.weekEventReg[63] &= (u8)~0x10;
    } else if (player->transformation == PLAYER_FORM_ZORA) {
        gSaveContext.save.weekEventReg[63] &= (u8)~8;
        gSaveContext.save.weekEventReg[63] |= 0x10;
    } else if (player->transformation == PLAYER_FORM_GORON) {
        gSaveContext.save.weekEventReg[63] |= 8;
        gSaveContext.save.weekEventReg[63] |= 0x10;
    } else if (player->transformation == PLAYER_FORM_HUMAN) {
        gSaveContext.save.weekEventReg[63] &= (u8)~8;
        gSaveContext.save.weekEventReg[63] &= (u8)~0x10;
    }
}

s32 func_80BED140(PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (player->transformation == PLAYER_FORM_DEKU) {
        if ((gSaveContext.save.weekEventReg[63] & 8) && !(gSaveContext.save.weekEventReg[63] & 0x10)) {
            return true;
        }
    } else if (player->transformation == PLAYER_FORM_ZORA) {
        if (!(gSaveContext.save.weekEventReg[63] & 8) && (gSaveContext.save.weekEventReg[63] & 0x10)) {
            return true;
        }
    } else if (player->transformation == PLAYER_FORM_GORON) {
        if ((gSaveContext.save.weekEventReg[63] & 8) && (gSaveContext.save.weekEventReg[63] & 0x10)) {
            return true;
        }
    } else if (player->transformation == PLAYER_FORM_HUMAN) {
        if (!(gSaveContext.save.weekEventReg[63] & 8) && !(gSaveContext.save.weekEventReg[63] & 0x10)) {
            return true;
        }
    }

    return false;
}

s32 func_80BED208(EnAkindonuts* this) {
    if ((u32)INV_CONTENT(ITEM_MAGIC_BEANS) != ITEM_MAGIC_BEANS) {
        return 0;
    }

    if (gSaveContext.save.playerData.rupees < 10) {
        return 1;
    }

    if (AMMO(ITEM_MAGIC_BEANS) >= 20) {
        return 2;
    }

    this->unk_364 = -10;
    this->unk_366 = 10;
    return 3;
}

s32 func_80BED27C(EnAkindonuts* this) {
    if (GET_CUR_UPG_VALUE(UPG_BOMB_BAG) == 3) {
        return 2;
    }

    if (GET_CUR_UPG_VALUE(UPG_BOMB_BAG) < 2) {
        return 0;
    }

    if (gSaveContext.save.playerData.rupees < 200) {
        return 1;
    }

    this->unk_364 = -200;
    this->unk_366 = 88;
    return 3;
}

s32 func_80BED2FC(EnAkindonuts* this) {
    if (!Inventory_HasEmptyBottle()) {
        return 2;
    }

    if (gSaveContext.save.playerData.rupees < 40) {
        return 1;
    }

    this->unk_364 = -40;
    this->unk_366 = 20;
    return 3;
}

s32 func_80BED35C(EnAkindonuts* this) {
    if (!Inventory_HasEmptyBottle()) {
        return 2;
    }

    if (gSaveContext.save.playerData.rupees < 100) {
        return 1;
    }

    this->unk_364 = -100;
    this->unk_366 = 21;
    return 3;
}

void func_80BED3BC(EnAkindonuts* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    switch (this->unk_33C) {
        case 0:
            if (player->transformation != PLAYER_FORM_DEKU) {
                this->unk_33C = 0x15E0;
                break;
            }
            this->unk_33C = 0x15E7;
            break;

        case 0x15E0:
            this->unk_33C = 0x15E1;
            break;

        case 0x15E1:
            this->unk_33C = 0x15E2;
            break;

        case 0x15E2:
            this->unk_33C = 0xFF;
            break;

        case 0x15E7:
            if (!(gSaveContext.save.weekEventReg[61] & 0x20)) {
                gSaveContext.save.weekEventReg[61] |= 0x20;
                this->unk_33C = 0x15E8;
                break;
            }
            this->unk_33C = 0x15E9;
            break;

        case 0x15E8:
            this->unk_33C = 0x15E9;
            break;

        case 0x15E9:
            this->unk_33C = 0x15EA;
            break;

        case 0x15EA:
            this->unk_32C |= 0x1;
            if (this->unk_32C & 8) {
                this->unk_32C &= ~8;

                switch (func_80BED208(this)) {
                    case 0:
                        play_sound(NA_SE_SY_ERROR);
                        this->unk_33C = 0x15EC;
                        break;

                    case 1:
                        play_sound(NA_SE_SY_ERROR);
                        this->unk_33C = 0x15ED;
                        break;

                    case 2:
                        play_sound(NA_SE_SY_ERROR);
                        this->unk_33C = 0x15EE;
                        break;

                    case 3:
                        func_8019F208();
                        this->unk_32C &= ~0x1;
                        this->unk_32C |= 0x40;
                        play->msgCtx.msgMode = 0x43;
                        play->msgCtx.stateTimer = 4;
                        this->unk_33C = 0x15EF;
                        this->actionFunc = func_80BEF360;
                        return;
                }
            } else if (this->unk_32C & 0x10) {
                this->unk_32C &= ~0x10;
                this->unk_33C = 0x15EB;
                break;
            }
            break;

        case 0x15E4:
            this->unk_33C = 0x15E5;
            this->unk_32C |= 1;
            break;

        case 0x15E5:
            this->unk_33C = 0x15E6;
            func_80BED090(play);
            gSaveContext.save.weekEventReg[61] |= 0x10;
            this->unk_32C |= 0x20;
            break;

        case 0x15EF:
            this->unk_32C |= 1;
            break;
    }

    Message_StartTextbox(play, this->unk_33C, &this->actor);
}

void func_80BED680(EnAkindonuts* this, PlayState* play) {
    switch (this->unk_33C) {
        case 0:
            if (func_80BED140(play) && !(gSaveContext.save.weekEventReg[61] & 0x40)) {
                gSaveContext.save.weekEventReg[61] |= 0x40;
                this->unk_33C = 0x15F0;
                break;
            }

            this->unk_33C = 0x15F2;
            break;

        case 0x15F0:
            this->unk_33C = 0x15F1;
            break;

        case 0x15F1:
        case 0x15F2:
            this->unk_33C = 0x15F3;
            break;

        case 0x15F3:
            this->unk_33C = 0x15EA;
            break;

        case 0x15EA:
            this->unk_32C |= 1;
            if (this->unk_32C & 8) {
                this->unk_32C &= ~8;

                switch (func_80BED208(this)) {
                    case 0:
                        play_sound(NA_SE_SY_ERROR);
                        this->unk_33C = 0x15EC;
                        break;

                    case 1:
                        play_sound(NA_SE_SY_ERROR);
                        this->unk_33C = 0x15ED;
                        break;

                    case 2:
                        play_sound(NA_SE_SY_ERROR);
                        this->unk_33C = 0x15EE;
                        break;

                    case 3:
                        func_8019F208();
                        this->unk_32C |= 0x40;
                        this->unk_32C &= ~0x1;
                        play->msgCtx.msgMode = 0x43;
                        play->msgCtx.stateTimer = 4;
                        this->unk_33C = 0x15EF;
                        this->actionFunc = func_80BEF360;
                        return;
                }
            } else if (this->unk_32C & 0x10) {
                this->unk_32C &= ~0x10;
                this->unk_33C = 0x15EB;
                break;
            }
            break;

        case 0x15EF:
            this->unk_32C |= 1;
            break;
    }

    Message_StartTextbox(play, this->unk_33C, &this->actor);
}

void func_80BED8A4(EnAkindonuts* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    switch (this->unk_33C) {
        case 0:
            if (player->transformation == PLAYER_FORM_DEKU) {
                this->unk_33C = 0x15F4;
                break;
            } else if (player->transformation == PLAYER_FORM_GORON) {
                this->unk_33C = 0x15FE;
                break;
            }
            this->unk_33C = 0x15FC;
            break;

        case 0x15F4:
            this->unk_33C = 0x15F5;
            break;

        case 0x15F5:
            this->unk_33C = 0x15F6;
            break;

        case 0x15F6:
            this->unk_33C = 0xFF;
            break;

        case 0x15FC:
            this->unk_33C = 0x15FD;
            this->unk_32C |= 1;
            break;

        case 0x15FE:
            if (!(gSaveContext.save.weekEventReg[62] & 1)) {
                gSaveContext.save.weekEventReg[62] |= 1;
                this->unk_33C = 0x15FF;
                break;
            }
            this->unk_33C = 0x1600;
            break;

        case 0x15FF:
            this->unk_33C = 0x1600;
            break;

        case 0x1600:
            this->unk_33C = 0x15EA;
            break;

        case 0x15EA:
            this->unk_32C |= 0x1;
            if (this->unk_32C & 8) {
                this->unk_32C &= ~8;

                switch (func_80BED27C(this)) {
                    case 2:
                        play_sound(NA_SE_SY_ERROR);
                        this->unk_33C = 0x1601;
                        break;

                    case 0:
                        play_sound(NA_SE_SY_ERROR);
                        this->unk_33C = 0x1602;
                        break;

                    case 1:
                        play_sound(NA_SE_SY_ERROR);
                        this->unk_33C = 0x1603;
                        break;

                    case 3:
                        func_8019F208();
                        this->unk_32C |= 0x40;
                        this->unk_32C &= ~0x1;
                        play->msgCtx.msgMode = 0x43;
                        play->msgCtx.stateTimer = 4;
                        this->unk_33C = 0x15EF;
                        this->actionFunc = func_80BEF360;
                        return;
                }
            } else if (this->unk_32C & 0x10) {
                this->unk_32C &= ~0x10;
                this->unk_33C = 0x1604;
                break;
            }
            break;

        case 0x15F9:
            this->unk_33C = 0x15FA;
            this->unk_32C |= 1;
            break;

        case 0x15FA:
            this->unk_33C = 0x15FB;
            gSaveContext.save.weekEventReg[61] |= 0x80;
            this->unk_32C |= 0x20;
            break;

        case 0x15EF:
            this->unk_32C |= 1;
            break;
    }

    Message_StartTextbox(play, this->unk_33C, &this->actor);
}

void func_80BEDB88(EnAkindonuts* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    switch (this->unk_33C) {
        case 0:
            if ((player->transformation == PLAYER_FORM_DEKU) && !(gSaveContext.save.weekEventReg[62] & 2)) {
                gSaveContext.save.weekEventReg[62] |= 2;
                this->unk_33C = 0x15F0;
                break;
            }
            this->unk_33C = 0x15F2;
            break;

        case 0x15F0:
            this->unk_33C = 0x1605;
            break;

        case 0x15F2:
        case 0x1605:
            this->unk_33C = 0x1606;
            break;

        case 0x1606:
            this->unk_33C = 0x15EA;
            break;

        case 0x15EA:
            this->unk_32C |= 0x1;
            if (this->unk_32C & 8) {
                this->unk_32C &= ~8;

                switch (func_80BED27C(this)) {
                    case 2:
                        play_sound(NA_SE_SY_ERROR);
                        this->unk_33C = 0x1601;
                        break;

                    case 0:
                        play_sound(NA_SE_SY_ERROR);
                        this->unk_33C = 0x1602;
                        break;

                    case 1:
                        play_sound(NA_SE_SY_ERROR);
                        this->unk_33C = 0x1603;
                        break;

                    case 3:
                        func_8019F208();
                        this->unk_32C |= 0x40;
                        this->unk_32C &= ~0x1;
                        play->msgCtx.msgMode = 0x43;
                        play->msgCtx.stateTimer = 4;
                        this->unk_33C = 0x15EF;
                        this->actionFunc = func_80BEF360;
                        return;
                }
            } else if (this->unk_32C & 0x10) {
                this->unk_32C &= ~0x10;
                this->unk_33C = 0x15EB;
                break;
            }
            break;

        case 0x15EF:
            this->unk_32C |= 1;
            break;
    }

    Message_StartTextbox(play, this->unk_33C, &this->actor);
}

void func_80BEDDAC(EnAkindonuts* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    switch (this->unk_33C) {
        case 0:
            if (player->transformation == PLAYER_FORM_GORON) {
                this->unk_33C = 0x1607;
                break;
            } else if (player->transformation == PLAYER_FORM_ZORA) {
                this->unk_33C = 0x1610;
                break;
            }
            this->unk_33C = 0x160E;
            break;

        case 0x1607:
            this->unk_33C = 0x1608;
            break;

        case 0x1608:
            this->unk_33C = 0x15E2;
            break;

        case 0x15E2:
            this->unk_33C = 0xFF;
            break;

        case 0x160E:
            this->unk_33C = 0x160F;
            this->unk_32C |= 1;
            break;

        case 0x1610:
            if (!(gSaveContext.save.weekEventReg[62] & 8)) {
                gSaveContext.save.weekEventReg[62] |= 8;
                this->unk_33C = 0x1611;
                break;
            }
            this->unk_33C = 0x1612;
            break;

        case 0x1611:
            this->unk_33C = 0x1612;
            break;

        case 0x1612:
            this->unk_33C = 0x15EA;
            break;

        case 0x15EA:
            this->unk_32C |= 0x1;
            if (this->unk_32C & 0x8) {
                this->unk_32C &= ~8;

                switch (func_80BED2FC(this)) {
                    case 2:
                        play_sound(NA_SE_SY_ERROR);
                        this->unk_33C = 0x1613;
                        break;

                    case 1:
                        play_sound(NA_SE_SY_ERROR);
                        this->unk_33C = 0x15ED;
                        break;

                    case 3:
                        func_8019F208();
                        this->unk_32C |= 0x40;
                        this->unk_32C &= ~0x1;
                        play->msgCtx.msgMode = 0x43;
                        play->msgCtx.stateTimer = 4;
                        this->unk_33C = 0x15EF;
                        this->actionFunc = func_80BEF360;
                        return;
                }
            } else if (this->unk_32C & 0x10) {
                this->unk_32C &= ~0x10;
                this->unk_33C = 0x15EB;
                break;
            }
            break;

        case 0x160C:
            this->unk_33C = 0x15FA;
            this->unk_32C |= 1;
            break;

        case 0x15FA:
            this->unk_33C = 0x160D;
            gSaveContext.save.weekEventReg[62] |= 4;
            this->unk_32C |= 0x20;
            break;

        case 0x15EF:
            this->unk_32C |= 1;
            break;
    }

    Message_StartTextbox(play, this->unk_33C, &this->actor);
}

void func_80BEE070(EnAkindonuts* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    switch (this->unk_33C) {
        case 0:
            if ((player->transformation == PLAYER_FORM_GORON) && !(gSaveContext.save.weekEventReg[62] & 0x10)) {
                gSaveContext.save.weekEventReg[62] |= 0x10;
                this->unk_33C = 0x1614;
                break;
            }
            this->unk_33C = 0x1616;
            break;

        case 0x1614:
            this->unk_33C = 0x1615;
            break;

        case 0x1615:
        case 0x1616:
            this->unk_33C = 0x1617;
            break;

        case 0x1617:
            this->unk_33C = 0x1618;
            break;

        case 0x1618:
            this->unk_32C |= 0x1;
            if (this->unk_32C & 0x8) {
                this->unk_32C &= ~8;

                switch (func_80BED2FC(this)) {
                    case 2:
                        play_sound(NA_SE_SY_ERROR);
                        this->unk_33C = 0x1613;
                        break;

                    case 1:
                        play_sound(NA_SE_SY_ERROR);
                        this->unk_33C = 0x15ED;
                        break;

                    case 3:
                        func_8019F208();
                        this->unk_32C |= 0x40;
                        this->unk_32C &= ~0x1;
                        play->msgCtx.msgMode = 0x43;
                        play->msgCtx.stateTimer = 4;
                        this->unk_33C = 0x161A;
                        this->actionFunc = func_80BEF360;
                        return;
                }
            } else if (this->unk_32C & 0x10) {
                this->unk_32C &= ~0x10;
                this->unk_33C = 0x1619;
                break;
            }
            break;

        case 0x161A:
            this->unk_32C |= 1;
            break;
    }

    Message_StartTextbox(play, this->unk_33C, &this->actor);
}

void func_80BEE274(EnAkindonuts* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    switch (this->unk_33C) {
        case 0:
            if (player->transformation == PLAYER_FORM_ZORA) {
                this->unk_33C = 0x161B;
                break;
            }
            this->unk_33C = 0x1624;
            break;

        case 0x161B:
            this->unk_33C = 0x161C;
            break;

        case 0x161C:
            this->unk_33C = 0x15E2;
            break;

        case 0x15E2:
            this->unk_33C = 0xFF;
            break;

        case 0x1624:
            if (!(gSaveContext.save.weekEventReg[62] & 0x40)) {
                gSaveContext.save.weekEventReg[62] |= 0x40;
                this->unk_33C = 0x1625;
                break;
            }
            this->unk_33C = 0x1627;
            break;

        case 0x1625:
        case 0x1627:
            this->unk_33C = 0x1626;
            break;

        case 0x1626:
            this->unk_33C = 0x15EA;
            break;

        case 0x15EA:
            this->unk_32C |= 0x1;
            if (this->unk_32C & 0x8) {
                this->unk_32C &= ~8;

                switch (func_80BED35C(this)) {
                    case 2:
                        play_sound(NA_SE_SY_ERROR);
                        this->unk_33C = 0x1613;
                        break;

                    case 1:
                        play_sound(NA_SE_SY_ERROR);
                        this->unk_33C = 0x15ED;
                        break;

                    case 3:
                        func_8019F208();
                        this->unk_32C |= 0x40;
                        this->unk_32C &= ~0x1;
                        play->msgCtx.msgMode = 0x43;
                        play->msgCtx.stateTimer = 4;
                        this->unk_33C = 0x1629;
                        this->actionFunc = func_80BEF360;
                        return;
                }
            } else if (this->unk_32C & 0x10) {
                this->unk_32C &= ~0x10;
                this->unk_33C = 0x1628;
                break;
            }
            break;

        case 0x1621:
            this->unk_33C = 0x1622;
            this->unk_32C |= 1;
            break;

        case 0x1622:
            this->unk_33C = 0x1623;
            gSaveContext.save.weekEventReg[62] |= 0x20;
            this->unk_32C |= 0x20;
            break;

        case 0x1629:
            this->unk_32C |= 1;
            break;
    }

    Message_StartTextbox(play, this->unk_33C, &this->actor);
}

void func_80BEE530(EnAkindonuts* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    switch (this->unk_33C) {
        case 0:
            if ((player->transformation == PLAYER_FORM_ZORA) && !(gSaveContext.save.weekEventReg[62] & 0x80)) {
                gSaveContext.save.weekEventReg[62] |= 0x80;
                this->unk_33C = 0x162A;
                break;
            }
            this->unk_33C = 0x162C;
            break;

        case 0x162A:
            this->unk_33C = 0x162B;
            break;

        case 0x162B:
        case 0x162C:
            this->unk_33C = 0x162D;
            break;

        case 0x162D:
            this->unk_33C = 0x15EA;
            break;

        case 0x15EA:
            this->unk_32C |= 0x1;
            if (this->unk_32C & 0x8) {
                this->unk_32C &= ~8;

                switch (func_80BED35C(this)) {
                    case 2:
                        play_sound(NA_SE_SY_ERROR);
                        this->unk_33C = 0x1613;
                        break;

                    case 1:
                        play_sound(NA_SE_SY_ERROR);
                        this->unk_33C = 0x15ED;
                        break;

                    case 3:
                        func_8019F208();
                        this->unk_32C |= 0x40;
                        this->unk_32C &= ~0x1;
                        play->msgCtx.msgMode = 0x43;
                        play->msgCtx.stateTimer = 4;
                        this->unk_33C = 0x15EF;
                        this->actionFunc = func_80BEF360;
                        return;
                }
            } else if (this->unk_32C & 0x10) {
                this->unk_32C &= ~0x10;
                this->unk_33C = 0x162E;
                break;
            }
            break;

        case 0x15EF:
            this->unk_32C |= 1;
            break;
    }

    Message_StartTextbox(play, this->unk_33C, &this->actor);
}

void func_80BEE73C(EnAkindonuts* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 pad;
    s32 params = ENAKINDONUTS_GET_3(&this->actor);
    s32 itemActionParam = func_80123810(play);

    if (itemActionParam > PLAYER_AP_NONE) {
        if (itemActionParam == PLAYER_AP_DEED_LAND) {
            player->actor.textId = D_80BF048C[params];
            this->unk_33C = player->actor.textId;
            if (this->unk_33C == 0x15E4) {
                player->exchangeItemId = itemActionParam;
                this->actionFunc = func_80BEF20C;
            } else {
                this->actionFunc = func_80BEF18C;
            }
        } else if (itemActionParam == PLAYER_AP_DEED_SWAMP) {
            player->actor.textId = D_80BF0494[params];
            this->unk_33C = player->actor.textId;
            if (this->unk_33C == 0x15F9) {
                player->exchangeItemId = itemActionParam;
                this->actionFunc = func_80BEF20C;
            } else {
                this->actionFunc = func_80BEF18C;
            }
        } else if (itemActionParam == PLAYER_AP_DEED_MOUNTAIN) {
            player->actor.textId = D_80BF049C[params];
            this->unk_33C = player->actor.textId;
            if (this->unk_33C == 0x160C) {
                player->exchangeItemId = itemActionParam;
                this->actionFunc = func_80BEF20C;
            } else {
                this->actionFunc = func_80BEF18C;
            }
        } else if (itemActionParam == PLAYER_AP_DEED_OCEAN) {
            player->actor.textId = D_80BF04A4[params];
            this->unk_33C = player->actor.textId;
            if (this->unk_33C == 0x1621) {
                player->exchangeItemId = itemActionParam;
                this->actionFunc = func_80BEF20C;
            } else {
                this->actionFunc = func_80BEF18C;
            }
        } else {
            player->actor.textId = D_80BF04AC[params];
            this->unk_33C = player->actor.textId;
            this->actionFunc = func_80BEF18C;
        }
        func_801477B4(play);
    } else if (itemActionParam < PLAYER_AP_NONE) {
        this->unk_33C = D_80BF04AC[params];
        func_80151938(play, this->unk_33C);
        this->actionFunc = func_80BEF18C;
    }
}

void func_80BEE938(EnAkindonuts* this, PlayState* play) {
    switch (ENAKINDONUTS_GET_3(&this->actor)) {
        case 0:
            if (gSaveContext.save.weekEventReg[61] & 0x10) {
                if (ENAKINDONUTS_GET_4(&this->actor)) {
                    this->unk_2DC = func_80BED680;
                } else {
                    Actor_Kill(&this->actor);
                }
            } else if (ENAKINDONUTS_GET_4(&this->actor)) {
                Actor_Kill(&this->actor);
            } else {
                this->unk_2DC = func_80BED3BC;
            }
            break;

        case 1:
            if (gSaveContext.save.weekEventReg[61] & 0x80) {
                if (ENAKINDONUTS_GET_4(&this->actor)) {
                    this->unk_2DC = func_80BEDB88;
                } else {
                    Actor_Kill(&this->actor);
                }
            } else if (ENAKINDONUTS_GET_4(&this->actor)) {
                Actor_Kill(&this->actor);
            } else {
                this->unk_2DC = func_80BED8A4;
            }
            break;

        case 2:
            if (gSaveContext.save.weekEventReg[62] & 4) {
                if (ENAKINDONUTS_GET_4(&this->actor)) {
                    this->unk_2DC = func_80BEE070;
                } else {
                    Actor_Kill(&this->actor);
                }
            } else if (ENAKINDONUTS_GET_4(&this->actor)) {
                Actor_Kill(&this->actor);
            } else {
                this->unk_2DC = func_80BEDDAC;
            }
            break;

        case 3:
            if (gSaveContext.save.weekEventReg[62] & 0x20) {
                if (ENAKINDONUTS_GET_4(&this->actor)) {
                    this->unk_2DC = func_80BEE530;
                } else {
                    Actor_Kill(&this->actor);
                }
            } else if (ENAKINDONUTS_GET_4(&this->actor)) {
                Actor_Kill(&this->actor);
            } else {
                this->unk_2DC = func_80BEE274;
            }
            break;
    }
}

void func_80BEEB20(EnAkindonuts* this, PlayState* play) {
    s16 sp26 = this->skelAnime.curFrame;
    s16 sp24 = Animation_GetLastFrame(&sAnimationInfo[this->unk_338].animation->common);
    s16 phi_v0;

    Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0xE38);

    if ((this->actor.playerHeightRel < 50.0f) && (this->actor.playerHeightRel > -50.0f)) {
        phi_v0 = true;
    } else {
        phi_v0 = false;
    }

    if (phi_v0) {
        if (this->actor.xzDistToPlayer < 200.0f) {
            phi_v0 = true;
        } else {
            phi_v0 = false;
        }

        if (phi_v0) {
            Actor_PlaySfxAtPos(&this->actor, NA_SE_EN_AKINDONUTS_HIDE);
            this->actionFunc = func_80BEEDC0;
            this->unk_338 = 3;
            this->collider.dim.height = 64;
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, 3);
            return;
        }
    }

    if (sp26 == sp24) {
        if ((this->unk_338 == 4) || (this->unk_338 == 18)) {
            this->unk_338 = 17;
            this->collider.dim.height = 0;
            Actor_PlaySfxAtPos(&this->actor, NA_SE_EN_NUTS_DOWN);
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, 17);
        } else if (this->unk_338 == 2) {
            this->unk_338 = 16;
            this->collider.dim.height = 32;
            Actor_PlaySfxAtPos(&this->actor, NA_SE_EN_NUTS_UP);
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, 16);
        } else if (this->unk_338 == 17) {
            phi_v0 = DECR(this->unk_33A);
            if (phi_v0 == 0) {
                this->unk_33A = Rand_ZeroOne() * 10.0f;
                this->unk_338 = 2;
                this->collider.dim.height = 32;
                SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, 2);
            }
        } else if (this->unk_338 == 16) {
            phi_v0 = DECR(this->unk_33A);
            if (phi_v0 == 0) {
                this->unk_33A = Rand_S16Offset(40, 40);
                this->unk_338 = 18;
                this->collider.dim.height = 32;
                SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, 18);
            }
        }
    }
}

void func_80BEEDC0(EnAkindonuts* this, PlayState* play) {
    if (this->skelAnime.curFrame == this->skelAnime.endFrame) {
        this->actionFunc = func_80BEEE10;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, 0);
    }
}

void func_80BEEE10(EnAkindonuts* this, PlayState* play) {
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 2000, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;

    if (Actor_ProcessTalkRequest(&this->actor, &play->state)) {
        this->unk_2DC(this, play);
        this->actionFunc = func_80BEEFA8;
    } else if (((this->actor.xzDistToPlayer < 100.0f) &&
                (((this->actor.playerHeightRel < 50.0f) && (this->actor.playerHeightRel > -50.0f)) ? true : false)) ||
               this->actor.isTargeted) {
        func_800B8614(&this->actor, play, 100.0f);
    } else if (!(((this->actor.playerHeightRel < 50.0f) && (this->actor.playerHeightRel > -50.0f)) ? true : false) ||
               !((this->actor.xzDistToPlayer < 200.0f) ? true : false)) {
        this->unk_338 = 4;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, 4);
        this->actionFunc = func_80BEEB20;
    }
}

void func_80BEEFA8(EnAkindonuts* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    if (talkState == TEXT_STATE_5) {
        if (Message_ShouldAdvance(play)) {
            if (this->unk_32C & 1) {
                this->unk_32C &= ~0x1;
                play->msgCtx.msgMode = 0x43;
                play->msgCtx.stateTimer = 4;
                this->unk_33C = 0;
                this->actionFunc = func_80BEEE10;
            } else if (this->unk_32C & 0x20) {
                this->unk_32C &= ~0x20;
                this->actor.flags &= ~ACTOR_FLAG_1;
                this->unk_32C &= ~0x4;
                play->msgCtx.msgMode = 0x43;
                play->msgCtx.stateTimer = 4;
                this->unk_338 = 8;
                this->unk_33C = 0;
                SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->unk_338);
                this->actionFunc = func_80BEF518;
            } else {
                this->unk_2DC(this, play);
            }
        }
    } else if (talkState == TEXT_STATE_CHOICE) {
        if (Message_ShouldAdvance(play)) {
            switch (play->msgCtx.choiceIndex) {
                case 0:
                    this->unk_32C |= 0x8;
                    this->unk_2DC(this, play);
                    break;

                case 1:
                    func_8019F230();
                    this->unk_32C |= 0x10;
                    this->unk_2DC(this, play);
                    break;
            }
        }
    } else if (talkState == TEXT_STATE_16) {
        func_80BEE73C(this, play);
    }
}

void func_80BEF18C(EnAkindonuts* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_5) && Message_ShouldAdvance(play)) {
        play->msgCtx.msgMode = 0x43;
        play->msgCtx.stateTimer = 4;
        this->unk_33C = 0;
        this->actionFunc = func_80BEEE10;
    }
}

void func_80BEF20C(EnAkindonuts* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);
    s16 sp24 = this->skelAnime.curFrame;
    s16 sp22 = Animation_GetLastFrame(&sAnimationInfo[this->unk_338].animation->common);

    if (this->unk_356 == 40) {
        this->unk_338 = 5;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, 5);
    }

    this->unk_356++;

    if ((sp24 == sp22) && (this->unk_338 == 5)) {
        this->unk_338 = 6;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, 6);
    }

    if ((talkState == TEXT_STATE_5) && Message_ShouldAdvance(play)) {
        if (this->unk_32C & 1) {
            this->unk_32C &= ~0x1;
            play->msgCtx.msgMode = 0x43;
            play->msgCtx.stateTimer = 4;
            this->actionFunc = func_80BEF360;
        } else {
            this->unk_2DC(this, play);
        }
    }
}

void func_80BEF360(EnAkindonuts* this, PlayState* play) {
    if (this->unk_32C & 0x40) {
        if (Actor_HasParent(&this->actor, play)) {
            this->actor.parent = NULL;
            Rupees_ChangeBy(this->unk_364);
            this->unk_32C &= ~0x40;
            this->actionFunc = func_80BEF450;
        } else {
            Actor_PickUp(&this->actor, play, func_80BED034(this), 300.0f, 300.0f);
        }
    } else if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actionFunc = func_80BEF450;
    } else {
        Actor_PickUp(&this->actor, play, func_80BECFBC(this), 300.0f, 300.0f);
    }
}

void func_80BEF450(EnAkindonuts* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
        func_800B85E0(&this->actor, play, 400.0f, -1);
        this->actionFunc = func_80BEF4B8;
    }
}

void func_80BEF4B8(EnAkindonuts* this, PlayState* play) {
    if (Actor_ProcessTalkRequest(&this->actor, &play->state)) {
        this->unk_2DC(this, play);
        this->actionFunc = func_80BEEFA8;
    } else {
        func_800B85E0(&this->actor, play, 400.0f, -1);
    }
}

void func_80BEF518(EnAkindonuts* this, PlayState* play) {
    s16 sp26 = this->skelAnime.curFrame;
    s16 sp24 = Animation_GetLastFrame(&sAnimationInfo[this->unk_338].animation->common);

    switch (sp26) {
        case 10:
            this->unk_33E = 1;
            this->unk_340 = 0.1f;
            this->unk_344 = 0.1f;
            this->unk_348 = 0.1f;
            break;

        case 11:
        case 12:
            this->unk_340 += 0.15f;
            this->unk_344 += 0.32f;
            this->unk_348 += 0.15f;
            break;

        case 13:
            this->unk_340 = 0.55f;
            this->unk_344 = 1.05f;
            this->unk_348 = 0.55f;
            break;

        case 14:
            this->unk_340 = 1.0f;
            this->unk_344 = 2.0f;
            this->unk_348 = 1.0f;
            break;

        case 15:
        case 16:
            this->unk_344 -= 0.33f;
            break;

        case 17:
            this->unk_344 = 1.0f;
            break;

        case 18:
        case 19:
            this->unk_344 += 0.27f;
            break;
        case 20:
            this->unk_33E = 2;
            this->unk_344 = 1.8f;
            break;

        case 21:
        case 22:
        case 23:
            this->unk_344 -= 0.2f;
            break;

        case 24:
            this->unk_344 = 1.0f;
            break;
    }

    if (this->unk_35E == 0) {
        if (ActorCutscene_GetCanPlayNext(this->cutscene)) {
            ActorCutscene_StartAndSetUnkLinkFields(this->cutscene, &this->actor);
            this->unk_35E = 1;
        } else {
            if (ActorCutscene_GetCurrentIndex() == 0x7C) {
                ActorCutscene_Stop(0x7C);
            }
            ActorCutscene_SetIntentToPlay(this->cutscene);
            return;
        }
    }

    if (sp26 == sp24) {
        this->unk_33E = 3;
        this->unk_338 = 19;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->unk_338);
        Actor_PlaySfxAtPos(&this->actor, NA_SE_EN_NUTS_DOWN);
        this->unk_32C &= ~2;
        this->unk_32C |= 0x80;
        this->unk_358 = this->actor.world.pos.y;
        this->actionFunc = func_80BEF770;
    }
}

void func_80BEF770(EnAkindonuts* this, PlayState* play) {
    f32 sp24 = this->unk_358 - this->actor.world.pos.y;
    Vec3f sp18;

    this->unk_352 += this->unk_362;
    if (!(this->unk_35C & 3)) {
        sp18.x = this->actor.world.pos.x;
        sp18.y = this->unk_358;
        sp18.z = this->actor.world.pos.z;
        func_800B14D4(play, 20.0f, &sp18);
    }

    if (sp24 > 5.0f) {
        this->unk_33A = 50;
        this->actor.velocity.y = 0.0f;
        this->actor.gravity = 0.0f;
        this->unk_368 = func_80BECF6C(this->path);
        this->actionFunc = func_80BEF83C;
    }
}

void func_80BEF83C(EnAkindonuts* this, PlayState* play) {
    Vec3f sp34;
    s16 sp32 = this->skelAnime.curFrame;
    s16 sp30 = Animation_GetLastFrame(&sAnimationInfo[this->unk_338].animation->common);

    if (sp32 == sp30) {
        Math_SmoothStepToS(&this->unk_362, 0x1C71, 3, 0x100, 0);
        this->unk_352 += this->unk_362;
        this->actor.shape.yOffset = 1500.0f;
    }

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->unk_368, 3, 2000, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;

    if (DECR(this->unk_33A) == 0) {
        if (!(this->unk_35C & 3)) {
            sp34 = this->actor.world.pos;
            func_800B14D4(play, 20.0f, &sp34);
        }
        this->actor.velocity.y = 5.0f;
    } else if (!(this->unk_35C & 3)) {
        sp34.x = this->actor.world.pos.x;
        sp34.y = this->unk_358;
        sp34.z = this->actor.world.pos.z;
        func_800B14D4(play, 20.0f, &sp34);
    }
    if ((this->actor.home.pos.y + 22.5f) < this->actor.world.pos.y) {
        this->unk_34C = 0.3f;
        this->unk_338 = 9;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->unk_338);
        Actor_PlaySfxAtPos(&this->actor, NA_SE_EN_AKINDONUTS_HIDE);
        this->actionFunc = func_80BEF9F0;
    }
}

void func_80BEF9F0(EnAkindonuts* this, PlayState* play) {
    this->unk_352 += this->unk_362;
    if (this->unk_34C >= 1.0f) {
        this->unk_34C = 1.0f;
        this->actor.velocity.y = 5.0f;
    } else {
        this->actor.velocity.y = 5.0f;
        this->unk_34C += 0.1f;
    }

    if ((this->actor.home.pos.y + 200.0f) < this->actor.world.pos.y) {
        Math_ApproachF(&this->actor.velocity.y, 0.0f, 0.2f, 1.0f);
        this->unk_338 = 10;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->unk_338);
        if (ENAKINDONUTS_GET_3(&this->actor) == ENAKINDONUTS_3_2) {
            this->unk_32C |= 0x2;
        }
        this->unk_356 = 0;
        this->actionFunc = func_80BEFAF0;
    }
}

void func_80BEFAF0(EnAkindonuts* this, PlayState* play) {
    Vec3s sp38;
    f32 sp34;
    s16 sp32 = false;

    this->actor.velocity.y = 0.0f;
    this->actor.gravity = 0.0f;

    if (this->path != NULL) {
        sp34 = func_80BECEAC(this->path, this->unk_334, &this->actor.world.pos, &sp38);
        if (this->actor.bgCheckFlags & 0x8) {
            sp38.y = this->actor.wallYaw;
        }

        if (ENAKINDONUTS_GET_3(&this->actor) == ENAKINDONUTS_3_1) {
            Math_SmoothStepToS(&this->actor.world.rot.y, sp38.y, 10, 1000, 0);
        } else {
            Math_SmoothStepToS(&this->actor.world.rot.y, sp38.y, 10, 300, 0);
        }

        this->actor.shape.rot.y = this->actor.world.rot.y;
        this->unk_350 = 0x1000;
        this->unk_352 += this->unk_362;
        this->actor.world.rot.x = -sp38.x;

        if (func_80BECD10(this, this->path, this->unk_334) && (sp34 < 10.0f)) {
            if (this->unk_334 >= (this->path->count - 1)) {
                ActorCutscene_Stop(this->cutscene);
                this->actionFunc = func_80BEFD74;
            } else {
                this->unk_334++;
            }
        }
    } else if (this->actor.playerHeightRel > 500.0f) {
        ActorCutscene_Stop(this->cutscene);
        this->actionFunc = func_80BEFD74;
    }

    if (ENAKINDONUTS_GET_3(&this->actor) == ENAKINDONUTS_3_2) {
        if (this->unk_334 >= 3) {
            sp32 = true;
        }
        Math_ApproachF(&this->actor.speedXZ, 1.5f, 0.2f, 1.0f);
    } else {
        Math_ApproachF(&this->actor.speedXZ, 2.0f, 0.2f, 1.0f);
    }

    func_80BECBE0(this, sp32);

    if (this->unk_35E == 2) {
        if (ActorCutscene_GetCanPlayNext(this->cutscene)) {
            ActorCutscene_StartAndSetUnkLinkFields(this->cutscene, &this->actor);
            this->unk_35E = 3;
        } else {
            ActorCutscene_SetIntentToPlay(this->cutscene);
            return;
        }
    } else if ((this->unk_35E == 1) && (this->unk_356 == 20)) {
        ActorCutscene_Stop(this->cutscene);
        this->cutscene = ActorCutscene_GetAdditionalCutscene(this->cutscene);
        ActorCutscene_SetIntentToPlay(this->cutscene);
        this->unk_35E = 2;
    }
    this->unk_356++;
}

void func_80BEFD74(EnAkindonuts* this, PlayState* play) {
    Actor_Kill(&this->actor);
}

void EnAkindonuts_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnAkindonuts* this = THIS;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    SkelAnime_InitFlex(play, &this->skelAnime, &object_dnt_Skel_00AC70, &object_dnt_Anim_005488, this->jointTable,
                       this->morphTable, 28);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinderType1(play, &this->collider, &this->actor, &sCylinderInit);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 35.0f);
    Actor_SetScale(&this->actor, 0.01f);

    this->actor.colChkInfo.cylRadius = 0;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.gravity = -1.0f;

    if (!ENAKINDONUTS_GET_4(&this->actor)) {
        this->path = SubS_GetPathByIndex(play, ENAKINDONUTS_GET_FC00(&this->actor), 0x3F);
        if (this->path == NULL) {
            Actor_Kill(&this->actor);
            return;
        }
    }

    SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, 4);
    this->unk_32C |= 0x2;
    this->unk_32C |= 0x4;
    this->unk_338 = 4;
    this->cutscene = this->actor.cutscene;
    func_80BEE938(this, play);
    this->actionFunc = func_80BEEB20;
}

void EnAkindonuts_Destroy(Actor* thisx, PlayState* play) {
    EnAkindonuts* this = THIS;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnAkindonuts_Update(Actor* thisx, PlayState* play) {
    EnAkindonuts* this = THIS;

    Actor_SetFocus(&this->actor, 60.0f);
    SkelAnime_Update(&this->skelAnime);
    Actor_MoveWithGravity(&this->actor);

    this->actionFunc(this, play);

    if (this->unk_32C & 0x80) {
        func_800B9010(&this->actor, NA_SE_EN_AKINDO_FLY - SFX_FLAG);
    }
    func_80BECC7C(this, play);
}

s32 EnAkindonuts_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnAkindonuts* this = THIS;

    if (((this->unk_338 == 4) && (this->unk_33E == 0)) || ((this->unk_338 == 8) && (this->unk_33E == 0)) ||
        (this->unk_338 == 18) || (this->unk_338 == 2) || (this->unk_338 == 3) || (this->unk_338 == 17) ||
        (this->unk_338 == 5) || (this->unk_338 == 6) || (this->unk_338 == 7) || (this->unk_338 == 16)) {
        if ((limbIndex == 16) || (limbIndex == 21) || (limbIndex == 27) || (limbIndex == 23) || (limbIndex == 24) ||
            (limbIndex == 15)) {
            *dList = NULL;
        }
    } else if ((this->unk_338 == 8) || (this->unk_338 == 19)) {
        switch (this->unk_33E) {
            case 1:
                if ((limbIndex == 16) || (limbIndex == 21) || (limbIndex == 27) || (limbIndex == 25)) {
                    *dList = NULL;
                }
                break;

            case 2:
            case 3:
                if ((limbIndex == 16) || (limbIndex == 21) || (limbIndex == 27) || (limbIndex == 15) ||
                    (limbIndex == 25)) {
                    *dList = NULL;
                }
                break;
        }
    } else if (((this->unk_338 == 9) || (this->unk_338 == 10)) && ((limbIndex == 15) || (limbIndex == 25))) {
        *dList = NULL;
    }

    if (limbIndex == 26) {
        if ((this->unk_338 == 6) || (this->unk_338 == 5) || (this->unk_338 == 7)) {
            *dList = object_dnt_DL_001350;
        } else {
            *dList = object_dnt_DL_008290;
        }
    }
    return false;
}

void EnAkindonuts_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
}

void EnAkindonuts_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
    EnAkindonuts* this = THIS;

    if (((this->unk_33E == 1) || (this->unk_33E == 2)) && ((limbIndex == 23) || (limbIndex == 24))) {
        Matrix_Scale(this->unk_340, this->unk_344, this->unk_348, MTXMODE_APPLY);
    }

    if ((this->unk_338 == 9) && ((limbIndex == 16) || (limbIndex == 21) || (limbIndex == 27))) {
        Matrix_Scale(this->unk_34C, this->unk_34C, this->unk_34C, MTXMODE_APPLY);
    }

    if (limbIndex == 24) {
        Matrix_RotateYS(this->unk_352, MTXMODE_APPLY);
    }
}

void EnAkindonuts_Draw(Actor* thisx, PlayState* play) {
    EnAkindonuts* this = THIS;

    func_8012C28C(play->state.gfxCtx);
    SkelAnime_DrawTransformFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   this->skelAnime.dListCount, EnAkindonuts_OverrideLimbDraw, EnAkindonuts_PostLimbDraw,
                                   EnAkindonuts_TransformLimbDraw, &this->actor);
}
