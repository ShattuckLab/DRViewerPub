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

#ifndef SPECTRALIMAGEINFO_H
#define SPECTRALIMAGEINFO_H

#include <string>
#include <map>
#include <nlohmann/json_fwd.hpp>
#include <QDebug>
#include <fstream>
#include <axisinfo.h>

class SpectralImageInfo {
public:
  enum ImageType {
    Spectral2DxSpatial3D,
    Spectral1DxSpatial3D,
    Spectral2DxSpatial2D,
    Spectral1DxSpatial2D,
    Spatial3DxSpectral2D,
    Spatial3DxSpectral1D,
    Spatial2DxSpectral2D,
    Spatial2DxSpectral1D,
    Spectral1D,
    Spectral2D,
    Spatial2D,
    Spatial3D,
    Unsupported,
    Unknown
  };
  std::string imageTypetext(const ImageType &t) const {
    auto it = typenames.find(t);
    if (it != typenames.end()) {
        return it->second;
    }
    return "Unknown";
  }
  static const std::map<ImageType,std::string> typenames;
  bool spectralFirst()
  {
    switch (imtype)
    {
      case Spectral2DxSpatial3D:
      case Spectral1DxSpatial3D:
      case Spectral2DxSpatial2D:
      case Spectral1DxSpatial2D: return true;
      default: return false;
    }
  }
  bool spatialFirst()
  {
    switch (imtype)
    {
      case Spatial3DxSpectral2D:
      case Spatial3DxSpectral1D:
      case Spatial2DxSpectral2D:
      case Spatial2DxSpectral1D: return true;
      default: return false;
    }
  }
  std::string imageTypetext() { return imageTypetext(imtype); }
  ImageType imageType() { return imtype; }
  ImageType imtype=Unknown;
  bool parse(const std::string &sidecarfile);
  std::vector<AxisInfo> axes;
  std::vector<int> spectral_dim;
  std::vector<int> spatial_dim;
};


#endif // SPECTRALIMAGEINFO_H
