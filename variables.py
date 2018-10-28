known_vars = {
    0x00B3C000:("","UNK_TYPE",False), # this seems low
    0x00C7A4E0:("","UNK_TYPE",False), # this seems low
    0x80000300:("osTvType","UNK_TYPE",False), # this seems low, but maybe it's set on boot?
    0x8000030C:("","UNK_TYPE",False), # this seems low
    0x80000318:("","UNK_TYPE",False), # this seems low
    0x8000031C:("","UNK_TYPE",False), # this seems low
    0x80000500:("","UNK_TYPE",False), # this seems low
    0x80025D00:("","UNK_TYPE",False),
    0x80096B20:("","u8",False),
    0x80096B24:("","u8*",False),
    0x80096B28:("","u32",False),
    0x80096B2C:("","u32",False),
    0x80096B30:("","f32",False),
    0x80096B34:("","f32",False),
    0x80096B40:("","struct s80092920*",False),
    0x80096B50:("","UNK_PTR",False),
    0x80096C40:("g_s80085320_ListHead","struct s80085320*",False), # TODO rename?
    0x80096C44:("g_s80085320_ListTail","struct s80085320*",False), # TODO rename?
    0x80097590:("","UNK_TYPE",False),
    0x80097E10:("__osThreadTail","OSThread*",False),
    0x80097E18:("__osRunQueue","OSThread*",False),
    0x80097E1C:("__osActiveQueue","OSThread*",False),
    0x80097E20:("__osRunningThread","OSThread*",False),
    0x80097E24:("__osFaultedThread","OSThread*",False),
    0x80097E30:("__osSiAccessQueueEnabled","UNK_TYPE",False),
    0x80097E50:("osClockRate","u64",False),
#    0x80097E54:("osClockRate","u64",False),
    0x80097E58:("","UNK_TYPE",False),
    0x80097E5C:("__osShutdown","UNK_TYPE",False),
    0x80097E60:("__OSGlobalIntMask","UNK_TYPE",False),
    0x80097E70:("__osPiDevMgr","UNK_TYPE",False),
    0x80097E8C:("","struct s80092920*",False),
    0x80097EA0:("__osPiAccessQueueEnabled","UNK_TYPE",False),
    0x80097F10:("","UNK_TYPE",False),
    0x80097F70:("__osTimerList","OSTimer*",False),
    0x80097FC0:("","u32*",False),
    0x80098010:("","u32*",False),
    0x800980C0:("__osViCurr","__OSViContext*",False),
    0x800980C4:("__osViNext","UNK_TYPE",False), # some struct pointer used in func_80095900
    0x800980D0:("","UNK_PTR",False),
    0x800980E0:("","u32*",False),
    0x80098130:("","UNK_TYPE",False), # ldigs in https:github.com/vocho/openqnx/blob/cc95df3543a1c481d4f8ad387e29f0d1ff0c87fe/trunk/lib/c/stdio/xlitob.c
    0x80098144:("","UNK_TYPE",False), # udigs in https:github.com/vocho/openqnx/blob/cc95df3543a1c481d4f8ad387e29f0d1ff0c87fe/trunk/lib/c/stdio/xlitob.c
    0x80098160:("__osViDevMgr","UNK_TYPE",False),
    0x8009817C:("","UNK_TYPE",False),
    0x80098190:("","UNK_TYPE",False),
    0x80098198:("","UNK_TYPE",False),
    0x800981A0:("","UNK_TYPE",False),
    0x800981A8:("","UNK_TYPE",False),
    0x800981B0:("","float",False),
    0x80098280:("","UNK_TYPE",False),
    0x80098290:("","UNK_TYPE",False),
    0x800982A4:("","UNK_TYPE",False),
    0x80099270:("__osHwIntTable","UNK_TYPE",False),
    0x80099290:("leoDiskStack","UNK_TYPE",False),
    0x800994D0:("","UNK_TYPE",False),
    0x800994DC:("","UNK_TYPE",False),
    0x80099500:("","struct s80085320",False),
    0x80099520:("","OSThread",False),
    0x800996D0:("","UNK_TYPE",False),
    0x80099AD0:("","struct s80085320",False),
    0x80099AF0:("","UNK_TYPE",False),
    0x80099EF0:("","UNK_TYPE",False),
    0x8009A170:("","UNK_TYPE",False),
    0x8009A670:("","struct s80085320",False),
    0x8009A690:("","UNK_TYPE",False),
    0x8009A840:("","UNK_TYPE",False),
    0x8009B140:("","struct s80085320",False),
    0x8009B160:("","UNK_TYPE",False),
    0x8009B228:("","UNK_TYPE",False),
    0x8009B240:("","u32*",False),
    0x8009B290:("","u8",False),
    0x8009BE30:("","UNK_TYPE",False),
    0x8009BE34:("","UNK_TYPE",False),
    0x8009BE38:("","UNK_TYPE",False),
    0x8009CD70:("tmp_task","OSTask",False), # static OSTask tmp_task in ../io/sptask.c
    0x8009CEB8:("__osSiAccessQueue","UNK_TYPE",False),
    0x8009CED0:("__osContPifRam","UNK_TYPE",False),
    0x8009CF10:("__osContLastCmd","u8",False),
    0x8009CF11:("__osMaxControllers","UNK_TYPE",False),
    0x8009CF70:("","UNK_TYPE",False),
    0x8009CF80:("__osThreadSave","UNK_TYPE",False),
    0x8009D134:("","u8",False),
    0x8009D135:("","u8",False),
    0x8009D136:("","u8",False),
    0x8009D137:("","u8",False),
    0x8009D138:("","u8",False),
    0x8009D1AC:("","u8",False),
    0x8009D1AD:("","u8",False),
    0x8009D1AE:("","u8",False),
    0x8009D1AF:("","u8",False),
    0x8009D1B0:("","u8",False),
    0x8009E3F8:("__osPiAccessQueue","UNK_TYPE",False),
    0x8009E510:("__osEventStateTab","__OSEventState",True), # TODO better array representation TODO size
    0x8009E5B0:("__osCurrentTime","UNK_TYPE",False),
#    0x8009E5B4:("__osCurrentTime","UNK_TYPE",False),
    0x8009E5B8:("__osBaseCounter","UNK_TYPE",False),
    0x8009E5BC:("__osViIntrCount","UNK_TYPE",False), # actually an array of structs?
    0x8009E5C0:("__osTimerCounter","UNK_TYPE",False),
    0x8009E610:("","struct s80092920",False),
    0x8009E624:("","UNK_TYPE",False),
    0x801ADE80:("","UNK_TYPE",False),
    0x801ADEAC:("","UNK_TYPE",False),
    0x801ADEB0:("D_801ADEB0","int",True), # TODO better array representation
    0x801AEFD0:("","s800BF9A0_s",False),
    0x801B4610:("","UNK_TYPE",False),
    0x801B9F20:("","s800E03A0",False), # actually an array of size 23?
    0x801DCBB0:("","UNK_TYPE",False),
    0x801DCBC4:("","UNK_TYPE",False),
    0x801DCBE4:("","UNK_TYPE",False),
    0x801DCBFC:("","UNK_TYPE",False),
    0x801E3FA0:("","UNK_TYPE",False),
    0x801ED930:("","UNK_TYPE",False),
    0x801ED940:("","UNK_TYPE",False),
    0x80208EA0:("","UNK_TYPE",False),
    
    # En_Test
    0x808637D0:("","f32",False)
    }
    
# these are extra variables needed for one reason or another, they should probably be deleted if possible
extra_vars = (
    ("D_800980D0_","UNK_PTR"), # needed to match?
    ("D_80099AD0_","UNK_TYPE"), # needed to match?
    ("D_8009A670_","UNK_TYPE"), # needed to match?
    ("D_8009B140_","UNK_TYPE"), # needed to match?
    )