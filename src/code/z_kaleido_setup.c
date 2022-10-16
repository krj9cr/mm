#include "global.h"
#include "z64rumble.h"
#include "z64shrink_window.h"
#include "z64view.h"
#include "overlays/kaleido_scope/ovl_kaleido_scope/z_kaleido_scope.h"

s16 D_801BDB00[] = { PAUSE_MAP, PAUSE_QUEST, PAUSE_MASK, PAUSE_ITEM };
f32 sKaleidoSetupEyeX[] = { -64.0f, 0.0f, 64.0f, 0.0f };
f32 sKaleidoSetupEyeZ[] = { 0.0f, -64.0f, 0.0f, 64.0f };

void func_800F4A10(PlayState* play) {
    PauseContext* pauseCtx = &play->pauseCtx;
    s16 i;

    Rumble_StateReset();

    pauseCtx->unk_206 = 0;
    pauseCtx->unk_200 = 1;
    pauseCtx->eye.x = sKaleidoSetupEyeX[pauseCtx->pageIndex];
    pauseCtx->eye.z = sKaleidoSetupEyeZ[pauseCtx->pageIndex];
    pauseCtx->pageIndex = D_801BDB00[pauseCtx->pageIndex];
    pauseCtx->unk_27E = -40;

    for (i = 0; i < ARRAY_COUNT(pauseCtx->worldMapPoints); i++) {
        pauseCtx->worldMapPoints[i] = false;
    }

    if (pauseCtx->state == 1) {
        for (i = 0; i < REGION_MAX; i++) {
            if ((gSaveContext.save.regionsVisited >> i) & 1) {
                pauseCtx->worldMapPoints[i] = true;
            }
        }
    } else {
        for (i = OWL_WARP_STONE_TOWER; i >= OWL_WARP_GREAT_BAY_COAST; i--) {
            if ((gSaveContext.save.playerData.owlActivationFlags >> i) & 1) {
                pauseCtx->worldMapPoints[i] = true;
                pauseCtx->cursorPoint[PAUSE_WORLD_MAP] = i;
            }
        }

        if ((gSaveContext.save.playerData.owlActivationFlags >> 4) & 1) {
            pauseCtx->cursorPoint[PAUSE_WORLD_MAP] = 4;
        }
    }

    YREG(11) = -0x00C8;
    YREG(12) = -0x3840;
    YREG(13) = 0x2710;
    YREG(14) = 0x2710;
    YREG(15) = 0x2710;
    YREG(16) = -0x00BE;
    YREG(17) = -0x06D6;
    YREG(18) = -0x0B90;
    YREG(19) = 0x06E0;
    YREG(20) = 0;
    YREG(21) = -0x0622;
    YREG(22) = -0x0C44;
    YREG(23) = 0x0622;
    R_PAUSE_WORLD_MAP_YAW = -0x622;
    R_PAUSE_WORLD_MAP_Y_OFFSET = -90;
    R_PAUSE_WORLD_MAP_DEPTH = -14400;
}

void KaleidoSetup_Update(PlayState* play) {
    Input* input = CONTROLLER1(&play->state);
    MessageContext* msgCtx = &play->msgCtx;
    Player* player = GET_PLAYER(play);
    PauseContext* pauseCtx = &play->pauseCtx;

    if (CHECK_BTN_ALL(input->cur.button, BTN_R)) {
        if (msgCtx && msgCtx) {}
    }

    if ((pauseCtx->state == 0) && (pauseCtx->debugEditor == DEBUG_EDITOR_NONE) &&
        (play->gameOverCtx.state == GAMEOVER_INACTIVE)) {
        if ((play->transitionTrigger == TRANS_TRIGGER_OFF) && (play->transitionMode == TRANS_MODE_OFF)) {
            if ((gSaveContext.save.cutscene < 0xFFF0) && (gSaveContext.nextCutsceneIndex < 0xFFF0)) {
                if (!Play_InCsMode(play) || ((msgCtx->msgMode != 0) && (msgCtx->currentTextId == 0xFF))) {
                    if ((play->unk_1887C < 2) && (gSaveContext.magicState != MAGIC_STATE_STEP_CAPACITY) &&
                        (gSaveContext.magicState != MAGIC_STATE_FILL)) {
                        if (!(gSaveContext.eventInf[1] & 0x80) && !(player->stateFlags1 & PLAYER_STATE1_20)) {
                            if (!(play->actorCtx.flags & ACTORCTX_FLAG_1) &&
                                !(play->actorCtx.flags & ACTORCTX_FLAG_2)) {
                                if ((play->actorCtx.unk268 == 0) && CHECK_BTN_ALL(input->press.button, BTN_START)) {
                                    gSaveContext.prevHudVisibility = gSaveContext.hudVisibility;
                                    pauseCtx->itemDescriptionOn = false;
                                    pauseCtx->state = 1;
                                    func_800F4A10(play);
                                    pauseCtx->mode = pauseCtx->pageIndex * 2 + 1;
                                    func_801A3A7C(1);
                                }

                                if (pauseCtx->state == 1) {
                                    Game_SetFramerateDivisor(&play->state, 2);
                                    if (ShrinkWindow_Letterbox_GetSizeTarget() != 0) {
                                        ShrinkWindow_Letterbox_SetSizeTarget(0);
                                    }
                                    func_801A3AEC(1);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void KaleidoSetup_Init(PlayState* play) {
    PauseContext* pauseCtx = &play->pauseCtx;
    s32 pad[2];

    bzero(pauseCtx, sizeof(PauseContext));

    pauseCtx->pageIndex = PAUSE_ITEM;

    pauseCtx->unk_21C = 160.0f;
    pauseCtx->unk_218 = 160.0f;
    pauseCtx->unk_214 = 160.0f;
    pauseCtx->unk_210 = 160.0f;

    pauseCtx->eye.x = -64.0f;
    pauseCtx->unk_20C = 936.0f;
    pauseCtx->unk_220 = -314.0f;

    pauseCtx->cursorPoint[PAUSE_MAP] = R_REVERSE_FLOOR_INDEX + (DUNGEON_FLOOR_INDEX_4 - 1);

    pauseCtx->cursorSpecialPos = PAUSE_CURSOR_PAGE_RIGHT;
    pauseCtx->pageSwitchTimer = 0;

    pauseCtx->cursorItem[PAUSE_ITEM] = PAUSE_ITEM_NONE;
    pauseCtx->cursorItem[PAUSE_MAP] = R_REVERSE_FLOOR_INDEX + (DUNGEON_FLOOR_INDEX_4 - 1);
    pauseCtx->cursorItem[PAUSE_QUEST] = PAUSE_ITEM_NONE;
    pauseCtx->cursorItem[PAUSE_MASK] = PAUSE_ITEM_NONE;

    pauseCtx->cursorSlot[PAUSE_ITEM] = 0;
    pauseCtx->cursorSlot[PAUSE_MAP] = R_REVERSE_FLOOR_INDEX + (DUNGEON_FLOOR_INDEX_4 - 1);

    pauseCtx->cursorColorSet = PAUSE_CURSOR_COLOR_SET_YELLOW;
    pauseCtx->ocarinaSongIndex = -1;
    pauseCtx->equipAnimScale = 320;
    pauseCtx->equipAnimShrinkRate = 40;
    pauseCtx->promptAlpha = 100;

    View_Init(&pauseCtx->view, play->state.gfxCtx);
}

void KaleidoSetup_Destroy(PlayState* play) {
}
