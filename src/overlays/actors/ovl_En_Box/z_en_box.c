/*
 * File: z_en_box.c
 * Overlay: ovl_En_Box
 * Description: Chest
 */

#include "z_en_box.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_box/object_box.h"
#include "overlays/actors/ovl_En_Elforg/z_en_elforg.h"

#define FLAGS 0x00000000

/*
set on init unless treasure flag is set
if clear, chest moves (Actor_MoveForward) (falls, likely)
ends up cleared from SWITCH_FLAG_FALL types when switch flag is set
*/
#define ENBOX_MOVE_IMMOBILE (1 << 0)
/*
set in the logic for SWITCH_FLAG_FALL types
otherwise unused
*/
#define ENBOX_MOVE_UNUSED (1 << 1)
/*
set with 50% chance on init for SWITCH_FLAG_FALL types
only used for SWITCH_FLAG_FALL types
ends up "blinking" (set/clear every frame) once switch flag is set,
if some collision-related condition (?) is met
only used for signum of z rotation
*/
#define ENBOX_MOVE_FALL_ANGLE_SIDE (1 << 2)
/*
when set, gets cleared next EnBox_Update call and clip to the floor
*/
#define ENBOX_MOVE_STICK_TO_GROUND (1 << 4)
#define ENBOX_MOVE_0x20 (1 << 5)
#define ENBOX_MOVE_0x40 (1 << 6)
#define ENBOX_MOVE_0x80 (1 << 7)

#define THIS ((EnBox*)thisx)

void EnBox_Init(Actor* thisx, PlayState* play);
void EnBox_Destroy(Actor* thisx, PlayState* play);
void EnBox_Update(Actor* thisx, PlayState* play);
void EnBox_Draw(Actor* thisx, PlayState* play);

void EnBox_FallOnSwitchFlag(EnBox* this, PlayState* play);
void EnBox_AppearSwitchFlag(EnBox* this, PlayState* play);
void EnBox_AppearOnRoomClear(EnBox* this, PlayState* play);
void func_80868AFC(EnBox* this, PlayState* play);
void func_80868B74(EnBox* this, PlayState* play);
void EnBox_WaitOpen(EnBox* this, PlayState* play);
void EnBox_Open(EnBox* this, PlayState* play);

void func_80867FBC(func_80867BDC_a0* arg0, PlayState* play, s32 arg2);
void func_80867FE4(func_80867BDC_a0* arg0, PlayState* play);

const ActorInit En_Box_InitVars = {
    ACTOR_EN_BOX,
    ACTORCAT_CHEST,
    FLAGS,
    OBJECT_BOX,
    sizeof(EnBox),
    (ActorFunc)EnBox_Init,
    (ActorFunc)EnBox_Destroy,
    (ActorFunc)EnBox_Update,
    (ActorFunc)EnBox_Draw,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 0, ICHAIN_STOP),
};

typedef struct {
    f32 data[5];
} EnBox_PlaybackSpeed; // 0x14

static EnBox_PlaybackSpeed sPlaybackSpeed = { { 1.5f, 1.0f, 1.5f, 1.0f, 1.5f } };

static AnimationHeader* sBigChestAnimations[5] = {
    &gBoxBigChestOpenAdultAnim, &gBoxBigChestOpenGoronAnim, &gBoxBigChestOpenAdultAnim,
    &gBoxBigChestOpenDekuAnim,  &gBoxBigChestOpenChildAnim,
};

void EnBox_SetupAction(EnBox* this, EnBoxActionFunc func) {
    this->actionFunc = func;
}

void func_80867BDC(func_80867BDC_a0* arg0, PlayState* play, Vec3f* pos) {
    arg0->pos = *pos;
    arg0->unk_0C = NULL;
    arg0->unk_10 = NULL;
    arg0->unk_14 = 0;
    arg0->unk_18 = 0;
    arg0->unk_1C = 0;
}

void func_80867C14(func_80867BDC_a0* arg0, PlayState* play) {
    arg0->unk_18++;
    if (arg0->unk_18 > 85) {
        arg0->unk_18 = 85;
    }
    if (arg0->unk_18 != 85) {
        arg0->unk_1C = arg0->unk_18 - 28;
        if (arg0->unk_18 - 28 < 0) {
            arg0->unk_1C = 0;
        }
    } else {
        arg0->unk_1C++;
        if (arg0->unk_1C > 85) {
            func_80867FE4(arg0, play);
        }
    }
    arg0->unk_14++;
}

