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

#include <iostream>
#include <volnd_t.h>

#define VolNDInstance(T)\
  template bool VolND<T>::read(std::string, Vol3DBase::AutoRotateCode);

std::unique_ptr<VolNBase> VolNBase::load(std::string ifname)
{
  std::unique_ptr<VolNBase> volume;
  SILT::izstream ifile(ifname);
  if (!ifile) return nullptr;
  nifti_1_header hdr;
  ifile.read(&hdr,sizeof(hdr));
  switch (hdr.datatype)
  {
    case DT_INT8    : volume=std::make_unique<VolND<int8_t  >>(); break;
    case DT_UINT8   : volume=std::make_unique<VolND<uint8_t >>(); break;
    case DT_INT32   : volume=std::make_unique<VolND<int32_t >>(); break;
    case DT_UINT16  : volume=std::make_unique<VolND<uint16_t>>(); break;
    case DT_INT16   : volume=std::make_unique<VolND<int16_t >>(); break;
    case DT_UINT32  : volume=std::make_unique<VolND<uint32_t>>(); break;
    case DT_INT64   : volume=std::make_unique<VolND<int64_t >>(); break;
    case DT_UINT64  : volume=std::make_unique<VolND<uint64_t>>(); break;
    case DT_FLOAT32 : volume=std::make_unique<VolND<float_t >>(); break;
    case DT_FLOAT64 : volume=std::make_unique<VolND<double_t>>(); break;
    default: std::cerr<<"type for "<<ifname<<" is unsupported."<<std::endl;
  }
  if (volume)
  {
    if (!volume->read(ifname)) return nullptr;
    if (volume->hdr.scl_inter!=0 || (volume->hdr.scl_slope !=1 && volume->hdr.scl_slope !=0))
    {
      std::cout<<"Rescaling "<<hdr.scl_inter<<"*x + "<<volume->hdr.scl_slope<<std::endl;
      volume=volume->rescaleAsFloat();
    }
  }
  return volume;
}

