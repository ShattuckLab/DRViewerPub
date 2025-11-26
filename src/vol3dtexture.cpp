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


#include <vol3dtexture.h>
#include <qdebug.h>
#include <checkglerror.h>
#include <silttypes.h>
#include <rgb8.h>

typedef float float32;

extern bool  debugTextures;
bool  debugTextures=false;
bool Vol3DTexture::loadRaw(const float *src, const size_t dx, const size_t dy, const size_t dz)
{
  initializeOpenGLFunctions();
  if (!src) { failedFlag = true; return false; }
  if (debugTextures) qDebug()<<"loading "<<src;

  if (!textureID)
  {
    glGenTextures(1, &textureID); CHECK_GL
  }
  else
    if (debugTextures) qDebug()<<"reusing "<<textureID;
  glBindTexture(GL_TEXTURE_3D, textureID);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);CHECK_GL
  {
    loadTextureRaw(src,dx,dy,dz);//,GL_R32F, GL_RED);
    if (debugTextures) qDebug()<<"loaded glTexImage3D "<<textureID;
    int w, h, d;
    int miplevel = 0;
    glGetTexLevelParameteriv(GL_TEXTURE_3D, miplevel, GL_TEXTURE_WIDTH, &w);
    glGetTexLevelParameteriv(GL_TEXTURE_3D, miplevel, GL_TEXTURE_HEIGHT, &h);
    glGetTexLevelParameteriv(GL_TEXTURE_3D, miplevel, GL_TEXTURE_DEPTH, &d);
    volDatatype = SILT::Float32;
    glGetTexLevelParameteriv(GL_TEXTURE_3D, 0, GL_TEXTURE_INTERNAL_FORMAT, &gpuDatatype);
    if (debugTextures) qDebug()<<"loaded glTexImage3D "<<textureID<<" "<<w<<"x"<<h<<"x"<<d;
    loaded = true;
    CHECK_GL
  }
  GLint dim;
  glGetTexLevelParameteriv(GL_TEXTURE_3D, 0, GL_TEXTURE_RED_TYPE, &dim);
  CHECK_GL
  glGetTexLevelParameteriv(GL_TEXTURE_3D, 0, GL_TEXTURE_WIDTH, &dim); cx = dim;
  glGetTexLevelParameteriv(GL_TEXTURE_3D, 0, GL_TEXTURE_HEIGHT, &dim); cy = dim;
  glGetTexLevelParameteriv(GL_TEXTURE_3D, 0, GL_TEXTURE_DEPTH, &dim); cz = dim;
  if (debugTextures) qDebug()<<"text dims: "<<cx<<" "<<cy<<" "<<cz;
  CHECK_GL
  const bool smooth=false;
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, smooth ? GL_LINEAR : GL_NEAREST); CHECK_GL
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, smooth ? GL_LINEAR : GL_NEAREST); CHECK_GL
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); CHECK_GL
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER); CHECK_GL
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER); CHECK_GL
  glBindTexture(GL_TEXTURE_3D, 0); CHECK_GL
  return true;
}

bool Vol3DTexture::loadTextureRaw(const float32 *src, const size_t dx, const size_t dy, const size_t dz)
{
  const int internalFormat=GL_R32F;
  const int dataFormat=GL_RED;
  glBindTexture(GL_TEXTURE_3D, textureID);
  glTexImage3D(GL_TEXTURE_3D, 0, internalFormat, GLsizei(dx), GLsizei(dy), GLsizei(dz), 0, dataFormat, GL_FLOAT, reinterpret_cast<const char *>(src));
  CHECK_GL
  return true;
}

bool Vol3DTexture::loadLUT(std::vector<rgb8> &vRGB, bool smooth)
{
  initializeOpenGLFunctions();
  if (!lutTextureID)
  {
    glGenTextures(1, &lutTextureID); CHECK_GL
  }
  else
    if (debugTextures) qDebug()<<"reusing lut texid "<<lutTextureID;
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glBindTexture(GL_TEXTURE_1D, lutTextureID);CHECK_GL
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, GLsizei(vRGB.size()), 0, GL_RGB, GL_UNSIGNED_BYTE, (char *)&vRGB[0]); // use 32bits for speed
  if (debugTextures) qDebug()<<"loaded lut glTexImage1D "<<lutTextureID;
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, smooth ? GL_LINEAR : GL_NEAREST); CHECK_GL
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, smooth ? GL_LINEAR : GL_NEAREST); CHECK_GL
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); CHECK_GL
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); CHECK_GL
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); CHECK_GL
  glBindTexture(GL_TEXTURE_1D, 0); CHECK_GL
  return true;
}

bool Vol3DTexture::loadLUT(uint32_t *lut, const uint32_t n, bool smooth)
{
  if (!lut) return false;
  std::vector<rgb8> vRGB(n);
  for (size_t x=0;x<n;x++) vRGB[x]=rgb8((lut[x]>>16)&0xFF,(lut[x]>>8)&0xFF,lut[x]&0xFF);
  return loadLUT(vRGB,smooth);
}
