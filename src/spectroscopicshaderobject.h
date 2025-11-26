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


#ifndef VOLUMESHADEROBJECT_H
#define VOLUMESHADEROBJECT_H

#include <QOpenGLFunctions_4_1_Core>
#include <volnd_t.h>
#include <vol3dtexture.h>

class DSShaderProgram;

class SpectroscopicShaderObject : protected QOpenGLFunctions_4_1_Core
{
public:
  class VAO {
  public:
    VAO() : vao(0), vbo(0), ibo(0) {}
    GLuint vao;
    GLuint vbo;
    GLuint ibo;
  };
  SpectroscopicShaderObject();
  virtual ~SpectroscopicShaderObject() override;
  void draw(bool points=false);
  void loadComponents(const VolND<float> *volume);
  void loadVolume1(const VolND<float> *volume, int u=0, int v=0);
  void loadVolume2(const VolND<float> *volume, int u=0, int v=0);
  void loadVolume3(const VolND<float> *volume, int u=0, int v=0);
  void loadVolume4(const VolND<float> *volume, int u=0, int v=0);
  void loadVolume5(const VolND<float> *volume, int u=0, int v=0);
  void loadComponentVolume4D(const VolND<float> *volume, int u=0, int v=0);
  void loadSpectralVolume(VolND<float> *volume, int z=0); // assumes (u,v) (x,y,z)
  void loadComponentMaskVolume(VolNBase *volume); // assumes (u,v) (x,y,z)
  void loadLUT1(uint32_t *lut);
  void loadLUT2(uint32_t *lut);
  void loadLUT3(uint32_t *lut);
  void loadLUT4(uint32_t *lut);
  void loadLUT5(uint32_t *lut);
  void loadSpectrumLUT(uint32_t *lut);
  void generateOrthogonalSlicePlanes(const glm::vec3 &p);
  void addPlane(std::vector<glm::vec3> &arrayData, std::vector<glm::vec3> &points);
  void setTextures(DSShaderProgram &program, bool smooth);
  auto center() { return glm::vec3(cx*rx/2.0f, cy*ry/2.0f, cz*rz/2.0f); }
  int vao() { return sliceVAO.vao; }
  size_t nplanes() { return slicePlaneSize.size(); }
private:
  Vol3DTexture vol1tex,vol2tex,vol3tex,vol4tex,vol5tex;
  Vol3DTexture spectrumTex,componentTex;
  Vol3DTexture labelTex,maskTex;

private:
  int cx=0,cy=0,cz=0;
  float rx=1.0f,ry=1.0f,rz=1.0f;
  std::vector<int> slicePlaneStart, slicePlaneSize;
  VAO sliceVAO;
public:
  bool showXYPlane=true;
  bool showXZPlane=false;
  bool showYZPlane=false;
};

#endif // VOLUMESHADEROBJECT_H
