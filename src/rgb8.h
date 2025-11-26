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

#ifndef RGB8_H
#define RGB8_H

#include <glm/glm.hpp>
#include <cstdint>

class rgb8 {
public:
  rgb8(const glm::vec3 &d) : r((uint8_t)d.x), g((uint8_t)d.y), b((uint8_t)d.z) {}
  rgb8(const uint8_t r=0, const uint8_t g=0, const uint8_t b=0) : r(r), g(g), b(b) {}
  uint8_t r,g,b;
};

#endif
