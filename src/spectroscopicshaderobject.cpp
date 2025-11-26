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


#include <spectroscopicshaderobject.h>
#include <qdebug.h>
#include <rgb8.h>
#include <dsshaderprogram.h>
#include <checkglerror.h>
#include <colorops.h>

SpectroscopicShaderObject::SpectroscopicShaderObject()
{
}

SpectroscopicShaderObject::~SpectroscopicShaderObject()
{
}

void SpectroscopicShaderObject::setTextures(DSShaderProgram &program, bool smooth)
{
  CHECK_GL
  if (!program.bind()) { qDebug()<<"SSO unbound"; }
  CHECK_GL

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_3D, spectrumTex.texID());
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//smooth ? GL_LINEAR : GL_NEAREST); CHECK_GL
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//smooth ? GL_LINEAR : GL_NEAREST); CHECK_GL
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_1D, spectrumTex.lutTexID());

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_3D, componentTex.texID());
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//smooth ? GL_LINEAR : GL_NEAREST); CHECK_GL
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//smooth ? GL_LINEAR : GL_NEAREST); CHECK_GL
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_1D, componentTex.lutTexID());

  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_3D, vol1tex.texID());
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, smooth ? GL_LINEAR : GL_NEAREST); CHECK_GL
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, smooth ? GL_LINEAR : GL_NEAREST); CHECK_GL
  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_1D, vol1tex.lutTexID());

  glActiveTexture(GL_TEXTURE6);
  glBindTexture(GL_TEXTURE_3D, vol2tex.texID());
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, smooth ? GL_LINEAR : GL_NEAREST); CHECK_GL
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, smooth ? GL_LINEAR : GL_NEAREST); CHECK_GL
  glActiveTexture(GL_TEXTURE7);
  glBindTexture(GL_TEXTURE_1D, vol2tex.lutTexID());

  glActiveTexture(GL_TEXTURE8);
  glBindTexture(GL_TEXTURE_3D, vol3tex.texID());
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, smooth ? GL_LINEAR : GL_NEAREST); CHECK_GL
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, smooth ? GL_LINEAR : GL_NEAREST); CHECK_GL
  glActiveTexture(GL_TEXTURE9);
  glBindTexture(GL_TEXTURE_1D, vol3tex.lutTexID());

  glActiveTexture(GL_TEXTURE10);
  glBindTexture(GL_TEXTURE_3D, vol4tex.texID());
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, smooth ? GL_LINEAR : GL_NEAREST); CHECK_GL
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, smooth ? GL_LINEAR : GL_NEAREST); CHECK_GL
  glActiveTexture(GL_TEXTURE11);
  glBindTexture(GL_TEXTURE_1D, vol4tex.lutTexID());

  glActiveTexture(GL_TEXTURE12);
  glBindTexture(GL_TEXTURE_3D, vol5tex.texID());
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, smooth ? GL_LINEAR : GL_NEAREST); CHECK_GL
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, smooth ? GL_LINEAR : GL_NEAREST); CHECK_GL
  glActiveTexture(GL_TEXTURE13);
  glBindTexture(GL_TEXTURE_1D, vol5tex.lutTexID());


  glActiveTexture(GL_TEXTURE14);
  glBindTexture(GL_TEXTURE_3D, maskTex.texID());
CHECK_GL
  glUniform1i(program.uniformLocation("spectrumtex"), 0);CHECK_GL
  glUniform1i(program.uniformLocation("spectrum_luttex"), 1);CHECK_GL
  glUniform1i(program.uniformLocation("component_tex"), 2);CHECK_GL
  glUniform1i(program.uniformLocation("component_luttex"), 3);CHECK_GL
  glUniform1i(program.uniformLocation("vol1texf"),4);CHECK_GL
  glUniform1i(program.uniformLocation("lut1tex"), 5); CHECK_GL
  glUniform1i(program.uniformLocation("vol2texf"),6);CHECK_GL
  glUniform1i(program.uniformLocation("lut2tex"), 7); CHECK_GL
  glUniform1i(program.uniformLocation("vol3texf"),8);CHECK_GL
  glUniform1i(program.uniformLocation("lut3tex"), 9); CHECK_GL
  glUniform1i(program.uniformLocation("vol4texf"),10);CHECK_GL
  glUniform1i(program.uniformLocation("lut4tex"), 11); CHECK_GL
  glUniform1i(program.uniformLocation("vol5texf"),12);CHECK_GL
  glUniform1i(program.uniformLocation("lut5tex"), 13); CHECK_GL
  glUniform1i(program.uniformLocation("masktex"), 14);CHECK_GL
