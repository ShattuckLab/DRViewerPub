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

#ifndef VolND_T_H
#define VolND_T_H

#include <volnd.h>
#include <strutil.h>
#include <zstream.h>
#include <iostream>
#include <algorithm>
template <class T>
void VolND<T>::threshold(const double t)
{
  for (auto &v : data) v=(v>t) ? v : 0;
}

template <class T>
void VolND<T>::makeThresholdMask(VolND<uint8_t> &vMask, const double t) const
{
  vMask.reshape(hdr);
  std::transform(data.cbegin(),data.cend(),vMask.begin(),[t](T v) { return (v>t) ? 255 : 0; });
}

template <class T>
std::unique_ptr<VolND<uint8_t>> VolND<T>::makeThresholdMask(const double t) const
{
  auto vMask=std::make_unique<VolND<uint8_t>>();
  makeThresholdMask(*vMask,t);
  return vMask;
}

template <class T>
bool VolND<T>::zeropad(VolND<T> &vOut, const uint16_t px, const uint16_t py, const uint16_t pz) const
{
  if (&vOut==this) return false;
  auto newhdr=hdr;
  newhdr.dim[1]+=px*2;
  newhdr.dim[2]+=py*2;
  newhdr.dim[3]+=pz*2;
  vOut.reshape(newhdr);
  std::fill(vOut.begin(),vOut.end(),0);
  const auto nx=dim[0];
  const auto ny=dim[1];
  const auto nz=dim[2];
  auto p=data.begin();
  for (size_t z=0;z<nz;z++)
    for (size_t y=0;y<ny;y++)
      for (size_t x=0;x<nx;x++)
        vOut(x+px,y+py,z+pz)=*p++;
  return true;
}

template <class T>
std::unique_ptr<VolNBase> VolND<T>::zeropad(const uint16_t px, const uint16_t py, const uint16_t pz) const
{
  auto vOut=std::make_unique<VolND<T>>();
  zeropad(*vOut,px,py,pz);
  return vOut;
}

template <class T>
bool VolND<T>::crop(VolND<T> &vOut, const uint16_t cx, const uint16_t cy, const uint16_t cz) const
{
  auto newhdr=hdr;
  newhdr.dim[1]-=cx*2;
  newhdr.dim[2]-=cy*2;
  newhdr.dim[3]-=cz*2;
  if (newhdr.dim[1]<=0||newhdr.dim[2]<=0||newhdr.dim[3]<=0) { std::cout<<"cropping removes all data."<<std::endl; return false; }
  vOut.reshape(newhdr);
  const auto nx=vOut.dim[0];
  const auto ny=vOut.dim[1];
  const auto nz=vOut.dim[2];
  auto &s(*this);
  for (size_t z=0;z<nz;z++)
    for (size_t y=0;y<ny;y++)
      for (size_t x=0;x<nx;x++)
        vOut(x,y,z)=s(x+cx,y+cy,z+cz);
  return true;
}

template <class T>
std::unique_ptr<VolNBase> VolND<T>::crop(const uint16_t cx, const uint16_t cy, const uint16_t cz) const
{
  auto vOut=std::make_unique<VolND<T>>();
  if (!crop(*vOut,cx,cy,cz)) vOut=nullptr;
  return vOut;
}

template <class T>
void VolND<T>::applyMask(const VolND<uint8_t> &vMask)
{
  if (vMask.size()==data.size())
  {
    std::transform(cbegin(), cend(), vMask.cbegin(), begin(), [](T v, uint8_t m) { return (m!=0) ? v : 0; });
  }
  else
  {
    std::cerr<<"incompatible mask! "<<vMask.size()<<" != "<<data.size()<<std::endl;
  }
}

template <class T>
void VolND<T>::reshape(const nifti_1_header &newhdr) // update shape from hdr information
{
  hdr=newhdr;
  hdr.datatype=niftiTypeID();
  hdr.vox_offset=352;
  for (size_t i=0;i<7;i++) dim[i]=hdr.dim[i+1];
  for (size_t i=0;i<7;i++) if (dim[i]==0) dim[i]=1;
  for (size_t i=0;i<7;i++) res[i]=hdr.pixdim[i+1];
  data.resize(dim[0]*dim[1]*dim[2]*dim[3]*dim[4]);
}

template <class T>
std::unique_ptr<VolNBase> VolND<T>::rescaleAsFloat()
{
  auto vScaled=std::make_unique<VolND<float_t>>();
  vScaled->reshape(hdr);
  std::cout<<"rescaling as x*"<<hdr.scl_slope<<" + "<<hdr.scl_inter<<std::endl;
  std::transform(data.cbegin(), data.cend(), vScaled->begin(), [this](T c) { return c*hdr.scl_slope+hdr.scl_inter; });
  vScaled->hdr.scl_slope=1;
  vScaled->hdr.scl_inter=0;
  return vScaled;
}

template <class T>
bool VolND<T>::read(const std::string &s)
{
  SILT::izstream ifile(s);
  if (!ifile) return false;
  filename=s;
  ifile.read(&hdr,sizeof(hdr));
  for (size_t i=0;i<7;i++) dim[i]=hdr.dim[i+1];
  for (size_t i=0;i<7;i++) if (dim[i]==0) dim[i]=1;
  for (size_t i=0;i<7;i++) res[i]=hdr.pixdim[i+1];
  data.resize(dim[0]*dim[1]*dim[2]*dim[3]*dim[4]);
  ifile.seekg(hdr.vox_offset);
  ifile.read(&data[0],data.size()*sizeof(T));
  return true;
}

template <class T>
bool VolND<T>::write(const std::string &s)
{
  auto ofname=s;
  bool isNIFTI = StrUtil::hasExtension(StrUtil::gzStrip(ofname),".nii");
  bool isAnalyze = StrUtil::hasExtension(StrUtil::gzStrip(ofname),".img")||StrUtil::hasExtension(ofname,".hdr");
  if (!(isNIFTI||isAnalyze)) { ofname += ".nii.gz"; isNIFTI=true; }
  SILT::ozstream ofile(s,StrUtil::isGZ(ofname) ? Z_DEFAULT_COMPRESSION : Z_NO_COMPRESSION);
  if (!ofile) return false;
  ofile.write((char *)&hdr, sizeof(hdr));
  char buf[4]={0,0,0,0};
  ofile.write(buf,4);
  size_t nb=data.size()*sizeof(T);
  size_t bytesWritten=ofile.write(reinterpret_cast<char *>(&data[0]),data.size()*sizeof(T));
  std::cout<<"wrote "<<bytesWritten<<" of "<<nb<<"bytes."<<std::endl;
  return true;
}

#endif
