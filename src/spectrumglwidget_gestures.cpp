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

#include <QtOpenGL>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QGestureEvent>
#include <glm/glm.hpp>
#include "spectrumglwidget.h"
#include <drwindow.h>

bool SpectrumGLWidget::event(QEvent *event)
{
  switch (event->type())
  {
    case QEvent::NativeGesture:
    {
      auto gesture = dynamic_cast<QNativeGestureEvent*>(event);
      if (gesture)
      {
        switch (gesture->gestureType())
        {
          case Qt::ZoomNativeGesture:
            viewController.zoom *= float(1+gesture->value());
            drWindow->syncWindows(this);
            update();
            break;
          case Qt::SmartZoomNativeGesture:
            if (gesture->value())
              viewController.zoom *= 0.5f;
            else
              viewController.zoom /= 0.5f;
            drWindow->syncWindows(this);
            update();
            break;
            break;
          default:
            break;
        }
      }
    }
    break;
    default:
      break;
  }
  return QOpenGLWidget::event(event);
}
