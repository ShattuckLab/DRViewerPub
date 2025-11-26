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
#include "ui_drwindow.h"
#include <QFileDialog>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QDebug>
#include <QOperatingSystemVersion>
#include <QtWidgets/qstyle.h>
#include <filesystem>
#include <memory>
#include <relaxationtoolboxform.h>
#include <colormap.h>
#include <strutil.h>
#include <QKeyEvent>
#include <algorithm>
#include <spectralimageinfo.h>
#include <drsuitesettings.h>
#include <algorithm>
#include <spectrumglwidget.h>

DRWindow::DRWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::DRWindow)
{
  ui->setupUi(this);
  ui->action_Open->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogOpenButton));
  ui->action_Image_Info->setIcon(QApplication::style()->standardIcon(QStyle::SP_FileDialogInfoView));//SP_MessageBoxInformation
  ui->action_Reset_View->setIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload));
  ui->action_Next_Component->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowForward));
  ui->action_Previous_Component->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowBack));
  ui->action_Next_Slice->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowUp));
  ui->action_Previous_Slice->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowDown));
  ui->actionToggleGrid->setIcon(QApplication::style()->standardIcon(QStyle::SP_FileDialogListView));
  ui->actionShow_Spectral->setIcon(QIcon(":/images/spectrum256px.png"));

// Load custom LUTs from home directory
  QStringList files = QDir(QDir::homePath()+"/.drviewer").entryList(QDir::Files);
  for(const auto &file : files)
  {
    std::string s=file.toStdString();
    if (StrUtil::hasExtension(s,".lut"))
    {
      loadCustomLUT((QDir::homePath()+"/.drviewer").toStdString()+'/'+s);
      status("loaded custom colormap "+s);
    }
  }

  createMenus();

  switch (QOperatingSystemVersion::currentType())
  {
    case QOperatingSystemVersion::MacOS:
      localSSOs=false;
      break;
    case QOperatingSystemVersion::Windows:
      localSSOs=true;
      break;
    default:
      localSSOs=true;
      break;
  }
  if (!std::filesystem::exists(shaderpath))
  {
    shaderpath=":/shaders/";
  }
  initWindowGrid();
  resetViews();
}

DRWindow::~DRWindow()
{
  delete ui;
}

void DRWindow::status(const std::string &msg)
{
  qDebug()<<msg.c_str();
  statusBar()->showMessage(msg.c_str());
  statusBar()->repaint();
}

void DRWindow::zoomIn()
{
  for (auto *w : spectrumWindows) w->viewController.zoom*=1.1f;
  refresh();
}

void DRWindow::zoomOut()
{
  for (auto *w : spectrumWindows) w->viewController.zoom/=1.1f;
  refresh();
}

void DRWindow::initWindowGrid()
{
  gridRows=2;
  gridCols=3;
  for (int r=0,idx=0;r<gridRows;r++)
    for (int c=0;c<gridCols;c++)
  {
    auto *spectrumWindow = new SpectrumGLWidget(this);
    ui->gridLayout_3->addWidget(spectrumWindow, r, c, 1, 1);
    spectrumWindows.push_back(spectrumWindow);
    spectrumWindow->setVisible(false);
    spectrumWindow->setObjectName(QString("SW ")+QString::number(idx++));
    spectrumWindow->showUPlot=1;
  }
  for (auto &window : spectrumWindows) { window->attach(this); }
  attachFormToDockWidget(relaxationToolboxForm,ui->relaxationDockWidget,"&Diffusion-Relaxation Toolbox","Press to show/hide diffusion-relaxtion toolbox.");
  ui->relaxationDockWidget->show();
  setViewMode(FourUpIntegratedSpectra,false);
}

glm::ivec3 DRWindow::worldToVoxel(const glm::vec3 &worldCoordinate) const
{
  if (spectrum())
  {
// include half-voxel shift because pixels voxel coordinates are centered.
    return glm::ivec3(std::clamp<float>(worldCoordinate.x/spectrum()->rx()+0.5f,0,spectrum()->nx()-1),
                      std::clamp<float>(worldCoordinate.y/spectrum()->ry()+0.5f,0,spectrum()->ny()-1),
                            currentVolumePosition.z);
  }
  else
    return worldCoordinate;
}

void DRWindow::mouseTrack(glm::vec3 worldClickPoint)
{
  currentVolumePosition=worldToVoxel(worldClickPoint);
  relaxationToolboxForm->updatePosition();
}

