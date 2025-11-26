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

#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QWidget>
#include <volnd.h>
#include <slicet.h>

class QPaintEvent;
class DRWindow;
class RelaxationToolboxForm;
typedef float float32;

class PlotWidget : public QWidget
{
  Q_OBJECT
public:
  explicit PlotWidget(QWidget *parent = nullptr);
  virtual ~PlotWidget();
  void paintEvent(QPaintEvent *ev) ;
  template <class T>
  void readULine(VolND<T> &p, int x, int y, int z, int u);
  void readULine(VolNBase *p, int x, int y, int z, int u);
  template <class T>
  void readVLine(VolND<T> &p, int x, int y, int z, int v);
  void readVLine(VolNBase *p, int x, int y, int z, int v);
  void readULine(SliceT<float32> &p, int u);
  void readVLine(SliceT<float32> &p, int v);
  bool vertical=false;
  bool plotColor=true;
  DRWindow *drWindow=nullptr;
  RelaxationToolboxForm *relaxationToolboxForm=nullptr;
  bool integrate=false;
  bool darkBackground=false;
private:
  std::vector<float> data;
};

#endif // PLOTWIDGET_H
