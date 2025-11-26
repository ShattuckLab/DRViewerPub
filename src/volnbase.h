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

#ifndef VolNBase_H
#define VolNBase_H

#include <memory>
#include <string>
#include <nifti1.h>
#include <glm/mat3x3.hpp>
#include <silttypes.h>

template <class T> class VolND;

class VolNBase  {
public:
// loads from nifti and interprets the data type
  virtual ~VolNBase() {}
	std::string datatypeName() const
	{
		return SILT::datatypeName(typeID());
	}
	virtual SILT::DataType typeID() const { return SILT::Unknown; }
  static std::unique_ptr<VolNBase> load(std::string ifname);
// type-specific loaders
  virtual bool read(const std::string &ifname)=0;
  virtual size_t size() const =0;
  virtual bool write(const std::string &ifname)=0;
  virtual std::unique_ptr<VolNBase> rescaleAsFloat()=0;
  virtual void applyMask(const VolND<uint8_t> &vMask)=0;
  virtual void threshold(const double t)=0;
  virtual std::unique_ptr<VolND<uint8_t>> makeThresholdMask(const double t) const =0;
  virtual void makeThresholdMask(VolND<uint8_t> &vMask, const double t) const =0;
  virtual void reshape(const nifti_1_header &hdr)=0;
  virtual int niftiTypeID() const { return DT_UNKNOWN; }
  static bool scanQForm(const nifti_1_header &header);  
  virtual bool isCompatible(const VolNBase *v) const { return v->size()==size(); }
	virtual bool isCompatible(const VolNBase &v) const { return v.size()==size(); }

  std::string filename;  
  auto rx() const { return res[0]; }
  auto ry() const { return res[1]; }
  auto rz() const { return res[2]; }
  auto ru() const { return res[3]; }
  auto rv() const { return res[4]; }
  auto nx() const { return dim[0]; }
  auto ny() const { return dim[1]; }
  auto nz() const { return dim[2]; }
  auto nu() const { return dim[3]; }
  auto nv() const { return dim[4]; }
	void scanQform();
  size_t dim[8]={0,0,0,0,0,0,0,0};
  float res[8]={1,1,1,1,1,1,1,1};
  nifti_1_header hdr;
};

#endif
