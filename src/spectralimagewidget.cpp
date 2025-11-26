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

#include "spectralimagewidget.h"
#include "drwindow.h"
#include <QMouseEvent>
#include <relaxationtoolboxform.h>
#include <QPainter>
#include <marchingsquares.h>
#include <colorops.h>

SpectralImageWidget::SpectralImageWidget(QWidget *parent) : QWidget(parent)
{
  spectrumBitmap.resize(100,100);
  const QRgb grey(0x202020);
  auto *pixel=spectrumBitmap();
  std::fill(pixel,pixel+100*100,grey);
}

void SpectralImageWidget::mousePressEvent(QMouseEvent *event)
{
  if (event->buttons() & Qt::RightButton)
  {
    drWindow->makeSpectralPopupMenu(event->globalPosition().toPoint());
  }
  else
  {
    origin=event->pos();
    tracking=true; grabKeyboard();
    mouseMoveEvent(event);
  }
}

void SpectralImageWidget::mouseReleaseEvent(QMouseEvent * /*event*/)
{
  tracking=false;
  releaseKeyboard();  if (drWindow) drWindow->updateViews();
}

UVPoint SpectralImageWidget::mapToSpectrum(const QPoint &clickPoint) const
{
  auto w=this->width();
  auto h=this->height();
  return UVPoint((float(clickPoint.x())/w),(float(clickPoint.y())/h));
}

void SpectralImageWidget::mouseMoveEvent(QMouseEvent *event)
{
  if (!tracking ) return;
  auto clickpoint = event->pos();
  auto w=this->width();
  auto h=this->height();
  if (event->modifiers() & Qt::ShiftModifier)
  {
    auto delta=origin-clickpoint;
    if (std::abs(delta.x())>std::abs(delta.y())) clickpoint.setY(origin.y());
    else clickpoint.setX(origin.x());
  }

  auto x=(float(clickpoint.x())/w);
  auto y=1-(float(clickpoint.y())/h);
  update();
  if (relaxationToolboxForm) relaxationToolboxForm->selectSpectralPointF(x,y);
}

void SpectralImageWidget::updateDisplay()
{
  update();
}

void SpectralImageWidget::keyPressEvent(QKeyEvent */*event*/)
{
}

void SpectralImageWidget::keyReleaseEvent(QKeyEvent */*event*/)
{
}

QPoint SpectralImageWidget::spectrumToScreenCoordinates(const UVPoint &pos) const
{
  return QPoint((pos.u+0.5)*width()/float(spectrumBitmap.image.width()),
                height()-(pos.v+0.5)*height()/float(spectrumBitmap.image.height()));
}

template <class P>
QPoint SpectralImageWidget::screenPosition(const P &pos) const
{
  return QPoint((pos.x)*width()/float(spectrumBitmap.image.width()),
                height()-(pos.y)*height()/float(spectrumBitmap.image.height()));
}

void SpectralImageWidget::paintEvent(QPaintEvent */*ev*/)
{
  QPainter painter(this);
  auto pos=relaxationToolboxForm->position();
  const auto scaled=spectrumToScreenCoordinates(pos);
  painter.fillRect(rect(), darkBackground ? Qt::black : Qt::white);

  if (plotSpectrumImage)
  {
    DSImage blendBitmap;
    blendBitmap.resize(spectrumBitmap.size().width(),spectrumBitmap.size().height());
    QPainter blendPainter(&blendBitmap.image);
    blendPainter.drawImage(blendBitmap.image.rect(),spectrumBitmap.image);
    if (plotComponent && componentOutlineBitmap.npix()>0)
    {
      componentBitmap.setAlpha(255);
      blendPainter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
      blendPainter.drawImage(blendBitmap.image.rect(),componentOutlineBitmap.image);
      blendPainter.setCompositionMode(QPainter::CompositionMode_Xor);
      blendPainter.drawImage(blendBitmap.image.rect(),componentOutlineBitmap.image);
    }
    // TODO enable/disable with setting
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.drawImage(rect(),blendBitmap.image);
  }
  else
  {
    painter.drawImage(rect(),componentOutlineBitmap.image);
  }

  auto &contourLevels=relaxationToolboxForm->contourLevels;

  if (drWindow->imageState.showCursor)
  {
    painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
    QPen pen(Qt::white, 0.5, Qt::DashLine);
    painter.setPen(pen);
    int x0=0;
    int y0=0;
    int y1=height();
    int x1=width();
    painter.drawLine(QPoint(scaled.x(),y0),QPoint(scaled.x(),y1));
    painter.drawLine(QPoint(x0,scaled.y()),QPoint(x1,scaled.y()));
    const auto radius=relaxationToolboxForm->spotlightRadius();
    if (radius>0)
    {
      const auto UR=spectrumToScreenCoordinates(UVPoint(pos.u-radius,pos.v-radius));
      const auto LR=spectrumToScreenCoordinates(UVPoint(pos.u+radius,pos.v+radius));
      painter.drawEllipse(QRect(UR,LR));
    }
  }
  if (plotContours && contourLevels.size())
  {
    painter.setCompositionMode(plotSpectrumImage ? QPainter::RasterOp_SourceXorDestination : QPainter::CompositionMode_SourceOver);
    QPen pen(Qt::white, 0.5, Qt::SolidLine);
    painter.setPen(pen);
    for (auto &level : contourLevels)
    {
      auto scale=drWindow->imageState.overlay1Brightness;
      scale = drWindow->spectrumScale();
      RangeLUTOp operation(drWindow->spectrumLUT(),0,scale);
      auto contourColor=QColor(operation.lut[std::clamp<uint8_t>(level.value*255/drWindow->spectrumScale(),0,255)]);
      painter.setPen(contourColor);
      for (auto &edge : level.contours)
      {
        painter.drawLine(screenPosition(edge.a),screenPosition(edge.b));
      }
    }
  }
}

QPoint SpectralImageWidget::yaxisTick(int y) const
{
  return spectrumToScreenCoordinates(UVPoint(0,y));
}

QPoint SpectralImageWidget::xaxisTick(int x) const
{
  return spectrumToScreenCoordinates(UVPoint(x,0));
}

