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

#include <arm_neon.h>

#include "unit.h"  // Note: Include common definitions for all units

#include "operator/operator.h"

enum : uint8_t
{
  kSynthParamIndexNote = 0,
  kSynthParamIndexCoarse_1,
  kSynthParamIndexFine_1,
  kSynthParamIndexLevel_1,
  kSynthParamIndexEgR1_1,
  kSynthParamIndexEgR2_1,
  kSynthParamIndexEgR3_1,
  kSynthParamIndexEgR4_1,
  kSynthParamIndexEgL1_1,
  kSynthParamIndexEgL2_1,
  kSynthParamIndexEgL3_1,
  kSynthParamIndexEgL4_1,
  kSynthParamIndexCoarse_2,
  kSynthParamIndexFine_2,
  kSynthParamIndexDetune_2,
  kSynthParamIndexLevel_2,
  kSynthParamIndexEgR1_2,
  kSynthParamIndexEgR2_2,
  kSynthParamIndexEgR3_2,
  kSynthParamIndexEgR4_2,
  kSynthParamIndexEgL1_2,
  kSynthParamIndexEgL2_2,
  kSynthParamIndexEgL3_2,
  kSynthParamIndexEgL4_2,
  KNumSynthParams
};

class Synth {
 public:
  /*===========================================================================*/
  /* Public Data Structures/Types. */
  /*===========================================================================*/

  /*===========================================================================*/
  /* Lifecycle Methods. */
  /*===========================================================================*/

  Synth(void) {}
  ~Synth(void) {}

  inline int8_t Init(const unit_runtime_desc_t * desc) {
    // Check compatibility of samplerate with unit, for drumlogue should be 48000
    if (desc->samplerate != 48000)
      return k_unit_err_samplerate;

    // Check compatibility of frame geometry
    if (desc->output_channels != 2)  // should be stereo output
      return k_unit_err_geometry;

    // Note: if need to allocate some memory can do it here and return k_unit_err_memory if getting allocation errors

    op1.Init(desc->samplerate);
    op2.Init(desc->samplerate);

    for (uint8_t i = 0; i < KNumSynthParams; i++)
    {
      params[i] = 0;
    }
    
    return k_unit_err_none;
  }

  inline void Teardown() {
    // Note: cleanup and release resources if any
  }

  inline void Reset() {
    // Note: Reset synth state. I.e.: Clear filter memory, reset oscillator
    // phase etc.
    op1.Reset();
    op2.Reset();
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
      float sig = op1.ProcessSample(op2.ProcessSample(0.0f));
      vst1_f32(out_p, vdup_n_f32(sig));
    }
  }

  inline void setParameter(uint8_t index, int32_t value) {
    params[index] = value;
    switch (index) {
      case kSynthParamIndexNote:
        op1.SetFreq((440.f / 32) * powf(2, (value - 9.0f) / 12.0f));
        op2.SetFreq((440.f / 32) * powf(2, (value - 9.0f) / 12.0f));
        break;
      case kSynthParamIndexCoarse_1:
        if (value == 0)
        {
          op1.SetParameter(kOpParamIndexCoarse, 0.5f);
        }
        else
        {
          op1.SetParameter(kOpParamIndexCoarse, (float)value);
        }
        break;
      case kSynthParamIndexFine_1:
        op1.SetParameter(kOpParamIndexFine, (float)value * 0.01f);
        break;
      case kSynthParamIndexLevel_1:
        op1.SetParameter(kOpParamIndexLevel, (float)value * 0.01f);
        break;
      case kSynthParamIndexEgR1_1:
        op1.SetParameter(kOpParamIndexEgR1, (float)value * 0.01f);
        break;
      case kSynthParamIndexEgR2_1:
        op1.SetParameter(kOpParamIndexEgR2, (float)value * 0.01f);
        break;
      case kSynthParamIndexEgR3_1:
        op1.SetParameter(kOpParamIndexEgR3, (float)value * 0.01f);
        break;
      case kSynthParamIndexEgR4_1:
        op1.SetParameter(kOpParamIndexEgR4, (float)value * 0.01f);
        break;
      case kSynthParamIndexEgL1_1:
        op1.SetParameter(kOpParamIndexEgL1, (float)value * 0.01f);
        break;
      case kSynthParamIndexEgL2_1:
        op1.SetParameter(kOpParamIndexEgL2, (float)value * 0.01f);
        break;
      case kSynthParamIndexEgL3_1:
        op1.SetParameter(kOpParamIndexEgL3, (float)value * 0.01f);
        break;
      case kSynthParamIndexEgL4_1:
        op1.SetParameter(kOpParamIndexEgL4, (float)value * 0.01f);
        break;
      case kSynthParamIndexCoarse_2:
        if (value == 0)
        {
          op2.SetParameter(kOpParamIndexCoarse, 0.5f);
        }
        else
        {
          op2.SetParameter(kOpParamIndexCoarse, (float)value);
        }
        break;
      case kSynthParamIndexFine_2:
        op2.SetParameter(kOpParamIndexFine, (float)value * 0.01f);
        break;
      case kSynthParamIndexDetune_2:
        op2.SetParameter(kOpParamIndexFine, (float)value);
        break;
      case kSynthParamIndexLevel_2:
        op2.SetParameter(kOpParamIndexLevel, (float)value * 0.01f);
        break;
      case kSynthParamIndexEgR1_2:
        op2.SetParameter(kOpParamIndexEgR1, (float)value * 0.01f);
        break;
      case kSynthParamIndexEgR2_2:
        op2.SetParameter(kOpParamIndexEgR2, (float)value * 0.01f);
        break;
      case kSynthParamIndexEgR3_2:
        op2.SetParameter(kOpParamIndexEgR3, (float)value * 0.01f);
        break;
      case kSynthParamIndexEgR4_2:
        op2.SetParameter(kOpParamIndexEgR4, (float)value * 0.01f);
        break;
      case kSynthParamIndexEgL1_2:
        op2.SetParameter(kOpParamIndexEgL1, (float)value * 0.01f);
        break;
      case kSynthParamIndexEgL2_2:
        op2.SetParameter(kOpParamIndexEgL2, (float)value * 0.01f);
        break;
      case kSynthParamIndexEgL3_2:
        op2.SetParameter(kOpParamIndexEgL3, (float)value * 0.01f);
        break;
      case kSynthParamIndexEgL4_2:
        op2.SetParameter(kOpParamIndexEgL4, (float)value * 0.01f);
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
    op1.NoteOn(note, velocity);
    op2.NoteOn(note, velocity);
  }

  inline void NoteOff(uint8_t note) {
    op1.NoteOff(note);
    op2.NoteOff(note);
  }

  inline void GateOn(uint8_t velocity) {
    op1.GateOn(velocity);
    op2.GateOn(velocity);
  }

  inline void GateOff() {
    op1.GateOff();
    op2.GateOff();
  }

  inline void AllNoteOff() {
    op1.GateOff();
    op2.GateOff();
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
  Operator op1;
  Operator op2;

  int32_t params[KNumSynthParams];

  /*===========================================================================*/
  /* Private Methods. */
  /*===========================================================================*/

  /*===========================================================================*/
  /* Constants. */
  /*===========================================================================*/
};
