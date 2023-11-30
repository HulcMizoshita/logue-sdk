#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <math.h>

#include <arm_neon.h>

#include "sine_osc_512.h"
#include "../envelope/envelope_generator.h"

enum : uint8_t
{
  kFreqModeRatio = 0,
  kFreqModeFixed
};

enum : uint8_t
{
  kOpParamIndexFreqMode = 0,
  kOpParamIndexCoarse,
  kOpParamIndexFine,
  kOpParamIndexDetune,
  kOpParamIndexEgR1,
  kOpParamIndexEgR2,
  kOpParamIndexEgR3,
  kOpParamIndexEgR4,
  kOpParamIndexEgL1,
  kOpParamIndexEgL2,
  kOpParamIndexEgL3,
  kOpParamIndexEgL4,
  kOpParamIndexAMSens,
  kOpParamIndexVelSens,
  kOpParamIndexLevel,
  kNumOpParams
};

// struct OpParams
// {
//   // frequency
//   uint8_t freqMode = kFreqModeRatio;
//   float coarse = 1.0f; // 0.5 ~ 31
//   float fine = 0.0f; // 0 ~ 1
//   float detune = 0.0f; // -7 ~ +7

//   // envelope
//   float r1, r2, r3, r4; // 0 ~ 1
//   float l1, l2, l3, l4; // 0 ~ 1
// };

class Operator
{
public:
  Operator();
  ~Operator();
  void Init(float samplerate);
  void Reset();
  void SetFreq(float freq);
  void SetParameter(uint8_t index, float value);
  float ProcessSample(float mod);
  void NoteOn(uint8_t note, uint8_t velocity);
  void NoteOff(uint8_t note);
  void GateOn(uint8_t velocity);
  void GateOff();
  void AllNoteOff();

private:
  float mSamplerateRecip = 0.0f;
  float mFreq = 440.0f;
  float mBaseFreq = 440.f;
  float mPhase = 0.f;
  float mVelocity = 0.f;
  float mParams[kNumOpParams];

  SineOsc512 mSineOsc;
  EnvelopeGenerator mEG;
};

Operator::Operator()
{
}

Operator::~Operator()
{
}

void Operator::Init(float sr)
{
  mSamplerateRecip = 1.0f / sr;
  mEG.Init(sr);
  for (uint8_t i = 0; i < kNumOpParams; i++)
  {
    mParams[i] = 0.0f;
  }
}

void Operator::Reset()
{
  mBaseFreq = 440.f;
}

void Operator::SetFreq(float freq)
{
  mFreq = freq;
  const float ratio = mParams[kOpParamIndexCoarse] * (1 + mParams[kOpParamIndexFine]);
  mBaseFreq = (mFreq + mParams[kOpParamIndexDetune]) * ratio;
}

void Operator::SetParameter(uint8_t index, float value)
{
  mParams[index] = value;
  switch (index)
  {
  case kOpParamIndexFreqMode:
  case kOpParamIndexCoarse:
  case kOpParamIndexFine:
  case kOpParamIndexDetune:
  {
    const float ratio = mParams[kOpParamIndexCoarse] * (1 + mParams[kOpParamIndexFine]);
    mBaseFreq = (mFreq + mParams[kOpParamIndexDetune]) * ratio;
    break;
  }
  case kOpParamIndexEgR1:
  case kOpParamIndexEgR2:
  case kOpParamIndexEgR3:
  case kOpParamIndexEgR4:
  case kOpParamIndexEgL1:
  case kOpParamIndexEgL2:
  case kOpParamIndexEgL3:
  case kOpParamIndexEgL4:
    mEG.SetParameter(index - kOpParamIndexEgR1, value);
    break;
  default:
    break;
  }
}

float Operator::ProcessSample(float mod)
{
  float w0 = mBaseFreq * mSamplerateRecip;
  float sig = mSineOsc.GetSample(mPhase);
  sig *= mEG.ProcessSample() * mVelocity;
  sig *= mParams[kOpParamIndexLevel];
  mPhase += (w0 + mod);
  mPhase = fmod(mPhase, 1.0f);
  return sig;
}

void Operator::NoteOn(uint8_t note, uint8_t velocity)
{
  SetFreq((440.f / 32) * powf(2, (note - 9.0) / 12));
  mPhase = 0.f;
  mVelocity = (velocity / 127.0f * mParams[kOpParamIndexVelSens]) + (1.0f - mParams[kOpParamIndexVelSens]);
  mEG.GateOn();
}

void Operator::NoteOff(uint8_t note)
{
  (void)note;
  mEG.GateOff();
}

void Operator::GateOn(uint8_t velocity) {
  mPhase = 0.f;
  mVelocity = velocity / 127.0f;
  mEG.GateOn();
}

void Operator::GateOff() {
  mEG.GateOff();
}

void Operator::AllNoteOff() {
  mEG.GateOff();
}