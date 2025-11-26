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

#include "verticaltextlabel.h"
#include <QPainter>

// see: https://stackoverflow.com/questions/9183050/vertical-qlabel-or-the-equivalent

VerticalTextLabel::VerticalTextLabel(QWidget *parent)
    : QLabel{parent}
{}

void VerticalTextLabel::paintEvent(QPaintEvent *)
{
  QPainter painter(this);
  auto textSize=sizeHint();
  auto r=rect();
  auto y=(r.height()-textSize.height())/2;
  painter.translate(rect().width(),rect().height());
  painter.rotate(-90);
  painter.drawText(y,0, text());
}

QSize VerticalTextLabel::minimumSizeHint() const
{
  auto s = QLabel::minimumSizeHint();
  return QSize(s.height(), s.width());
}

QSize VerticalTextLabel::sizeHint() const
{
  auto s = QLabel::sizeHint();
  return QSize(s.height(), s.width());
}
