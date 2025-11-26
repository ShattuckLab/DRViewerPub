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

#include <axisinfo.h>
#include <nlohmann/json.hpp>
#include <QDebug>

std::vector<AxisInfo> AxisInfo::parse(nlohmann::json &axes)
{
  std::map<std::string,DimType> typemap;
  typemap["spatial"]=DimType::Spatial;
  typemap["spectral"]=DimType::Spectral;
  std::vector<AxisInfo> axisInfo;
  if (axes.contains("type"))
  {
    auto axistypes=axes["type"].get<std::vector<std::string>>();
    qDebug()<<"types: "<<axistypes;
    axisInfo.resize(axistypes.size());
    for (size_t i=0;i<axisInfo.size();i++) axisInfo[i].dimtype=typemap[axistypes[i]];
  }
  if (axes.contains("name"))
  {
    auto names=axes["name"].get<std::vector<std::string>>();
    qDebug()<<"names: "<<names;
    axisInfo.resize(names.size());
    for (size_t i=0;i<names.size();i++) axisInfo[i].name=names[i];
  }
  if (axes.contains("unit"))
  {
    auto units=axes["unit"].get<std::vector<std::string>>();
    qDebug()<<"units: "<<units;
    axisInfo.resize(units.size());
    for (size_t i=0;i<units.size();i++) axisInfo[i].units=units[i];
  }
  if (axes.contains("units"))
  {
    auto units=axes["units"].get<std::vector<std::string>>();
    qDebug()<<"units: "<<units;
    axisInfo.resize(units.size());
    for (size_t i=0;i<units.size();i++) axisInfo[i].units=units[i];
  }
  if (axes.contains("sample"))
  {
    auto axisSamples=axes["sample"].get<std::vector<std::vector<float>>>();
    axisInfo.resize(axisSamples.size());
    for (size_t i=0;i<axisSamples.size();i++)
    {
      axisInfo[i].samples=axisSamples[i];
    }
  }
  if (axes.contains("samples"))
  {
    auto axisSamples=axes["samples"].get<std::vector<std::vector<float>>>();
    axisInfo.resize(axisSamples.size());
    for (size_t i=0;i<axisSamples.size();i++)
    {
      axisInfo[i].samples=axisSamples[i];
    }
  }
  return axisInfo;
}
