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

#ifndef SliceT_H
#define SliceT_H

#include <fstream>
#include <vector>

template <class T>
class SliceT {
public:
  SliceT() : data(0), cx(0), cy(0) {}
  SliceT(const size_t cx_, const size_t cy_) : data(0), cx(0), cy(0) { create(cx_,cy_); }
  ~SliceT() { }
  size_t size() { return data.size(); }
  bool save(const char *ofname)
  {
    std::ofstream ofile(ofname,std::ios::binary);
    if (!ofile) return false;
    ofile.write(reinterpret_cast<char *>(&data[0]),data.size()*sizeof(T));
    ofile.close();
    return true;
  }
  void set(const T &t)
  {
    T *d = data;
    const size_t ss = data.size();
    for (size_t i=0;i<ss;i++) d[i] = t;
  }
  void create(size_t cx_, size_t cy_)
  {
    cx = cx_;
    cy = cy_;
    data.resize(cx*cy);
  }
  void setsize(size_t cx_, size_t cy_)
  {
    create(cx_,cy_);
  }
  T *begin() { return &data[0]; }
  T *start() { return &data[0]; } // should be a vector under here!
  T const *start() const { return &data[0]; }
  T &operator[](const size_t pos)       { return data[pos]; }
  T  operator[](const size_t pos) const { return data[pos]; }
  T  operator()(const size_t x, const size_t y) const { return data[y*cx+x]; }
  T &operator()(const size_t x, const size_t y)       { return data[y*cx+x]; }
  std::vector<T> data;
  float rx=1,ry=1,rz=1;
  size_t cx;
  size_t cy;

};

#endif
