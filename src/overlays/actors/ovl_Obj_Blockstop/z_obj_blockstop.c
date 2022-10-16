/*
 * File: z_obj_blockstop.c
 * Overlay: ovl_Obj_Blockstop
 * Description: Push Block Trigger - Blockstop switch triggered by pushblock (Snowhead)
 */

#include "z_obj_blockstop.h"
#include "overlays/actors/ovl_Obj_Oshihiki/z_obj_oshihiki.h"

#define FLAGS (ACTOR_FLAG_10)

#define THIS ((ObjBlockstop*)thisx)

void ObjBlockstop_Init(Actor* thisx, PlayState* play);
void ObjBlockstop_Update(Actor* thisx, PlayState* play);

void ObjBlockstop_CheckForBlock(ObjBlockstop* this, PlayState* play);
void ObjBlockstop_TryPlayCutscene(ObjBlockstop* this, PlayState* play);

const ActorInit Obj_Blockstop_InitVars = {
    ACTOR_OBJ_BLOCKSTOP,
    ACTORCAT_PROP,
    FLAGS,
    GAMEPLAY_KEEP,
    sizeof(ObjBlockstop),
    (ActorFunc)ObjBlockstop_Init,
    (ActorFunc)Actor_Noop,
    (ActorFunc)ObjBlockstop_Update,
    (ActorFunc)NULL,
};

void ObjBlockstop_Init(Actor* thisx, PlayState* play) {
    ObjBlockstop* this = THIS;

    if (Flags_GetSwitch(play, this->actor.params)) {
        Actor_Kill(&this->actor);
    }
    this->actionFunc = ObjBlockstop_CheckForBlock;
}

void ObjBlockstop_CheckForBlock(ObjBlockstop* this, PlayState* play) {
    Actor* prop = play->actorCtx.actorLists[ACTORCAT_PROP].first;

    while (prop != NULL) {
        if ((prop->id == ACTOR_OBJ_OSHIHIKI) && (fabsf(prop->world.pos.x - this->actor.world.pos.x) < 20.0f) &&
            (fabsf(prop->world.pos.z - this->actor.world.pos.z) < 20.0f) &&
            (fabsf(prop->world.pos.y - this->actor.world.pos.y) < 20.0f)) {

            s32 params = OBJOSHIHIKI_GET_F(prop);
            if (params < 3) {
                ActorCutscene_SetIntentToPlay(this->actor.cutscene);
                this->actionFunc = ObjBlockstop_TryPlayCutscene;
            }
        }
        prop = prop->next;
    }
}

void ObjBlockstop_TryPlayCutscene(ObjBlockstop* this, PlayState* play) {
    if (ActorCutscene_GetCanPlayNext(this->actor.cutscene)) {
        Flags_SetSwitch(play, this->actor.params);
        if (ActorCutscene_GetLength(this->actor.cutscene) != -1) {
            ActorCutscene_StartAndSetUnkLinkFields(this->actor.cutscene, &this->actor);
        }
        Actor_Kill(&this->actor);
        return;
    }
    ActorCutscene_SetIntentToPlay(this->actor.cutscene);
}

void ObjBlockstop_Update(Actor* thisx, PlayState* play) {
    ObjBlockstop* this = THIS;

    this->actionFunc(this, play);
}
