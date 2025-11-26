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

#include <drwindow.h>
#include "ui_drwindow.h"
#include <colormap.h>
#include <drsuitesettings.h>
#include <spectrumglwidget.h>
#include <relaxationtoolboxform.h>
#include <QFileInfo>
#include <QMenu>

Q_DECLARE_METATYPE(LUTPtr)

QAction *DRWindow::inMenu(QMenu *menu, std::string item)
{
  foreach (QAction *action, menu->actions())
    if (!action->isSeparator() && !action->menu() &&action->text()==item.c_str())
      return action;
  return nullptr;
}

void DRWindow::makeSpectralPopupMenu(QPoint point)
{
  contextMenu->popup(point);
}

bool DRWindow::loadCustomLUT(std::string lutFilename)
{
  if (!lutList.loadCustomLUT(lutFilename,true)) return false;
  std::string lutName=StrUtil::getFilename(lutFilename);
  auto lut=lutList.map[lutName];
  connectImageLUTMenuItem(ui->menu_ImageLUT,lutName,lut);
  connectSpectrumLUTMenuItem(ui->menu_SpectrumLUT,lutName,lut);
  return true;
}

bool DRWindow::changeLUT(uint32_t *&LUTptr, const uint32_t *newLUT)
{
  if (!newLUT) return false;
  if (!LUTptr) return false;
  for (auto &p : lutList.map)
  {
    if (p.second == newLUT)
    {
      LUTptr=p.second;
      return true;
    }
  }
  return false;
}

void DRWindow::changeImageLUT()
{
  QAction *selectedAction = qobject_cast<QAction *>(sender());
  if (selectedAction)
  {
    auto newLUT=selectedAction->data().value<LUTPtr>();
    if (changeLUT(imLUT,newLUT))
    {
      for (auto *w : spectrumWindows)
      {
        w->updateImageColorbar();
      }
      reloadSSOs();
      refresh();
      foreach (QAction *action, ui->menu_ImageLUT->actions())
        if (action != selectedAction && !action->isSeparator() && !action->menu())
           action->setChecked(false);
    }
  }
}

void DRWindow::changeSpectrumLUT()
{
  QAction *selectedAction = qobject_cast<QAction *>(sender());
  if (selectedAction)
  {
    auto newLUT=selectedAction->data().value<LUTPtr>();
    if (changeLUT(spectLUT,newLUT))
    {
      for (auto *w : spectrumWindows)
      {
        w->updateImageColorbar();
        w->updateSpectrumColorbar();
        w->update();
      }
      reloadSSOs();
      refresh();
      relaxationToolboxForm->updateColorbar();
      relaxationToolboxForm->updatePosition();
      foreach (QAction *action, ui->menu_SpectrumLUT->actions())
        if (action != selectedAction && !action->isSeparator() && !action->menu())
          action->setChecked(false);
    }
  }
}

void DRWindow::connectImageLUTMenuItem(QMenu *menu, std::string lutName, const LUTPtr lut)
{
  QAction *action = inMenu(menu,lutName);
  if (!action)
  {
    action = menu->addAction(lutName.c_str());
    if (!action) return;
    connect(action, SIGNAL(triggered()),this, SLOT(changeImageLUT()));
  }
  QVariant var;
  var.setValue(lut);
  action->setData(var);
  action->setCheckable(true);
  action->setChecked(imLUT == lut);
}

void DRWindow::connectSpectrumLUTMenuItem(QMenu *menu, std::string lutName, const LUTPtr lut)
{
  QAction *action = inMenu(menu,lutName);
  if (!action)
  {
    action = menu->addAction(lutName.c_str());
    if (!action) return;
    connect(action, SIGNAL(triggered()),this, SLOT(changeSpectrumLUT()));
  }
  QVariant var;
  var.setValue(lut);
  action->setData(var);
  action->setCheckable(true);
  action->setChecked(spectLUT == lut);
}

void DRWindow::createLUTMenus()
{
  for (const auto &lut : lutList.map)
  {
    connectImageLUTMenuItem(ui->menu_ImageLUT,lut.first,lut.second);
    connectSpectrumLUTMenuItem(ui->menu_SpectrumLUT,lut.first,lut.second);
  }
}

void DRWindow::updateRecentFileActions()
{
  auto files=DRSuiteSettings::recentFiles();
  int numRecentFiles = std::min((int)files.size(),(int)recentFileActions.size());
  for (int i = 0; i < numRecentFiles; i++)
  {
    QString filename = QFileInfo(files[i]).fileName();
    recentFileActions[i]->setText(filename);
    recentFileActions[i]->setData(files[i]);
    recentFileActions[i]->setVisible(true);
  }
}

void DRWindow::createMenus()
{
  const int maxRecentFiles = 8; // TODO: move this to a central location
  recentFileActions.resize(8,(QAction *)nullptr);
  for (int i = 0; i < maxRecentFiles; ++i)
  {
    recentFileActions[i] = new QAction(this);
    recentFileActions[i]->setVisible(false);
    connect(recentFileActions[i], SIGNAL(triggered()),this, SLOT(openRecentFile()));
  }
  for (int i = 0; i < maxRecentFiles; ++i)
    ui->menu_Recent->addAction(recentFileActions[i]);
  updateRecentFileActions();
  contextMenu = new QMenu(this);
  contextMenu->addMenu(ui->menu_Recent);
  contextMenu->addMenu(ui->menu_ImageLUT);
  contextMenu->addMenu(ui->menu_SpectrumLUT);
  contextMenu->addAction(ui->action_Image_Info);
  contextMenu->addAction(ui->actionShow_Spectral);
  contextMenu->addAction(ui->actionToggleGrid);
  contextMenu->addAction(ui->action_Sync_Intensity);
  contextMenu->addAction(ui->action_SyncViews);
  createLUTMenus();
}

