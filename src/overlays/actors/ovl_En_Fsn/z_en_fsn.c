/*
 * File: z_en_fsn.c
 * Overlay: ovl_En_Fsn
 * Description: Curiosity Shop Man
 */

#include "z_en_fsn.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_1 | ACTOR_FLAG_8 | ACTOR_FLAG_10)

#define THIS ((EnFsn*)thisx)

#define SI_NONE 0

#define ENFSN_END_CONVERSATION (1 << 0)
#define ENFSN_GIVE_ITEM (1 << 1)
#define ENFSN_GAVE_KEATONS_MASK (1 << 2)
#define ENFSN_GAVE_LETTER_TO_MAMA (1 << 3)

void EnFsn_Init(Actor* thisx, PlayState* play);
void EnFsn_Destroy(Actor* thisx, PlayState* play);
void EnFsn_Update(Actor* thisx, PlayState* play);
void EnFsn_Draw(Actor* thisx, PlayState* play);

void EnFsn_Idle(EnFsn* this, PlayState* play);
void EnFsn_ConverseBackroom(EnFsn* this, PlayState* play);
void EnFsn_Haggle(EnFsn* this, PlayState* play);
void EnFsn_BeginInteraction(EnFsn* this, PlayState* play);
void EnFsn_GiveItem(EnFsn* this, PlayState* play);
void EnFsn_AskBuyOrSell(EnFsn* this, PlayState* play);
void EnFsn_StartBuying(EnFsn* this, PlayState* play);
void EnFsn_SetupEndInteractionImmediately(EnFsn* this, PlayState* play);
void EnFsn_SetupResumeInteraction(EnFsn* this, PlayState* play);
void EnFsn_FaceShopkeeperSelling(EnFsn* this, PlayState* play);
void EnFsn_DeterminePrice(EnFsn* this, PlayState* play);
void EnFsn_SetupDeterminePrice(EnFsn* this, PlayState* play);
void EnFsn_SetupEndInteraction(EnFsn* this, PlayState* play);
void EnFsn_MakeOffer(EnFsn* this, PlayState* play);
void EnFsn_ResumeInteraction(EnFsn* this, PlayState* play);
void EnFsn_BrowseShelf(EnFsn* this, PlayState* play);
void EnFsn_ResumeShoppingInteraction(EnFsn* this, PlayState* play);
void EnFsn_AskCanBuyMore(EnFsn* this, PlayState* play);
void EnFsn_AskCanBuyAterRunningOutOfItems(EnFsn* this, PlayState* play);
void EnFsn_SelectItem(EnFsn* this, PlayState* play);
void EnFsn_LookToShopkeeperFromShelf(EnFsn* this, PlayState* play);
void EnFsn_PlayerCannotBuy(EnFsn* this, PlayState* play);

typedef enum {
    /* 0 */ ENFSN_CUTSCENESTATE_STOPPED,
    /* 1 */ ENFSN_CUTSCENESTATE_WAITING,
    /* 2 */ ENFSN_CUTSCENESTATE_PLAYING
} EnFsnCutsceneState;

typedef enum {
    /*  0 */ FSN_ANIM_IDLE,
    /*  1 */ FSN_ANIM_SCRATCH_BACK,
    /*  2 */ FSN_ANIM_TURN_AROUND_FORWARD,
    /*  3 */ FSN_ANIM_TURN_AROUND_REVERSE,
    /*  4 */ FSN_ANIM_HANDS_ON_COUNTER_START,
    /*  5 */ FSN_ANIM_HANDS_ON_COUNTER_LOOP,
    /*  6 */ FSN_ANIM_HAND_ON_FACE_START,
    /*  7 */ FSN_ANIM_HAND_ON_FACE_LOOP,
    /*  8 */ FSN_ANIM_LEAN_FORWARD_START,
    /*  9 */ FSN_ANIM_LEAN_FORWARD_LOOP,
    /* 10 */ FSN_ANIM_SLAM_COUNTER_START,
    /* 11 */ FSN_ANIM_SLAM_COUNTER_LOOP,
    /* 12 */ FSN_ANIM_MAKE_OFFER,
    /* 13 */ FSN_ANIM_MAX
} FsnAnimation;

const ActorInit En_Fsn_InitVars = {
    ACTOR_EN_FSN,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_FSN,
    sizeof(EnFsn),
    (ActorFunc)EnFsn_Init,
    (ActorFunc)EnFsn_Destroy,
    (ActorFunc)EnFsn_Update,
    (ActorFunc)EnFsn_Draw,
};

static AnimationInfoS sAnimationInfo[] = {
    { &gFsnIdleAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },
    { &gFsnScratchBackAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },
    { &gFsnTurnAroundAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },
    { &gFsnTurnAroundAnim, -1.0f, 0, -1, ANIMMODE_ONCE, 0 },
    { &gFsnHandsOnCounterStartAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },
    { &gFsnHandsOnCounterLoopAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },
    { &gFsnHandOnFaceStartAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },
    { &gFsnHandOnFaceLoopAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },
    { &gFsnLeanForwardStartAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },
    { &gFsnLeanForwardLoopAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },
    { &gFsnSlamCounterStartAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },
    { &gFsnSlamCounterLoopAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },
    { &gFsnMakeOfferAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
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

static Vec3f sShopItemPositions[3] = {
    { -5.0f, 35.0f, -95.0f },
    { 13.0f, 35.0f, -95.0f },
    { 31.0f, 35.0f, -95.0f },
};

s32 EnFsn_TestItemSelected(PlayState* play) {
    MessageContext* msgCtx = &play->msgCtx;

    if (msgCtx->unk12020 == 0x10 || msgCtx->unk12020 == 0x11) {
        return CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_A);
    }
    return CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_A) ||
           CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_B) ||
           CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_CUP);
}

u16 EnFsn_GetWelcome(PlayState* play) {
    switch (Player_GetMask(play)) {
        case PLAYER_MASK_NONE:
            return 0x29CC;
        case PLAYER_MASK_DEKU:
            return 0x29FC;
        case PLAYER_MASK_GORON:
        case PLAYER_MASK_ZORA:
            return 0x29FD;
        case PLAYER_MASK_KAFEIS_MASK:
            return 0x2364;
        default:
            return 0x29FE;
    }
}

void EnFsn_HandleConversationBackroom(EnFsn* this, PlayState* play) {
    switch (this->textId) {
        case 0:
            if (!(gSaveContext.save.weekEventReg[80] & 0x10)) {
                this->textId = 0x29E0;
                break;
            } else {
                this->textId = 0x29E4;
                this->flags |= ENFSN_END_CONVERSATION;
                break;
            }
        case 0x29E0:
            if (INV_CONTENT(ITEM_MASK_KEATON) == ITEM_MASK_KEATON) {
                this->flags |= ENFSN_GIVE_ITEM;
                this->flags |= ENFSN_GAVE_LETTER_TO_MAMA;
                this->getItemId = GI_LETTER_TO_MAMA;
                gSaveContext.save.weekEventReg[80] |= 0x10;
                this->textId = 0x29F1;
                break;
            } else {
                this->textId = 0x29E1;
                break;
            }
        case 0x29E1:
            this->textId = 0x29E2;
            break;
        case 0x29E2:
            this->flags |= ENFSN_GIVE_ITEM;
            this->flags |= ENFSN_GAVE_KEATONS_MASK;
            this->getItemId = GI_MASK_KEATON;
            this->textId = 0x29E3;
            break;
        case 0x29E3:
            this->flags |= ENFSN_GIVE_ITEM;
            this->flags |= ENFSN_GAVE_LETTER_TO_MAMA;
            this->getItemId = GI_LETTER_TO_MAMA;
            gSaveContext.save.weekEventReg[80] |= 0x10;
            this->textId = 0x29F1;
            break;
        case 0x29F1:
            this->textId = 0x29E4;
            this->flags |= ENFSN_END_CONVERSATION;
            break;
    }
    Message_StartTextbox(play, this->textId, &this->actor);
    if (this->flags & ENFSN_END_CONVERSATION) {
        if (this->flags & ENFSN_GAVE_LETTER_TO_MAMA) {
            this->flags &= ~ENFSN_GAVE_LETTER_TO_MAMA;
            func_80151BB4(play, 34);
        }
        if (this->flags & ENFSN_GAVE_KEATONS_MASK) {
            this->flags &= ~ENFSN_GAVE_KEATONS_MASK;
            func_80151BB4(play, 33);
        }
        func_80151BB4(play, 3);
    }
}

