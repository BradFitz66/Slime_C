#pragma once

extern int g_drawCallCount;

static inline void DRAWSTATS_Inc(void) { g_drawCallCount++; }
