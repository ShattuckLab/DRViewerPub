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

#include "marchingsquares.h"

inline MarchingSquares::LineSegment translate(const MarchingSquares::LineSegment &seg, float x, float y)
{
  MarchingSquares::PointType a{ seg.a.x+x,seg.a.y+y };
  MarchingSquares::PointType b{ seg.b.x+x,seg.b.y+y };
  return {a,b};
}

void MarchingSquares::march(std::vector<LineSegment> &output, SliceT<ImageType> &image, ImageType threshold, bool interpolate)
{
  const auto cx=image.cx;
  const auto cy=image.cy;
  output.clear();
  for (size_t y=1;y<cy;y++)
    for (size_t x=1;x<cx;x++)
    {
      const auto p00=image(x-1,y-1); // x is first number@
      const auto p10=image(x  ,y-1);
      const auto p01=image(x-1,y  ); // top right
      const auto p11=image(x  ,y  ); // bottom left
      uint32_t code=
           ((p00>threshold)<<3) | ((p10>threshold)<<2)
         | ((p01>threshold)<<1) | ((p11>threshold)   );
      const auto &edgeList=edgeTable[code];
      if (!interpolate)
      {
        if (edgeList.e1) output.push_back(translate(segments[edgeList.e1],x,y));
        if (edgeList.e2) output.push_back(translate(segments[edgeList.e2],x,y));
      }
      else
      {
        switch(edgeList.e1)
        {
          case 1: // edge (ab)-(bd) or mid p00/p10 and {0,-0.5} - { 0.5,0} // or {-0.5+xfac,-0.5} - { 0.5,-0.5+yfac}
            {
              LineSegment seg;
              const auto factorXTop=(threshold-p00)/(p10-p00);
              seg.a.x=-0.5+factorXTop;
              seg.a.y=-0.5;
              const auto factorYRight=(threshold-p10)/(p11-p10);
              seg.b.x=0.5;
              seg.b.y=-0.5+factorYRight;
              output.push_back(translate(seg,x,y));
            }
            break;
          case 2: // { MarchingSquares::v0,MarchingSquares::v2 } -- vertical line
            {
              LineSegment seg;
              const auto factorXTop =(threshold-p00)/(p10-p00); // same as case 1
              seg.a.x=-0.5+factorXTop;
              seg.a.y=-0.5;
              const auto factorXBottom=(threshold-p01)/(p11-p01); // for bottom of square
              seg.b.x=-0.5+factorXBottom;
              seg.b.y=+0.5;
              output.push_back(translate(seg,x,y));
            }
            break;
          case 3: //{ MarchingSquares::v0,MarchingSquares::v3 } from top mid to left mid
          {
            LineSegment seg;
            auto factorXTop =(threshold-p00)/(p10-p00); // same as case 1, top mid
            seg.a.x=-0.5+factorXTop;
            seg.a.y=-0.5;
            auto factorYLeft=(threshold-p00)/(p01-p00); // left-mid
            seg.b.x=-0.5;
            seg.b.y=-0.5+factorYLeft;
            output.push_back(translate(seg,x,y));
          }
          break;
            case 4: //{ MarchingSquares::v1,MarchingSquares::v2 }, from right mid to bottom mid
            {
              LineSegment seg;
              const auto factorYRight=(threshold-p10)/(p11-p10); // right mid
              seg.a.x=0.5;
              seg.a.y=-0.5+factorYRight;
              const auto factorXBottom=(threshold-p01)/(p11-p01); // for bottom of square
              seg.b.x=-0.5+factorXBottom;
              seg.b.y=+0.5;
              output.push_back(translate(seg,x,y));
            }
            break;
            case 5: //{ MarchingSquares::v1,MarchingSquares::v3 }, from right mid to left mid (line)
            {
              LineSegment seg;
              const auto factorYRight=(threshold-p10)/(p11-p10); // right mid
              seg.a.x=0.5;
              seg.a.y=-0.5+factorYRight;
              auto factorYLeft=(threshold-p00)/(p01-p00); // left-mid
              seg.b.x=-0.5;
              seg.b.y=-0.5+factorYLeft;
              output.push_back(translate(seg,x,y));
            }
            break;
          case 6: //{ MarchingSquares::v2,MarchingSquares::v3 }, from bottom mid to left mid
            {
              LineSegment seg;
              const auto factorXBottom=(threshold-p01)/(p11-p01); // for bottom of square
              seg.a.x=-0.5+factorXBottom;
              seg.a.y=+0.5;
              auto factorYLeft=(threshold-p00)/(p01-p00); // left-mid
              seg.b.x=-0.5;
              seg.b.y=-0.5+factorYLeft;
              output.push_back(translate(seg,x,y));
            }
            break;
          default:
            break;
        }
      }
      switch(edgeList.e2) // exact copy -- could loop
      {
        case 1: // edge (ab)-(bd) or mid p00/p10 and {0,-0.5} - { 0.5,0} // or {-0.5+xfac,-0.5} - { 0.5,-0.5+yfac}
          {
            LineSegment seg;
            const auto factorXTop=(threshold-p00)/(p10-p00);
            seg.a.x=-0.5+factorXTop;
            seg.a.y=-0.5;
            const auto factorYRight=(threshold-p10)/(p11-p10);
            seg.b.x=0.5;
            seg.b.y=-0.5+factorYRight;
            output.push_back(translate(seg,x,y));
          }
          break;
        case 2: // { MarchingSquares::v0,MarchingSquares::v2 } -- vertical line
          {
            LineSegment seg;
            const auto factorXTop =(threshold-p00)/(p10-p00); // same as case 1
            seg.a.x=-0.5+factorXTop;
            seg.a.y=-0.5;
            const auto factorXBottom=(threshold-p01)/(p11-p01); // for bottom of square
            seg.b.x=-0.5+factorXBottom;
            seg.b.y=+0.5;
            output.push_back(translate(seg,x,y));
          }
          break;
        case 3: //{ MarchingSquares::v0,MarchingSquares::v3 } from top mid to left mid
          {
            LineSegment seg;
            auto factorXTop =(threshold-p00)/(p10-p00); // same as case 1, top mid
            seg.a.x=-0.5+factorXTop;
            seg.a.y=-0.5;
            auto factorYLeft=(threshold-p00)/(p01-p00); // left-mid
            seg.b.x=-0.5;
            seg.b.y=-0.5+factorYLeft;
            output.push_back(translate(seg,x,y));
          }
          break;
          case 4: //{ MarchingSquares::v1,MarchingSquares::v2 }, from right mid to bottom mid
          {
            LineSegment seg;
            const auto factorYRight=(threshold-p10)/(p11-p10); // right mid
            seg.a.x=0.5;
            seg.a.y=-0.5+factorYRight;
            const auto factorXBottom=(threshold-p01)/(p11-p01); // for bottom of square
            seg.b.x=-0.5+factorXBottom;
            seg.b.y=+0.5;
            output.push_back(translate(seg,x,y));
          }
          break;
        case 5: //{ MarchingSquares::v1,MarchingSquares::v3 }, from right mid to left mid (line)
          {
            LineSegment seg;
            const auto factorYRight=(threshold-p10)/(p11-p10); // right mid
            seg.a.x=0.5;
            seg.a.y=-0.5+factorYRight;
            auto factorYLeft=(threshold-p00)/(p01-p00); // left-mid
            seg.b.x=-0.5;
            seg.b.y=-0.5+factorYLeft;
            output.push_back(translate(seg,x,y));
          }
        break;
        case 6: //{ MarchingSquares::v2,MarchingSquares::v3 }, from bottom mid to left mid
          {
            LineSegment seg;
            const auto factorXBottom=(threshold-p01)/(p11-p01); // for bottom of square
            seg.a.x=-0.5+factorXBottom;
            seg.a.y=+0.5;
            auto factorYLeft=(threshold-p00)/(p01-p00); // left-mid
            seg.b.x=-0.5;
            seg.b.y=-0.5+factorYLeft;
            output.push_back(translate(seg,x,y));
          }
          break;
        default:
          break;
      }
    }
}

// x doesn't shift... does y?

    // distance from v00 : e.g., 0,1: t=0.25, then edge is 0.25 from v00
    // assume v00 < thresh -> v01 > v00, so +-ive
    // assume v00 >= thresh -> v01 > v00, so equiv to (v00-threshold)/v00-v010 +-ive
// assume v00 is > v01, and thresh is midway: then th=(v00+v01)/2
// so (thresh - v00)/(v01-v00) -> (v00-thresh)/(v00-v01)=[v00-(v00+v01)/2]/(v00-v01)=>[v00-v00/2-v01/2]/(v00-v01)=>[v00/2-v01/2]/(v00-v01)=>[v00-v01]/2(v00-v01) = 1/2
// assume t is 0.25v00 + 0.75 v1 => want point at 0.75
// (0.25v00  - v00)/(v01-v00) => (0.75v00)/(v01-v00) => 0.75 v0 / (delta)
// 1-(t-v0)/(v1-v0)