bool DRWindow::setViewMode(ViewModes mode, bool update)
{
  static const std::vector<glm::vec3> baseColors={
    {1.0f,1.0f,1.0f},
    {1.0f,0,0},
    {0.2118f,0.3176f,1.0f},
    {0,1.0f,0},
    {0.6f,0.6f,0}
  };
  switch (mode)
  {
    case DRWindow::FourUpIndividualSpectra:
    case DRWindow::FourUpIntegratedSpectra:
      viewMode=mode; // can always be in these modes
      break;
    case DRWindow::SixUpComposite:
      if (!vComponentMask)
      {
        statusBar()->showMessage("Please load a component mask to enter component mode.");
        return false;
      }
      viewMode=mode;
      break;
    case DRWindow::FourUpComponent:
      if (!vComponentMask)
      {
        statusBar()->showMessage("Please load a component mask to enter composite mode.");
        return false;
      }
      viewMode=mode;
      break;
  }
  switch (viewMode)
  {
    case DRWindow::FourUpIndividualSpectra:
    case DRWindow::FourUpIntegratedSpectra:
    case DRWindow::FourUpComponent:
      for (auto *w : spectrumWindows)
      {
        w->showSpectra=0;
        w->showUPlot=0;
        w->showVPlot=0;
      }
      windowgrid(0,0)->showUPlot=1;
      windowgrid(1,0)->showSpectra=1;
      windowgrid(1,1)->showVPlot=1;
      break;
    case DRWindow::SixUpComposite:
      status("changing to composite view");
      for (auto *w : spectrumWindows)
      {
        w->setVisible(true);
        w->showSpectra=0;
        w->showUPlot=0;
        w->showVPlot=0;
        w->updateSpectrumColorbar();
      }
      if (maxComponentIntensities.size()>=4)
      {
        for (int r=0,i=0;r<2;r++)
          for (int c=0;c<2;c++,i++)
          {
            windowgrid(r,c)->showComposite=false;
            windowgrid(r,c)->showComponent=i;
            windowgrid(r,c)->localBrightness=maxComponentIntensities[i];
            qDebug()<<r<<c<<maxComponentIntensities[i];
          }
        windowgrid(0,2)->showComposite=0;
        windowgrid(0,2)->showComponent=-1;
        windowgrid(0,2)->localBrightness=*std::max_element(maxComponentIntensities.begin(),maxComponentIntensities.end());
        windowgrid(1,2)->showComposite=1;
        windowgrid(1,2)->showComponent=-1;
        windowgrid(1,2)->localBrightness=*std::max_element(maxComponentIntensities.begin(),maxComponentIntensities.end());
        windowgrid(0,0)->useBasecolor=true;
        windowgrid(1,0)->useBasecolor=true;
        windowgrid(0,1)->useBasecolor=true;
        windowgrid(1,1)->useBasecolor=true;
        windowgrid(0,0)->setBaseColor(baseColors[1]);
        windowgrid(1,0)->setBaseColor(baseColors[2]);
        windowgrid(0,1)->setBaseColor(baseColors[3]);
        windowgrid(1,1)->setBaseColor(baseColors[4]);
      }
      break;
  }
  switch (viewMode)
  {
    case DRWindow::FourUpIndividualSpectra:
    case DRWindow::FourUpIntegratedSpectra:
      if (relaxationToolboxForm) relaxationToolboxForm->setComponentsOn(false);
      for (auto w : spectrumWindows)
        w->showComponent=-1;
      break;
    case DRWindow::FourUpComponent:
      if (relaxationToolboxForm) relaxationToolboxForm->setComponentsOn(true);
      for (auto w : spectrumWindows)
        w->showComponent=relaxationToolboxForm->showComponent();
      break;
    case SixUpComposite:
      if (relaxationToolboxForm) relaxationToolboxForm->setComponentsOn(false);
      break;
  }
  for (auto *w : spectrumWindows)
  {
    w->updateImageColorbar();
    w->updateSpectrumColorbar();
  }
  windowgrid(0,0)->setVisible(true);
  windowgrid(0,1)->setVisible(true);
  windowgrid(1,0)->setVisible(true);
  windowgrid(1,1)->setVisible(true);
  windowgrid(0,2)->setVisible(viewMode==SixUpComposite);
  windowgrid(1,2)->setVisible(viewMode==SixUpComposite);

  if (update)
    refresh();
  return true;
}

