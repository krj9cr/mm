/*
 * File: z_en_ru.c
 * Overlay: ovl_En_Ru
 * Description: OoT's Adult Ruto (unused)
 */

#include "z_en_ru.h"

#define FLAGS (ACTOR_FLAG_1 | ACTOR_FLAG_8 | ACTOR_FLAG_10)

#define THIS ((EnRu*)thisx)

void EnRu_Init(Actor* thisx, PlayState* play);
void EnRu_Destroy(Actor* thisx, PlayState* play);
void EnRu_Update(Actor* thisx, PlayState* play);
void EnRu_Draw(Actor* thisx, PlayState* play);

void EnRu_DoNothing(EnRu* this, PlayState* play);

const ActorInit En_Ru_InitVars = {
    ACTOR_EN_RU,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_RU2,
    sizeof(EnRu),
    (ActorFunc)EnRu_Init,
    (ActorFunc)EnRu_Destroy,
    (ActorFunc)EnRu_Update,
    (ActorFunc)EnRu_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT0,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        TOUCH_NONE | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_ON,
    },
    { 18, 64, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

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

static AnimationInfoS sAnimationInfo[] = {
    { &gAdultRutoIdleAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },
    { &gAdultRutoIdleAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },
    { &gAdultRutoRaisingArmsUpAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },
    { &gAdultRutoCrossingArmsAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },
    { &gAdultRutoLookingDownLeftAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },
    { &gAdultRutoIdleHandsOnHipsAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },
    { &gAdultRutoHeadTurnDownLeftAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },
    { &gAdultRutoSwimmingUpAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },
};

// in PostLimbdraw, converts limbIndex to bodyPartsPos index
static s8 sBodyPartPosIndices[] = {
    -1, -1, 12, 13, 14, -1, 9, 10, 11, -1, 0, 6, -1, -1, 7, 8, 2, -1, -1, 3, 4, 2, 1,
};

static s8 sRuBodyParts[RU_BODYPARTSPOS_COUNT] = {
    0, 0, 0, 0, 3, 4, 0, 6, 7, 0, 9, 10, 0, 12, 13,
};

static u8 sRuShadowSizes[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

static TrackOptionsSet sTrackOptions = {
    { 0xFA0, 4, 1, 3 }, { 0x1770, 4, 1, 6 }, { 0xFA0, 4, 1, 3 }, { 0x1770, 4, 1, 6 }
};

s32 EnRu_ChangeAnim(SkelAnime* skelAnime, s16 animIndex) {
    s16 lastFrame;
    s32 ret = false;

    if ((animIndex >= 0) && (animIndex < ARRAY_COUNT(sAnimationInfo))) {
        lastFrame = sAnimationInfo[animIndex].frameCount;

        ret = true;
        if (lastFrame < 0) {
            lastFrame = Animation_GetLastFrame(sAnimationInfo[animIndex].animation);
        }

        Animation_Change(skelAnime, sAnimationInfo[animIndex].animation, sAnimationInfo[animIndex].playSpeed,
                         sAnimationInfo[animIndex].startFrame, lastFrame, sAnimationInfo[animIndex].mode,
                         sAnimationInfo[animIndex].morphFrames);
    }

    return ret;
}

// En_Zo has a copy of this function
s32 EnRu_PlayWalkingSound(EnRu* this, PlayState* play) {
    u8 leftWasGrounded;
    u8 rightWasGrounded;
    s32 waterSfxId;
    s16 sfxId;
    u8 isFootGrounded;

    leftWasGrounded = this->isLeftFootGrounded;
    rightWasGrounded = this->isRightFootGrounded;

    if (this->actor.bgCheckFlags & 0x20) {
        if (this->actor.depthInWater < 20.0f) {
            waterSfxId = NA_SE_PL_WALK_WATER0 - SFX_FLAG;
        } else {
            waterSfxId = NA_SE_PL_WALK_WATER1 - SFX_FLAG;
        }

        sfxId = waterSfxId + SFX_FLAG;

    } else {
        sfxId = SurfaceType_GetSfx(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId) + SFX_FLAG;
    }

    this->isLeftFootGrounded = isFootGrounded = SubS_IsFloorAbove(play, &this->leftFootPos, -6.0f);

    if (this->isLeftFootGrounded && !leftWasGrounded && isFootGrounded) {
        Actor_PlaySfxAtPos(&this->actor, sfxId);
    }

    this->isRightFootGrounded = isFootGrounded = SubS_IsFloorAbove(play, &this->rightFootPos, -6.0f);
    if (this->isRightFootGrounded && !rightWasGrounded && isFootGrounded) {
        Actor_PlaySfxAtPos(&this->actor, sfxId);
    }

    return false;
}

// This function is only ever called with eyeStateMax == 3
void EnRu_UpdateEyes(EnRu* this, s32 eyeStateMax) {
    if (DECR(this->blinkTimer) == 0) {
        this->eyeState++;
        if (this->eyeState >= eyeStateMax) {
            this->eyeState = 0;
            this->blinkTimer = Rand_S16Offset(30, 30);
        }
    }
}

void EnRu_UpdateCollider(EnRu* this, PlayState* play) {
    this->collider.dim.pos.x = this->actor.world.pos.x;
    this->collider.dim.pos.y = this->actor.world.pos.y;
    this->collider.dim.pos.z = this->actor.world.pos.z;
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void EnRu_UpdateModel(EnRu* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    SkelAnime_Update(&this->skelAnime);

    // Head and Torso tracking to Player if in front of Ru.
    if (SubS_AngleDiffLessEqual(this->actor.shape.rot.y, 0x2710, this->actor.yawTowardsPlayer)) {
        Vec3f playerPos;

        playerPos.x = player->actor.world.pos.x;
        playerPos.y = player->bodyPartsPos[7].y + 3.0f;
        playerPos.z = player->actor.world.pos.z;
        SubS_TrackPoint(&playerPos, &this->actor.focus.pos, &this->actor.shape.rot, &this->trackTarget, &this->headRot,
                        &this->torsoRot, &sTrackOptions);

    } else { // smooth her back to facing forward
        Math_SmoothStepToS(&this->trackTarget.x, 0, 4, 0x3E8, 1);
        Math_SmoothStepToS(&this->trackTarget.y, 0, 4, 0x3E8, 1);
        Math_SmoothStepToS(&this->headRot.x, 0, 4, 0x3E8, 1);
        Math_SmoothStepToS(&this->headRot.y, 0, 4, 0x3E8, 1);
        Math_SmoothStepToS(&this->torsoRot.x, 0, 4, 0x3E8, 1);
        Math_SmoothStepToS(&this->torsoRot.y, 0, 4, 0x3E8, 1);
    }

    EnRu_UpdateEyes(this, 3);
    EnRu_PlayWalkingSound(this, play);
    SubS_FillLimbRotTables(play, this->limbRotTableY, this->limbRotTableZ, RU2_LIMB_MAX);
}

void EnRu_DoNothing(EnRu* this, PlayState* play) {
}

void EnRu_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnRu* this = THIS;

    ActorShape_Init(&this->actor.shape, 0.0f, NULL, 0.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gAdultRutoSkel, NULL, this->jointTable, this->morphTable, RU2_LIMB_MAX);
    EnRu_ChangeAnim(&this->skelAnime, 0);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);

    this->path = SubS_GetPathByIndex(play, RU_GET_PATH(thisx), 0x3F);
    Actor_SetScale(&this->actor, 0.01f);
    this->actionFunc = EnRu_DoNothing;
    this->actor.gravity = -4.0f;
}

void EnRu_Destroy(Actor* thisx, PlayState* play) {
    EnRu* this = THIS;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnRu_Update(Actor* thisx, PlayState* play) {
    EnRu* this = THIS;

    this->actionFunc(this, play);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 4);
    EnRu_UpdateModel(this, play);
    EnRu_UpdateCollider(this, play);
}

s32 EnRu_OverrideLimbdraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx,
                          Gfx** gfx) {
    EnRu* this = THIS;

    if (limbIndex == RU2_LIMB_HEAD) {
        Matrix_Translate(1500.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        Matrix_RotateXS(this->headRot.y, MTXMODE_APPLY);
        Matrix_RotateZS(this->headRot.x, MTXMODE_APPLY);
        Matrix_Translate(-1500.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }

    if (limbIndex == RU2_LIMB_TORSO) {
        Matrix_RotateYS(this->torsoRot.y, MTXMODE_APPLY);
        Matrix_RotateXS(this->torsoRot.x, MTXMODE_APPLY);
    }

    if ((limbIndex == RU2_LIMB_TORSO) || (limbIndex == RU2_LIMB_LEFT_UPPER_ARM) ||
        (limbIndex == RU2_LIMB_RIGHT_UPPER_ARM)) {
        rot->y += (s16)(Math_SinS(this->limbRotTableY[limbIndex]) * 200.0f);
        rot->z += (s16)(Math_CosS(this->limbRotTableZ[limbIndex]) * 200.0f);
    }

    return false;
}

void EnRu_PostLimbdraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx, Gfx** gfx) {
    EnRu* this = THIS;
    Vec3f headFocus = { 800.0f, 0, 0 };
    Vec3f bodyPartPos = { 0, 0, 0 };

    if (sBodyPartPosIndices[limbIndex] >= 0) {
        Matrix_MultVec3f(&bodyPartPos, &this->bodyPartsPos[sBodyPartPosIndices[limbIndex]]);
    }
    if (limbIndex == RU2_LIMB_HEAD) {
        Matrix_MultVec3f(&headFocus, &thisx->focus.pos);
    }

    if (limbIndex == RU2_LIMB_LEFT_FOOT) {
        Matrix_MultVec3f(&bodyPartPos, &this->leftFootPos);
    }
    if (limbIndex == RU2_LIMB_RIGHT_FOOT) {
        Matrix_MultVec3f(&bodyPartPos, &this->rightFootPos);
    }
}

// This is a copy of displaylist found in En_Zo
static Gfx sTransparencyDlist[] = {
    gsDPSetRenderMode(AA_EN | Z_CMP | Z_UPD | IM_RD | CLR_ON_CVG | CVG_DST_WRAP | ZMODE_XLU | FORCE_BL |
                          G_RM_FOG_SHADE_A,
                      AA_EN | Z_CMP | Z_UPD | IM_RD | CLR_ON_CVG | CVG_DST_WRAP | ZMODE_XLU | FORCE_BL |
                          GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA)),
    gsDPSetAlphaCompare(G_AC_THRESHOLD),
    gsSPEndDisplayList(),
};

