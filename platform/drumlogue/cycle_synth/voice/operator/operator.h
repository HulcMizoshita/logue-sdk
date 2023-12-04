#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <math.h>

#include <arm_neon.h>

#include "sine_osc_512.h"
#include "../envelope/envelope_generator.h"
#include "../constants.h"

enum : uint8_t
{
  kFreqModeRatio = 0,
  kFreqModeFixed
};

class Operator
{
public:
  Operator();
  ~Operator();
  void Init(float samplerate);
  void Reset();
  void CalculateFreq(uint8_t note);
  void SetParameter(uint8_t index, int32_t value);
  float ProcessSample(float phase_mod, float pitch_mod, float amp_mod);
  void NoteOn(uint8_t note, uint8_t velocity);
  void NoteOff(uint8_t note);

private:
  float mSamplerateRecip = 0.0f;

  uint8_t mNote = 0;
  float mVelocity = 0.f;
  float mBaseFreq = 440.f;

  float mPhase = 0.0f;

  int32_t mParams[kNumSyxIndexOpParams];

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
  for (uint8_t i = 0; i < kNumSyxIndexOpParams; i++)
  {
    mParams[i] = 0.0f;
  }
}

void Operator::Reset()
{
  mBaseFreq = 440.f;
}

void Operator::CalculateFreq(uint8_t note)
{
  if (mParams[kSyxIndexOpOscMode] == kFreqModeRatio)
  {
    float freq = (440.f / 32.0f) * powf(2.0f, ((float)note - 9.0f) / 12.0f);
    float coarse = (float)mParams[kSyxIndexOpOscCoarse] > 0 ? (float)mParams[kSyxIndexOpOscCoarse] : 0.5f;
    float fine = (float)mParams[kSyxIndexOpOscFine] * 0.01f;
    float detune = (float)mParams[kSyxIndexOpOscDetune] - 7;
    const float ratio = coarse * (1 + fine);
    mBaseFreq = (freq + detune) * ratio;
  }
  else
  {
    float detune = ((float)mParams[kSyxIndexOpOscDetune] - 7.0f) * 0.01f;
    float freq = powf(10.0f + detune, (float)(mParams[kSyxIndexOpOscCoarse] % 4) + (float)mParams[kSyxIndexOpOscFine] * 0.01f);
    mBaseFreq = (freq);
  }
}

void Operator::SetParameter(uint8_t index, int32_t value)
{
  mParams[index] = value;

  switch (index)
  {
  case kSyxIndexOpOscMode:
  case kSyxIndexOpOscCoarse:
  case kSyxIndexOpOscFine:
  case kSyxIndexOpOscDetune:
    CalculateFreq(mNote);
    break;
  default:
    break;
  }
}

float Operator::ProcessSample(float phase_mod, float pitch_mod, float amp_mod)
{
  for (size_t i = 0; i < kNumEgParams; i++)
  {
    mEG.SetParameter(i, (float)mParams[kSyxIndexOpEgR1 + i] * 0.01f);
  }

  float w0 = mBaseFreq * mSamplerateRecip * powf(2, (pitch_mod * 8 - 4));
  float sig = mSineOsc.GetSample(mPhase);
  sig *= mEG.ProcessSample() * mVelocity;
  sig *= ((float)mParams[kSyxIndexOpOutputLevel] * 0.01f);

  mPhase += (w0 + phase_mod * 0.1f);
  mPhase = fmod(mPhase, 1.0f);
  return sig;
}

void Operator::NoteOn(uint8_t note, uint8_t velocity)
{
  mNote = note;
  CalculateFreq(mNote);
  float velSens = (float)mParams[kSyxIndexOpKeyVelSens] / 7.0f;
  mVelocity = (velocity / 127.0f * velSens) + (1.0f - velSens);
  mPhase = 0.f;
  mEG.GateOn();
}

void Operator::NoteOff(uint8_t note)
{
  (void)note;
  mEG.GateOff();
}