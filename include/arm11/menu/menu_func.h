#pragma once

/*
 *   This file is part of fastboot 3DS
 *   Copyright (C) 2017 derrek, profi200, d0k3
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "types.h"

u32 menuPresetNandTools(void);
u32 menuPresetBootMenu(void);
u32 menuPresetBootConfig(void);
u32 menuPresetSlotConfig1(void);
u32 menuPresetSlotConfig2(void);
u32 menuPresetSlotConfig3(void);
u32 menuPresetBootMode(void);

u32 menuSetBootMode(PrintConsole* term_con, PrintConsole* menu_con, u32 param);
u32 menuSetupBootSlot(PrintConsole* term_con, PrintConsole* menu_con, u32 param);
u32 menuSetupBootKeys(PrintConsole* term_con, PrintConsole* menu_con, u32 param);
u32 menuLaunchFirm(PrintConsole* term_con, PrintConsole* menu_con, u32 param);
u32 menuShowCredits(PrintConsole* term_con, PrintConsole* menu_con, u32 param);
u32 menuContinueBoot(PrintConsole* term_con, PrintConsole* menu_con, u32 param);

u32 menuDummyFunc(PrintConsole* term_con, PrintConsole* menu_con, u32 param);
u32 debugSettingsView(PrintConsole* term_con, PrintConsole* menu_con, u32 param);
u32 debugEscapeTest(PrintConsole* term_con, PrintConsole* menu_con, u32 param);