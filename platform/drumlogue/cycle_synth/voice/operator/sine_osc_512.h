#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <math.h>

#include <arm_neon.h>

static const float sine_table[512] = {
  0.0, 0.012271538, 0.024541229, 0.036807224, 0.049067676, 0.061320737, 0.07356457, 0.08579731, 0.09801714, 0.110222206, 0.12241068, 0.1345807, 0.14673047, 0.15885815, 0.17096189, 0.18303989,
  0.19509032, 0.20711137, 0.21910124, 0.2310581, 0.24298018, 0.25486565, 0.26671275, 0.2785197, 0.29028466, 0.30200595, 0.31368175, 0.3253103, 0.33688986, 0.34841868, 0.35989505, 0.3713172,
  0.38268343, 0.39399204, 0.4052413, 0.41642955, 0.42755508, 0.43861625, 0.44961134, 0.46053872, 0.47139674, 0.48218378, 0.4928982, 0.50353837, 0.51410276, 0.52458966, 0.53499764, 0.545325,
  0.55557024, 0.5657318, 0.57580817, 0.58579785, 0.5956993, 0.60551107, 0.6152316, 0.6248595, 0.6343933, 0.64383155, 0.65317285, 0.6624158, 0.671559, 0.680601, 0.68954057, 0.69837624, 0.70710677,
  0.71573085, 0.7242471, 0.7326543, 0.7409511, 0.7491364, 0.7572088, 0.76516724, 0.77301043, 0.7807372, 0.7883464, 0.7958369, 0.8032075, 0.81045717, 0.8175848, 0.8245893, 0.8314696, 0.8382247,
  0.8448536, 0.8513552, 0.8577286, 0.86397284, 0.87008697, 0.8760701, 0.8819213, 0.88763964, 0.8932243, 0.8986745, 0.9039893, 0.909168, 0.9142098, 0.9191139, 0.9238795, 0.9285061, 0.9329928, 0.937339,
  0.94154406, 0.9456073, 0.94952816, 0.953306, 0.95694035, 0.9604305, 0.96377605, 0.96697646, 0.97003126, 0.97293997, 0.9757021, 0.9783174, 0.98078525, 0.9831055, 0.98527765, 0.9873014, 0.9891765,
  0.99090266, 0.99247956, 0.993907, 0.9951847, 0.9963126, 0.99729043, 0.9981181, 0.99879545, 0.99932235, 0.9996988, 0.9999247, 1.0, 0.9999247, 0.9996988, 0.99932235, 0.99879545, 0.9981181, 0.99729043,
  0.9963126, 0.9951847, 0.993907, 0.99247956, 0.99090266, 0.9891765, 0.9873014, 0.98527765, 0.9831055, 0.98078525, 0.9783174, 0.9757021, 0.97293997, 0.97003126, 0.96697646, 0.96377605, 0.9604305,
  0.95694035, 0.953306, 0.94952816, 0.9456073, 0.94154406, 0.937339, 0.9329928, 0.9285061, 0.9238795, 0.9191139, 0.9142098, 0.909168, 0.9039893, 0.8986745, 0.8932243, 0.88763964, 0.8819213, 0.8760701,
  0.87008697, 0.86397284, 0.8577286, 0.8513552, 0.8448536, 0.8382247, 0.8314696, 0.8245893, 0.8175848, 0.81045717, 0.8032075, 0.7958369, 0.7883464, 0.7807372, 0.77301043, 0.76516724, 0.7572088,
  0.7491364, 0.7409511, 0.7326543, 0.7242471, 0.71573085, 0.70710677, 0.69837624, 0.68954057, 0.680601, 0.671559, 0.6624158, 0.65317285, 0.64383155, 0.6343933, 0.6248595, 0.6152316, 0.60551107,
  0.5956993, 0.58579785, 0.57580817, 0.5657318, 0.55557024, 0.545325, 0.53499764, 0.52458966, 0.51410276, 0.50353837, 0.4928982, 0.48218378, 0.47139674, 0.46053872, 0.44961134, 0.43861625, 0.42755508,
  0.41642955, 0.4052413, 0.39399204, 0.38268343, 0.3713172, 0.35989505, 0.34841868, 0.33688986, 0.3253103, 0.31368175, 0.30200595, 0.29028466, 0.2785197, 0.26671275, 0.25486565, 0.24298018, 0.2310581,
  0.21910124, 0.20711137, 0.19509032, 0.18303989, 0.17096189, 0.15885815, 0.14673047, 0.1345807, 0.12241068, 0.110222206, 0.09801714, 0.08579731, 0.07356457, 0.061320737, 0.049067676, 0.036807224,
  0.024541229, 0.012271538, 1.2246469e-16, -0.012271538, -0.024541229, -0.036807224, -0.049067676, -0.061320737, -0.07356457, -0.08579731, -0.09801714, -0.110222206, -0.12241068, -0.1345807,
  -0.14673047, -0.15885815, -0.17096189, -0.18303989, -0.19509032, -0.20711137, -0.21910124, -0.2310581, -0.24298018, -0.25486565, -0.26671275, -0.2785197, -0.29028466, -0.30200595, -0.31368175,
  -0.3253103, -0.33688986, -0.34841868, -0.35989505, -0.3713172, -0.38268343, -0.39399204, -0.4052413, -0.41642955, -0.42755508, -0.43861625, -0.44961134, -0.46053872, -0.47139674, -0.48218378,
  -0.4928982, -0.50353837, -0.51410276, -0.52458966, -0.53499764, -0.545325, -0.55557024, -0.5657318, -0.57580817, -0.58579785, -0.5956993, -0.60551107, -0.6152316, -0.6248595, -0.6343933,
  -0.64383155, -0.65317285, -0.6624158, -0.671559, -0.680601, -0.68954057, -0.69837624, -0.70710677, -0.71573085, -0.7242471, -0.7326543, -0.7409511, -0.7491364, -0.7572088, -0.76516724, -0.77301043,
  -0.7807372, -0.7883464, -0.7958369, -0.8032075, -0.81045717, -0.8175848, -0.8245893, -0.8314696, -0.8382247, -0.8448536, -0.8513552, -0.8577286, -0.86397284, -0.87008697, -0.8760701, -0.8819213,
  -0.88763964, -0.8932243, -0.8986745, -0.9039893, -0.909168, -0.9142098, -0.9191139, -0.9238795, -0.9285061, -0.9329928, -0.937339, -0.94154406, -0.9456073, -0.94952816, -0.953306, -0.95694035,
  -0.9604305, -0.96377605, -0.96697646, -0.97003126, -0.97293997, -0.9757021, -0.9783174, -0.98078525, -0.9831055, -0.98527765, -0.9873014, -0.9891765, -0.99090266, -0.99247956, -0.993907, -0.9951847,
  -0.9963126, -0.99729043, -0.9981181, -0.99879545, -0.99932235, -0.9996988, -0.9999247, -1.0, -0.9999247, -0.9996988, -0.99932235, -0.99879545, -0.9981181, -0.99729043, -0.9963126, -0.9951847,
  -0.993907, -0.99247956, -0.99090266, -0.9891765, -0.9873014, -0.98527765, -0.9831055, -0.98078525, -0.9783174, -0.9757021, -0.97293997, -0.97003126, -0.96697646, -0.96377605, -0.9604305, -0.95694035,
  -0.953306, -0.94952816, -0.9456073, -0.94154406, -0.937339, -0.9329928, -0.9285061, -0.9238795, -0.9191139, -0.9142098, -0.909168, -0.9039893, -0.8986745, -0.8932243, -0.88763964, -0.8819213,
  -0.8760701, -0.87008697, -0.86397284, -0.8577286, -0.8513552, -0.8448536, -0.8382247, -0.8314696, -0.8245893, -0.8175848, -0.81045717, -0.8032075, -0.7958369, -0.7883464, -0.7807372, -0.77301043,
  -0.76516724, -0.7572088, -0.7491364, -0.7409511, -0.7326543, -0.7242471, -0.71573085, -0.70710677, -0.69837624, -0.68954057, -0.680601, -0.671559, -0.6624158, -0.65317285, -0.64383155, -0.6343933,
  -0.6248595, -0.6152316, -0.60551107, -0.5956993, -0.58579785, -0.57580817, -0.5657318, -0.55557024, -0.545325, -0.53499764, -0.52458966, -0.51410276, -0.50353837, -0.4928982, -0.48218378,
  -0.47139674, -0.46053872, -0.44961134, -0.43861625, -0.42755508, -0.41642955, -0.4052413, -0.39399204, -0.38268343, -0.3713172, -0.35989505, -0.34841868, -0.33688986, -0.3253103, -0.31368175,
  -0.30200595, -0.29028466, -0.2785197, -0.26671275, -0.25486565, -0.24298018, -0.2310581, -0.21910124, -0.20711137, -0.19509032, -0.18303989, -0.17096189, -0.15885815, -0.14673047, -0.1345807,
  -0.12241068, -0.110222206, -0.09801714, -0.08579731, -0.07356457, -0.061320737, -0.049067676, -0.036807224, -0.024541229, -0.012271538
};

class SineOsc512
{
private:
  /* data */
public:
  SineOsc512(){};
  ~SineOsc512(){};

  float GetSample(float phase)
  {
    const float index = fmod(phase * 512.0f + 512.0f, 512.0f);
    const int indexInt = floor(index);
    const int indexNext = (indexInt + 1) % 512;
    const float frac = index - indexInt;
    return sine_table[indexInt] * (1.0f - frac) + sine_table[indexNext] * frac;
  };
};