void func_80867C8C(func_80867BDC_a0* arg0, PlayState* play) {
    s32 temp_s6 = arg0->unk_18 - arg0->unk_1C;
    s32 i;
    f32 pad;

    if (temp_s6 > 0) {
        OPEN_DISPS(play->state.gfxCtx);
        Matrix_Push();
        for (i = 0; i < temp_s6; i++) {
            f32 temp_f0 = (f32)i / temp_s6;
            s16 temp_s0 = ((f32)arg0->unk_18 - arg0->unk_1C) * temp_f0 + arg0->unk_1C;
            f32 temp_f24 = 1.0f - (temp_s0 * (1 / 85.0f));
            f32 temp_f26 = Rand_ZeroOne() * 0.03f * temp_f0 * temp_f24;
            f32 temp_f28;
            f32 temp_f30 = Math_SinS(temp_s0 * 0x9A6) * 45.0f + arg0->pos.x;
            f32 phi_f24;

            if (arg0->unk_20 != 0) {
                phi_f24 = arg0->pos.y - (0.03f * temp_s0) - (0.01f * temp_s0 * temp_s0);
            } else {
                phi_f24 = arg0->pos.y + (0.03f * temp_s0) + (0.01f * temp_s0 * temp_s0);
            }
            temp_f28 = (Math_CosS(temp_s0 * 0x9A6) * 45.0f) + arg0->pos.z;
            Matrix_Translate(2.0f * Rand_Centered() + temp_f30, 2.0f * Rand_Centered() + phi_f24,
                             2.0f * Rand_Centered() + temp_f28, MTXMODE_NEW);
            Matrix_Scale(temp_f26, temp_f26, temp_f26, MTXMODE_APPLY);
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

void func_80867FBC(func_80867BDC_a0* arg0, PlayState* play, s32 arg2) {
    arg0->unk_0C = func_80867C14;
    arg0->unk_10 = func_80867C8C;
    arg0->unk_20 = arg2;
}

void func_80867FE4(func_80867BDC_a0* arg0, PlayState* play) {
    arg0->unk_0C = NULL;
    arg0->unk_10 = NULL;
    func_80867BDC(arg0, play, &arg0->pos);
}

void EnBox_ClipToGround(EnBox* this, PlayState* play) {
    s32 pad;
    CollisionPoly* poly;
    s32 bgId;
    Vec3f pos;
    f32 floorHeight;

    if (!(this->movementFlags & ENBOX_MOVE_0x80)) {
        pos = this->dyna.actor.world.pos;
        pos.y += 1.0f;
        floorHeight = BgCheck_EntityRaycastFloor5(&play->colCtx, &poly, &bgId, &this->dyna.actor, &pos);
        if (floorHeight != BGCHECK_Y_MIN) {
            this->dyna.actor.world.pos.y = floorHeight;
            this->dyna.actor.floorHeight = floorHeight;
        }
    }
}

void EnBox_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnBox* this = THIS;
    s16 cutsceneIdx;
    CollisionHeader* colHeader;
    f32 animFrame;
    f32 animFrameEnd;

    colHeader = NULL;
    animFrame = 0.0f;
    animFrameEnd = Animation_GetLastFrame(&gBoxChestOpenAnim);
    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, 0);
    CollisionHeader_GetVirtual(&gBoxChestCol, &colHeader);

    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    this->movementFlags = 0;
    this->type = ENBOX_GET_TYPE(&this->dyna.actor);
    this->iceSmokeTimer = 0;
    this->unk_1F3 = 0;
    this->dyna.actor.gravity = -5.5f;
    this->dyna.actor.terminalVelocity = -50.0f;
    this->switchFlag = ENBOX_GET_SWITCH_FLAG(&this->dyna.actor);
    this->dyna.actor.floorHeight = this->dyna.actor.world.pos.y;
    if (this->dyna.actor.world.rot.x == 180) {
        this->movementFlags |= ENBOX_MOVE_0x80;
        this->dyna.actor.world.rot.x = 0x7FFF;
        this->collectableFlag = 0;
    } else {
        func_800C636C(play, &play->colCtx.dyna, this->dyna.bgId);
        this->collectableFlag = (this->dyna.actor.world.rot.x & 0x7F);
        this->dyna.actor.world.rot.x = 0;
    }
    thisx->shape.rot.x = this->dyna.actor.world.rot.x;
    this->getItemId = ENBOX_GET_ITEM(thisx);

    if (Flags_GetTreasure(play, ENBOX_GET_CHEST_FLAG(&this->dyna.actor)) || this->getItemId == GI_NONE) {
        this->alpha = 255;
        this->iceSmokeTimer = 100;
        EnBox_SetupAction(this, EnBox_Open);
        this->movementFlags |= ENBOX_MOVE_STICK_TO_GROUND;
        animFrame = animFrameEnd;
    } else if (((this->type == ENBOX_TYPE_BIG_SWITCH_FLAG_FALL) || (this->type == ENBOX_TYPE_SMALL_SWITCH_FLAG_FALL)) &&
               !Flags_GetSwitch(play, this->switchFlag)) {
        func_800C62BC(play, &play->colCtx.dyna, this->dyna.bgId);
        if (Rand_ZeroOne() < 0.5f) {
            this->movementFlags |= ENBOX_MOVE_FALL_ANGLE_SIDE;
        }
        this->unk_1A0 = -12;
        EnBox_SetupAction(this, EnBox_FallOnSwitchFlag);
        this->alpha = 0;
        this->movementFlags |= ENBOX_MOVE_IMMOBILE;
        this->dyna.actor.flags |= ACTOR_FLAG_10;
    } else if (((this->type == ENBOX_TYPE_BIG_ROOM_CLEAR) || (this->type == ENBOX_TYPE_SMALL_ROOM_CLEAR)) &&
               !Flags_GetClear(play, this->dyna.actor.room)) {
        EnBox_SetupAction(this, EnBox_AppearOnRoomClear);
        func_800C62BC(play, &play->colCtx.dyna, this->dyna.bgId);
        if (this->movementFlags & ENBOX_MOVE_0x80) {
            this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y + 50.0f;
        } else {
            this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y - 50.0f;
        }
        this->alpha = 0;
        this->movementFlags |= ENBOX_MOVE_IMMOBILE;
        this->dyna.actor.flags |= ACTOR_FLAG_10;
    } else if ((this->type == ENBOX_TYPE_BIG_SONG_ZELDAS_LULLABY) || (this->type == ENBOX_TYPE_BIG_SONG_SUNS)) {

    } else if (((this->type == ENBOX_TYPE_BIG_SWITCH_FLAG) || (this->type == ENBOX_TYPE_SMALL_SWITCH_FLAG)) &&
               !Flags_GetSwitch(play, this->switchFlag)) {
        EnBox_SetupAction(this, EnBox_AppearSwitchFlag);
        func_800C62BC(play, &play->colCtx.dyna, this->dyna.bgId);
        if (this->movementFlags & ENBOX_MOVE_0x80) {
            this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y + 50.0f;
        } else {
            this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y - 50.0f;
        }
        this->alpha = 0;
        this->movementFlags |= ENBOX_MOVE_IMMOBILE;
        this->dyna.actor.flags |= ACTOR_FLAG_10;
    } else {
        if ((this->type == ENBOX_TYPE_BIG_INVISIBLE) || (this->type == ENBOX_TYPE_SMALL_INVISIBLE)) {
            this->dyna.actor.flags |= ACTOR_FLAG_80;
        }
        EnBox_SetupAction(this, EnBox_WaitOpen);
        this->movementFlags |= ENBOX_MOVE_IMMOBILE;
        this->movementFlags |= ENBOX_MOVE_STICK_TO_GROUND;
    }

    if ((this->getItemId == GI_STRAY_FAIRY) && !Flags_GetTreasure(play, ENBOX_GET_CHEST_FLAG(&this->dyna.actor))) {
        this->dyna.actor.flags |= ACTOR_FLAG_10;
    }

    this->dyna.actor.shape.rot.y += 0x8000;
    this->dyna.actor.home.rot.z = this->dyna.actor.world.rot.z = this->dyna.actor.shape.rot.z = 0;

    SkelAnime_Init(play, &this->skelAnime, &gBoxChestSkel, &gBoxChestOpenAnim, this->jointTable, this->morphTable,
                   OBJECT_BOX_CHEST_LIMB_MAX);
    Animation_Change(&this->skelAnime, &gBoxChestOpenAnim, 1.5f, animFrame, animFrameEnd, ANIMMODE_ONCE, 0.0f);
    if (Actor_IsSmallChest(this)) {
        Actor_SetScale(&this->dyna.actor, 0.0075f);
        Actor_SetFocus(&this->dyna.actor, 20.0f);
    } else {
        Actor_SetScale(&this->dyna.actor, 0.01f);
        Actor_SetFocus(&this->dyna.actor, 40.0f);
    }

    this->cutsceneIdxA = -1;
    this->cutsceneIdxB = -1;
    cutsceneIdx = this->dyna.actor.cutscene;

    while (cutsceneIdx != -1) {
        if (func_800F2178(cutsceneIdx) == 1) {
            this->cutsceneIdxB = cutsceneIdx;
        } else {
            this->cutsceneIdxA = cutsceneIdx;
        }
        cutsceneIdx = ActorCutscene_GetAdditionalCutscene(cutsceneIdx);
    }
    func_80867BDC(&this->unk_1F4, play, &this->dyna.actor.home.pos);
}

void EnBox_Destroy(Actor* thisx, PlayState* play) {
    EnBox* this = THIS;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void EnBox_RandomDustKinematic(EnBox* this, Vec3f* pos, Vec3f* velocity, Vec3f* accel) {
    f32 randomRadius = Rand_ZeroOne() * 25.0f;
    s16 randomAngle = Rand_ZeroOne() * 0x10000;

    *pos = this->dyna.actor.world.pos;
    pos->x += Math_SinS(randomAngle) * randomRadius;
    pos->z += Math_CosS(randomAngle) * randomRadius;

    velocity->y = 1.0f;
    velocity->x = Math_SinS(randomAngle);
    velocity->z = Math_CosS(randomAngle);

    accel->x = 0.0f;
    accel->y = 0.0f;
    accel->z = 0.0f;
}

void EnBox_SpawnDust(EnBox* this, PlayState* play) {
    s32 i;
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;

    for (i = 0; i < 20; i++) {
        EnBox_RandomDustKinematic(this, &pos, &velocity, &accel);
        func_800B1280(play, &pos, &velocity, &accel, 100, 30, 15);
    }
}

void EnBox_Fall(EnBox* this, PlayState* play) {
    f32 yDiff;

    this->alpha = 255;
    this->movementFlags &= ~ENBOX_MOVE_IMMOBILE;
    if (this->dyna.actor.bgCheckFlags & 1) {
        this->movementFlags |= ENBOX_MOVE_UNUSED;
        if (this->movementFlags & ENBOX_MOVE_FALL_ANGLE_SIDE) {
            this->movementFlags &= ~ENBOX_MOVE_FALL_ANGLE_SIDE;
        } else {
            this->movementFlags |= ENBOX_MOVE_FALL_ANGLE_SIDE;
        }
        if (this->type == ENBOX_TYPE_BIG_SWITCH_FLAG_FALL) {
            this->dyna.actor.velocity.y = -this->dyna.actor.velocity.y * 0.55f;
        } else {
            this->dyna.actor.velocity.y = -this->dyna.actor.velocity.y * 0.65f;
        }
        if (this->dyna.actor.velocity.y < 5.5f) {
            this->dyna.actor.shape.rot.z = 0;
            this->dyna.actor.world.pos.y = this->dyna.actor.floorHeight;
            EnBox_SetupAction(this, EnBox_WaitOpen);
        }
        Audio_PlaySfxAtPos(&this->dyna.actor.projectedPos, NA_SE_EV_TRE_BOX_BOUND);
        EnBox_SpawnDust(this, play);
    }
    yDiff = this->dyna.actor.world.pos.y - this->dyna.actor.floorHeight;
    if (this->movementFlags & ENBOX_MOVE_FALL_ANGLE_SIDE) {
        this->dyna.actor.shape.rot.z = yDiff * 50.0f;
    } else {
        this->dyna.actor.shape.rot.z = -yDiff * 50.0f;
    }
}

void EnBox_FallOnSwitchFlag(EnBox* this, PlayState* play) {
    func_800B8C50(&this->dyna.actor, play);
    if (this->unk_1A0 >= 0) {
        EnBox_SetupAction(this, EnBox_Fall);
        func_800C6314(play, &play->colCtx.dyna, this->dyna.bgId);
    } else if (this->unk_1A0 >= -11) {
        this->unk_1A0++;
    } else if (Flags_GetSwitch(play, this->switchFlag)) {
        this->unk_1A0++;
    }
}

void EnBox_AppearSwitchFlag(EnBox* this, PlayState* play) {
    func_800B8C50(&this->dyna.actor, play);
    if (Flags_GetSwitch(play, this->switchFlag)) {
        if (ActorCutscene_GetCanPlayNext(this->cutsceneIdxA)) {
            ActorCutscene_StartAndSetUnkLinkFields(this->cutsceneIdxA, &this->dyna.actor);
            EnBox_SetupAction(this, func_80868AFC);
            this->unk_1A0 = -30;
        } else {
            ActorCutscene_SetIntentToPlay(this->cutsceneIdxA);
        }
    }
}

void EnBox_AppearOnRoomClear(EnBox* this, PlayState* play) {
    func_800B8C50(&this->dyna.actor, play);
    if (Flags_GetClearTemp(play, this->dyna.actor.room)) {
        if (ActorCutscene_GetCanPlayNext(this->cutsceneIdxA)) {
            ActorCutscene_StartAndSetUnkLinkFields(this->cutsceneIdxA, &this->dyna.actor);
            Flags_SetClear(play, this->dyna.actor.room);
            EnBox_SetupAction(this, func_80868AFC);
            this->unk_1A0 = -30;
        } else {
            ActorCutscene_SetIntentToPlay(this->cutsceneIdxA);
        }
    }
}

void func_80868AFC(EnBox* this, PlayState* play) {
    if ((func_800F22C4(this->cutsceneIdxA, &this->dyna.actor) != 0) || (this->unk_1A0 != 0)) {
        EnBox_SetupAction(this, func_80868B74);
        this->unk_1A0 = 0;
        func_80867FBC(&this->unk_1F4, play, (this->movementFlags & ENBOX_MOVE_0x80) != 0);
        Audio_PlaySfxAtPos(&this->dyna.actor.projectedPos, NA_SE_EV_TRE_BOX_APPEAR);
    }
}

void func_80868B74(EnBox* this, PlayState* play) {
    func_800C6314(play, &play->colCtx.dyna, this->dyna.bgId);
    if (this->unk_1A0 < 0) {
        this->unk_1A0++;
    } else if (this->unk_1A0 < 40) {
        if (this->movementFlags & ENBOX_MOVE_0x80) {
            this->dyna.actor.world.pos.y -= 1.25f;
        } else {
            this->dyna.actor.world.pos.y += 1.25f;
        }
        this->unk_1A0++;
        if ((this->cutsceneIdxA != -1) && ActorCutscene_GetCurrentIndex() == this->cutsceneIdxA) {
            if (this->unk_1A0 == 2) {
                func_800B724C(play, &this->dyna.actor, 4);
            } else if (this->unk_1A0 == 22) {
                func_800B724C(play, &this->dyna.actor, 1);
            }
        }
    } else if (this->unk_1A0 < 60) {
        this->alpha += 12;
        this->unk_1A0++;
        this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y;
    } else {
        EnBox_SetupAction(this, EnBox_WaitOpen);
        ActorCutscene_Stop(this->dyna.actor.cutscene);
    }
}

void EnBox_WaitOpen(EnBox* this, PlayState* play) {
    s32 pad;
    AnimationHeader* animHeader;
    f32 frameCount;
    f32 playbackSpeed;
    EnBox_PlaybackSpeed playbackSpeedTable;
    Player* player;
    Vec3f offset;

    this->alpha = 255;
    this->movementFlags |= ENBOX_MOVE_IMMOBILE;
    if ((this->unk_1EC != 0) && ((this->cutsceneIdxB < 0) || (ActorCutscene_GetCurrentIndex() == this->cutsceneIdxB) ||
                                 (ActorCutscene_GetCurrentIndex() == -1))) {
        if (this->unk_1EC < 0) {
            animHeader = &gBoxChestOpenAnim;
            playbackSpeed = 1.5f;
        } else {
            u8 playerForm;

            playbackSpeedTable = sPlaybackSpeed;
            playerForm = gSaveContext.save.playerForm;
            animHeader = sBigChestAnimations[playerForm];
            playbackSpeed = playbackSpeedTable.data[playerForm];
        }

        frameCount = Animation_GetLastFrame(animHeader);
        Animation_Change(&this->skelAnime, animHeader, playbackSpeed, 0.0f, frameCount, ANIMMODE_ONCE, 0.0f);
        EnBox_SetupAction(this, EnBox_Open);
        if (this->unk_1EC > 0) {
            Actor_SpawnAsChild(&play->actorCtx, &this->dyna.actor, play, ACTOR_DEMO_TRE_LGT,
                               this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y, this->dyna.actor.world.pos.z,
                               this->dyna.actor.shape.rot.x, this->dyna.actor.shape.rot.y, this->dyna.actor.shape.rot.z,
                               -1);
            Audio_PlayFanfare(NA_BGM_OPEN_CHEST | 0x900);
        }

        if (this->getItemId == GI_STRAY_FAIRY) {
            this->movementFlags |= ENBOX_MOVE_0x20;
        } else {
            if ((this->getItemId == GI_HEART_PIECE) || (this->getItemId == GI_BOTTLE)) {
                Flags_SetCollectible(play, this->collectableFlag);
            }
            Flags_SetTreasure(play, ENBOX_GET_CHEST_FLAG(&this->dyna.actor));
        }
    } else {
        player = GET_PLAYER(play);
        Actor_OffsetOfPointInActorCoords(&this->dyna.actor, &offset, &player->actor.world.pos);
        if (offset.z > -50.0f && offset.z < 0.0f && fabsf(offset.y) < 10.0f && fabsf(offset.x) < 20.0f &&
            Player_IsFacingActor(&this->dyna.actor, 0x3000, play)) {
            if (((this->getItemId == GI_HEART_PIECE) || (this->getItemId == GI_BOTTLE)) &&
                Flags_GetCollectible(play, this->collectableFlag)) {
                this->getItemId = GI_RECOVERY_HEART;
            }
            if ((this->getItemId == GI_MASK_CAPTAIN) && (INV_CONTENT(ITEM_MASK_CAPTAIN) == ITEM_MASK_CAPTAIN)) {
                this->getItemId = GI_RECOVERY_HEART;
            }
            if ((this->getItemId == GI_MASK_GIANT) && (INV_CONTENT(ITEM_MASK_GIANT) == ITEM_MASK_GIANT)) {
                this->getItemId = GI_RECOVERY_HEART;
            }
            Actor_PickUpNearby(&this->dyna.actor, play, -this->getItemId);
        }
        if (Flags_GetTreasure(play, ENBOX_GET_CHEST_FLAG(&this->dyna.actor))) {
            EnBox_SetupAction(this, EnBox_Open);
        }
    }
}

void EnBox_Open(EnBox* this, PlayState* play) {
    s32 pad;

    this->dyna.actor.flags &= ~ACTOR_FLAG_80;

    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->unk_1EC > 0) {
            if (this->unk_1EC < 120) {
                this->unk_1EC++;
            } else {
                Math_StepToF(&this->unk_1A8, 0.0f, 0.05f);
            }
        } else if (this->unk_1EC > -120) {
            this->unk_1EC--;
        } else {
            Math_StepToF(&this->unk_1A8, 0.0f, 0.05f);
        }
        if (this->movementFlags & ENBOX_MOVE_0x20) {
            this->movementFlags &= ~ENBOX_MOVE_0x20;
            Actor_SpawnAsChild(&play->actorCtx, &this->dyna.actor, play, ACTOR_EN_ELFORG, this->dyna.actor.world.pos.x,
                               this->dyna.actor.world.pos.y, this->dyna.actor.world.pos.z, this->dyna.actor.world.rot.x,
                               this->dyna.actor.world.rot.y, this->dyna.actor.world.rot.z,
                               STRAY_FAIRY_PARAMS(ENBOX_GET_CHEST_FLAG(&this->dyna.actor), 0, STRAY_FAIRY_TYPE_CHEST));
        } else if (this->movementFlags & ENBOX_MOVE_0x40) {
            this->movementFlags &= ~ENBOX_MOVE_0x40;
        }
    } else {
        f32 waterSurface;
        WaterBox* waterBox;
        s32 bgId;
        u16 sfxId = 0;

        if (Animation_OnFrame(&this->skelAnime, gSaveContext.save.playerForm == PLAYER_FORM_DEKU ? 14.0f : 30.0f)) {
            sfxId = NA_SE_EV_TBOX_UNLOCK;
        } else if (Animation_OnFrame(&this->skelAnime,
                                     gSaveContext.save.playerForm == PLAYER_FORM_DEKU ? 15.0f : 90.0f)) {
            sfxId = NA_SE_EV_TBOX_OPEN;
        }
        if (sfxId != 0) {
            Audio_PlaySfxAtPos(&this->dyna.actor.projectedPos, sfxId);
        }
        if (this->skelAnime.jointTable[3].z > 0) {
            this->unk_1A8 = (0x7D00 - this->skelAnime.jointTable[3].z) * 0.00006f;
            if (this->unk_1A8 < 0.0f) {
                this->unk_1A8 = 0.0f;
            } else if (this->unk_1A8 > 1.0f) {
                this->unk_1A8 = 1.0f;
            }
        }
        if (WaterBox_GetSurfaceImpl(play, &play->colCtx, this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.z,
                                    &waterSurface, &waterBox, &bgId) &&
            (this->dyna.actor.floorHeight < waterSurface)) {
            EffectSsBubble_Spawn(play, &this->dyna.actor.world.pos, 5.0f, 2.0f, 20.0f, 0.3f);
        }
    }
}

