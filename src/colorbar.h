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

#ifndef COLORBAR_H
#define COLORBAR_H

#include <QWidget>
#include <dsimage.h>
#include <silttypes.h>
#include <imagestate.h>
class DRWindow;

class ColorBar : public QWidget
{
//  Q_OBJECT
  public:
  explicit ColorBar(QWidget *parent = nullptr);
  virtual ~ColorBar();
  QColor mapValue(const float tick);
  void setImageScale(const float scale) { imageScale=scale; }
  void setLUT(uint32_t *v);
  void paintEvent(QPaintEvent *ev) override;
  void paintRegion(QPainter &painter, QRect region, float minval, float maxval, int maxwidth=20);
  std::vector<float> customTicks;
  bool showCustomTicks=false;
  bool showContourTickLabels=true;
  DRWindow *drWindow=nullptr;
  uint32_t lut[256];
  Qt::AlignmentFlag align=Qt::AlignLeft;
  int maxbarwidth=20;
  bool showTicks=false;
  private:
  float imageScale=1.0f;
  DSImage lutColor;

  signals:
};

#endif // COLORBAR_H