void DRWindow::toggleSingleWindow(SpectrumGLWidget *requestingWindow)
{
  int nVisible=0;
  for (auto &w : spectrumWindows) nVisible+=w->isVisible();

  if (nVisible>1||!requestingWindow->isActiveWindow())
    for (auto &w : spectrumWindows)
    {
      w->setVisible(false);
      requestingWindow->setVisible(true);
    }
  else
  {
    switch (viewMode)
    {
      case FourUpIntegratedSpectra:
      case FourUpIndividualSpectra:
      case FourUpComponent:
          windowgrid(0,0)->setVisible(true);
          windowgrid(0,1)->setVisible(true);
          windowgrid(1,0)->setVisible(true);
          windowgrid(1,1)->setVisible(true);
        break;
      case SixUpComposite:
        for (auto &w : spectrumWindows)
          w->setVisible(true);
        break;
    }
  }
}

template <class Form>
bool DRWindow::attachFormToDockWidget(Form *&form, QDockWidget *dockWidget, std::string menu, std::string tooltip)
{
  form = new Form(this,dockWidget);
  dockWidget->hide();
  dockWidget->setWidget(form);
  QAction *action = dockWidget->toggleViewAction();
  action->setText(QString(menu.c_str()));
  action->setStatusTip(QString(tooltip.c_str()));
  action->setChecked(false);
  ui->menu_View->addAction(action);
  return true;
}

void DRWindow::refresh()
{
  for (auto &w : spectrumWindows) w->update();
}

void DRWindow::resetViews()
{
  if (spectrumWindows.size())
  {
    imageState.zoomf=0.95*2.0/std::max(volumeExtent.x,volumeExtent.y/spectrumWindows[0]->viewController.aspectRatio()); // ignore z for now
  }
  else
  {
    imageState.zoomf=2.0/std::max(volumeExtent.x,volumeExtent.y); // ignore z for now
  }
  for (auto *w : spectrumWindows)
  {
    w->viewController.reset();
    w->viewController.zoom=imageState.zoomf;
  }
  updateViews();
}

void DRWindow::updateViews()
{
  switch (viewMode)
  {
    case FourUpIndividualSpectra:
    case FourUpIntegratedSpectra:
    case FourUpComponent:
    case SixUpComposite: break;
  }
  for (auto &w : spectrumWindows) w->update();
  if (relaxationToolboxForm)
    relaxationToolboxForm->updatePosition();
}

void DRWindow::updateSliceSSOs()
{
  volumeShaderObject.loadSpectralVolume(vSpectralSlices.get(),currentVolumePosition.z);
  for (auto &w : spectrumWindows)
  {
    w->localSSO.loadSpectralVolume(vSpectralSlices.get(),currentVolumePosition.z);
  }
}

void DRWindow::loadComponentMasktoSSO(SpectroscopicShaderObject &vso)
{
  vso.loadComponentMaskVolume(vComponentMask.get());
}

void DRWindow::loadComponentMasktoSSOs()
{
  loadComponentMasktoSSO(volumeShaderObject);
  for (auto &w : spectrumWindows) loadComponentMasktoSSO(w->localSSO);
}

void DRWindow::loadComponentSSO(SpectroscopicShaderObject &sso)
{
  auto n=nComponents();
  sso.loadComponents(vComponentMaps.get());
  if (n>0) sso.loadVolume2(vComponentMaps.get(),0,0); // TODO: use one texture
  if (n>1) sso.loadVolume3(vComponentMaps.get(),1,0);
  if (n>2) sso.loadVolume4(vComponentMaps.get(),2,0);
  if (n>3) sso.loadVolume5(vComponentMaps.get(),3,0);
}

void DRWindow::reloadComponentSSOs()
{
  if (useLocalSSOs())
    for (auto *w : spectrumWindows)
    {
      loadComponentSSO(w->localSSO);
      w->showComposite=0;
    }
  else
  {
    loadComponentSSO(volumeShaderObject);
    for (auto *w : spectrumWindows)
    {
      w->showComposite=0;
    }
  }
}

void DRWindow::reloadSSOs()
{
  if (useLocalSSOs()) for (auto *w : spectrumWindows) loadSSO(w->localSSO);
  else loadSSO(volumeShaderObject);
}

void DRWindow::loadSSO(SpectroscopicShaderObject &sso)
{
  if (viewMode!=FourUpIndividualSpectra)
  {
    sso.loadVolume1(vIntegratedSpatialImage.get(),0,0);
    imageState.brightness=maxIntegratedSpatialImage;
  }
  else
  {
    sso.loadVolume1(vSpectrum.get(),currentSpectralPosition.u,currentSpectralPosition.v);
    imageState.brightness=maxFullSpectrum;
  }
  sso.loadSpectralVolume(vSpectralSlices.get(),currentVolumePosition.z);
  sso.loadLUT1(imageLUT());
  sso.loadLUT2(spectrumLUT());
}