void EnFsn_HandleSetupResumeInteraction(EnFsn* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play) &&
        this->cutsceneState == ENFSN_CUTSCENESTATE_STOPPED) {
        Actor_ProcessTalkRequest(&this->actor, &play->state);
        func_800B85E0(&this->actor, play, 400.0f, PLAYER_AP_MINUS1);
        if (ENFSN_IS_SHOP(&this->actor)) {
            this->actor.textId = 0;
        }
        this->actionFunc = EnFsn_ResumeInteraction;
    }
}

void EnFsn_UpdateCollider(EnFsn* this, PlayState* play) {
    this->collider.dim.pos.x = this->actor.world.pos.x;
    this->collider.dim.pos.y = this->actor.world.pos.y;
    this->collider.dim.pos.z = this->actor.world.pos.z;
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void EnFsn_HandleLookToShopkeeperBuyingCutscene(EnFsn* this) {
    if ((this->cutsceneState == ENFSN_CUTSCENESTATE_PLAYING) &&
        (this->lookToShopkeeperBuyingCutscene != this->cutscene) && (this->actor.textId == 0x29CE)) {
        ActorCutscene_Stop(this->cutscene);
        if (ActorCutscene_GetCurrentIndex() == 0x7C) {
            ActorCutscene_Stop(0x7C);
        }
        this->cutscene = this->lookToShopkeeperBuyingCutscene;
        ActorCutscene_SetIntentToPlay(this->cutscene);
        this->cutsceneState = ENFSN_CUTSCENESTATE_WAITING;
    } else if (this->cutsceneState == ENFSN_CUTSCENESTATE_WAITING) {
        if (ActorCutscene_GetCanPlayNext(this->cutscene)) {
            ActorCutscene_Start(this->cutscene, &this->actor);
            this->cutsceneState = ENFSN_CUTSCENESTATE_PLAYING;
        } else {
            ActorCutscene_SetIntentToPlay(this->cutscene);
        }
    }
}

u8 EnFsn_SetCursorIndexFromNeutral(EnFsn* this) {
    if (this->itemIds[0] != -1) {
        return 0;
    }
    if (this->itemIds[1] != -1) {
        return 1;
    }
    if (this->itemIds[2] != -1) {
        return 2;
    }
    return CURSOR_INVALID;
}

void EnFsn_CursorLeftRight(EnFsn* this) {
    u8 cursorScan = this->cursorIndex;

    if (this->stickAccumX > 0) {
        if (cursorScan != this->totalSellingItems - 1) {
            while (cursorScan != this->totalSellingItems - 1) {
                cursorScan++;
                if (this->itemIds[cursorScan] != -1) {
                    this->cursorIndex = cursorScan;
                    break;
                }
            }
        } else if (this->itemIds[cursorScan] != -1) {
            this->cursorIndex = cursorScan;
        }
    } else if (this->stickAccumX < 0) {
        if (cursorScan != 0) {
            while (cursorScan != 0) {
                cursorScan--;
                if (this->itemIds[cursorScan] != -1) {
                    this->cursorIndex = cursorScan;
                    break;
                } else if (cursorScan == 0) {
                    play_sound(NA_SE_SY_CURSOR);
                    this->drawCursor = 0;
                    this->actionFunc = EnFsn_LookToShopkeeperFromShelf;
                    break;
                }
            }
        } else {
            play_sound(NA_SE_SY_CURSOR);
            this->drawCursor = 0;
            this->actionFunc = EnFsn_LookToShopkeeperFromShelf;
            if (this->itemIds[cursorScan] != -1) {
                this->cursorIndex = cursorScan;
            }
        }
    }
}

s16 EnFsn_GetThirdDayItemId(void) {
    if (!(gSaveContext.save.weekEventReg[33] & 4) && CURRENT_DAY == 3) {
        if (!(gSaveContext.save.weekEventReg[33] & 8) && !(gSaveContext.save.weekEventReg[79] & 0x40)) {
            return SI_BOMB_BAG_30_1;
        }
        return SI_MASK_ALL_NIGHT;
    }
    return SI_NONE;
}

s16 EnFsn_GetStolenItemId(u32 stolenItem) {
    switch (stolenItem) {
        case ITEM_BOTTLE:
            return SI_BOTTLE;
        case ITEM_SWORD_GREAT_FAIRY:
            return SI_SWORD_GREAT_FAIRY;
        case ITEM_SWORD_KOKIRI:
            return SI_SWORD_KOKIRI;
        case ITEM_SWORD_RAZOR:
            return SI_SWORD_RAZOR;
        case ITEM_SWORD_GILDED:
            return SI_SWORD_GILDED;
    }
    return SI_NONE;
}

s32 EnFsn_HasItemsToSell(void) {
    if (CURRENT_DAY != 3) {
        if ((STOLEN_ITEM_1 != STOLEN_ITEM_NONE) || (STOLEN_ITEM_2 != STOLEN_ITEM_NONE)) {
            return true;
        }
        return false;
    }

    if ((STOLEN_ITEM_1 != STOLEN_ITEM_NONE) || (STOLEN_ITEM_2 != STOLEN_ITEM_NONE) ||
        !(gSaveContext.save.weekEventReg[33] & 4)) {
        return true;
    }

    return false;
}

void EnFsn_GetShopItemIds(EnFsn* this) {
    u32 stolenItem1 = STOLEN_ITEM_1;
    u32 stolenItem2 = STOLEN_ITEM_2;
    s16 itemId;

    this->stolenItem1 = this->stolenItem2 = 0;
    this->itemIds[0] = this->itemIds[1] = this->itemIds[2] = 0;

    itemId = EnFsn_GetThirdDayItemId();
    this->itemIds[this->totalSellingItems] = itemId;
    if (itemId != SI_NONE) {
        this->totalSellingItems++;
    }
    itemId = EnFsn_GetStolenItemId(stolenItem1);
    this->itemIds[this->totalSellingItems] = itemId;
    if (itemId != SI_NONE) {
        this->stolenItem1 = this->totalSellingItems;
        this->totalSellingItems++;
    }
    itemId = EnFsn_GetStolenItemId(stolenItem2);
    this->itemIds[this->totalSellingItems] = itemId;
    if (itemId != SI_NONE) {
        this->stolenItem2 = this->totalSellingItems;
        this->totalSellingItems++;
    }
    this->numSellingItems = this->totalSellingItems;
}

void EnFsn_SpawnShopItems(EnFsn* this, PlayState* play) {
    s32 i;

    EnFsn_GetShopItemIds(this);
    for (i = 0; i < this->totalSellingItems; i++) {
        if (this->itemIds[i] < 0) {
            this->items[i] = NULL;
        } else {
            this->items[i] =
                (EnGirlA*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_GIRLA, sShopItemPositions[i].x,
                                      sShopItemPositions[i].y, sShopItemPositions[i].z, 0, 0, 0, this->itemIds[i]);
        }
    }
}

void EnFsn_EndInteraction(EnFsn* this, PlayState* play) {
    if (this->cutsceneState == ENFSN_CUTSCENESTATE_PLAYING) {
        ActorCutscene_Stop(this->cutscene);
        this->cutsceneState = ENFSN_CUTSCENESTATE_STOPPED;
    }
    Actor_ProcessTalkRequest(&this->actor, &play->state);
    play->msgCtx.msgMode = 0x43;
    play->msgCtx.stateTimer = 4;
    Interface_SetHudVisibility(HUD_VISIBILITY_ALL);
    this->drawCursor = 0;
    this->stickLeftPrompt.isEnabled = false;
    this->stickRightPrompt.isEnabled = false;
    play->interfaceCtx.unk_222 = 0;
    play->interfaceCtx.unk_224 = 0;
    this->actor.textId = 0;
    this->actionFunc = EnFsn_Idle;
}

s32 EnFsn_TestEndInteraction(EnFsn* this, PlayState* play, Input* input) {
    if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
        this->actor.textId = (CURRENT_DAY == 3) ? 0x29DF : 0x29D1;
        Message_StartTextbox(play, this->actor.textId, &this->actor);
        func_80151BB4(play, 3);
        this->actionFunc = EnFsn_SetupEndInteraction;
        return true;
    }
    return false;
}

s32 EnFsn_TestCancelOption(EnFsn* this, PlayState* play, Input* input) {
    if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
        this->actionFunc = this->prevActionFunc;
        func_80151938(play, this->items[this->cursorIndex]->actor.textId);
        return true;
    }
    return false;
}

