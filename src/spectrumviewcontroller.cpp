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

#include "spectrumviewcontroller.h"
#include <QtCore/qnamespace.h>
#include <QDebug>

void SpectrumViewController::setup(int width, int height)
{
  viewport=glm::vec2(width,height);
}

bool SpectrumViewController::mouseMove(glm::vec2 point, uint32_t buttonFlags, uint32_t modifiers)
{
  glm::vec2 delta = point-mouseDownPoint;
  bool altKeyDown = modifiers & Qt::AltModifier;
  if (altKeyDown && buttonFlags)
  {
    shift.x += (viewport.x>0) ? delta.x / viewport.x : 0;
    shift.y -= (viewport.y>0) ? delta.y / viewport.x : 0;
    mouseDownPoint = point;
    return true;
  }
  else if ((modifiers&Qt::ControlModifier && buttonFlags)||(buttonFlags & Qt::MiddleButton ))
  {
    zoom *= (((delta.y)>0) ? 1.1f : 1.0f/1.1f);
    mouseDownPoint = point;
    return true;
  }
  else if (modifiers&Qt::ShiftModifier && (buttonFlags  & Qt::LeftButton ))
  {
    auto truedelta=point-originalDownPoint;
    // lock to one axis
    if (std::abs(truedelta.x)>std::abs(truedelta.y))
      scale.x=this->originalScale.x*std::pow(2.0f,-truedelta.x/100.0f);
    else
      scale.y=this->originalScale.y*std::pow(2.0f, truedelta.y/100.0f);
    mouseDownPoint = point;
    return true;
  }
  else return false;
}

void SpectrumViewController::reset()
{
  scale=glm::vec2(1.0f,1.0f);
  shift=glm::vec2(0,0);
}
