#pragma once

#include <trx/game/output/types.h>

void OutputSource_Misc_Init(void);
void OutputSource_Misc_Shutdown(void);

void OutputSource_Misc_StageSphere(RGBA_F color);
void OutputSource_Misc_StageCuboid(void);
void OutputSource_Misc_StageLightningSegment(void);