void EnFsn_UpdateCursorPos(EnFsn* this, PlayState* play) {
    s16 x;
    s16 y;
    f32 xOffset = 0.0f;
    f32 yOffset = 17.0f;

    Actor_GetScreenPos(play, &this->items[this->cursorIndex]->actor, &x, &y);
    this->cursorPos.x = x + xOffset;
    this->cursorPos.y = y + yOffset;
    this->cursorPos.z = 1.2f;
}

s32 EnFsn_FacingShopkeeperDialogResult(EnFsn* this, PlayState* play) {
    switch (play->msgCtx.choiceIndex) {
        case 0:
            func_8019F208();
            if (CURRENT_DAY != 3) {
                this->actor.textId = 0x29FB;
            } else if (gSaveContext.save.weekEventReg[33] & 4) {
                this->actor.textId = 0x29FF;
            } else if (!(gSaveContext.save.weekEventReg[33] & 8) && !(gSaveContext.save.weekEventReg[79] & 0x40)) {
                this->actor.textId = 0x29D7;
            } else {
                this->actor.textId = 0x29D8;
            }
            Message_StartTextbox(play, this->actor.textId, &this->actor);
            return true;
        case 1:
            func_8019F230();
            this->actor.textId = (CURRENT_DAY == 3) ? 0x29DF : 0x29D1;
            Message_StartTextbox(play, this->actor.textId, &this->actor);
            func_80151BB4(play, 3);
            this->actionFunc = EnFsn_SetupEndInteraction;
            return true;
    }
    return false;
}

s32 EnFsn_HasPlayerSelectedItem(EnFsn* this, PlayState* play, Input* input) {
    if (EnFsn_TestEndInteraction(this, play, input)) {
        return true;
    }
    if (EnFsn_TestItemSelected(play)) {
        if (!this->items[this->cursorIndex]->isOutOfStock) {
            this->prevActionFunc = this->actionFunc;
            func_80151938(play, this->items[this->cursorIndex]->choiceTextId);
            play_sound(NA_SE_SY_DECIDE);
            this->stickLeftPrompt.isEnabled = false;
            this->stickRightPrompt.isEnabled = false;
            this->drawCursor = 0;
            this->actionFunc = EnFsn_SelectItem;
        } else {
            play_sound(NA_SE_SY_ERROR);
        }
        return true;
    }
    return false;
}

void EnFsn_UpdateJoystickInputState(EnFsn* this, PlayState* play) {
    s8 stickX = CONTROLLER1(&play->state)->rel.stick_x;
    s8 stickY = CONTROLLER1(&play->state)->rel.stick_y;

    if (this->stickAccumX == 0) {
        if (stickX > 30 || stickX < -30) {
            this->stickAccumX = stickX;
        }
    } else if (stickX <= 30 && stickX >= -30) {
        this->stickAccumX = 0;
    } else if ((this->stickAccumX * stickX) < 0) { // Stick has swapped directions
        this->stickAccumX = stickX;
    } else {
        this->stickAccumX += stickX;
        if (this->stickAccumX > 2000) {
            this->stickAccumX = 2000;
        } else if (this->stickAccumX < -2000) {
            this->stickAccumX = -2000;
        }
    }
    if (this->stickAccumY == 0) {
        if (stickY > 30 || stickY < -30) {
            this->stickAccumY = stickY;
        }
    } else if (stickY <= 30 && stickY >= -30) {
        this->stickAccumY = 0;
    } else if ((this->stickAccumY * stickY) < 0) { // Stick has swapped directions
        this->stickAccumY = stickY;
    } else {
        this->stickAccumY += stickY;
        if (this->stickAccumY > 2000) {
            this->stickAccumY = 2000;
        } else if (this->stickAccumY < -2000) {
            this->stickAccumY = -2000;
        }
    }
}

void EnFsn_PositionSelectedItem(EnFsn* this) {
    Vec3f selectedItemPosition = { 13.0f, 38.0f, -71.0f };
    u8 i = this->cursorIndex;
    EnGirlA* item = this->items[i];
    Vec3f worldPos;

    VEC3F_LERPIMPDST(&worldPos, &sShopItemPositions[i], &selectedItemPosition, this->shopItemSelectedTween);

    item->actor.world.pos.x = worldPos.x;
    item->actor.world.pos.y = worldPos.y;
    item->actor.world.pos.z = worldPos.z;
}

/*
 *    Returns true if animation has completed
 */
s32 EnFsn_TakeItemOffShelf(EnFsn* this) {
    Math_ApproachF(&this->shopItemSelectedTween, 1.0f, 1.0f, 0.15f);
    if (this->shopItemSelectedTween >= 0.85f) {
        this->shopItemSelectedTween = 1.0f;
    }
    EnFsn_PositionSelectedItem(this);
    if (this->shopItemSelectedTween == 1.0f) {
        return true;
    }
    return false;
}

/*
 *    Returns true if animation has completed
 */
s32 EnFsn_ReturnItemToShelf(EnFsn* this) {
    Math_ApproachF(&this->shopItemSelectedTween, 0.0f, 1.0f, 0.15f);
    if (this->shopItemSelectedTween <= 0.15f) {
        this->shopItemSelectedTween = 0.0f;
    }
    EnFsn_PositionSelectedItem(this);
    if (this->shopItemSelectedTween == 0.0f) {
        return true;
    }
    return false;
}

void EnFsn_UpdateItemSelectedProperty(EnFsn* this) {
    EnGirlA** items;
    s32 i;

    for (items = this->items, i = 0; i < this->totalSellingItems; items++, i++) {
        if (this->actionFunc != EnFsn_SelectItem && this->actionFunc != EnFsn_PlayerCannotBuy &&
            this->drawCursor == 0) {
            (*items)->isSelected = false;
        } else {
            (*items)->isSelected = (i == this->cursorIndex) ? true : false;
        }
    }
}

void EnFsn_UpdateCursorAnim(EnFsn* this) {
    f32 t = this->cursorAnimTween;

    if (this->cursorAnimState == 0) {
        t += 0.05f;
        if (t >= 1.0f) {
            t = 1.0f;
            this->cursorAnimState = 1;
        }
    } else {
        t -= 0.05f;
        if (t <= 0.0f) {
            t = 0.0f;
            this->cursorAnimState = 0;
        }
    }
    this->cursorColor.r = COL_CHAN_MIX(0, 0.0f, t);
    this->cursorColor.g = COL_CHAN_MIX(80, 80.0f, t);
    this->cursorColor.b = COL_CHAN_MIX(255, 0.0f, t);
    this->cursorColor.a = COL_CHAN_MIX(255, 0.0f, t);
    this->cursorAnimTween = t;
}

void EnFsn_UpdateStickDirectionPromptAnim(EnFsn* this) {
    f32 arrowAnimTween = this->arrowAnimTween;
    f32 stickAnimTween = this->stickAnimTween;
    s32 maxColor = 255; //! FAKE:

    if (this->arrowAnimState == 0) {
        arrowAnimTween += 0.05f;
        if (arrowAnimTween > 1.0f) {
            arrowAnimTween = 1.0f;
            this->arrowAnimState = 1;
        }
    } else {
        arrowAnimTween -= 0.05f;
        if (arrowAnimTween < 0.0f) {
            arrowAnimTween = 0.0f;
            this->arrowAnimState = 0;
        }
    }
    this->arrowAnimTween = arrowAnimTween;

    if (this->stickAnimState == 0) {
        stickAnimTween += 0.1f;
        if (stickAnimTween > 1.0f) {
            stickAnimTween = 1.0f;
            this->stickAnimState = 1;
        }
    } else {
        stickAnimTween = 0.0f;
        this->stickAnimState = 0;
    }
    this->stickAnimTween = stickAnimTween;

    this->stickLeftPrompt.arrowColor.r = COL_CHAN_MIX(255, 155.0f, arrowAnimTween);
    this->stickLeftPrompt.arrowColor.g = COL_CHAN_MIX(maxColor, 155.0f, arrowAnimTween);
    this->stickLeftPrompt.arrowColor.b = COL_CHAN_MIX(0, -100.0f, arrowAnimTween);
    this->stickLeftPrompt.arrowColor.a = COL_CHAN_MIX(200, 50.0f, arrowAnimTween);

    this->stickRightPrompt.arrowTexX = 290.0f;

    this->stickRightPrompt.arrowColor.r = COL_CHAN_MIX(maxColor, 155.0f, arrowAnimTween);
    this->stickRightPrompt.arrowColor.g = COL_CHAN_MIX(255, 155.0f, arrowAnimTween);
    this->stickRightPrompt.arrowColor.b = COL_CHAN_MIX(0, -100.0f, arrowAnimTween);
    this->stickRightPrompt.arrowColor.a = COL_CHAN_MIX(200, 50.0f, arrowAnimTween);

    this->stickLeftPrompt.arrowTexX = 33.0f;

    this->stickRightPrompt.stickTexX = 274.0f;
    this->stickRightPrompt.stickTexX += 8.0f * stickAnimTween;

    this->stickLeftPrompt.stickTexX = 49.0f;
    this->stickLeftPrompt.stickTexX -= 8.0f * stickAnimTween;

    this->stickRightPrompt.arrowTexY = 91.0f;
    this->stickLeftPrompt.arrowTexY = 91.0f;

    this->stickRightPrompt.stickTexY = 95.0f;
    this->stickLeftPrompt.stickTexY = 95.0f;
}

