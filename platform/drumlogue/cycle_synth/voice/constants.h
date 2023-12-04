#pragma once
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <iostream>

const static uint8_t kNumOperaters = 6;
const static uint8_t kNumVoices = 32;
const static uint8_t kNumSyxOpParamsRaw = 17;
const static uint8_t kNumSyxVoiceParamsRaw = 26;

enum : uint16_t
{
  kSyxIndexHeaderStatus = 0,
  kSyxIndexHeaderId,
  kSyxIndexHeaderSubStatusChannelNum,
  kSyxIndexHeaderFormatNumber,
  kSyxIndexHeaderByteCountMs,
  kSyxIndexHeaderByteCountLs,
  kNumSyxIndexHeader
};

enum : uint16_t
{
  kSyxIndexOpEgR1 = 0,
  kSyxIndexOpEgR2,
  kSyxIndexOpEgR3,
  kSyxIndexOpEgR4,
  kSyxIndexOpEgL1,
  kSyxIndexOpEgL2,
  kSyxIndexOpEgL3,
  kSyxIndexOpEgL4,
  kSyxIndexOpKbdLvlSclBrkPt,
  kSyxIndexOpKbdLvlSclLftDepth,
  kSyxIndexOpKbdLvlSclRhtDepth,

  //↓set
  kSyxIndexOpKbdLvlSclLftCurve, // 0x3 & tmp;
  kSyxIndexOpKbdLvlSclRhtCurve, // 0x3 & (tmp >> 2);

  //↓set
  kSyxIndexOpOscDetune,       // 0x0f & (tmp >> 3);
  kSyxIndexOpKbdRateScaling,  // 0x07 & tmp;

  //↓set
  kSyxIndexOpKeyVelSens, // 0x07 & (tmp >> 2);
  kSyxIndexOpAmpModSens, // 0x03 & tmp;

  kSyxIndexOpOutputLevel,

  //↓set
  kSyxIndexOpOscCoarse, // 0x1f & (tmp >> 1);
  kSyxIndexOpOscMode,   // 0x01 & tmp;

  kSyxIndexOpOscFine,

  kNumSyxIndexOpParams
};

enum : uint16_t
{
  kSyxIndexVoicePitchEgR1 = 0,
  kSyxIndexVoicePitchEgR2,
  kSyxIndexVoicePitchEgR3,
  kSyxIndexVoicePitchEgR4,
  kSyxIndexVoicePitchEgL1,
  kSyxIndexVoicePitchEgL2,
  kSyxIndexVoicePitchEgL3,
  kSyxIndexVoicePitchEgL4,
  kSyxIndexVoiceAlgorithmNum,

  //↓set
  kSyxIndexVoiceFeedback, // 0x07 & tmp;
  kSyxIndexVoiceOscSync,  // 0x01 & (tmp >> 3);

  kSyxIndexVoiceLfoSpeed,
  kSyxIndexVoiceLfoDelay,
  kSyxIndexVoicePitchModDepth,
  kSyxIndexVoiceAmpModDepth,

  //↓set
  kSyxIndexVoicePitchModSens, // 0x07 & (tmp >> 4);
  kSyxIndexVoiceLfoWaveform,  // 0x07 & (tmp >> 1);
  kSyxIndexVoiceLfoSync,      // 0x01 & tmp;

  kSyxIndexVoiceTranspose,

  //   for(int i = 0; i < 10; ++i) {
  //     syx_name[i] = FixChar(dx.GetData());
  //   }
  kSyxIndexVoiceName,
  kNumSyxIndexVoiceParams
};

#endif