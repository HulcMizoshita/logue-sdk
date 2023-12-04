#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <math.h>

#include <arm_neon.h>

enum : uint8_t{
  kEgState1 = 0,  // Attack
  kEgState2,      // Decay1
  kEgState3,      // Decay2
  kEgState4,      // Sustain
  kEgState5,      // Release
  kEgStateIdle,
  kNumEgState
};

enum : uint8_t
{
  kEgParamIndexR1 = 0, // 0 = faster, 1 = slower
  kEgParamIndexR2,
  kEgParamIndexR3,
  kEgParamIndexR4,
  kEgParamIndexL1,
  kEgParamIndexL2,
  kEgParamIndexL3,
  kEgParamIndexL4,
  kNumEgParams
};

class EnvelopeGenerator
{
public:
  EnvelopeGenerator();
  ~EnvelopeGenerator();
  void Init(float sr);
  void SetParameter(uint8_t index, float value);
  float ProcessSample();
  void GateOn();
  void GateOff();
private:
  void NextState();

  float mSampleRate = 0.0f;
  uint8_t mState = kEgStateIdle;
  float mPhase = 0.0f;
  float mParams[kNumEgParams];
  float mStart = 0.0f;
  float mEnd = 0.0f;
  float mEnvelopeVal = 0.0f;
};

EnvelopeGenerator::EnvelopeGenerator()
{
}

EnvelopeGenerator::~EnvelopeGenerator()
{
}

void EnvelopeGenerator::Init(float sr)
{
  mSampleRate = sr;
  mState = kEgStateIdle;
  mPhase = 0.0f;
  mStart = 0.0f;
  mEnd = 0.0f;
  mEnvelopeVal = 0.0f;
  for (uint8_t i = 0; i < kNumEgParams; i++)
  {
    mParams[i] = 0.0f;
  }
}

void EnvelopeGenerator::SetParameter(uint8_t index, float value)
{
  mParams[index] = value;
}

float EnvelopeGenerator::ProcessSample()
{
  switch (mState)
  {
  case kEgState1: // Attack
  {
    float dist = mEnd - mStart;
    float rate = dist / (powf(1.0f - mParams[kEgParamIndexR1], 4.0f) * mSampleRate);
    mEnvelopeVal += rate;
    if (abs(dist) < abs(mEnvelopeVal - mStart))
    {
      mEnvelopeVal = mEnd;
      NextState();
    }
  }
    break;
  case kEgState2: // Decay1
  {
    float dist = mEnd - mStart;
    float rate = dist / (powf(1.0f - mParams[kEgParamIndexR2], 4.0f) * mSampleRate);
    mEnvelopeVal += rate;
    if (abs(dist) < abs(mEnvelopeVal - mStart))
    {
      mEnvelopeVal = mEnd;
      NextState();
    }
  }
    break;
  case kEgState3: // Decay2
  {
    float dist = mEnd - mStart;
    float rate = dist / (powf(1.0f - mParams[kEgParamIndexR3], 4.0f) * mSampleRate);
    mEnvelopeVal += rate;
    if (abs(dist) < abs(mEnvelopeVal - mStart))
    {
      mEnvelopeVal = mEnd;
      NextState();
    }
  }
    break;
  case kEgState4: // Sustain
    mEnvelopeVal = mParams[kEgParamIndexL3];
    break;
  case kEgState5: // Release
  {
    float dist = mEnd - mStart;
    float rate = dist / (powf(1.0f - mParams[kEgParamIndexR4], 4.0f) * mSampleRate);
    mEnvelopeVal += rate;
    if (abs(dist) < abs(mEnvelopeVal - mStart))
    {
      mEnvelopeVal = mEnd;
      NextState();
    }
  }
    break;
  case kEgStateIdle:
    mEnvelopeVal = mParams[kEgParamIndexL4];
    break;
  default:
    break;
  }
  return mEnvelopeVal;
}

void EnvelopeGenerator::GateOn()
{
  if (mParams[kEgParamIndexR1] < 1.0f)
  {
    mEnvelopeVal = 0.0f;
    mStart = 0.0f;
    mEnd = mParams[kEgParamIndexL1];
    mState = kEgState1;
  }
  else if(mParams[kEgParamIndexR2] < 1.0f)
  {
    mEnvelopeVal = mParams[kEgParamIndexL1];
    mStart = mParams[kEgParamIndexL1];
    mEnd = mParams[kEgParamIndexL2];
    mState = kEgState2;
  }
  else if(mParams[kEgParamIndexR3] < 1.0f)
  {
    mEnvelopeVal = mParams[kEgParamIndexL2];
    mStart = mParams[kEgParamIndexL2];
    mEnd = mParams[kEgParamIndexL3];
    mState = kEgState3;
  }
  else
  {
    mState = kEgState4;
  }
}

void EnvelopeGenerator::GateOff()
{
  if (mState != kEgStateIdle)
  {
    if (mParams[kEgParamIndexR4] < 1.0f)
    {
      mStart = mEnvelopeVal;
      mEnd = mParams[kEgParamIndexL4];
      mState = kEgState5;
    }
    else
    {
      mEnvelopeVal = mParams[kEgParamIndexL4];
      mState = kEgStateIdle;
    }
  }
}

void EnvelopeGenerator::NextState()
{
  switch (mState)
  {
  case kEgState1:
    if(mParams[kEgParamIndexR2] < 1.0f)
    {
      mStart = mParams[kEgParamIndexL1];
      mEnd = mParams[kEgParamIndexL2];
      mState = kEgState2;
    }
    else if(mParams[kEgParamIndexR3] < 1.0f)
    {
      mStart = mParams[kEgParamIndexL2];
      mEnd = mParams[kEgParamIndexL3];
      mState = kEgState3;
    }
    else
    {
      mState = kEgState4;
    }
    break;
  case kEgState2:
    if(mParams[kEgParamIndexR3] < 1.0f)
    {
      mStart = mParams[kEgParamIndexL2];
      mEnd = mParams[kEgParamIndexL3];
      mState = kEgState3;
    }
    else
    {
      mState = kEgState4;
    }
    break;
  case kEgState3:
    mState = kEgState4;
    break;
  case kEgState5:
    mState = kEgStateIdle;
    break;
  default:
    break;
  }
}