void EnFsn_InitShop(EnFsn* this, PlayState* play) {
    if (EnFsn_HasItemsToSell()) {
        EnFsn_SpawnShopItems(this, play);

        this->cursorPos.y = this->cursorPos.x = 100.0f;
        this->stickAccumY = 0;
        this->stickAccumX = 0;

        this->cursorPos.z = 1.2f;
        this->cursorColor.r = 0;
        this->cursorColor.g = 80;
        this->cursorColor.b = 255;
        this->cursorColor.a = 255;
        this->cursorAnimTween = 0.0f;
        this->cursorAnimState = 0;
        this->drawCursor = 0;

        this->stickLeftPrompt.stickColor.r = 200;
        this->stickLeftPrompt.stickColor.g = 200;
        this->stickLeftPrompt.stickColor.b = 200;
        this->stickLeftPrompt.stickColor.a = 180;
        this->stickLeftPrompt.stickTexX = 49.0f;
        this->stickLeftPrompt.stickTexY = 95.0f;
        this->stickLeftPrompt.arrowColor.r = 255;
        this->stickLeftPrompt.arrowColor.g = 255;
        this->stickLeftPrompt.arrowColor.b = 0;
        this->stickLeftPrompt.arrowColor.a = 200;
        this->stickLeftPrompt.arrowTexX = 33.0f;
        this->stickLeftPrompt.arrowTexY = 91.0f;
        this->stickLeftPrompt.texZ = 1.0f;
        this->stickLeftPrompt.isEnabled = 0;

        this->stickRightPrompt.stickColor.r = 200;
        this->stickRightPrompt.stickColor.g = 200;
        this->stickRightPrompt.stickColor.b = 200;
        this->stickRightPrompt.stickColor.a = 180;
        this->stickRightPrompt.stickTexX = 274.0f;
        this->stickRightPrompt.stickTexY = 95.0f;
        this->stickRightPrompt.arrowColor.r = 255;
        this->stickRightPrompt.arrowColor.g = 0;
        this->stickRightPrompt.arrowColor.b = 0;
        this->stickRightPrompt.arrowColor.a = 200;
        this->stickRightPrompt.arrowTexX = 290.0f;
        this->stickRightPrompt.arrowTexY = 91.0f;
        this->stickRightPrompt.texZ = 1.0f;
        this->stickRightPrompt.isEnabled = 0;

        this->arrowAnimState = 0;
        this->stickAnimState = 0;
        this->stickAnimTween = this->arrowAnimTween = 0.0f;
    }
    this->blinkTimer = 20;
    this->animIndex = FSN_ANIM_HANDS_ON_COUNTER_START;
    this->eyeTexIndex = 0;
    SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
    this->actionFunc = EnFsn_Idle;
}

void EnFsn_Idle(EnFsn* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->animIndex == FSN_ANIM_HANDS_ON_COUNTER_START) {
        s16 curFrame = this->skelAnime.curFrame;
        s16 frameCount = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);
        if (curFrame == frameCount) {
            this->animIndex = FSN_ANIM_HANDS_ON_COUNTER_LOOP;
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
        }
        return;
    }

    if (this->flags & ENFSN_HAGGLE) {
        this->actionFunc = EnFsn_Haggle;
        return;
    }

    if (Actor_ProcessTalkRequest(&this->actor, &play->state)) {
        if (this->cutsceneState == ENFSN_CUTSCENESTATE_STOPPED) {
            if (ActorCutscene_GetCurrentIndex() == 0x7C) {
                ActorCutscene_Stop(0x7C);
            }
            this->cutscene = this->lookToShopkeeperCutscene;
            ActorCutscene_SetIntentToPlay(this->cutscene);
            this->cutsceneState = ENFSN_CUTSCENESTATE_WAITING;
        }
        this->actor.textId = EnFsn_GetWelcome(play);
        Message_StartTextbox(play, this->actor.textId, &this->actor);
        player->actor.world.pos.x = 1.0f;
        player->actor.world.pos.z = -34.0f;
        this->actionFunc = EnFsn_BeginInteraction;
    } else if (((player->actor.world.pos.x >= -50.0f) && (player->actor.world.pos.x <= 15.0f)) &&
               (player->actor.world.pos.y > 0.0f) &&
               ((player->actor.world.pos.z >= -35.0f) && (player->actor.world.pos.z <= -20.0f))) {
        func_800B8614(&this->actor, play, 400.0f);
    }
}

void EnFsn_Haggle(EnFsn* this, PlayState* play) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 frameCount = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    if (this->flags & ENFSN_ANGRY) {
        this->flags &= ~ENFSN_ANGRY;
        this->animIndex = FSN_ANIM_SLAM_COUNTER_LOOP;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
    } else {
        if (this->animIndex == FSN_ANIM_SLAM_COUNTER_LOOP && Animation_OnFrame(&this->skelAnime, 18.0f)) {
            Actor_PlaySfxAtPos(&this->actor, NA_SE_EV_HANKO);
        }
        if (this->flags & ENFSN_CALM_DOWN) {
            this->flags &= ~ENFSN_CALM_DOWN;
            this->animIndex = FSN_ANIM_HANDS_ON_COUNTER_LOOP;
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
        } else if (this->flags & ENFSN_OFFER_FINAL_PRICE) {
            this->flags &= ~ENFSN_OFFER_FINAL_PRICE;
            this->animIndex = FSN_ANIM_MAKE_OFFER;
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
        } else {
            if (this->animIndex == FSN_ANIM_MAKE_OFFER) {
                if (curFrame == frameCount) {
                    this->animIndex = FSN_ANIM_HANDS_ON_COUNTER_LOOP;
                    SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
                } else {
                    if (Animation_OnFrame(&this->skelAnime, 28.0f)) {
                        Actor_PlaySfxAtPos(&this->actor, NA_SE_EV_HANKO);
                    }
                    return;
                }
            }
            if (!(this->flags & ENFSN_HAGGLE)) {
                this->actionFunc = EnFsn_Idle;
            }
        }
    }
}

void EnFsn_BeginInteraction(EnFsn* this, PlayState* play) {
    if (this->cutsceneState == ENFSN_CUTSCENESTATE_WAITING) {
        if (ActorCutscene_GetCanPlayNext(this->cutscene)) {
            ActorCutscene_StartAndSetFlag(this->cutscene, &this->actor);
            this->cutsceneState = ENFSN_CUTSCENESTATE_PLAYING;
            if (Player_GetMask(play) == PLAYER_MASK_NONE) {
                func_8011552C(play, 16);
                if (EnFsn_HasItemsToSell()) {
                    this->actionFunc = EnFsn_AskBuyOrSell;
                } else {
                    this->isSelling = false;
                    this->actionFunc = EnFsn_StartBuying;
                }
            } else {
                this->actionFunc = EnFsn_SetupEndInteractionImmediately;
            }
        } else {
            ActorCutscene_SetIntentToPlay(this->cutscene);
        }
    }
}

void EnFsn_StartBuying(EnFsn* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);
    Player* player = GET_PLAYER(play);

    EnFsn_HandleLookToShopkeeperBuyingCutscene(this);
    if ((talkState == TEXT_STATE_5) && Message_ShouldAdvance(play)) {
        switch (this->actor.textId) {
            case 0x29CC:
                this->actor.textId = 0x29CD;
                Message_StartTextbox(play, this->actor.textId, &this->actor);
                break;
            case 0x29CD:
                this->actor.textId = 0x29CE;
                EnFsn_HandleLookToShopkeeperBuyingCutscene(this);
                Message_StartTextbox(play, this->actor.textId, &this->actor);
                break;
            case 0x29CE:
                this->actor.textId = 0xFF;
                Message_StartTextbox(play, this->actor.textId, &this->actor);
                this->actionFunc = EnFsn_DeterminePrice;
                break;
            case 0x29CF:
                player->exchangeItemId = PLAYER_AP_NONE;
                this->actionFunc = EnFsn_SetupDeterminePrice;
                break;
        }
    }
}