CHECK_GL
}

extern bool debugTextures;
void SpectroscopicShaderObject::loadLUT1(uint32_t *lut)
{
  if (!lut) return;
  if (debugTextures) qDebug()<<"updating vol1tex.LUT1";
  vol1tex.loadLUT(lut);
}

void SpectroscopicShaderObject::loadLUT2(uint32_t *lut)
{
  if (!lut) return;
  if (debugTextures) qDebug()<<"updating vol2tex.LUT1";
  vol2tex.loadLUT(lut);
}

void SpectroscopicShaderObject::loadLUT3(uint32_t *lut)
{
  if (!lut) return;
  if (debugTextures) qDebug()<<"updating vol3tex.LUT1";
  vol3tex.loadLUT(lut);
}

void SpectroscopicShaderObject::loadSpectrumLUT(uint32_t *lut)
{
  if (!lut) return;
  if (debugTextures) qDebug()<<"updating spectrum.LUT";
  vol3tex.loadLUT(lut);
}

void SpectroscopicShaderObject::draw(bool /*points*/)
{
  CHECK_GL
  glDisable(GL_BLEND);CHECK_GL
  glBindVertexArray(sliceVAO.vao);CHECK_GL
  glMultiDrawArrays(GL_TRIANGLE_FAN, &slicePlaneStart[0], &slicePlaneSize[0], GLsizei(slicePlaneStart.size()));CHECK_GL
  glBindVertexArray(0);CHECK_GL
  glDisable(GL_BLEND);
}

void SpectroscopicShaderObject::loadComponents(const VolND<float> *volume)
//component_tex
{
  if (!volume) return;
  initializeOpenGLFunctions();
  CHECK_GL
  if (debugTextures) qDebug()<<"vshader loading components"<<volume->filename.c_str();
  auto cx=int(volume->nx());
  auto cy=int(volume->ny());
  auto cz=int(volume->nz()*volume->nu());
  CHECK_GL
  if (!componentTex.loadRaw(volume->volume(0,0),cx,cy,cz))
  {
    qDebug()<<"vshader couldn't load components from ND";
    return;
  }
  CHECK_GL
  if (debugTextures) qDebug()<<"loaded tex1";
  return;
}

void SpectroscopicShaderObject::loadVolume1(const VolND<float> *volume, int u, int v)
{
  if (!volume) return;
  initializeOpenGLFunctions();
  CHECK_GL
  if (debugTextures) qDebug()<<"vshader loading 1ND"<<volume->filename.c_str();
  auto cx=int(volume->nx());
  auto cy=int(volume->ny());
  auto cz=int(volume->nz());
  CHECK_GL
  if (!vol1tex.loadRaw(volume->volume(u,v),cx,cy,cz))
  {
    qDebug()<<"vshader couldn't load vol1text from ND";
    return;
  }
  CHECK_GL
  if (debugTextures) qDebug()<<"loaded tex1";
  return;
}

void SpectroscopicShaderObject::loadVolume2(const VolND<float> *volume, int u, int v)
{
  if (!volume) return;
  initializeOpenGLFunctions();
  CHECK_GL
  if (debugTextures) qDebug()<<"vshader loading 2ND"<<volume->filename.c_str();
  auto cx=int(volume->nx());
  auto cy=int(volume->ny());
  auto cz=int(volume->nz());
  CHECK_GL
  if (!vol2tex.loadRaw(volume->volume(u,v),cx,cy,cz))
  {
    qDebug()<<"vshader couldn't load vol2text from ND";
    return;
  }
  if (debugTextures) qDebug()<<"loaded "<<cx*cy*cz;
  CHECK_GL
  return;
}


