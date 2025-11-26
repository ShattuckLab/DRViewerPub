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

#include <colorbar.h>
#include <QPainter>
#include <drwindow.h>
#include "colorops.h"

ColorBar::ColorBar(QWidget *parent)
    : QWidget{parent}
{
  lutColor.resize(1,256);
  setLUT(ColorMap::jetLUT);
}

ColorBar::~ColorBar()
{
}

void ColorBar::setLUT(uint32_t *LUT)
{
  QRgb* pixel = lutColor.pixels();
  for (int i=0;i<256;i++)
  {
    lut[i]=LUT[i];
    pixel[255-i] = LUT[i];
  }
}

QColor ColorBar::mapValue(const float tick)
{
  auto scaled=std::clamp((tick-0)/(imageScale-0),0.0f,1.0f);
  return QColor(lut[std::clamp<uint8_t>(scaled*255,0,255)]);
}

void ColorBar::paintRegion(QPainter &painter, QRect region, float minval, float maxval, int maxwidth)
{
  int tickwidth=10;
  const auto maxString=QString::number(maxval,'g',4);
  const auto maxStringFM=painter.fontMetrics().boundingRect(maxString);
  const auto minString=QString::number(minval,'g',4);
  const auto minStringFM=painter.fontMetrics().boundingRect(minString);
  const auto offset=std::max(maxStringFM.height(),minStringFM.height())/2;
  const auto ascent=painter.fontMetrics().ascent();
  auto cbarWidth=(maxwidth>0 && maxwidth < region.width()) ? maxwidth : region.width();
  auto colorRect(region);
  const auto topText=region.top()+maxStringFM.height()+3;
  const auto botText=region.bottom()-minStringFM.height()-3;
  int tickLeft=region.left();
  int tickRight=region.right();
  if (align & Qt::AlignLeft)
  {
    colorRect=QRect(QPoint(region.left(),topText),
                    QPoint(region.left()+cbarWidth,botText));
    painter.setPen(showCustomTicks ? mapValue(imageScale) : Qt::white);
    painter.drawText(region.left(),region.top()+maxStringFM.height(),maxString);
    painter.setPen(showCustomTicks ? mapValue(0) : Qt::white);
    painter.drawText(region.left(),region.bottom(),minString);
    tickLeft=colorRect.right();
    tickRight=tickLeft+tickwidth;
  }
  else if (align & Qt::AlignRight)
  {
    colorRect=QRect(QPoint(region.right()-cbarWidth,topText),
                    QPoint(region.right(),botText));
    painter.setPen(showCustomTicks ? mapValue(imageScale) : Qt::white);
    painter.drawText(region.right()-maxStringFM.width(),region.top()+maxStringFM.height(),maxString);
    painter.setPen(showCustomTicks ? mapValue(0) : Qt::white);
    painter.drawText(region.right()-minStringFM.width(),region.bottom(),minString);
    tickRight=colorRect.left();
    tickLeft=tickRight-tickwidth;
  }
  else if (align & Qt::AlignCenter||align & Qt::AlignHCenter)
  {
    int midpoint=(region.left()+region.right())/2;
    colorRect=QRect(QPoint(midpoint-cbarWidth/2,topText),
                    QPoint(midpoint+cbarWidth/2,botText));
    painter.setPen(showCustomTicks ? mapValue(imageScale) : Qt::white);
    painter.drawText(midpoint-maxStringFM.width()/2,region.top()+maxStringFM.height(),maxString);
    painter.setPen(showCustomTicks ? mapValue(0) : Qt::white);
    painter.drawText(midpoint-minStringFM.width()/2,region.bottom(),minString);
    tickLeft=colorRect.left()-tickwidth/2;
    tickRight=colorRect.right()+tickwidth/2;
  }
  const auto bot=colorRect.bottom()+1;
  const auto top=colorRect.top();
  const float zero=(1-(0-0)/(imageScale-0))*colorRect.height()+offset;
  if (0<0 && zero>(top+ascent/2-1+offset) && zero<(colorRect.height()-offset))
  {
    painter.setPen(showCustomTicks ? mapValue(0) : Qt::white);
    painter.drawText(tickwidth,zero+offset-3,minString);
  }
  painter.drawImage(colorRect,lutColor.image);
  if (showTicks)
  {
    painter.setPen(Qt::white);
    painter.setPen(Qt::white);
    painter.drawLine(tickLeft,bot,tickRight,bot);
    painter.drawLine(tickLeft,top,tickRight,top);
    painter.drawRect(colorRect);
  }
}

void ColorBar::paintEvent(QPaintEvent */*ev*/)
{
  QPainter painter(this);
  paintRegion(painter,rect(),0,imageScale);
  return;
}