void EnFsn_AskBuyOrSell(EnFsn* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    if (talkState == TEXT_STATE_5) {
        if (Message_ShouldAdvance(play)) {
            switch (this->actor.textId) {
                case 0x29CC:
                    if (CURRENT_DAY != 3) {
                        this->actor.textId = 0x29FA;
                    } else {
                        this->actor.textId = 0x29D2;
                    }
                    Message_StartTextbox(play, this->actor.textId, &this->actor);
                    break;
                case 0x29D2:
                    if (gSaveContext.save.weekEventReg[33] & 4) {
                        this->actor.textId = 0x2A01;
                    } else if (!(gSaveContext.save.weekEventReg[33] & 8) &&
                               !(gSaveContext.save.weekEventReg[79] & 0x40)) {
                        this->actor.textId = 0x29D3;
                    } else {
                        this->actor.textId = 0x29D4;
                    }
                    Message_StartTextbox(play, this->actor.textId, &this->actor);
                    break;
                case 0x29D3:
                case 0x29D4:
                case 0x29FA:
                case 0x2A01:
                    this->actor.textId = 0x29D5;
                    Message_StartTextbox(play, this->actor.textId, &this->actor);
                    break;
            }
        }
    } else if (talkState == TEXT_STATE_CHOICE) {
        func_8011552C(play, 6);
        if (!EnFsn_TestEndInteraction(this, play, CONTROLLER1(&play->state)) && Message_ShouldAdvance(play)) {
            switch (play->msgCtx.choiceIndex) {
                case 0:
                    func_8019F208();
                    this->isSelling = true;
                    this->stickLeftPrompt.isEnabled = false;
                    this->stickRightPrompt.isEnabled = true;
                    this->actor.textId = 0x29D6;
                    Message_StartTextbox(play, this->actor.textId, &this->actor);
                    this->actionFunc = EnFsn_FaceShopkeeperSelling;
                    break;
                case 1:
                    func_8019F208();
                    this->isSelling = false;
                    this->actor.textId = 0x29CE;
                    EnFsn_HandleLookToShopkeeperBuyingCutscene(this);
                    Message_StartTextbox(play, this->actor.textId, &this->actor);
                    this->actionFunc = EnFsn_StartBuying;
                    break;
            }
        }
    }
}

void EnFsn_SetupDeterminePrice(EnFsn* this, PlayState* play) {
    if (Actor_ProcessTalkRequest(&this->actor, &play->state)) {
        this->actor.textId = 0xFF;
        Message_StartTextbox(play, this->actor.textId, &this->actor);
        this->actionFunc = EnFsn_DeterminePrice;
    }
}

void EnFsn_DeterminePrice(EnFsn* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 itemActionParam;
    u8 buttonItem;

    if (Message_GetState(&play->msgCtx) == TEXT_STATE_16) {
        itemActionParam = func_80123810(play);
        if (itemActionParam > PLAYER_AP_NONE) {
            buttonItem = GET_CUR_FORM_BTN_ITEM(player->heldItemButton);
            this->price = (buttonItem < ITEM_MOON_TEAR) ? gItemPrices[buttonItem] : 0;
            if (this->price > 0) {
                player->actor.textId = 0x29EF;
                player->exchangeItemId = buttonItem;
                this->actionFunc = EnFsn_MakeOffer;
            } else {
                player->actor.textId = 0x29CF;
                this->actionFunc = EnFsn_StartBuying;
            }
            this->actor.textId = player->actor.textId;
            func_801477B4(play);
        } else if (itemActionParam < PLAYER_AP_NONE) {
            if (CURRENT_DAY == 3) {
                this->actor.textId = 0x29DF;
            } else {
                this->actor.textId = 0x29D1;
            }
            Message_StartTextbox(play, this->actor.textId, &this->actor);
            func_80151BB4(play, 3);
            this->actionFunc = EnFsn_SetupEndInteraction;
        }
    }
}

void EnFsn_MakeOffer(EnFsn* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);
    Player* player = GET_PLAYER(play);

    if (talkState == TEXT_STATE_CHOICE && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0:
                func_8019F208();
                play->msgCtx.msgMode = 0x43;
                play->msgCtx.stateTimer = 4;
                if (this->cutsceneState == ENFSN_CUTSCENESTATE_PLAYING) {
                    ActorCutscene_Stop(this->cutscene);
                    this->cutsceneState = ENFSN_CUTSCENESTATE_STOPPED;
                }
                switch (this->price) {
                    case 5:
                        this->getItemId = GI_RUPEE_BLUE;
                        break;
                    case 10:
                        this->getItemId = GI_RUPEE_10;
                        break;
                    case 20:
                        this->getItemId = GI_RUPEE_RED;
                        break;
                    case 50:
                        this->getItemId = GI_RUPEE_PURPLE;
                        break;
                    case 200:
                        this->getItemId = GI_RUPEE_HUGE;
                        break;
                }
                this->actionFunc = EnFsn_GiveItem;
                break;
            case 1:
                func_8019F230();
                player->exchangeItemId = PLAYER_AP_NONE;
                this->actionFunc = EnFsn_SetupDeterminePrice;
                break;
        }
    }
}

void EnFsn_GiveItem(EnFsn* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        if ((this->isSelling == true) && (this->items[this->cursorIndex]->getItemId == GI_MASK_ALL_NIGHT)) {
            func_80151BB4(play, 45);
            func_80151BB4(play, 3);
        }
        this->actor.parent = NULL;
        if (ENFSN_IS_SHOP(&this->actor) && !this->isSelling) {
            func_80123D50(play, GET_PLAYER(play), ITEM_BOTTLE, PLAYER_AP_BOTTLE);
        }
        this->actionFunc = EnFsn_SetupResumeInteraction;
    } else if (this->isSelling == true) {
        Actor_PickUp(&this->actor, play, this->items[this->cursorIndex]->getItemId, 300.0f, 300.0f);
    } else {
        Actor_PickUp(&this->actor, play, this->getItemId, 300.0f, 300.0f);
    }
}

void EnFsn_SetupResumeInteraction(EnFsn* this, PlayState* play) {
    if (CHECK_QUEST_ITEM(QUEST_BOMBERS_NOTEBOOK)) {
        if (play->msgCtx.unk120B1 == 0) {
            EnFsn_HandleSetupResumeInteraction(this, play);
        }
    } else {
        EnFsn_HandleSetupResumeInteraction(this, play);
    }
}

void EnFsn_ResumeInteraction(EnFsn* this, PlayState* play) {
    if (Actor_ProcessTalkRequest(&this->actor, &play->state)) {
        if (ENFSN_IS_SHOP(&this->actor)) {
            if (!this->isSelling) {
                this->cutscene = this->lookToShopkeeperBuyingCutscene;
                this->actor.textId = 0x29D0;
            } else {
                this->cutscene = this->lookToShopkeeperCutscene;
                this->actor.textId = (this->numSellingItems <= 0) ? 0x29DE : 0x29D6;
            }
            Message_StartTextbox(play, this->actor.textId, &this->actor);
            if (ActorCutscene_GetCurrentIndex() == 0x7C) {
                ActorCutscene_Stop(0x7C);
            }
            ActorCutscene_SetIntentToPlay(this->cutscene);
            this->actionFunc = EnFsn_ResumeShoppingInteraction;
        } else {
            EnFsn_HandleConversationBackroom(this, play);
            this->actionFunc = EnFsn_ConverseBackroom;
        }
    } else {
        func_800B85E0(&this->actor, play, 400.0f, PLAYER_AP_MINUS1);
    }
}