void SpectroscopicShaderObject::loadVolume3(const VolND<float> *volume, int u, int v)
{
  if (!volume) return;
  initializeOpenGLFunctions();
  CHECK_GL
  if (debugTextures) qDebug()<<"vshader loading 2ND"<<volume->filename.c_str();
  auto cx=int(volume->nx());
  auto cy=int(volume->ny());
  auto cz=int(volume->nz());
  CHECK_GL
  if (!vol3tex.loadRaw(volume->volume(u,v),cx,cy,cz))
  {
    qDebug()<<"vshader couldn't load vol3text from ND";
    return;
  }
  if (debugTextures) qDebug()<<"loaded "<<cx*cy*cz;
  CHECK_GL
  return;
}

void SpectroscopicShaderObject::loadVolume4(const VolND<float> *volume, int u, int v)
{
  if (!volume) return;
  initializeOpenGLFunctions();
  CHECK_GL
  if (debugTextures) qDebug()<<"vshader loading 2ND"<<volume->filename.c_str();
  auto cx=int(volume->nx());
  auto cy=int(volume->ny());
  auto cz=int(volume->nz());
  CHECK_GL
  if (!vol4tex.loadRaw(volume->volume(u,v),cx,cy,cz))
  {
    qDebug()<<"vshader couldn't load vol4text from ND";
    return;
  }
  if (debugTextures) qDebug()<<"loaded "<<cx*cy*cz;
  CHECK_GL
  return;
}

void SpectroscopicShaderObject::loadVolume5(const VolND<float> *volume, int u, int v)
{
  if (!volume) return;
  initializeOpenGLFunctions();
  CHECK_GL
  if (debugTextures) qDebug()<<"vshader loading 0ND"<<volume->filename.c_str();
  auto cx=int(volume->nx());
  auto cy=int(volume->ny());
  auto cz=int(volume->nz());
  CHECK_GL
  if (!vol5tex.loadRaw(volume->volume(u,v),cx,cy,cz))
  {
    qDebug()<<"vshader couldn't load vol5text from ND";
    return;
  }
  if (debugTextures) qDebug()<<"loaded "<<cx*cy*cz;
  CHECK_GL
  if (debugTextures) qDebug()<<"loaded tex5";
  return;
}

void SpectroscopicShaderObject::loadSpectralVolume(VolND<float> *volume, int z)
{
 if (!volume) return;
 initializeOpenGLFunctions();
 CHECK_GL
 if (debugTextures) qDebug()<<"vshader loading spectrum "<<volume->filename.c_str();
 glActiveTexture(GL_TEXTURE0); // Volume texture is now 0
 const auto cu=int(volume->nx()); // in spectrum, u and v are the 1st and 2nd dims
 const auto cv=int(volume->ny()); // order is (u,v) x (x,y,z)
 const auto cx=int(volume->nz()); // dims
 const auto cy=int(volume->nu()); //
 this->cx=volume->nz();
 this->cy=volume->nu();
 this->cz=volume->nv();
 this->rx=volume->rz();
 this->ry=volume->ru();
 this->rz=volume->rv();
 CHECK_GL
 int stride=cu*cv*cx*cy;
 if (!spectrumTex.loadRaw(volume->volume(0)+z*stride,cu,cv,cx*cy))
 {
   qDebug()<<"vshader couldn't load spectrum texture";
   return;
 }
 CHECK_GL
 return;
}

template <class T>
std::vector<float> copyToVec(const VolND<T> *volume)
{
  if (!volume) return std::vector<float>(0);
  const auto n=volume->size();
  std::vector<float> v(n);
  auto src=volume->cbegin();
  for (size_t i=0;i<n;i++) v[i]=src[i];
  return v;
}

std::vector<float> copyToVec(VolNBase *volume)
{
  switch (volume->typeID())
  {
    case SILT::Uint8: return copyToVec(dynamic_cast<VolND<uint8_t> *>(volume)); break;
    default:
      qDebug()<<"don't know what to do with mask of type "<<volume->datatypeName();
  }
  return std::vector<float>(0);
}

void SpectroscopicShaderObject::loadComponentMaskVolume(VolNBase *volume)
{
 if (!volume) return;
 initializeOpenGLFunctions();
 if (debugTextures) qDebug()<<"vshader loading tex3"<<volume->filename.c_str();
 CHECK_GL
 auto v=copyToVec(volume);
 if (v.size()>0)
 {
   if (!maskTex.loadRaw(&v[0],volume->nx(),volume->ny(),volume->nz()))
   {
     qDebug()<<"vshader couldn't load component mask from ND";
   }
 }
 else
   qDebug()<<"vshader couldn't load component mask from ND";
 CHECK_GL
 return;
}