void EnBox_SpawnIceSmoke(EnBox* this, PlayState* play) {
    Vec3f pos;
    Vec3f velocity = { 0, 1.0f, 0 };
    Vec3f accel = { 0, 0, 0 };
    f32 randomf;

    this->iceSmokeTimer++;
    //! @bug sfxId should be NA_SE_EN_MIMICK_BREATH, but uses OoT's sfxId value
    func_800B9010(&this->dyna.actor, NA_SE_EN_LAST3_COIL_ATTACK_OLD - SFX_FLAG);
    if (Rand_ZeroOne() < 0.3f) {
        randomf = 2.0f * Rand_ZeroOne() - 1.0f;
        pos = this->dyna.actor.world.pos;
        pos.x += randomf * 20.0f * Math_SinS(this->dyna.actor.world.rot.y + 0x4000);
        pos.z += randomf * 20.0f * Math_CosS(this->dyna.actor.world.rot.y + 0x4000);

        randomf = 2.0f * Rand_ZeroOne() - 1.0f;
        velocity.x = randomf * 1.6f * Math_SinS(this->dyna.actor.world.rot.y);
        velocity.y = 1.8f;
        velocity.z = randomf * 1.6f * Math_CosS(this->dyna.actor.world.rot.y);
        EffectSsIceSmoke_Spawn(play, &pos, &velocity, &accel, 150);
    }
}

