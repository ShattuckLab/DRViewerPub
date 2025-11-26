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

#include "drsuitesettings.h"

QString DRSuiteSettings::drSuiteQtKey=PROGNAME;
const QString DRSuiteSettings::organization="DRSuite";
const QString DRSuiteSettings::application=PROGNAME;

void DRSuiteSettings::setKeyValue(std::string key, bool value)
{
  QSettings settings(organization,application);
  settings.setValue(key.c_str(),QVariant(value));
}

void DRSuiteSettings::setKeyValue(std::string key, std::string value)
{
  QSettings settings(organization,application);
  settings.setValue(key.c_str(),value.c_str());
}

QStringList DRSuiteSettings::recentFiles()
{
  QSettings settings(organization,drSuiteQtKey);
  return settings.value("recentFileList").toStringList();
}

void DRSuiteSettings::addRecentFile(const std::string &filename)
{
  if (filename.empty()) return;
  QString filenameQS(filename.c_str());
  QSettings settings(organization,drSuiteQtKey);
  auto files=settings.value("recentFileList").toStringList();
  files.removeAll(filenameQS); // bump latest to top
  files.prepend(filenameQS);
  const int MaxRecentFiles = 8;
  while (files.size() > MaxRecentFiles) files.removeLast();
  settings.setValue("recentFileList", files);
}

void DRSuiteSettings::setMostRecentDirectory(const std::string &filepath)
{
  setKeyValue("mostRecentDirectory",filepath);
}

std::string DRSuiteSettings::mostRecentDirectory()
{
  QSettings settings(organization,drSuiteQtKey);
  return settings.value("mostRecentDirectory").toString().toStdString();
}
