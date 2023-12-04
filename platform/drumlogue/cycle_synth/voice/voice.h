#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <math.h>

#include <arm_neon.h>
#include <array>

#include "envelope/envelope_generator.h"
#include "operator/operator.h"
#include "constants.h"
#include "algorithm.h"

class Voice
{
public:
  Voice();
  ~Voice();
  void Init(float sr);
  void Reset();
  float ProcessSample();
  void SetParameter(uint16_t index, int32_t value);
  void NoteOn(uint8_t note, uint8_t velocity);
  void NoteOff(uint8_t note);

private:
  Operator mOperators[kNumOperaters];
  EnvelopeGenerator mPitchEg;

  uint8_t mNote = 0;
  int32_t mParams[kNumSyxIndexVoiceParams];
  
  float mOutputsZ[kNumOperaters] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
  float mFeedbackZ = 0.0f;
};

Voice::Voice(/* args */)
{
}

Voice::~Voice()
{
}

void Voice::Init(float sr)
{
  for (size_t i = 0; i < kNumOperaters; i++)
  {
    mOperators[i].Init(sr);
    mOutputsZ[i] = 0.0f;
  }
  
  mPitchEg.Init(sr);

  mFeedbackZ = 0.0f;

  for (uint8_t i = 0; i < kNumSyxIndexVoiceParams; i++)
  {
    mParams[i] = 0;
  }
}

void Voice::Reset()
{

}

float Voice::ProcessSample()
{
  for (size_t i = 0; i < kNumEgParams; i++)
  {
    mPitchEg.SetParameter(i, (float)mParams[kSyxIndexVoicePitchEgR1 + i] * 0.01f);
  }

  const float pitchEnv = mPitchEg.ProcessSample();
  const float fbLevel = (float)mParams[kSyxIndexVoiceFeedback] / 7.0f;
  const int16_t algorithm = mParams[kSyxIndexVoiceAlgorithmNum];

  float tmpOut[kNumOperaters] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

  for (uint8_t outCh = 0; outCh < kNumOperaters; outCh++)
  {
    float mod = 0.0f;
    uint8_t modNum = 0;
    for (uint8_t inCh = 0; inCh < kNumOperaters; inCh++)
    {
      if (inCh == outCh) continue;
      
      const bool modGate = kAlgorithmMatrix[algorithm][outCh][inCh];
      if (modGate)
      {
        mod += mOutputsZ[inCh];
        modNum++;
      }
    }

    const bool fbGate = kAlgorithmMatrix[algorithm][outCh][kNumOperaters];
    if (fbGate)
    {
      mod += mFeedbackZ * fbLevel;
      modNum++;
    }

    mod = modNum > 0 ? mod / (float)modNum : 0.0f;
    tmpOut[outCh] = mOperators[outCh].ProcessSample(mod, pitchEnv, 0.0f);
  }

  float fb = 0.0f;
  float out = 0.0f;
  uint8_t fbNum = 0;
  uint8_t outNum = 0;
  for (uint8_t outCh = 0; outCh < kNumOperaters; outCh++)
  {
    const bool fbGate = kAlgorithmMatrix[algorithm][kNumOperaters][outCh];
    if (fbGate)
    {
      fb += tmpOut[outCh];
      fbNum++;
    }
    
    const bool outGate = kAlgorithmMatrix[algorithm][kNumOperaters + 1][outCh];
    if (outGate)
    {
      out += tmpOut[outCh];
      outNum++;
    }
    mOutputsZ[outCh] = tmpOut[outCh];
  }

  mFeedbackZ = fbNum > 0 ? fb / (float)fbNum : 0.0f;
  out = outNum > 0 ? out / (float)outNum : 0.0f;

  return out;
}

void Voice::SetParameter(uint16_t index, int32_t value)
{
  if (index < kNumOperaters * kNumSyxIndexOpParams)
  {
    uint16_t opIndex = kNumOperaters - (index / kNumSyxIndexOpParams) - 1;
    uint16_t paramIndex = index % kNumSyxIndexOpParams;
    mOperators[opIndex].SetParameter(paramIndex, value);
  }
  else
  {
    mParams[index - kNumOperaters * kNumSyxIndexOpParams] = value;
  }
}

void Voice::NoteOn(uint8_t note, uint8_t velocity)
{
  for (uint8_t i = 0; i < kNumOperaters; i++)
  {
    mOperators[i].NoteOn(note, velocity);
  }
}

void Voice::NoteOff(uint8_t note)
{
  for (uint8_t i = 0; i < kNumOperaters; i++)
  {
    mOperators[i].NoteOff(note);
  }
}
