/**
 *  @file header.c
 *  @brief drumlogue SDK unit header
 *
 *  Copyright (c) 2020-2022 KORG Inc. All rights reserved.
 *
 */

#include "unit.h"  // Note: Include common definitions for all units

// ---- Unit header definition  --------------------------------------------------------------------

const __unit_header unit_header_t unit_header = {
    .header_size = sizeof(unit_header_t),                  // leave as is, size of this header
    .target = UNIT_TARGET_PLATFORM | k_unit_module_synth,  // target platform and module for this unit
    .api = UNIT_API_VERSION,                               // logue sdk API version against which unit was built
    .dev_id = 0x0U,                                        // developer identifier
    .unit_id = 0x0U,                                       // Id for this unit, should be unique within the scope of a given dev_id
    .version = 0x00010000U,                                // This unit's version: major.minor.patch (major<<16 minor<<8 patch).
    .name = "cycle_synth",                                       // Name for this unit, will be displayed on device
    .num_presets = 0,                                      // Number of internal presets this unit has
    .num_params = 24,                                       // Number of parameters for this unit, max 24
    .params = {
        // Format: min, max, center, default, type, fractional, frac. type, <reserved>, name

        // See common/runtime.h for type enum and unit_param_t structure

        // Page 1
        {0, 127, 0, 48, k_unit_param_type_midi_note, 0, 0, 0, {"NOTE"}},
        {0, 31, 0, 1, k_unit_param_type_none, 0, 0, 0, {"1.COARSE"}},
        {0, 99, 0, 0, k_unit_param_type_none, 0, 0, 0, {"1.FINE"}},
        {0, 100, 0, 0, k_unit_param_type_none, 0, 0, 0, {"1.LEVEL"}},

        // Page 2
        {0, 100, 0, 0, k_unit_param_type_none, 0, 0, 0, {"1.R1"}},
        {0, 100, 0, 0, k_unit_param_type_none, 0, 0, 0, {"1.R2"}},
        {0, 100, 0, 0, k_unit_param_type_none, 0, 0, 0, {"1.R3"}},
        {0, 100, 0, 0, k_unit_param_type_none, 0, 0, 0, {"1.R4"}},

        // Page 3
        {0, 100, 0, 0, k_unit_param_type_none, 0, 0, 0, {"1.L1"}},
        {0, 100, 0, 0, k_unit_param_type_none, 0, 0, 0, {"1.L2"}},
        {0, 100, 0, 0, k_unit_param_type_none, 0, 0, 0, {"1.L3"}},
        {0, 100, 0, 0, k_unit_param_type_none, 0, 0, 0, {"1.L4"}},

        // Page 4
        {0, 31, 0, 1, k_unit_param_type_none, 0, 0, 0, {"2.COARSE"}},
        {0, 99, 0, 0, k_unit_param_type_none, 0, 0, 0, {"2.FINE"}},
        {-7, 7, 0, 0, k_unit_param_type_none, 0, 0, 0, {"2.DETUNE"}},
        {0, 100, 0, 0, k_unit_param_type_none, 0, 0, 0, {"2.LEVEL"}},

        // Page 5
        {0, 100, 0, 0, k_unit_param_type_none, 0, 0, 0, {"2.R1"}},
        {0, 100, 0, 0, k_unit_param_type_none, 0, 0, 0, {"2.R2"}},
        {0, 100, 0, 0, k_unit_param_type_none, 0, 0, 0, {"2.R3"}},
        {0, 100, 0, 0, k_unit_param_type_none, 0, 0, 0, {"2.R4"}},

        // Page 6
        {0, 100, 0, 0, k_unit_param_type_none, 0, 0, 0, {"2.L1"}},
        {0, 100, 0, 0, k_unit_param_type_none, 0, 0, 0, {"2.L2"}},
        {0, 100, 0, 0, k_unit_param_type_none, 0, 0, 0, {"2.L3"}},
        {0, 100, 0, 0, k_unit_param_type_none, 0, 0, 0, {"2.L4"}}
        }};
