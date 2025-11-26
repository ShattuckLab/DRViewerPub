// Copyright (C) 2025 The Regents of the University of California
//
// Created by David W. Shattuck, Ph.D.
//
// This file is part of Diffusion Relaxation Suite Viewer (DRViewer).
//
// DRViewer is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, version 2.1.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
//

#ifndef ColorOps_H
#define ColorOps_H

#include <cstdint>
#include <rgb8.h>
typedef float float32;
typedef double float64;
typedef int32_t sint32;
typedef int16_t sint16;

inline uint8_t clampFloat32(const float32 v) { return uint8_t((v<255) ? ((v>=0) ? v : 0) : 255); }
inline uint8_t clampFloat32(const float64 v) { return uint8_t((v<255) ? ((v>=0) ? v : 0) : 255); }
inline uint8_t clamp32(const uint32_t v) { return (v<255) ? v : 255; }
inline uint8_t clamp32s(const sint32 v) { return (v<255) ? ((v>=0) ? v : 0) : 255; }
inline uint8_t clamp16(const uint16_t v) { return (v<255) ? v : 255; }
inline uint8_t clamp16s(const sint16 v) { return (v<255) ? ((v>=0) ? v : 0) : 255; }
inline uint8_t clamp(const uint32_t v) { return (v<255) ? v : 255; }
inline uint8_t clamp(const uint16_t v) { return (v<255) ? v : 255; }
inline uint8_t clamp(const sint16 v) { return (v<255) ? ((v>=0) ? v : 0) : 255; }

inline uint32_t rgb2word(const rgb8 v) { return ((uint32_t(v.r))<<16) | ((uint32_t(v.g))<<8) | ((uint32_t(v.b))<<0); }

class RawOp {
public:
  void operator()(uint32_t &dst, const uint8_t  src) { dst = 0x10101*(uint8_t)src; }
  void operator()(uint32_t &dst, const uint16_t src) { dst = 0x10101*clamp(src); }
  void operator()(uint32_t &dst, const uint32_t src) { dst = 0x10101*clamp(src); }
  void operator()(uint32_t &dst, const sint16 src) { dst = 0x10101*clamp(src); }
  void operator()(uint32_t &dst, const rgb8   src) { dst = rgb2word(src); }
  void operator()(uint32_t &dst, const float32 src){ dst = 0x10101*clampFloat32(src); }
  void operator()(uint32_t &dst, const float64 src){ dst = 0x10101*clampFloat32(src); }
};

class RawOpAlpha {
public:
  void operator()(uint32_t &dst, const uint8_t  src) { dst = 0x01010101*(uint8_t)src; }
  void operator()(uint32_t &dst, const uint16_t src) { dst = 0x01010101*clamp(src); }
  void operator()(uint32_t &dst, const uint32_t src) { dst = 0x01010101*clamp(src); }
  void operator()(uint32_t &dst, const sint16 src) { dst = 0x01010101*clamp(src); }
  void operator()(uint32_t &dst, const rgb8   src) { dst = rgb2word(src); }
  void operator()(uint32_t &dst, const float32 src){ dst = 0x01010101*clampFloat32(src); }
  void operator()(uint32_t &dst, const float64 src){ dst = 0x01010101*clampFloat32(src); }
};

class LUTOp {
public:
  const uint32_t *lut;
  LUTOp(const uint32_t *lut_, const float fbright) : lut(lut_), bright((fbright>1) ? (int)fbright : 1), brightF(fbright>0 ? fbright : 1) {}
  void operator()(uint32_t &dst, const uint8_t  src) { dst = lut[clamp16(uint16_t(src*255)/bright)]; }
  void operator()(uint32_t &dst, const uint16_t src) { dst = lut[clamp32(uint32_t(src*255)/bright)]; }
  void operator()(uint32_t &dst, const sint16 src) { dst = lut[clamp32s(sint32(src*255)/bright)]; }
  void operator()(uint32_t &dst, const uint32_t src) { dst = lut[clamp32(uint32_t(src*255)/bright)]; }
  void operator()(uint32_t &dst, const float32 src) { dst =lut[clampFloat32(src*255.0f/brightF)]; }
  void operator()(uint32_t &dst, const float64 src) { dst =lut[clampFloat32(src*255.0/brightF)]; }
  void operator()(uint32_t &dst, const rgb8   src) {
    uint8_t *b = (uint8_t *)&dst;
    b[0] = clamp(uint16_t((255*src.b)/bright));
    b[1] = clamp(uint16_t((255*src.g)/bright));
    b[2] = clamp(uint16_t((255*src.r)/bright));
  }
  const int bright;
  const float brightF;
};

class RangeLUTOp{
public:
  const uint32_t *lut;
  RangeLUTOp(const uint32_t *lut_, const float fmin, const float fmax)
    : lut(lut_), minI((int)fmin), maxI((fmax>1) ? (int)fmax : 1), minF(fmin), maxF(fmax>0 ? fmax : 1),
      scaleF(255.0f/(maxF-minF)) {}
  template <class T>
  void operator()(uint32_t &dst, const T src) const
  {
    dst =lut[clampFloat32((src-minF)*scaleF)];
  }
  void operator()(uint32_t &dst, const rgb8 src)  const
  {
    uint8_t *b = (uint8_t *)&dst;
    b[0] = clampFloat32((src.b-minF)*scaleF);
    b[1] = clampFloat32((src.g-minF)*scaleF);
    b[2] = clampFloat32((src.r-minF)*scaleF);
  }
  const int minI;
  const int maxI;
  const float minF;
  const float maxF;
  const float scaleF;
};

class LabelOp {
public:
  static uint32_t colorTable[65536];
  void operator()(uint32_t &dst, const uint8_t  src) { dst = colorTable[src]; }
  void operator()(uint32_t &dst, const uint16_t src) { dst = colorTable[src]; }
  void operator()(uint32_t &dst, const sint16 src) { dst = colorTable[(uint16_t)src]; }
  void operator()(uint32_t &dst, const uint32_t src) { dst = colorTable[(uint16_t)src]; }
  void operator()(uint32_t &dst, const rgb8   src) { dst = rgb2word(src); }
  void operator()(uint32_t &dst, const float32 src) { dst = colorTable[(uint16_t)src]; }
  void operator()(uint32_t &dst, const float64 src) { dst = colorTable[(uint16_t)src]; }
};

#endif
