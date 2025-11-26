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

#ifndef ColorMap_H
#define ColorMap_H

#include <map>
#include <list>
#include <vector>
#include <glm/glm.hpp>

class ColorMap {
public:
  uint32_t *operator()(std::string name)
  {
    MapType::iterator colormap = map.find(name);
    return (colormap != map.end()) ? colormap->second : (uint32_t *) jetLUT;
  }
  ColorMap();
  typedef std::map<std::string,uint32_t *> MapType;
  MapType map;
  bool loadCustomLUThex(std::string lutFilename, bool verbose);
  bool loadCustomLUT(std::string lutFilename, bool verbose);
  static uint32_t jetLUT[];
  static uint32_t hsvLUT[];
  static uint32_t coolLUT[];
  static uint32_t copperLUT[];
  static uint32_t pinkLUT[];
  static uint32_t greyLUT[];
  static uint32_t greyInverseLUT[];
  static uint32_t hotLUT[];
  static uint32_t redLUT[];
  static uint32_t greenLUT[];
  static uint32_t blueLUT[];
  static uint32_t prismLUT[];
  static uint32_t boneLUT[];
  static glm::vec3 rgb2pt(uint32_t a)
  {
    unsigned char *p = (unsigned char *)&a;
    return glm::vec3(p[2]/255.0f,p[1]/255.0f,p[0]/255.0f);
  }
  static glm::vec3 colormap(uint32_t *LUT, float value, float lower, float upper)
  {
    float range = upper - lower;
    if (range==0) return rgb2pt(LUT[0]);
    int scale = (int)(255 * (value - lower)/range);
    if (scale<0) scale = 0;
    if (scale>255) scale = 255;
    return rgb2pt(LUT[scale]);
  }
  static void initialize(); // constructs grey inverse table
  std::list<std::vector<uint32_t>> customMaps;
};

#endif
