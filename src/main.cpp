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

#include "drwindow.h"

#include <QApplication>
#include <QSurfaceFormat>
#include <QDebug>
#include <QOpenGLContext>

int main(int argc, char *argv[])
{
  std::cout<<"Welcome to Diffusion-Relaxation Suite Viewer (DRViewer)\n"
             "Copyright (C) 2025 The Regents of the University of California\n"
             "This program is free software and is released under a GNU Lesser\n"
             "General Public License, version 2.1 (LGPLv2.1-only). For more\n"
             "details, please see the enclosed license files.\n"
             "DRViewer uses the Qt GUI Toolkit 6.9.2 under the LGPL v3.0 license.\n"
             "The Qt Toolkit is Copyright (C) 2018 The Qt Company Ltd. and other\n"
             "contributors.\n";
  QSurfaceFormat surfaceFormat = QSurfaceFormat::defaultFormat();
  surfaceFormat.setSamples(0); // for QOpenGLWidget, this is required to be zero to enable the depth buffer to be sampled
  surfaceFormat.setDepthBufferSize(24);
  surfaceFormat.setVersion(4, 1);
  surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
  QSurfaceFormat::setDefaultFormat(surfaceFormat);
  QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
  QApplication::setAttribute(Qt::AA_DontShowIconsInMenus, true);
  QApplication a(argc, argv);
  DRWindow w;
  w.show();
  return a.exec();
}
