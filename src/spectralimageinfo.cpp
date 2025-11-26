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

#include <spectralimageinfo.h>
#include <nlohmann/json.hpp>


const std::map<SpectralImageInfo::ImageType,std::string> SpectralImageInfo::typenames =
{
  {SpectralImageInfo::Spectral2DxSpatial3D, "Spectral2DxSpatial3D"},
  {SpectralImageInfo::Spectral1DxSpatial3D, "Spectral1DxSpatial3D"},
  {SpectralImageInfo::Spectral2DxSpatial2D, "Spectral2DxSpatial2D"},
  {SpectralImageInfo::Spectral1DxSpatial2D, "Spectral1DxSpatial2D"},
  {SpectralImageInfo::Spatial3DxSpectral2D, "Spatial3DxSpectral2D"},
  {SpectralImageInfo::Spatial3DxSpectral1D, "Spatial3DxSpectral1D"},
  {SpectralImageInfo::Spatial2DxSpectral2D, "Spatial2DxSpectral2D"},
  {SpectralImageInfo::Spatial2DxSpectral1D, "Spatial2DxSpectral1D"},
  {SpectralImageInfo::Spectral1D, "Spectral1D"},
  {SpectralImageInfo::Spectral2D, "Spectral2D"},
  {SpectralImageInfo::Spatial2D, "Spatial2D"},
  {SpectralImageInfo::Spatial3D, "Spatial3D"},
  {SpectralImageInfo::Unsupported, "Unsupported"},
  {SpectralImageInfo::Unknown, "Unknown"}
};

bool SpectralImageInfo::parse(const std::string &sidecarfile)
{
  std::ifstream jsonfile(sidecarfile);
  if (!jsonfile)
  {
    qDebug()<<"can't read "<<sidecarfile;
    return false;
  }
  qDebug()<<"reading image metadata from "<<sidecarfile;
  nlohmann::json metadata = nlohmann::json::parse(jsonfile);
  try {
    if (metadata.contains("spectral_dim")) spectral_dim=metadata["spectral_dim"].get<std::vector<int>>();
    if (metadata.contains("spatial_dim")) spatial_dim=metadata["spatial_dim"].get<std::vector<int>>();
    if (metadata.contains("axes"))
    {
      axes=AxisInfo::parse(metadata["axes"]);
    }
    else
    {
      qDebug()<<"file contains no axis data";
      return false;
    }
  }
  catch (const nlohmann::json::exception& e)
  {
      qDebug()<< e.what();
      return false;
  }
  catch (...)
  {
    qDebug()<<"unknown exception!";
    return false;
  }
  auto firstDim=axes[0].dimtype;
  if (spectral_dim.size()==2)
  {
    switch (spatial_dim.size())
    {
      case 0:  imtype=Spectral2D; break;
      case 2:  imtype=(firstDim==AxisInfo::DimType::Spectral) ? Spectral2DxSpatial2D : Spatial2DxSpectral2D; break;
      case 3:  imtype=(firstDim==AxisInfo::DimType::Spectral) ? Spectral2DxSpatial3D : Spatial3DxSpectral2D; break;
      default: imtype=Unsupported; break;
    }
  }
  else if (spectral_dim.size()==1)
  {
    switch (spatial_dim.size())
    {
      case 0:  imtype=Spectral2D; break;
      case 2:  imtype=(firstDim==AxisInfo::DimType::Spectral) ? Spectral1DxSpatial2D : Spatial2DxSpectral1D; break;
      case 3:  imtype=(firstDim==AxisInfo::DimType::Spectral) ? Spectral1DxSpatial3D : Spatial3DxSpectral1D; break;
      default: imtype=Unsupported; break;
    }
  }
  else
  {
    switch (spatial_dim.size())
    {
      case 2:  imtype=Spatial2D; break;
      case 3:  imtype=Spatial3D; break;
      default: imtype=Unsupported; break;
    }
  }
  return true;
}
