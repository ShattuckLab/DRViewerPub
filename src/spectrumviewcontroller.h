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

#ifndef SpectrumViewController_H
#define SpectrumViewController_H

#include <glm/glm.hpp>

class SpectrumViewController {
public:
  void setup(int width, int height);
  template <class PointType>
  bool mouseDown(const PointType &p)
  {
    mouseDownPoint.x = p.x();
    mouseDownPoint.y = p.y();
    originalDownPoint=mouseDownPoint;
    originalScale=scale;
    return false;
  }
  template <class PointType>
  bool mouseMoveT(const PointType &point, uint32_t buttonFlags, uint32_t modifiers)
  {
    return mouseMove(glm::vec2(point.x(),point.y()),buttonFlags,modifiers);
  }
  void reset();
  float scaleX() const { return scale.x; }
  float scaleY() const { return scale.y; }
  float aspectRatio() const { return float(viewport.x) / float((viewport.y>0) ? viewport.y : 1.0f); } // TODO: Check this!
  bool mouseMove(glm::vec2 point, uint32_t buttonFlags, uint32_t modifiers);
  float zoom=1.0f;
  glm::vec2 viewport=glm::vec2(0,0);
  glm::vec2 shift=glm::vec2(0,0);
  glm::vec2 mouseDownPoint=glm::vec2(0,0);
  glm::vec2 originalDownPoint=glm::vec2(0,0);
  glm::vec2 originalScale=glm::vec2(0,0);
  glm::vec2 scale=glm::vec2(1.0f,1.0f);
  glm::vec2 spectralCoordinate=glm::vec2(0,0);
  glm::vec3 spatialCoordinate=glm::vec3(0,0,0);
};

#endif // SpectrumViewController_H
