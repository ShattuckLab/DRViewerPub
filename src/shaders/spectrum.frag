#version 410
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
uniform mat4 matrix;
uniform sampler3D vol1texf;
uniform sampler1D lut1tex;
uniform sampler3D spectrumtex;
uniform sampler1D spectrum_luttex;
uniform sampler3D component_tex;
uniform sampler1D component_luttex;
uniform sampler3D vol2texf;
uniform sampler1D lut2tex;
uniform sampler3D vol3texf;
uniform sampler1D lut3tex;
uniform sampler3D vol4texf;
uniform sampler1D lut4tex;
uniform sampler3D vol5texf;
uniform sampler1D lut5tex;

uniform sampler3D masktex;

uniform ivec3 voxeldimensions;
uniform vec2 spectraldimensions;
uniform vec2 spectralcoord;
uniform int ncomponents;
uniform int showComponent;
uniform int showComposite;

uniform float spotlightRadius;
uniform int showSpectra;
uniform int showUPlot;
uniform int showVPlot;
uniform float gridwidth;
uniform float scale1Min;
uniform float scale1Max;
uniform float scale2Min;
uniform float scale2Max;
uniform vec3 basecolor;
uniform int useBasecolor;
uniform vec4 componentWeights;

in vec3 v3uvcoords;
in vec3 opos;
out vec4 outputColor;

void main()
{
  vec4 gridcolor=vec4(1,1,1,0);
  outputColor=vec4(v3uvcoords,1);
  int dx=voxeldimensions.x;
  int dy=voxeldimensions.y;
  int dz=1;// fixed at one slice for now
  vec3 vc = v3uvcoords;
  float x = vc.x*dx;
  float y = vc.y*dy;
  vc.x=(x-floor(x)); // fractional part is u,v
  vc.y=(y-floor(y));
  if (vc.x<gridwidth||vc.y<gridwidth||(vc.x>1-gridwidth)||(vc.y>1-gridwidth))
  {
    outputColor=gridcolor;
    return;
  }
  if (showComposite>0)
  {
      float oc1=0,oc2=0,oc3=0,oc4=0;
      float r1=texture(vol2texf, v3uvcoords).r;
      if (r1>scale1Min) oc1=(r1-scale1Min)/(scale1Max-scale1Min);
      float r2=texture(vol3texf, v3uvcoords).r;
      if (r2>scale1Min) oc2=(r2-scale1Min)/(scale1Max-scale1Min);
      float r3=texture(vol4texf, v3uvcoords).r;
      if (r3>scale1Min) oc3=(r3-scale1Min)/(scale1Max-scale1Min);
      float r4=texture(vol5texf, v3uvcoords).r;
      if (r4>scale1Min) oc4=(r4-scale1Min)/(scale1Max-scale1Min);
      outputColor=vec4(oc1*componentWeights.x*vec3(1,0,0)+
                         oc2*componentWeights.y*vec3(0.2118,0.3176,1.0)+
                         oc3*componentWeights.z*vec3(0,1,0)+
                         oc4*componentWeights.w*vec3(0.6,0.6,1),1);
      return;
  }
  float depth=floor(y)*dx+floor(x);
  vc.z=depth/(dx*dy);
  if (true)
  {
    float oc=0;
    float r=0;
    switch (showComponent)
    {
        case 0: r=texture(vol2texf, v3uvcoords).r; break;
        case 1: r=texture(vol3texf, v3uvcoords).r; break;
        case 2: r=texture(vol4texf, v3uvcoords).r; break;
        case 3: r=texture(vol5texf, v3uvcoords).r; break;
        default: r=texture(vol1texf, v3uvcoords).r; break;
    }
    if (r>scale1Min) oc=(r-scale1Min)/(scale1Max-scale1Min);
    if (useBasecolor>0)
      outputColor=vec4(oc*basecolor,1);
    else
      outputColor=vec4(texture(lut1tex,oc).rgb,1);
  }
  {
    float spotlightRadiusNormalized=spotlightRadius/spectraldimensions.x;
    if (showSpectra>0)
    {
      if (spotlightRadius>0)
      {
        vec2 delta=spectralcoord-vc.xy*spectraldimensions;
        bool spotlight=(dot(delta,delta)<spotlightRadius*spotlightRadius);
        if (!spotlight) return;
      }
      float r=texture(spectrumtex, vc).r;
      if (r<=scale1Min) r=scale1Min;
      float i=(r-scale2Min)/(scale2Max-scale2Min);
      if (showComponent>=0)
      {
        float nc=showComponent/float(ncomponents);
        float maskvalue=texture(masktex, vec3(vc.xy,nc)).r/255;
        if (maskvalue>0 && i>0.01)
        {
          outputColor=mix(vec4(texture(lut2tex,i).rgb,1),outputColor,0);
        }
      }
      else if (i>0.01)
        outputColor=mix(vec4(texture(lut2tex,i).rgb,1),outputColor,0);
    }
    if (showVPlot>0)
    {
      vec3 vcU=vc;
      vcU.x=spectralcoord.x/spectraldimensions.x; // keep spect coord fixed!
      float r=texture(spectrumtex, vcU).r;
      if (r<=scale2Min) r=scale2Min;
      float i=(r-scale2Min)/(scale2Max-scale2Min);
      if (spotlightRadius>0)
      {
      }
      if (i>=vc.x)
      {
        outputColor=mix(vec4(texture(lut2tex,i).rgb,1),outputColor,0);
      }
    }
    if (showUPlot>0)
    {
      vec3 vcU=vc;
      vcU.y=spectralcoord.y/spectraldimensions.y; // keep spect coord fixed!
      float r=texture(spectrumtex, vcU).r;
      if (r<=scale2Min) r=scale2Min;
      float i=(r-scale2Min)/(scale2Max-scale2Min);
      if (i>=vc.y)
      {
        outputColor=mix(vec4(texture(lut2tex,i).rgb,1),outputColor,0);
      }
    }
  }
}