void DRWindow::on_action_Open_triggered()
{
  QString filename = QFileDialog::getOpenFileName(this,tr("Load Spectroscopic Image"),currentDatapathQS(),
                                                  tr("3D Image Volume Files (*.img *.nii *.img.gz *.nii.gz)"),0,
                                                  QFileDialog::Options());
  readFile(filename.toStdString());
}

void DRWindow::on_action_Exit_triggered()
{
  close();
}

void DRWindow::reloadShaders()
{
  for (auto *w : spectrumWindows) w->loadShaders();
  updateViews();
}

float DRWindow::spectrumScale()
{
  float scale=imageState.overlay1Brightness;
  if (spectrumWindows.size()>=2)
    scale*=spectrumWindows[1]->viewController.scaleX();
  else if (spectrumWindows.size()==1)
    scale*=spectrumWindows[0]->viewController.scaleX();
  return scale;
}

void DRWindow::syncWindows(const SpectrumGLWidget *source)
{
  if (synchronizeViews)
    for (auto w : spectrumWindows)
      if (w!=source)
      {
        w->viewController.zoom=source->viewController.zoom;
        w->viewController.shift=source->viewController.shift;
      }
  if (syncBrightness)
    for (auto w : spectrumWindows)
      if (w!=source)
        w->viewController.scale=source->viewController.scale;
  for (auto w : spectrumWindows)
      w->update();

  if (relaxationToolboxForm)
    relaxationToolboxForm->updatePosition();
}

void DRWindow::on_action_SyncViews_toggled(bool arg1)
{
  synchronizeViews=arg1;
  syncWindows(spectrumWindows[0]);
}

void DRWindow::updateComponentVolumes()
{
  if (!vSpectralSlices) return;
  if (!vComponentMask) return;
  maxComponentIntensities=std::vector<float>(vComponentMask->nz(),0.0f);
  vComponentMaps=integrationOverMaskedSpectra(vSpectrum.get(),vComponentMask.get());
  const size_t volstride=vComponentMaps->nx()*vComponentMaps->ny()*vComponentMaps->nz();
  for (size_t n=0;n<vComponentMask->nz();n++)
  {
    maxComponentIntensities[n]=*std::max_element(vComponentMaps->volume(n,0),vComponentMaps->volume(n,0)+volstride);
  }
  reloadComponentSSOs();
  loadComponentMasktoSSOs();
}

std::string DRWindow::findSidecar(std::string niftiFile) const
{
  auto base=StrUtil::gzStrip(niftiFile);
  base=StrUtil::extStrip(StrUtil::gzStrip(niftiFile),"nii");
  auto sidecar=base+".json";
  if (!std::filesystem::exists(sidecar))
  {
    qDebug()<<"couldn't open sidecar"<<sidecar;
    sidecar="";
  }
  return sidecar;
}

void DRWindow::dragEnterEvent(QDragEnterEvent *event)
{
  if (event->mimeData()->hasFormat("text/uri-list"))
  {
    event->acceptProposedAction();
  }
}


void DRWindow::openRecentFile()
{
  QAction *action = qobject_cast<QAction *>(sender());
  if (action)
  {
    readFile(action->data().toString().toStdString());
    update();
  }
}

std::string DRWindow::coordInfoU()
{
  return relaxationToolboxForm->coordInfoU(currentSpectralPosition.u);
}

std::string DRWindow::coordInfoV()
{
  return relaxationToolboxForm->coordInfoV(currentSpectralPosition.v);
}
void DRWindow::moveToSpectralPoint(float u, float v)
{
  if (!vSpectrum) return;
  currentSpectralPosition.u=std::clamp<int>(u*vSpectrum->nu(),0,vSpectrum->nu()-1);
  currentSpectralPosition.v=std::clamp<int>(v*vSpectrum->nv(),0,vSpectrum->nv()-1);
  if (solitaryMode)
  {
    reloadSSOs();
  }
  updateViews();
}

void DRWindow::moveToSlice(int z)
{
  if (!spectrum()) return;
  if (z>=(int)(spectrum()->nz())) { z=spectrum()->nz(); z--; } // avoid case of nz=0 becoming max int!
  if (z<0) z=0;
  currentVolumePosition.z=z;
  updateSliceSSOs();
  updateViews();
  if (relaxationToolboxForm) relaxationToolboxForm->updateSliders();
}

void DRWindow::toggleSpectralOverlay()
{
  showSpectralOverlay^=true;
  this->updateViews();
}