void EnRu_Draw(Actor* thisx, PlayState* play) {
    EnRu* this = THIS;
    u8* shadowTex = GRAPH_ALLOC(play->state.gfxCtx, SUBS_SHADOW_TEX_SIZE);
    u8* shadowTexIter;
    s32 i;
    TexturePtr eyeTextures[] = { gAdultRutoEyeOpenTex, gAdultRutoEyeHalfTex, gAdultRutoEyeClosedTex };

    OPEN_DISPS(play->state.gfxCtx);

    func_8012C28C(play->state.gfxCtx);

    gDPPipeSync(POLY_OPA_DISP++);

    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);

    gDPPipeSync(POLY_OPA_DISP++);

    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(eyeTextures[this->eyeState]));

    // only runs the last command of the display list, which is gsSPEndDisplayList
    gSPSegment(POLY_OPA_DISP++, 0x0C, &sTransparencyDlist[2]);

    POLY_OPA_DISP =
        SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                           EnRu_OverrideLimbdraw, EnRu_PostLimbdraw, &this->actor, POLY_OPA_DISP);

    Matrix_RotateXS(0, MTXMODE_NEW);

    for (i = 0, shadowTexIter = shadowTex; i < SUBS_SHADOW_TEX_SIZE; i++) {
        *shadowTexIter++ = 0;
    }

    for (i = 0; i < 5; i++) {
        SubS_GenShadowTex(&this->bodyPartsPos[0], &this->actor.world.pos, shadowTex, (i / 5.0f),
                          (ARRAY_COUNT(sRuBodyParts)), sRuShadowSizes, sRuBodyParts);
    }

    SubS_DrawShadowTex(&this->actor, &play->state, shadowTex);

    CLOSE_DISPS(play->state.gfxCtx);
}
