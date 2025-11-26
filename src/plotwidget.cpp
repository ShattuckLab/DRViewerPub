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

#include <plotwidget.h>
#include <QPainter>
#include <relaxationtoolboxform.h>
#include <drwindow.h>
#include <colorops.h>
#include <cstdint>
#include <colormap.h>

PlotWidget::PlotWidget(QWidget *parent) : QWidget(parent)
{
}

PlotWidget::~PlotWidget()
{
}

void PlotWidget::readULine(SliceT<float32> &p, int u)
{
  data.resize(p.cy);
  int v=0;
  for (auto &d : data) d=p(u,v++);
}

void PlotWidget::readVLine(SliceT<float32> &p, int v)
{
  data.resize(p.cx);
  int u=0;
  for (auto &d : data) d=p(u++,v);
}


template <class T>
void PlotWidget::readULine(VolND<T> &p, int x, int y, int z, int u)
{
  if (!integrate)
  {
    data.resize(p.nv());
    int v=0;
    for (auto &d : data) d=p(x,y,z,u,v++);
  }
  else
  {
    const auto nv=p.nv();
    const auto nu=p.nu();
    data.resize(p.nv());
    std::fill(data.begin(),data.end(),0);
    for (size_t v=0;v<nv;v++)
      for (size_t u=0;u<nu;u++)
        data[v]+=p(x,y,z,u,v);
  }
}

void PlotWidget::readULine(VolNBase *p, int x, int y, int z, int u)
{
  if (!p) return;
  switch (p->typeID())
  {
  case SILT::Float32 : readULine((*static_cast<VolND<float32> *>(p)),x,y,z,u); break;
  case SILT::Float64 : readULine((*static_cast<VolND<float64> *>(p)),x,y,z,u); break;
  case SILT::Sint16  : readULine((*static_cast<VolND<sint16>  *>(p)),x,y,z,u); break;
  case SILT::Uint16  : readULine((*static_cast<VolND<uint16_t>  *>(p)),x,y,z,u); break;
    default: data.resize(p->nv()); std::fill(data.begin(),data.end(),0);
  }
}

template <class T>
void PlotWidget::readVLine(VolND<T> &p, int x, int y, int z, int v)
{
  if (!integrate)
  {
    data.resize(p.nu());
    int u=0;
    for (auto &d : data) d=p(x,y,z,u++,v);
  }
  else
  {
    const auto nv=p.nv();
    const auto nu=p.nu();
    data.resize(nu);
    std::fill(data.begin(),data.end(),0);
    for (size_t v=0;v<nv;v++)
      for (size_t u=0;u<nu;u++)
        data[u]+=p(x,y,z,u,v);
    // for (auto &v : data) v/=nt;
  }
}

void PlotWidget::readVLine(VolNBase *p, int x, int y, int z, int v)
{
  if (!p) return;
  switch (p->typeID())
  {
    case SILT::Float32 : readVLine((*static_cast<VolND<float32> *>(p)),x,y,z,v); break;
    case SILT::Float64 : readVLine((*static_cast<VolND<float64> *>(p)),x,y,z,v); break;
    case SILT::Sint16  : readVLine((*static_cast<VolND<sint16>  *>(p)),x,y,z,v); break;
    case SILT::Uint16  : readVLine((*static_cast<VolND<uint16_t>  *>(p)),x,y,z,v); break;
    default: data.resize(p->nv()); std::fill(data.begin(),data.end(),0);
  }
}

//TODO: switch to bitmaps
void PlotWidget::paintEvent(QPaintEvent */*ev*/)
{
  if (drWindow && drWindow->spectrum() && relaxationToolboxForm)
  {
    auto *lut=drWindow->spectrumLUT();
    const auto scale = drWindow->spectrumScale();
    RangeLUTOp operation(lut,0,scale);
    const auto uv=relaxationToolboxForm->position();
    auto nu=drWindow->spectrum()->nu();
    auto nv=drWindow->spectrum()->nv();
    QPainter painter(this);
    painter.fillRect(rect(),darkBackground ? Qt::black : Qt::white);
    const QColor lineColor(darkBackground ? Qt::white : Qt::black);
    auto scaled0=(0-0)/(scale-0); // baseline
    const auto w=rect().width();
    const auto h=rect().height();
    if (vertical)
    {
      const float vscale=h/float(nv);
      readULine(relaxationToolboxForm->spectralSlice,uv.u);
      const auto x0=w*scaled0;
      if (plotColor)
        for (size_t i=0;i<nv;i++)
        {
          auto scaled=std::clamp((data[nv-i-1]-0)/(scale-0),0.0f,1.0f);
          painter.fillRect(QRect(QPoint(x0,vscale*(i)),QPoint(w*scaled,vscale*(i+1.0f))),QColor(operation.lut[std::clamp<uint8_t>(scaled*255,0,255)]));
        }
      else
        for (size_t i=0;i<nv;i++)
        {
          auto scaled=std::clamp((data[nv-i-1]-0)/(scale-0),0.0f,1.0f);
          painter.fillRect(QRect(QPoint(x0,vscale*(i)),QPoint(w*scaled,vscale*(i+1.0f))),lineColor);
        }
      QPen pen(Qt::black, 0.5, Qt::DashLine);
      pen.setDashOffset(1);
      painter.setPen(pen);
      painter.drawLine(QPoint(x0,0),QPoint(x0,h));
    }
    else
    {
      const float hscale=(w/float(nu));
      readVLine(relaxationToolboxForm->spectralSlice,uv.v);//,pos.x,pos.y,pos.z,uv.v);
      const auto y0=rect().height()*(1-scaled0);
      if (plotColor)
        for (size_t i=0;i<nu;i++)
        {
          const auto scaled=std::clamp((data[i]-0)/(scale-0),0.0f,1.0f);
          painter.fillRect(QRect(QPoint(hscale*(i),y0),QPoint(hscale*(i+1.0f),h*(1-scaled))),QColor(operation.lut[std::clamp<uint8_t>(scaled*255,0,255)]));
        }
      else
        for (size_t i=0;i<nu;i++)
        {
          const auto scaled=std::clamp((data[i]-0)/(scale-0),0.0f,1.0f);
          painter.fillRect(QRect(QPoint(hscale*(i),y0),QPoint(hscale*(i+1.0f),h*(1-scaled))),lineColor);
        }
      QPen pen(Qt::black, 0.5, Qt::DashLine);
      painter.setPen(pen);
      painter.drawLine(QPoint(0,y0),QPoint(w,y0));
    }
  }
}
