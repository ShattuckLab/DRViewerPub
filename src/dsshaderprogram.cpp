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

#include <dsshaderprogram.h>

bool DSShaderProgram::loadShader(std::unique_ptr<DSShaderProgram> &program, std::string vshader, std::string fshader, std::string gshader)
{
  bool debug=false;
  if (!program) program = std::make_unique<DSShaderProgram>();
  program->removeAllShaders();
  if (!program->addShaderFromSourceFile(QOpenGLShader::Vertex, vshader.c_str()))
  {
    qDebug()<<"error with vshader: "<<vshader.c_str();
    program->vertfname=vshader+" : error";
    return false;
  }
  if (debug) qDebug()<<"compiled vshader "<<vshader.c_str();
  program->vertfname=vshader;
  if (!program->addShaderFromSourceFile(QOpenGLShader::Fragment,fshader.c_str()))
  {
    qDebug()<<"error with fshader: "<<fshader.c_str();
    program->fragfname=fshader+" : error";
    return false;
  }
  if (debug) qDebug()<<"compiled fshader "<<fshader.c_str();
  program->fragfname=fshader;
  program->geomfname="";
  if (gshader.empty()==false)
  {
    if (!program->addShaderFromSourceFile(QOpenGLShader::Geometry,gshader.c_str()))
    {
      qDebug()<<"error with gshader: "<<gshader.c_str();
      program->geomfname=gshader+" : error";
      return false;
    }
    else
    {
      if (debug) qDebug()<<"compiled gshader "<<gshader.c_str();
    }
    program->geomfname=gshader;
  }
  if (!program->link())
    qDebug()<<"error with link "<<vshader.c_str()<<" "<<fshader.c_str()<<" "<<gshader.c_str();
  if (!program->bind())
    qDebug()<<"error with bind "<<vshader.c_str()<<" "<<fshader.c_str()<<" "<<gshader.c_str();
  return true;
}
