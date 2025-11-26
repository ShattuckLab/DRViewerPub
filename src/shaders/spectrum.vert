#version 410
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
uniform mat4 matrix;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 v3ColorIn;

out vec4 v4Color;
out vec3 v3uvcoords;

void main()
{
  gl_Position = matrix * vec4(position,1);
  v4Color =  vec4( v3ColorIn.rgb,1);
  v3uvcoords=v3ColorIn;
}
