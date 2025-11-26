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

#ifndef MARCHINGSQUARES_H
#define MARCHINGSQUARES_H

//#include "vol3ddatatypes.h"
#include <slicet.h>
#include <uvpoint.h>
#include <vector>

class MarchingSquares {
public:
    struct EdgeVertex {
      float x,y;
    };
    struct LineSegment {
      EdgeVertex a;
      EdgeVertex b;
    };
  // MarchingSquares();
    // template <class PointType, class ImageType>
    typedef float ImageType;
    typedef EdgeVertex PointType;
    static void march(std::vector<LineSegment> &output, SliceT<ImageType> &image, ImageType threshold, bool interpolate);
    struct EdgeList {// only 0, 1, or 2 edges possible per square
      uint8_t n; // # of edges
      uint8_t e1;
      uint8_t e2;
    };
    static constexpr EdgeVertex v0{   0,-0.5}; // 0 because shifted to voxel center, vertically shifted -0.5 to edge
    static constexpr EdgeVertex v1{ 0.5,   0};
    static constexpr EdgeVertex v2{   0, 0.5};
    static constexpr EdgeVertex v3{-0.5,   0};

    static constexpr LineSegment segments[]=
    {
      { MarchingSquares::v0,MarchingSquares::v0 }, // not used
      { MarchingSquares::v0,MarchingSquares::v1 },
      { MarchingSquares::v0,MarchingSquares::v2 },
      { MarchingSquares::v0,MarchingSquares::v3 },
      { MarchingSquares::v1,MarchingSquares::v2 },
      { MarchingSquares::v1,MarchingSquares::v3 },
      { MarchingSquares::v2,MarchingSquares::v3 }
    };
    //    encoded as abcd
    //     a 0 b
    //     3   1
    //     c 2 d
    // edges: AB-BD: 01 10
    //        BD-CD :
    // possible edges : 01, 02, 03, 12, 13, 23
    // encodings:
    //  0 : no edge
    //  1 : edge 0-1 (or b000011 -> 3?)
    //  2 : edge 0-2
    //  3 : edge 0-3
    //  4 : edge 1-2
    //  5 : edge 1-3
    //  6 : edge 2-3
    static constexpr EdgeList edgeTable[] =
    {
      {0,0,0},  // 0000 : empty
      {1,4,0}, // 0001 :    d : edge 12 : 00000000
      {1,6,0}, // 0010 :   c  : edge 23
      {1,5,0}, // 0011 :   cd : edge 13
      {1,1,0}, // 0100 :  b   : edge 01
      {1,2,0}, // 0101 :  b d : edge 02
      {2,3,4}, // 0110 :  bc  : edges 03,12
      {1,3,0}, // 0111 :  bcd : edge 03
      {1,3,0}, // 1000 :  a   : 03
      {1,1,6}, // 1001 :  ad : 01,23
      {1,2,0}, // 1010 :  ac : 02
      {1,1,0}, // 1011 :  acd: 01
      {1,5,0}, // 1100 :  ab : 13
      {1,6,0}, // 1101 : abd : 23
      {1,4,0}, // 1110 : abc : 12
      {0,0,0}, // 1111 : empty
    };
};

class ContourLevel {
  public:
  std::vector<MarchingSquares::LineSegment> contours;
  float value=0;
};

#endif // MARCHINGSQUARES_H