void EnFsn_ResumeShoppingInteraction(EnFsn* this, PlayState* play) {
    if (this->cutsceneState == ENFSN_CUTSCENESTATE_STOPPED) {
        if (ActorCutscene_GetCanPlayNext(this->cutscene)) {
            ActorCutscene_StartAndSetFlag(this->cutscene, &this->actor);
            this->cutsceneState = ENFSN_CUTSCENESTATE_PLAYING;
            if (!this->isSelling) {
                this->actionFunc = EnFsn_AskCanBuyMore;
            } else if (this->actor.textId != 0x29D6) {
                this->actionFunc = EnFsn_AskCanBuyAterRunningOutOfItems;
            } else {
                func_8011552C(play, 6);
                this->stickLeftPrompt.isEnabled = false;
                this->stickRightPrompt.isEnabled = true;
                this->actionFunc = EnFsn_FaceShopkeeperSelling;
            }
        } else {
            if (ActorCutscene_GetCurrentIndex() == 0x7C) {
                ActorCutscene_Stop(0x7C);
            }
            this->cutscene = !this->isSelling ? this->lookToShopkeeperBuyingCutscene : this->lookToShopkeeperCutscene;
            ActorCutscene_SetIntentToPlay(this->cutscene);
        }
    }
}

void EnFsn_LookToShelf(EnFsn* this, PlayState* play) {
    if (this->cutsceneState == ENFSN_CUTSCENESTATE_PLAYING) {
        ActorCutscene_Stop(this->cutscene);
        if (ActorCutscene_GetCurrentIndex() == 0x7C) {
            ActorCutscene_Stop(0x7C);
        }
        this->cutscene = this->lookToShelfCutscene;
        ActorCutscene_SetIntentToPlay(this->cutscene);
        this->cutsceneState = ENFSN_CUTSCENESTATE_WAITING;
    } else if (this->cutsceneState == ENFSN_CUTSCENESTATE_WAITING) {
        if (ActorCutscene_GetCanPlayNext(this->cutscene) != 0) {
            ActorCutscene_StartAndSetFlag(this->cutscene, &this->actor);
            this->cutsceneState = ENFSN_CUTSCENESTATE_PLAYING;
            EnFsn_UpdateCursorPos(this, play);
            this->actionFunc = EnFsn_BrowseShelf;
            func_80151938(play, this->items[this->cursorIndex]->actor.textId);
        } else {
            ActorCutscene_SetIntentToPlay(this->cutscene);
        }
    }
}

void EnFsn_BrowseShelf(EnFsn* this, PlayState* play) {
    u8 talkstate = Message_GetState(&play->msgCtx);
    s32 pad;
    u8 prevCursorIdx = this->cursorIndex;

    if (!EnFsn_ReturnItemToShelf(this)) {
        this->delayTimer = 3;
    } else if (this->delayTimer != 0) {
        this->delayTimer--;
    } else {
        this->drawCursor = 0xFF;
        this->stickLeftPrompt.isEnabled = true;
        EnFsn_UpdateCursorPos(this, play);
        if (talkstate == TEXT_STATE_5) {
            func_8011552C(play, 6);
            if (!EnFsn_HasPlayerSelectedItem(this, play, CONTROLLER1(&play->state))) {
                EnFsn_CursorLeftRight(this);
                if (this->cursorIndex != prevCursorIdx) {
                    play_sound(NA_SE_SY_CURSOR);
                    func_80151938(play, this->items[this->cursorIndex]->actor.textId);
                }
            }
        }
    }
}

void EnFsn_LookToShopkeeperFromShelf(EnFsn* this, PlayState* play) {
    if (this->cutsceneState == ENFSN_CUTSCENESTATE_PLAYING) {
        ActorCutscene_Stop(this->cutscene);
        if (ActorCutscene_GetCurrentIndex() == 0x7C) {
            ActorCutscene_Stop(0x7C);
        }
        this->cutscene = this->lookToShopkeeperFromShelfCutscene;
        ActorCutscene_SetIntentToPlay(this->cutscene);
        this->cutsceneState = ENFSN_CUTSCENESTATE_WAITING;
    } else if (this->cutsceneState == ENFSN_CUTSCENESTATE_WAITING) {
        if (ActorCutscene_GetCanPlayNext(this->cutscene)) {
            ActorCutscene_StartAndSetFlag(this->cutscene, &this->actor);
            this->cutsceneState = ENFSN_CUTSCENESTATE_PLAYING;
            this->stickLeftPrompt.isEnabled = false;
            this->stickRightPrompt.isEnabled = true;
            this->actor.textId = 0x29D6;
            func_80151938(play, this->actor.textId);
            this->actionFunc = EnFsn_FaceShopkeeperSelling;
        } else {
            ActorCutscene_SetIntentToPlay(this->cutscene);
        }
    }
}

void EnFsn_HandleCanPlayerBuyItem(EnFsn* this, PlayState* play) {
    EnGirlA* item = this->items[this->cursorIndex];

    switch (item->canBuyFunc(play, item)) {
        case CANBUY_RESULT_SUCCESS_2:
            func_8019F208();
            gSaveContext.save.weekEventReg[33] |= 4;
        case CANBUY_RESULT_SUCCESS_1:
            if (this->cutsceneState == ENFSN_CUTSCENESTATE_PLAYING) {
                ActorCutscene_Stop(this->cutscene);
                this->cutsceneState = ENFSN_CUTSCENESTATE_STOPPED;
            }
            func_8019F208();
            item = this->items[this->cursorIndex];
            item->buyFanfareFunc(play, item);
            Actor_PickUp(&this->actor, play, this->items[this->cursorIndex]->getItemId, 300.0f, 300.0f);
            play->msgCtx.msgMode = 0x43;
            play->msgCtx.stateTimer = 4;
            Interface_SetHudVisibility(HUD_VISIBILITY_ALL);
            this->drawCursor = 0;
            this->shopItemSelectedTween = 0.0f;
            item = this->items[this->cursorIndex];
            item->boughtFunc(play, item);
            if (this->stolenItem1 == this->cursorIndex) {
                SET_STOLEN_ITEM_1(STOLEN_ITEM_NONE);
            } else if (this->stolenItem2 == this->cursorIndex) {
                SET_STOLEN_ITEM_2(STOLEN_ITEM_NONE);
            }
            this->numSellingItems--;
            this->itemIds[this->cursorIndex] = -1;
            this->actionFunc = EnFsn_GiveItem;
            break;
        case CANBUY_RESULT_NEED_RUPEES:
            play_sound(NA_SE_SY_ERROR);
            func_80151938(play, 0x29F0);
            this->actionFunc = EnFsn_PlayerCannotBuy;
            break;
        case CANBUY_RESULT_CANNOT_GET_NOW:
            play_sound(NA_SE_SY_ERROR);
            func_80151938(play, 0x29DD);
            this->actionFunc = EnFsn_PlayerCannotBuy;
            break;
    }
}

void EnFsn_SetupEndInteraction(EnFsn* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    if (((talkState == TEXT_STATE_5) || (talkState == TEXT_STATE_DONE)) && Message_ShouldAdvance(play)) {
        if (CHECK_QUEST_ITEM(QUEST_BOMBERS_NOTEBOOK)) {
            if (play->msgCtx.unk120B1 == 0) {
                EnFsn_EndInteraction(this, play);
            } else {
                play->msgCtx.msgMode = 0x43;
                play->msgCtx.stateTimer = 4;
            }
        } else {
            EnFsn_EndInteraction(this, play);
        }
    }
}

void EnFsn_SelectItem(EnFsn* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    if (EnFsn_TakeItemOffShelf(this) && (talkState == TEXT_STATE_CHOICE)) {
        func_8011552C(play, 6);
        if (!EnFsn_TestCancelOption(this, play, CONTROLLER1(&play->state)) && Message_ShouldAdvance(play)) {
            switch (play->msgCtx.choiceIndex) {
                case 0:
                    EnFsn_HandleCanPlayerBuyItem(this, play);
                    break;
                case 1:
                    func_8019F230();
                    this->actionFunc = this->prevActionFunc;
                    func_80151938(play, this->items[this->cursorIndex]->actor.textId);
            }
        }
    }
}

void EnFsn_PlayerCannotBuy(EnFsn* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_5) && Message_ShouldAdvance(play)) {
        this->actionFunc = this->prevActionFunc;
        func_80151938(play, this->items[this->cursorIndex]->actor.textId);
    }
}

