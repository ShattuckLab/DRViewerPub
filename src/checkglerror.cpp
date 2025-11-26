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

#include <checkglerror.h>
#include <QOpenGLFunctions>
#include <qdebug.h>

std::string glerrstring(GLenum code)
{
  switch (code)
  {
    case 0x0500: return std::string("GL_INVALID_ENUM");
    case 0x0501: return "GL_INVALID_VALUE";
    case 0x0502: return "GL_INVALID_OPERATION";
    case 0x0503: return "GL_STACK_OVERFLOW";
    case 0x0504: return "GL_STACK_UNDERFLOW";
    case 0x0505: return "GL_OUT_OF_MEMORY";
    case 0x0506: return "GL_INVALID_FRAMEBUFFER_OPERATION";
  }
  return "unk";
}

void checkGLErrors(std::string file, std::string function, int line)
{
  GLenum err=0;
  while((err = glGetError()) != GL_NO_ERROR)
  {
    qDebug()<<"GL ERROR in "<<file<<" "<<function.c_str()<<" : "<<line<<" : "<<err<<" "<<glerrstring(err).c_str();
  }
}
