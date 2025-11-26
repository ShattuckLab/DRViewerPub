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


#ifndef Vol3DTexture_H
#define Vol3DTexture_H

#include <QOpenGLFunctions_4_1_Core>
#include <glm/glm.hpp>
#include <silttypes.h>
#include <rgb8.h>

class Vol3DTexture : protected QOpenGLFunctions_4_1_Core {
public:
  bool loadRaw(const float *src, const size_t dx, const size_t dy, const size_t cz);
  bool loadLUT(std::vector<rgb8> &vRGB, bool smooth=true);
  bool loadLUT(uint32_t *lut, const uint32_t n=256, bool smooth=true);
  GLuint texID() const { return textureID; }
  GLuint lutTexID() const { return lutTextureID; }
 private:
  bool loadTextureRaw(const float *src, const size_t dx, const size_t dy, const size_t dz);
  bool loadTextureRaw(glm::vec3 *src, const size_t dx, const size_t dy, const size_t dz);
  GLuint textureID=0;
  GLuint lutTextureID=0;
  float scaleFactor=1.0f;
  int gpuDatatype = 0;
  SILT::DataType volDatatype = SILT::Unknown;
  int cx=0,cy=0,cz=0;
  bool failedFlag=false;
  bool isRGB=false;
  bool loaded = false;
  bool debug=false;
  int pad2=0;
};

#endif
