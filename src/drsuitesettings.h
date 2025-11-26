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

#ifndef DRSUITESETTINGS_H
#define DRSUITESETTINGS_H

#include <string>
#include <QString>
#include <QSettings>

class DRSuiteSettings {
public:
    static void initialize();
    static std::string qtMostRecentDirectoryKey;
    static QString drSuiteQtKey;
    static void addRecentFile(const std::string &filename);
    static void setMostRecentDirectory(const std::string &filepath);
    static std::string mostRecentDirectory();
    static QStringList recentFiles();
private:
    static void setKeyValue(std::string key, bool value);
    static void setKeyValue(std::string key, std::string value);
    static std::string currentDirectory;
    static const QString organization;
    static const QString application;
};

#endif // DRSUITESETTINGS_H
