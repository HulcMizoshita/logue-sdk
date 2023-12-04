#pragma once
/*
 *  File: synth.h
 *
 *  Dummy Synth Class.
 *
 *
 *  2021-2022 (c) Korg
 *
 */

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <math.h>
#include <string.h>

#include <arm_neon.h>

#include "unit.h"  // Note: Include common definitions for all units

#include "voice/voice.h"

enum : uint8_t
{
  kSynthParamIndexNote = 0,
  kSynthParamIndexVoice,
  KNumSynthParams
};

// This only handles 32-voice packed DX7 .syx files
const int SYX_FILE_SIZE  = 4096 + 8;
const int SYX_NUM_VOICES = 32;
const int SYX_VOICE_SIZE = 4096/SYX_NUM_VOICES;
const int SYX_NUM_OSC    = 6;

static bool FindSyx(uint8_t* from, uint8_t* to,
                    uint8_t*& begin, uint8_t*& end)
{
  begin = to;
  end = to;
  for (; from != to; ++from) {
    if (*from == 0xf0) {
      begin = from;
      for (; from != to; ++from) {
        if (*from == 0xf7) {
          end = ++from;
          return true;
        }
      }
    }
  }
  return false;
}

class Synth {
 public:
  /*===========================================================================*/
  /* Public Data Structures/Types. */
  /*===========================================================================*/

  /*===========================================================================*/
  /* Lifecycle Methods. */
  /*===========================================================================*/

  Synth(void)
  {

  }

  ~Synth(void) {}

  inline int8_t Init(const unit_runtime_desc_t * desc) {
    // Check compatibility of samplerate with unit, for drumlogue should be 48000
    if (desc->samplerate != 48000)
      return k_unit_err_samplerate;

    // Check compatibility of frame geometry
    if (desc->output_channels != 2)  // should be stereo output
      return k_unit_err_geometry;

    // Note: if need to allocate some memory can do it here and return k_unit_err_memory if getting allocation errors

    mVoice.Init(desc->samplerate);

    for (uint8_t i = 0; i < KNumSynthParams; i++)
    {
      params[i] = 0;
    }

    uint8_t syxData[SYX_FILE_SIZE * 2] = {
      #include "2.txt"
    };

    uint8_t* dataBegin = &syxData[0];
    uint8_t* dataEnd = &syxData[SYX_FILE_SIZE * 2 - 1];
    uint8_t* syxBegin = dataBegin;
    uint8_t* syxEnd = dataBegin;

    if (FindSyx(dataBegin, dataEnd, syxBegin, syxEnd))
    {
      dataBegin = syxEnd;
      if (syxEnd - syxBegin == SYX_FILE_SIZE)
      {
        memcpy(mSyxData, &*syxBegin, SYX_FILE_SIZE);
        SetVoiceParams();
      }

      return k_unit_err_none;
    }
    else
    {
      return k_unit_err_none;
    }
  }

  inline void Teardown() {
    // Note: cleanup and release resources if any
  }

  inline void Reset() {
    // Note: Reset synth state. I.e.: Clear filter memory, reset oscillator
    // phase etc.
    mVoice.Reset();
  }

  inline void Resume() {
    // Note: Synth will resume and exit suspend state. Usually means the synth
    // was selected and the render callback will be called again
  }

  inline void Suspend() {
    // Note: Synth will enter suspend state. Usually means another synth was
    // selected and thus the render callback will not be called
  }

  /*===========================================================================*/
  /* Other Public Methods. */
  /*===========================================================================*/

  fast_inline void Render(float * out, size_t frames) {
    float * __restrict out_p = out;
    const float * out_e = out_p + (frames << 1);  // assuming stereo output

    for (; out_p != out_e; out_p += 2) {
      // Note: should take advantage of NEON ArmV7 instructions
      // vst1_f32(out_p, vdup_n_f32(0.f));
      float sig = mVoice.ProcessSample();
      vst1_f32(out_p, vdup_n_f32(sig));
    }
  }

  inline void setParameter(uint8_t index, int32_t value) {
    params[index] = value;
    switch (index) {
      case kSynthParamIndexNote:
        mNote = value;
        break;
      case kSynthParamIndexVoice:
        SetVoiceParams(value);
        break;
      default:
        break;
    }
  }

  inline int32_t getParameterValue(uint8_t index) const {
    return params[index];
  }

  inline const char * getParameterStrValue(uint8_t index, int32_t value) const {
    (void)value;
    switch (index) {
      // Note: String memory must be accessible even after function returned.
      //       It can be assumed that caller will have copied or used the string
      //       before the next call to getParameterStrValue
      default:
        break;
    }
    return nullptr;
  }

  inline const uint8_t * getParameterBmpValue(uint8_t index,
                                              int32_t value) const {
    (void)value;
    switch (index) {
      // Note: Bitmap memory must be accessible even after function returned.
      //       It can be assumed that caller will have copied or used the bitmap
      //       before the next call to getParameterBmpValue
      // Note: Not yet implemented upstream
      default:
        break;
    }
    return nullptr;
  }

