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

#ifndef VolND_H
#define VolND_H

#include <vector>
#include <volnbase.h>

template <class T>
class VolND : public VolNBase {
public:
  virtual ~VolND() {}
  virtual int niftiTypeID() const override { return DT_UNKNOWN; }
	SILT::DataType typeID() const override;
  auto start() { return &data[0]; }
  auto cstart() const { return &data[0]; }
  auto begin() { return data.begin(); }
  auto end() { return data.end(); }
  auto cbegin() const { return data.cbegin(); }
  auto cend() const { return data.cend(); }
  T &operator[](const size_t t) { return data[t]; }
  T  operator[](const size_t t) const { return data[t]; }
// It might be better to derive 2D,3D, and 4D classes that provide these operators
  size_t offset(const size_t x, const size_t y) const { return x+dim[0]*y; }
  size_t offset(const size_t x, const size_t y, const size_t z) const { return x+dim[0]*(y+dim[1]*z); }
  size_t offset(const size_t x, const size_t y, const size_t z, const size_t t) const { return x+dim[0]*(y+dim[1]*(z+dim[2]*t)); }
  T &operator()(const size_t x, const size_t y)       { return data[x+dim[0]*y]; }
  T  operator()(const size_t x, const size_t y) const { return data[x+dim[0]*y]; }
  T &operator()(const size_t x, const size_t y, const size_t z)       { return data[x+dim[0]*(y+dim[1]*z)]; }
  T  operator()(const size_t x, const size_t y, const size_t z) const { return data[x+dim[0]*(y+dim[1]*z)]; }
  T &operator()(const size_t x, const size_t y, const size_t z, const size_t u)       { return data[x+dim[0]*(y+dim[1]*(z+dim[2]*u))]; }
  T  operator()(const size_t x, const size_t y, const size_t z, const size_t u) const { return data[x+dim[0]*(y+dim[1]*(z+dim[2]*u))]; }
  T &operator()(const size_t x, const size_t y, const size_t z, const size_t u, const size_t v)       { return data[x+dim[0]*(y+dim[1]*(z+dim[2]*(u+v*dim[3])))]; }
  T  operator()(const size_t x, const size_t y, const size_t z, const size_t u, const size_t v) const { return data[x+dim[0]*(y+dim[1]*(z+dim[2]*(u+v*dim[3])))]; }
  const T *slice(const size_t z) const { return &data[dim[0]*dim[1]*z]; }
  T *slice(const size_t z) { return &data[dim[0]*dim[1]*z]; }
	T *volume(const size_t t) { return &data[dim[0]*dim[1]*dim[2]*t]; }
	const T *volume(const size_t t) const { return &data[dim[0]*dim[1]*dim[2]*t]; }
  T *volume(const size_t u, const size_t v) { return &data[dim[0]*dim[1]*dim[2]*(u+v*dim[3])]; }
  const T *volume(const size_t u, const size_t v) const { return &data[dim[0]*dim[1]*dim[2]*(u+v*dim[3])]; }
//  virtual bool makeCompatible(const VolNBase *v);

  virtual std::unique_ptr<VolNBase> rescaleAsFloat() override;
  virtual T maxvalue() const {
    auto r=std::max_element(data.cbegin(),data.cend());
    return (r!=data.cend()) ? (*r) : 0;
  }
  virtual void applyMask(const VolND<uint8_t> &vMask) override;
  virtual void threshold(const double t) override;
  virtual std::unique_ptr<VolND<uint8_t>> makeThresholdMask(const double t) const override;
  virtual void makeThresholdMask(VolND<uint8_t> &vMask, const double t) const override;
  virtual std::unique_ptr<VolNBase> zeropad(const uint16_t px, const uint16_t py, const uint16_t pz) const;
  virtual std::unique_ptr<VolNBase> crop(const uint16_t cx, const uint16_t cy, const uint16_t cz) const;
  virtual bool zeropad(VolND<T> &vOut, const uint16_t px, const uint16_t py, const uint16_t pz) const;
  virtual bool crop(VolND<T> &dst, const uint16_t cx, const uint16_t cy, const uint16_t cz) const;
  virtual void reshape(const nifti_1_header &newhdr) override; // update shape from hdr information
  bool read(const std::string &s) override;
  bool write(const std::string &s) override;
  virtual size_t size() const override { return data.size(); }
private:
  std::vector<T> data;
};

// DSCodes
// VECTOR CODES/MULTICHANNEL
// #define DS_VEC3F           4096  //2048:0001'0000'0000'0000
// #define DS_VEC3D           4352  //2048:0001'0001'0000'0000
// #define DS_SHC45           8192  //2048:0010'0000'0000'0000

template<> inline int VolND<int8_t  >::niftiTypeID() const { return DT_INT8; }
template<> inline int VolND<uint8_t >::niftiTypeID() const { return DT_UINT8; }
template<> inline int VolND<int16_t >::niftiTypeID() const { return DT_INT16; }
template<> inline int VolND<uint16_t>::niftiTypeID() const { return DT_UINT16; }
template<> inline int VolND<int32_t >::niftiTypeID() const { return DT_INT32; }
template<> inline int VolND<uint32_t>::niftiTypeID() const { return DT_UINT32; }
template<> inline int VolND<int64_t >::niftiTypeID() const { return DT_INT64; }
template<> inline int VolND<uint64_t>::niftiTypeID() const { return DT_UINT64; }
template<> inline int VolND<float   >::niftiTypeID() const { return DT_FLOAT32; }
template<> inline int VolND<double  >::niftiTypeID() const { return DT_FLOAT64; }
//template<> inline int VolND<rgb8>::niftiTypeID() const { return DT_RGB; }

template<> inline SILT::DataType VolND<uint8_t>::typeID() const { return SILT::Uint8; }
template<> inline SILT::DataType VolND<int8_t >::typeID() const { return SILT::Sint8; }
template<> inline SILT::DataType VolND<uint16_t>::typeID() const { return SILT::Uint16; }
template<> inline SILT::DataType VolND<int16_t >::typeID() const { return SILT::Sint16; }
template<> inline SILT::DataType VolND<int32_t >::typeID() const { return SILT::Sint32; }
template<> inline SILT::DataType VolND<uint32_t>::typeID() const { return SILT::Uint32; }
template<> inline SILT::DataType VolND<int64_t >::typeID() const { return SILT::Sint64; }
template<> inline SILT::DataType VolND<uint64_t>::typeID() const { return SILT::Uint64; }
template<> inline SILT::DataType VolND<float>::typeID() const { return SILT::Float32; }
template<> inline SILT::DataType VolND<double>::typeID() const { return SILT::Float64; }
// template<> inline SILT::DataType VolND<rgb8>::typeID() const { return SILT::RGB8; }
//template<> inline SILT::DataType VolND<EigenSystem3x3f>::typeID() const { return SILT::Eigensystem3x3f; }

#endif