void EnFsn_AskCanBuyMore(EnFsn* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    if (this->cutsceneState == ENFSN_CUTSCENESTATE_STOPPED) {
        if (ActorCutscene_GetCanPlayNext(this->cutscene)) {
            ActorCutscene_StartAndSetFlag(this->cutscene, &this->actor);
            this->cutsceneState = ENFSN_CUTSCENESTATE_PLAYING;
        } else {
            if (ActorCutscene_GetCurrentIndex() == 0x7C) {
                ActorCutscene_Stop(0x7C);
            }
            this->cutscene = this->lookToShopkeeperCutscene;
            ActorCutscene_SetIntentToPlay(this->cutscene);
        }
    }
    if (talkState == TEXT_STATE_CHOICE) {
        if (Message_ShouldAdvance(play)) {
            switch (play->msgCtx.choiceIndex) {
                case 0:
                    func_8019F208();
                    this->actor.textId = 0xFF;
                    Message_StartTextbox(play, this->actor.textId, &this->actor);
                    this->actionFunc = EnFsn_DeterminePrice;
                    break;
                case 1:
                    func_8019F230();
                    this->actor.textId = (CURRENT_DAY == 3) ? 0x29DF : 0x29D1;
                    Message_StartTextbox(play, this->actor.textId, &this->actor);
                    func_80151BB4(play, 3);
                    break;
            }
        }
    } else if (((talkState == TEXT_STATE_5) || (talkState == TEXT_STATE_DONE)) && Message_ShouldAdvance(play)) {
        if (CHECK_QUEST_ITEM(QUEST_BOMBERS_NOTEBOOK)) {
            if (play->msgCtx.unk120B1 == 0) {
                EnFsn_EndInteraction(this, play);
            } else {
                play->msgCtx.msgMode = 0x43;
                play->msgCtx.stateTimer = 4;
            }
        } else {
            EnFsn_EndInteraction(this, play);
        }
    }
}

void EnFsn_AskCanBuyAterRunningOutOfItems(EnFsn* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    if (this->cutsceneState == ENFSN_CUTSCENESTATE_STOPPED) {
        if (ActorCutscene_GetCanPlayNext(this->cutscene)) {
            ActorCutscene_StartAndSetFlag(this->cutscene, &this->actor);
            this->cutsceneState = ENFSN_CUTSCENESTATE_PLAYING;
        } else {
            if (ActorCutscene_GetCurrentIndex() == 0x7C) {
                ActorCutscene_Stop(0x7C);
            }
            this->cutscene = this->lookToShopkeeperCutscene;
            ActorCutscene_SetIntentToPlay(this->cutscene);
        }
    }
    if (talkState == TEXT_STATE_CHOICE) {
        if (Message_ShouldAdvance(play)) {
            switch (play->msgCtx.choiceIndex) {
                case 0:
                    func_8019F208();
                    this->isSelling = false;
                    this->actor.textId = 0x29CE;
                    Message_StartTextbox(play, this->actor.textId, &this->actor);
                    this->actionFunc = EnFsn_StartBuying;
                    break;
                case 1:
                    func_8019F230();
                    this->actor.textId = (CURRENT_DAY == 3) ? 0x29DF : 0x29D1;
                    Message_StartTextbox(play, this->actor.textId, &this->actor);
                    func_80151BB4(play, 3);
                    break;
            }
        }
    } else if (((talkState == TEXT_STATE_5) || (talkState == TEXT_STATE_DONE)) && Message_ShouldAdvance(play)) {
        if (CHECK_QUEST_ITEM(QUEST_BOMBERS_NOTEBOOK)) {
            if (play->msgCtx.unk120B1 == 0) {
                EnFsn_EndInteraction(this, play);
            } else {
                play->msgCtx.msgMode = 0x43;
                play->msgCtx.stateTimer = 4;
            }
        } else {
            EnFsn_EndInteraction(this, play);
        }
    }
}

void EnFsn_FaceShopkeeperSelling(EnFsn* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);
    u8 cursorIndex;

    if (talkState == TEXT_STATE_CHOICE) {
        func_8011552C(play, 6);
        if (!EnFsn_TestEndInteraction(this, play, CONTROLLER1(&play->state)) &&
            (!Message_ShouldAdvance(play) || !EnFsn_FacingShopkeeperDialogResult(this, play)) &&
            this->stickAccumX > 0) {
            cursorIndex = EnFsn_SetCursorIndexFromNeutral(this);
            if (cursorIndex != CURSOR_INVALID) {
                this->cursorIndex = cursorIndex;
                this->actionFunc = EnFsn_LookToShelf;
                func_8011552C(play, 6);
                this->stickRightPrompt.isEnabled = false;
                play_sound(NA_SE_SY_CURSOR);
            }
        }
    } else if ((talkState == TEXT_STATE_5) && Message_ShouldAdvance(play)) {
        this->actor.textId = 0x29D6;
        Message_StartTextbox(play, this->actor.textId, &this->actor);
        if (play) {}
    }
}

void EnFsn_SetupEndInteractionImmediately(EnFsn* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_5) && Message_ShouldAdvance(play)) {
        EnFsn_EndInteraction(this, play);
    }
}

void EnFsn_IdleBackroom(EnFsn* this, PlayState* play) {
    if (Actor_ProcessTalkRequest(&this->actor, &play->state)) {
        this->textId = 0;
        EnFsn_HandleConversationBackroom(this, play);
        this->actionFunc = EnFsn_ConverseBackroom;
    } else if (this->actor.xzDistToPlayer < 100.0f || this->actor.isTargeted) {
        func_800B8614(&this->actor, play, 100.0f);
    }
}

void EnFsn_ConverseBackroom(EnFsn* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_5) && Message_ShouldAdvance(play)) {
        if (this->flags & ENFSN_END_CONVERSATION) {
            this->flags &= ~ENFSN_END_CONVERSATION;
            play->msgCtx.msgMode = 0x43;
            play->msgCtx.stateTimer = 4;
            this->actionFunc = EnFsn_IdleBackroom;
        } else if (this->flags & ENFSN_GIVE_ITEM) {
            this->flags &= ~ENFSN_GIVE_ITEM;
            play->msgCtx.msgMode = 0x43;
            play->msgCtx.stateTimer = 4;
            this->actionFunc = EnFsn_GiveItem;
        } else {
            EnFsn_HandleConversationBackroom(this, play);
        }
    }
}

void EnFsn_GetCutscenes(EnFsn* this) {
    this->lookToShopkeeperCutscene = this->actor.cutscene;
    this->lookToShelfCutscene = ActorCutscene_GetAdditionalCutscene(this->lookToShopkeeperCutscene);
    this->lookToShopkeeperFromShelfCutscene = ActorCutscene_GetAdditionalCutscene(this->lookToShelfCutscene);
    this->lookToShopkeeperBuyingCutscene = ActorCutscene_GetAdditionalCutscene(this->lookToShopkeeperFromShelfCutscene);
}

void EnFsn_Blink(EnFsn* this) {
    s16 decr = this->blinkTimer - 1;

    if (decr >= 3) {
        this->eyeTexIndex = 0;
        this->blinkTimer = decr;
    } else if (decr == 0) {
        this->eyeTexIndex = 2;
        this->blinkTimer = (s32)(Rand_ZeroOne() * 60.0f) + 20;
    } else {
        this->eyeTexIndex = 1;
        this->blinkTimer = decr;
    }
}

void EnFsn_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnFsn* this = THIS;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 20.0f);

    // Note: adding 1 to FSN_LIMB_MAX due to bug in object_fsn, see bug in object_fsn.xml
    SkelAnime_InitFlex(play, &this->skelAnime, &gFsnSkel, &gFsnIdleAnim, this->jointTable, this->morphTable,
                       FSN_LIMB_MAX + 1);
    if (ENFSN_IS_SHOP(&this->actor)) {
        this->actor.shape.rot.y = BINANG_ROT180(this->actor.shape.rot.y);
        this->actor.flags &= ~ACTOR_FLAG_1;
        EnFsn_GetCutscenes(this);
        EnFsn_InitShop(this, play);
    } else {
        if ((gSaveContext.save.weekEventReg[33] & 8) || (gSaveContext.save.weekEventReg[79] & 0x40)) {
            Actor_Kill(&this->actor);
            return;
        }
        Collider_InitCylinder(play, &this->collider);
        Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
        this->blinkTimer = 20;
        this->eyeTexIndex = 0;
        this->actor.flags |= ACTOR_FLAG_1;
        this->actor.targetMode = 0;
        this->animIndex = FSN_ANIM_IDLE;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
        this->actionFunc = EnFsn_IdleBackroom;
    }
}