void EnBox_Update(Actor* thisx, PlayState* play) {
    EnBox* this = THIS;

    if (this->movementFlags & ENBOX_MOVE_STICK_TO_GROUND) {
        this->movementFlags &= ~ENBOX_MOVE_STICK_TO_GROUND;
        EnBox_ClipToGround(this, play);
    }
    if ((this->getItemId == GI_STRAY_FAIRY) && !Flags_GetTreasure(play, ENBOX_GET_CHEST_FLAG(&this->dyna.actor))) {
        play->actorCtx.flags |= ACTORCTX_FLAG_3;
    }
    this->actionFunc(this, play);
    if (this->movementFlags & ENBOX_MOVE_0x80) {
        this->movementFlags |= ENBOX_MOVE_IMMOBILE;
    }
    if (!(this->movementFlags & ENBOX_MOVE_IMMOBILE)) {
        Actor_MoveWithGravity(&this->dyna.actor);
        Actor_UpdateBgCheckInfo(play, &this->dyna.actor, 0.0f, 0.0f, 0.0f, 0x1C);
    }
    Actor_SetFocus(&this->dyna.actor, 40.0f);
    if ((this->getItemId == GI_ICE_TRAP) && (this->actionFunc == EnBox_Open) && (this->skelAnime.curFrame > 45.0f) &&
        (this->iceSmokeTimer < 100)) {
        EnBox_SpawnIceSmoke(this, play);
    }
    if (this->unk_1F4.unk_0C != NULL) {
        this->unk_1F4.unk_0C(&this->unk_1F4, play);
    }
}