void DRWindow::keyPressEvent(QKeyEvent *event)
{
  switch (event->key())
  {
    case Qt::Key_Up:
      on_action_Next_Slice_triggered();
      break;
    case Qt::Key_Down:
      on_action_Previous_Slice_triggered();
      break;
    case Qt::Key_Left: on_action_Previous_Component_triggered(); break;
    case Qt::Key_Right: on_action_Next_Component_triggered(); break;
    case 'S':
    case 's':
      toggleSpectralOverlay();
      break;
    case 'H':
    case 'h':
      for (auto *w : spectrumWindows) w->viewController.reset();
      this->imageState.zoomf=1;
      relaxationToolboxForm->update();
      refresh();
      break;
    case '+':
    case '=': zoomIn(); break;
    case '-': zoomOut(); break;
  }
}

void DRWindow::on_actionIntegrated_over_spectra_triggered()
{
  if (vIntegratedSpatialImage)
  {
    QString filename = QFileDialog::getSaveFileName(this,tr("Save Integrated Image"),currentDatapathQS(),
                                                    tr("3D Image Volume Files (*.img *.nii *.img.gz *.nii.gz)"),0,
                                                    QFileDialog::Options());
    vIntegratedSpatialImage->write(filename.toStdString());
  }
}

int DRWindow::showComponent()
{
  if (!vComponentMask) return -1;
  return relaxationToolboxForm->showComponent();
}

float DRWindow::spotlightRadius()
{
  return (relaxationToolboxForm) ? relaxationToolboxForm->spotlightRadius() : 0.0f;
}


float DRWindow::gridwidth()
{
  if (ui->actionToggleGrid->isChecked()) return 0.0125f;
  return 0;
}


void DRWindow::on_actionToggleGrid_toggled(bool /*arg1*/)
{
    updateViews();
}


void DRWindow::on_actionNavigator_triggered()
{
    if (relaxationToolboxForm)
    {
      ui->relaxationDockWidget->show();
      ui->relaxationDockWidget->raise();
    }
}

void DRWindow::on_actionShow_Spectral_triggered()
{
  toggleSpectralOverlay();
}

void DRWindow::on_actionAutoscale_triggered()
{
  int component=relaxationToolboxForm->activeComponent;
  if (component>0 && component<(int)maxComponentIntensities.size())
  {
    imageState.brightness=maxComponentIntensities[component];
  }
  else
    imageState.brightness=maxIntegratedSpatialImage;
  imageState.overlay1Brightness=maxFullSpectrum;

}

void DRWindow::updateComponentSelection()
{
  auto component=relaxationToolboxForm->activeComponent;
  for (auto *w : spectrumWindows)
  {
    w->showComponent=component;
    w->localBrightness=maxComponentIntensities[component];
  }
  updateViews();
}

int DRWindow::nComponents()
{
  return vComponentMask ? static_cast<int>(vComponentMask->nz()) : 0;
}

void DRWindow::on_action_Next_Component_triggered()
{
  if (vComponentMask)
  {
    int component=relaxationToolboxForm->activeComponent;
    component++;
    if (component<0) component=0;
    if (component>=(int)nComponents()) component=0;
    relaxationToolboxForm->selectComponent(component);
  }
}

void DRWindow::on_action_Previous_Component_triggered()
{
  if (vComponentMask)
  {
    int component=relaxationToolboxForm->activeComponent;
    component--;
    if (component<0||component>=(int)nComponents()) component=nComponents()-1;
    relaxationToolboxForm->selectComponent(component);
  }
}

void DRWindow::on_action_Sync_Intensity_triggered()
{
    syncBrightness^=true;
    if (syncBrightness) updateViews();
}

void DRWindow::on_action_Image_Info_toggled(bool /*arg1*/)
{
  showImageInfo^=true;
  updateViews();
}

void DRWindow::on_action_Next_Slice_triggered()
{
  moveToSlice(currentVolumePosition.z+1);
}

void DRWindow::on_action_Previous_Slice_triggered()
{
  moveToSlice(currentVolumePosition.z-1);
}

void DRWindow::on_action_Reset_View_triggered()
{
  resetViews();
}

void DRWindow::on_action_ViewAverageSpectroscopicImage_triggered()
{
  setViewMode(FourUpIntegratedSpectra,true);
  reloadSSOs();
}

void DRWindow::on_action_Toggle_View2_toggled(bool /*arg1*/)
{
  setViewMode(FourUpIndividualSpectra,true);
  reloadSSOs();
}

void DRWindow::on_action_Toggle_View3_toggled(bool /*arg1*/)
{
  if (setViewMode(FourUpComponent,true)) reloadSSOs();
}

void DRWindow::on_action_Toggle_View4_toggled(bool /*arg1*/)
{
  setViewMode(SixUpComposite,true);
  reloadSSOs();
}