  inline void NoteOn(uint8_t note, uint8_t velocity) {
    mNote = note;
    mVoice.NoteOn(mNote, velocity);
  }

  inline void NoteOff(uint8_t note) {
    mNote = note;
    mVoice.NoteOff(mNote);
  }

  inline void GateOn(uint8_t velocity) {
    mVoice.NoteOn(mNote, velocity);
  }

  inline void GateOff() {
    mVoice.NoteOff(mNote);
  }

  inline void AllNoteOff() {
    mVoice.NoteOff(mNote);
  }

  inline void PitchBend(uint16_t bend) { (void)bend; }

  inline void ChannelPressure(uint8_t pressure) { (void)pressure; }

  inline void Aftertouch(uint8_t note, uint8_t aftertouch) {
    (void)note;
    (void)aftertouch;
  }

  inline void LoadPreset(uint8_t idx) { (void)idx; }

  inline uint8_t getPresetIndex() const { return 0; }

  /*===========================================================================*/
  /* Static Members. */
  /*===========================================================================*/

  static inline const char * getPresetName(uint8_t idx) {
    (void)idx;
    // Note: String memory must be accessible even after function returned.
    //       It can be assumed that caller will have copied or used the string
    //       before the next call to getPresetName
    return nullptr;
  }

 private:
  /*===========================================================================*/
  /* Private Member Variables. */
  /*===========================================================================*/

  std::atomic_uint_fast32_t flags_;
  Voice mVoice;

  uint8_t mNote = 0;
  int32_t params[KNumSynthParams];

  uint8_t mSyxData[SYX_FILE_SIZE];

  /*===========================================================================*/
  /* Private Methods. */
  /*===========================================================================*/

  void SetVoiceParams(uint8_t voiceIndex = 0)
  {
    //　↓ startIndex
    uint16_t dataIndex = kNumSyxIndexHeader + ((kNumSyxOpParamsRaw * kNumOperaters) + kNumSyxVoiceParamsRaw) * voiceIndex;

    for (uint16_t i = 0; i < kNumSyxIndexOpParams * kNumOperaters; i++)
    {
      uint16_t paramIndex = i % kNumSyxIndexOpParams;
      switch (paramIndex)
      {
      case kSyxIndexOpKbdLvlSclLftCurve:
        mVoice.SetParameter(i, (0x3 & mSyxData[dataIndex]));
        break;
      case kSyxIndexOpKbdLvlSclRhtCurve:
        mVoice.SetParameter(i, (0x3 & (mSyxData[dataIndex] >> 2)));
        dataIndex++;
        break;
      case kSyxIndexOpOscDetune:
        mVoice.SetParameter(i, (0x0f & (mSyxData[dataIndex] >> 3)));
        break;
      case kSyxIndexOpKbdRateScaling:
        mVoice.SetParameter(i, (0x07 & mSyxData[dataIndex]));
        dataIndex++;
        break;
      case kSyxIndexOpKeyVelSens:
        mVoice.SetParameter(i, (0x07 & (mSyxData[dataIndex] >> 2)));
        break;
      case kSyxIndexOpAmpModSens:
        mVoice.SetParameter(i, (0x03 & mSyxData[dataIndex]));
        dataIndex++;
        break;
      case kSyxIndexOpOscCoarse:
        mVoice.SetParameter(i, (0x1f & (mSyxData[dataIndex] >> 1)));
        break;
      case kSyxIndexOpOscMode:
        mVoice.SetParameter(i, (0x01 & mSyxData[dataIndex]));
        dataIndex++;
        break;
      default:
        mVoice.SetParameter(i, mSyxData[dataIndex]);
        dataIndex++;
        break;
      }
    }
    for (uint16_t paramIndex = 0; paramIndex < kNumSyxIndexVoiceParams; paramIndex++)
    {
      uint16_t i = paramIndex + kNumSyxIndexOpParams * kNumOperaters;
      switch (paramIndex)
      {
      case kSyxIndexVoiceFeedback:
        mVoice.SetParameter(i, (0x07 & mSyxData[dataIndex]));
        break;
      case kSyxIndexVoiceOscSync:
        mVoice.SetParameter(i, (0x01 & (mSyxData[dataIndex] >> 3)));
        dataIndex++;
        break;
      case kSyxIndexVoicePitchModSens:
        mVoice.SetParameter(i, (0x07 & (mSyxData[dataIndex] >> 4)));
        break;
      case kSyxIndexVoiceLfoWaveform:
        mVoice.SetParameter(i, (0x07 & (mSyxData[dataIndex] >> 1)));
        break;
      case kSyxIndexVoiceLfoSync:
        mVoice.SetParameter(i, (0x01 & mSyxData[dataIndex]));
        dataIndex++;
        break;
      case kSyxIndexVoiceName:
        for(int i = 0; i < 10; ++i) {
          dataIndex++;
        }
      default:
        mVoice.SetParameter(i, mSyxData[dataIndex]);
        dataIndex++;
        break;
      }
    }
  }

  /*===========================================================================*/
  /* Constants. */
  /*===========================================================================*/
};
