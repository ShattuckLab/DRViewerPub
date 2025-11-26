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


#ifndef DSSHADERPROGRAM_H
#define DSSHADERPROGRAM_H

#include <QOpenGLShaderProgram>
#include <string>
#include <memory>

class DSShaderProgram : public QOpenGLShaderProgram {
public:
  static bool loadShader(std::unique_ptr<DSShaderProgram> &program, std::string vshader, std::string fshader, std::string gshader);
  std::string vertfname;
  std::string geomfname;
  std::string fragfname;
};


#endif // DSSHADERPROGRAM_H
