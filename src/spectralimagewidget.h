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

#ifndef SPECTRALIMAGEWIDGET_H
#define SPECTRALIMAGEWIDGET_H

#include <QWidget>
#include <dsimage.h>
#include <uvpoint.h>
#include <QOpenGLWidget>

class RelaxationToolboxForm;
class DRWindow;

class SpectralImageWidget : public QWidget
{
public:
  explicit SpectralImageWidget(QWidget *parent = nullptr);
  ~SpectralImageWidget() {}
  DRWindow *drWindow=nullptr;
  QPoint yaxisTick(int y) const;
  QPoint xaxisTick(int x) const;
  UVPoint mapToSpectrum(const QPoint &clickPoint) const;
  void updateDisplay();
  void mouseMoveEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent * event) override;
  void paintEvent(QPaintEvent *ev) override;
  void keyPressEvent(QKeyEvent *event) override;
  void keyReleaseEvent(QKeyEvent *event) override;
  QPoint spectrumToScreenCoordinates(const UVPoint &p) const;
  template <class P>
  QPoint screenPosition(const P &p) const;
  RelaxationToolboxForm *relaxationToolboxForm=nullptr;
  DSImage spectrumBitmap;
  DSImage componentBitmap,componentOutlineBitmap;
  DSImage labelBitmap;
  UVPoint spectralPosition;
  QPoint origin;
  bool tracking=false;
  bool plotContours=true;
  bool plotSpectrumImage=true;
  bool plotComponent=false;
  bool plotComposite=false;
  bool darkBackground=false;
  bool useBasecolor=false;
};

#endif // SPECTRALIMAGEWIDGET_H