void SpectroscopicShaderObject::generateOrthogonalSlicePlanes(const glm::vec3 &p)
{
  CHECK_GL
  std::vector<glm::vec3> arrayData;
  slicePlaneStart.clear();
  slicePlaneSize.clear();
  slicePlaneStart.reserve(3);
  slicePlaneSize.reserve(3);
  float sx = static_cast<float>(cx)*rx;
  float sy = static_cast<float>(cy)*ry;
  float sz = static_cast<float>(cz)*rz;
  int position = 0;
  glm::vec3 HALFVOX(rx/2,ry/2,rz/2);
  auto offset=HALFVOX;
  CHECK_GL
  {
    offset=-HALFVOX;offset.x=0;
    if (showYZPlane && p.x >= 0 && p.x < sx)
    {
      float tx = p.x / sx;
      arrayData.push_back(offset+glm::vec3(p.x, 0, 0));
      arrayData.push_back(glm::vec3(tx, 0, 0));
      arrayData.push_back(offset+glm::vec3(p.x, sy, 0));
      arrayData.push_back(glm::vec3(tx, 1, 0));
      arrayData.push_back(offset+glm::vec3(p.x, sy, sz));
      arrayData.push_back(glm::vec3(tx, 1, 1));
      arrayData.push_back(offset+glm::vec3(p.x, 0, sz));
      arrayData.push_back(glm::vec3(tx, 0, 1));
      slicePlaneStart.push_back(position);
      slicePlaneSize.push_back(4);
      position += 4;
    }
    offset=-HALFVOX;offset.y=0;
    if (showXZPlane && p.y >= 0 && p.y < sy)
    {
      const float ty = p.y / sy;
      arrayData.push_back(offset+glm::vec3(0, p.y, 0));
      arrayData.push_back(glm::vec3(0, ty, 0));
      arrayData.push_back(offset+glm::vec3(sx, p.y, 0));
      arrayData.push_back(glm::vec3(1, ty, 0));
      arrayData.push_back(offset+glm::vec3(sx, p.y, sz));
      arrayData.push_back(glm::vec3(1, ty, 1));
      arrayData.push_back(offset+glm::vec3(0, p.y, sz));
      arrayData.push_back(glm::vec3(0, ty, 1));
      slicePlaneStart.push_back(position);
      slicePlaneSize.push_back(4);
      position += 4;
    }
    offset=-HALFVOX;offset.z=0;
    if (showXYPlane && p.z >= 0 && p.z < sz)
    {
      arrayData.push_back(offset+ glm::vec3(0, 0, p.z));
      arrayData.push_back( glm::vec3(0, 0, p.z / sz));
      arrayData.push_back(offset+ glm::vec3(sx, 0, p.z));
      arrayData.push_back( glm::vec3(1, 0, p.z / sz));
      arrayData.push_back(offset+ glm::vec3(sx, sy, p.z));
      arrayData.push_back( glm::vec3(1, 1, p.z / sz));
      arrayData.push_back(offset+ glm::vec3(0, sy, p.z));
      arrayData.push_back( glm::vec3(0, 1, p.z / sz));
      slicePlaneStart.push_back(position);
      slicePlaneSize.push_back(4);
//      position += 4;
    }
  }
  CHECK_GL
  if (sliceVAO.vao == 0)
  {
    if (debugTextures) qDebug()<<("creating vao for slice plane\n");
    glGenVertexArrays(1, &sliceVAO.vao);
    glBindVertexArray(sliceVAO.vao);
    glGenBuffers(1, &sliceVAO.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, sliceVAO.vbo);
    GLuint stride = 2 * 3 * sizeof(float);
    uintptr_t offset = 0;
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (const void *)offset);
    offset += sizeof(glm::vec3);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (const void *)offset);
    glBindVertexArray(0);
  }
  glBindBuffer(GL_ARRAY_BUFFER, sliceVAO.vbo);
  if (arrayData.size() > 0)
  {
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * arrayData.size(), &arrayData[0], GL_STREAM_DRAW);
  }
}