void EnBox_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx, Gfx** gfx) {
    s32 pad;
    EnBox* this = THIS;

    if (limbIndex == OBJECT_BOX_CHEST_LIMB_01) {
        gSPMatrix((*gfx)++, Matrix_NewMtx(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        if (this->type == ENBOX_TYPE_BIG_ORNATE) {
            gSPDisplayList((*gfx)++, &gBoxChestBaseOrnateDL);
        } else if (Actor_IsSmallChest(this)) {
            if (this->getItemId == GI_KEY_SMALL) {
                gSPDisplayList((*gfx)++, &gBoxChestBaseGildedDL);
            } else {
                gSPDisplayList((*gfx)++, &gBoxChestBaseDL);
            }
        } else {
            gSPDisplayList((*gfx)++, &gBoxChestBaseGildedDL);
        }
    } else if (limbIndex == OBJECT_BOX_CHEST_LIMB_03) {
        gSPMatrix((*gfx)++, Matrix_NewMtx(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        if (this->type == ENBOX_TYPE_BIG_ORNATE) {
            gSPDisplayList((*gfx)++, &gBoxChestLidOrnateDL);
        } else if (Actor_IsSmallChest(this)) {
            if (this->getItemId == GI_KEY_SMALL) {
                gSPDisplayList((*gfx)++, &gBoxChestLidGildedDL);
            } else {
                gSPDisplayList((*gfx)++, &gBoxChestLidDL);
            }
        } else {
            gSPDisplayList((*gfx)++, &gBoxChestLidGildedDL);
        }
    }
}

Gfx* EnBox_SetRenderMode1(GraphicsContext* gfxCtx) {
    Gfx* dl = GRAPH_ALLOC(gfxCtx, sizeof(Gfx) * 2);

    gSPEndDisplayList(dl);
    return dl;
}

Gfx* EnBox_SetRenderMode2(GraphicsContext* gfxCtx) {
    Gfx* dl = GRAPH_ALLOC(gfxCtx, sizeof(Gfx) * 2);
    Gfx* cur = dl;

    gDPSetRenderMode(
        cur++, AA_EN | Z_CMP | Z_UPD | IM_RD | CLR_ON_CVG | CVG_DST_WRAP | ZMODE_XLU | FORCE_BL | G_RM_FOG_SHADE_A,
        AA_EN | Z_CMP | Z_UPD | IM_RD | CLR_ON_CVG | CVG_DST_WRAP | ZMODE_XLU | FORCE_BL |
            GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA));

    gSPEndDisplayList(cur++);
    return dl;
}

Gfx* EnBox_SetRenderMode3(GraphicsContext* gfxCtx) {
    Gfx* dl = GRAPH_ALLOC(gfxCtx, sizeof(Gfx) * 2);
    Gfx* cur = dl;

    gDPSetRenderMode(cur++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2);
    gSPEndDisplayList(cur++);
    return dl;
}

void EnBox_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnBox* this = THIS;

    OPEN_DISPS(play->state.gfxCtx);
    if (this->unk_1F4.unk_10 != NULL) {
        this->unk_1F4.unk_10(&this->unk_1F4, play);
    }
    if (((this->alpha == 255) && (this->type != ENBOX_TYPE_BIG_INVISIBLE) &&
         (this->type != ENBOX_TYPE_SMALL_INVISIBLE)) ||
        (!CHECK_FLAG_ALL(this->dyna.actor.flags, ACTOR_FLAG_80) &&
         ((this->type == ENBOX_TYPE_BIG_INVISIBLE) || (this->type == ENBOX_TYPE_SMALL_INVISIBLE)))) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
        gSPSegment(POLY_OPA_DISP++, 0x08, EnBox_SetRenderMode1(play->state.gfxCtx));
        func_8012C28C(play->state.gfxCtx);
        POLY_OPA_DISP = SkelAnime_Draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, NULL,
                                       EnBox_PostLimbDraw, &this->dyna.actor, POLY_OPA_DISP);
    } else if (this->alpha != 0) {
        gDPPipeSync(POLY_XLU_DISP++);
        func_8012C2DC(play->state.gfxCtx);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);
        if ((this->type == ENBOX_TYPE_BIG_INVISIBLE) || (this->type == ENBOX_TYPE_SMALL_INVISIBLE)) {
            gSPSegment(POLY_XLU_DISP++, 0x08, EnBox_SetRenderMode3(play->state.gfxCtx));
        } else {
            gSPSegment(POLY_XLU_DISP++, 0x08, EnBox_SetRenderMode2(play->state.gfxCtx));
        }
        POLY_XLU_DISP = SkelAnime_Draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, NULL,
                                       EnBox_PostLimbDraw, &this->dyna.actor, POLY_XLU_DISP);
    }
    CLOSE_DISPS(play->state.gfxCtx);
}