void EnFsn_Destroy(Actor* thisx, PlayState* play) {
    EnFsn* this = THIS;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnFsn_Update(Actor* thisx, PlayState* play) {
    EnFsn* this = THIS;

    this->actionFunc(this, play);
    Actor_MoveWithGravity(&this->actor);
    Actor_TrackPlayer(play, &this->actor, &this->headRot, &this->unk27A, this->actor.focus.pos);
    SubS_FillLimbRotTables(play, this->limbRotYTable, this->limbRotZTable, ARRAY_COUNT(this->limbRotYTable));
    EnFsn_Blink(this);
    if (ENFSN_IS_SHOP(&this->actor) && EnFsn_HasItemsToSell()) {
        EnFsn_UpdateJoystickInputState(this, play);
        EnFsn_UpdateItemSelectedProperty(this);
        EnFsn_UpdateStickDirectionPromptAnim(this);
        EnFsn_UpdateCursorAnim(this);
    }
    SkelAnime_Update(&this->skelAnime);
    if (ENFSN_IS_BACKROOM(&this->actor)) {
        EnFsn_UpdateCollider(this, play);
    }
}

void EnFsn_DrawCursor(EnFsn* this, PlayState* play, f32 x, f32 y, f32 z, u8 drawCursor) {
    s32 ulx;
    s32 uly;
    s32 lrx;
    s32 lry;
    f32 w;
    s32 dsdx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    if (drawCursor != 0) {
        func_8012C654(play->state.gfxCtx);
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, this->cursorColor.r, this->cursorColor.g, this->cursorColor.b,
                        this->cursorColor.a);
        gDPLoadTextureBlock_4b(OVERLAY_DISP++, gSelectionCursorTex, G_IM_FMT_IA, 16, 16, 0, G_TX_MIRROR | G_TX_WRAP,
                               G_TX_MIRROR | G_TX_WRAP, 4, 4, G_TX_NOLOD, G_TX_NOLOD);
        w = 16.0f * z;
        ulx = (x - w) * 4.0f;
        uly = (y - w + -12.0f) * 4.0f;
        lrx = (x + w) * 4.0f;
        lry = (y + w + -12.0f) * 4.0f;
        dsdx = (1.0f / z) * 1024.0f;
        gSPTextureRectangle(OVERLAY_DISP++, ulx, uly, lrx, lry, G_TX_RENDERTILE, 0, 0, dsdx, dsdx);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnFsn_DrawTextRec(PlayState* play, s32 r, s32 g, s32 b, s32 a, f32 x, f32 y, f32 z, s32 s, s32 t, f32 dx, f32 dy) {
    f32 unk;
    s32 ulx;
    s32 uly;
    s32 lrx;
    s32 lry;
    f32 w;
    f32 h;
    s32 dsdx;
    s32 dtdy;

    OPEN_DISPS(play->state.gfxCtx);

    gDPPipeSync(OVERLAY_DISP++);
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, r, g, b, a);

    w = 8.0f * z;
    ulx = (x - w) * 4.0f;
    lrx = (x + w) * 4.0f;

    h = 12.0f * z;
    uly = (y - h) * 4.0f;
    lry = (y + h) * 4.0f;

    unk = 1024 * (1.0f / z);
    dsdx = unk * dx;
    dtdy = dy * unk;

    gSPTextureRectangle(OVERLAY_DISP++, ulx, uly, lrx, lry, G_TX_RENDERTILE, s, t, dsdx, dtdy);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnFsn_DrawStickDirectionPrompts(EnFsn* this, PlayState* play) {
    s32 drawStickRightPrompt = this->stickLeftPrompt.isEnabled;
    s32 drawStickLeftPrompt = this->stickRightPrompt.isEnabled;

    OPEN_DISPS(play->state.gfxCtx);

    if (drawStickRightPrompt || drawStickLeftPrompt) {
        func_8012C654(play->state.gfxCtx);
        gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPLoadTextureBlock(OVERLAY_DISP++, gArrowCursorTex, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 24, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 4, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        if (drawStickRightPrompt) {
            EnFsn_DrawTextRec(play, this->stickLeftPrompt.arrowColor.r, this->stickLeftPrompt.arrowColor.g,
                              this->stickLeftPrompt.arrowColor.b, this->stickLeftPrompt.arrowColor.a,
                              this->stickLeftPrompt.arrowTexX, this->stickLeftPrompt.arrowTexY,
                              this->stickLeftPrompt.texZ, 0, 0, -1.0f, 1.0f);
        }
        if (drawStickLeftPrompt) {
            EnFsn_DrawTextRec(play, this->stickRightPrompt.arrowColor.r, this->stickRightPrompt.arrowColor.g,
                              this->stickRightPrompt.arrowColor.b, this->stickRightPrompt.arrowColor.a,
                              this->stickRightPrompt.arrowTexX, this->stickRightPrompt.arrowTexY,
                              this->stickRightPrompt.texZ, 0, 0, 1.0f, 1.0f);
        }
        gDPLoadTextureBlock(OVERLAY_DISP++, gControlStickTex, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 16, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 4, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        if (drawStickRightPrompt) {
            EnFsn_DrawTextRec(play, this->stickLeftPrompt.stickColor.r, this->stickLeftPrompt.stickColor.g,
                              this->stickLeftPrompt.stickColor.b, this->stickLeftPrompt.stickColor.a,
                              this->stickLeftPrompt.stickTexX, this->stickLeftPrompt.stickTexY,
                              this->stickLeftPrompt.texZ, 0, 0, -1.0f, 1.0f);
        }
        if (drawStickLeftPrompt) {
            EnFsn_DrawTextRec(play, this->stickRightPrompt.stickColor.r, this->stickRightPrompt.stickColor.g,
                              this->stickRightPrompt.stickColor.b, this->stickRightPrompt.stickColor.a,
                              this->stickRightPrompt.stickTexX, this->stickRightPrompt.stickTexY,
                              this->stickRightPrompt.texZ, 0, 0, 1.0f, 1.0f);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

s32 EnFsn_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnFsn* this = THIS;
    s32 limbRotTableIdx;

    if (limbIndex == FSN_LIMB_HEAD) {
        Matrix_RotateXS(this->headRot.y, MTXMODE_APPLY);
    }
    if (ENFSN_IS_BACKROOM(&this->actor)) {
        switch (limbIndex) {
            case FSN_LIMB_TORSO:
                limbRotTableIdx = 0;
                break;
            case FSN_LIMB_LEFT_HAND:
                limbRotTableIdx = 1;
                break;
            case FSN_LIMB_HEAD:
                limbRotTableIdx = 2;
                break;
            default:
                limbRotTableIdx = 9;
                break;
        }
        if (limbRotTableIdx < 9) {
            rot->y += (s16)(Math_SinS(this->limbRotYTable[limbRotTableIdx]) * 200.0f);
            rot->z += (s16)(Math_CosS(this->limbRotZTable[limbRotTableIdx]) * 200.0f);
        }
    }
    if (limbIndex == FSN_LIMB_TOUPEE) {
        *dList = NULL;
    }
    return false;
}

void EnFsn_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnFsn* this = THIS;

    if (limbIndex == FSN_LIMB_HEAD) {
        this->actor.focus.pos.x = this->actor.world.pos.x;
        this->actor.focus.pos.y = this->actor.world.pos.y + 60.0f;
        this->actor.focus.pos.z = this->actor.world.pos.z;

        OPEN_DISPS(play->state.gfxCtx);

        gSPDisplayList(POLY_OPA_DISP++, gFsnGlassesFrameDL);
        gSPDisplayList(POLY_OPA_DISP++, gFsnGlassesLensesDL);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void EnFsn_Draw(Actor* thisx, PlayState* play) {
    static TexturePtr sEyeTextures[] = { gFsnEyeOpenTex, gFsnEyeHalfTex, gFsnEyeClosedTex };
    s32 pad;
    EnFsn* this = THIS;
    s16 i;

    OPEN_DISPS(play->state.gfxCtx);

    func_8012C5B0(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sEyeTextures[this->eyeTexIndex]));
    gSPSegment(POLY_OPA_DISP++, 0x09, Lib_SegmentedToVirtual(sEyeTextures[this->eyeTexIndex]));
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnFsn_OverrideLimbDraw, EnFsn_PostLimbDraw, &this->actor);

    for (i = 0; i < this->totalSellingItems; i++) {
        this->items[i]->actor.scale.x = 0.2f;
        this->items[i]->actor.scale.y = 0.2f;
        this->items[i]->actor.scale.z = 0.2f;
    }

    EnFsn_DrawCursor(this, play, this->cursorPos.x, this->cursorPos.y, this->cursorPos.z, this->drawCursor);
    EnFsn_DrawStickDirectionPrompts(this, play);

    CLOSE_DISPS(play->state.gfxCtx);
}
