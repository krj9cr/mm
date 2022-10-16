/*
 * File: z_elf_msg.c
 * Overlay: ovl_Elf_Msg
 * Description: Tatl Hint (proximity-activated C-up hint?)
 */

#include "z_elf_msg.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"

#define FLAGS (ACTOR_FLAG_10)

#define THIS ((ElfMsg*)thisx)

void ElfMsg_Init(Actor* thisx, PlayState* play);
void ElfMsg_Destroy(Actor* thisx, PlayState* play);
void ElfMsg_Update(Actor* thisx, PlayState* play);

void ElfMsg_SetupAction(ElfMsg* this, ElfMsgActionFunc actionFunc);
void func_8092E284(ElfMsg* this, PlayState* play);

const ActorInit Elf_Msg_InitVars = {
    ACTOR_ELF_MSG,
    ACTORCAT_ITEMACTION,
    FLAGS,
    GAMEPLAY_KEEP,
    sizeof(ElfMsg),
    (ActorFunc)ElfMsg_Init,
    (ActorFunc)ElfMsg_Destroy,
    (ActorFunc)ElfMsg_Update,
    (ActorFunc)NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_STOP),
};

void ElfMsg_SetupAction(ElfMsg* this, ElfMsgActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

s32 func_8092DF9C(ElfMsg* this, PlayState* play) {
    if ((this->actor.home.rot.y > 0) && (this->actor.home.rot.y < 0x81) &&
        (Flags_GetSwitch(play, this->actor.home.rot.y - 1))) {
        (void)"共倒れ"; // "Collapse together"
        if (ELFMSG_GET_SWITCHFLAG(&this->actor) != 0x7F) {
            Flags_SetSwitch(play, ELFMSG_GET_SWITCHFLAG(&this->actor));
        }
        Actor_Kill(&this->actor);
        return true;
    }
    if (this->actor.home.rot.y == 0x81) {
        if (Flags_GetClear(play, this->actor.room)) {
            if (ELFMSG_GET_SWITCHFLAG(&this->actor) != 0x7F) {
                Flags_SetSwitch(play, ELFMSG_GET_SWITCHFLAG(&this->actor));
            }
            Actor_Kill(&this->actor);
            return true;
        }
    }
    if (ELFMSG_GET_SWITCHFLAG(&this->actor) == 0x7F) {
        return false;
    }
    if (Flags_GetSwitch(play, ELFMSG_GET_SWITCHFLAG(&this->actor))) {
        (void)"共倒れ"; // "Collapse together"
        Actor_Kill(&this->actor);
        return true;
    }
    return false;
}

void ElfMsg_Init(Actor* thisx, PlayState* play) {
    ElfMsg* this = THIS;

    if (!func_8092DF9C(this, play)) {
        Actor_ProcessInitChain(&this->actor, sInitChain);
        if (ABS_ALT(this->actor.home.rot.x) == 0) {
            this->actor.scale.x = this->actor.scale.z = 0.4f;
        } else {
            this->actor.scale.x = this->actor.scale.z = ABS_ALT(this->actor.home.rot.x) * 0.04f;
        }
        if (this->actor.home.rot.z == 0) {
            this->actor.scale.y = 0.4f;
        } else {
            this->actor.scale.y = this->actor.home.rot.z * 0.04f;
        }
        ElfMsg_SetupAction(this, func_8092E284);
        this->actor.shape.rot.x = this->actor.shape.rot.y = this->actor.shape.rot.z = 0;
    }
}

void ElfMsg_Destroy(Actor* thisx, PlayState* play) {
}

s32 func_8092E1D0(ElfMsg* this) {
    if (ELFMSG_GET_8000(&this->actor) != 0) {
        return ELFMSG_GET_FF(&this->actor) + 0x200;
    } else {
        return -0x200 - ELFMSG_GET_FF(&this->actor);
    }
}

s32 func_8092E1FC(ElfMsg* this) {
    return (this->actor.xzDistToPlayer < (100.0f * this->actor.scale.x)) && (this->actor.playerHeightRel >= 0.0f) &&
           (this->actor.playerHeightRel < (100.0f * this->actor.scale.y));
}

void func_8092E284(ElfMsg* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    EnElf* tatl = (EnElf*)player->tatlActor;

    if ((player->tatlActor != NULL) && ((func_8092E1FC(this)))) {
        player->tatlTextId = func_8092E1D0(this);
        ActorCutscene_SetIntentToPlay(0x7C);
        tatl->elfMsg = &this->actor;
        if (this->actor.cutscene == -1) {
            this->actor.cutscene = 0x7C;
        }
        if ((player->tatlTextId < 0) && (this->actor.home.rot.x < 0)) {
            if (ActorCutscene_GetCurrentIndex() == 0x7D) {
                ActorCutscene_Stop(0x7D);
                ActorCutscene_SetIntentToPlay(this->actor.cutscene);
            } else if (ActorCutscene_GetCanPlayNext(this->actor.cutscene)) {
                this->actor.home.rot.x = 0;
                ActorCutscene_Start(this->actor.cutscene, &this->actor);
                func_800E0348(play->cameraPtrs[CAM_ID_MAIN]);
            } else {
                ActorCutscene_SetIntentToPlay(this->actor.cutscene);
            }
        }
    }
}

void ElfMsg_Update(Actor* thisx, PlayState* play) {
    ElfMsg* this = THIS;

    if (func_8092DF9C(this, play) == 0) {
        if (Actor_ProcessTalkRequest(&this->actor, &play->state)) {
            if (ELFMSG_GET_SWITCHFLAG(thisx) != 0x7F) {
                Flags_SetSwitch(play, ELFMSG_GET_SWITCHFLAG(thisx));
            }
            Actor_Kill(&this->actor);
            return;
        }

        if ((this->actor.home.rot.y >= 0) || (this->actor.home.rot.y < -0x80) ||
            (Flags_GetSwitch(play, -1 - this->actor.home.rot.y))) {
            this->actionFunc(this, play);
        }
    }
}
