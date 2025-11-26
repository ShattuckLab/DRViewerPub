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
#include <ui_drwindow.h>
#include <spectralimageinfo.h>
#include <relaxationtoolboxform.h>
#include <QFileInfo>
#include <QMimeData>
#include <QDropEvent>
#include <QFileDialog>
#include <nlohmann/json.hpp>
#include <waitcursor.h>

bool DRWindow::readFile(const std::string &filename)
{
  WaitCursor waitCursor(this);
  status("loading file"+filename);
  if (!std::filesystem::exists(filename))
  {
    status(filename+" does not exist!");
    return false;
  }
  auto sidecar=findSidecar(filename);
  if (!sidecar.empty())
  {
    std::ifstream jsonfile(sidecar);
    qDebug()<<"reading metadata";
    try {
      nlohmann::json metadata = nlohmann::json::parse(jsonfile);
      auto name=metadata["name"].get<std::string>();
  //    if (name=="im_mask") return readSpatialMask(filename); // not currently supported
      if (name=="spec_mask") return readComponentMask(filename);
      if (name=="spectral_image") return readSpectralImage(filename);
  //    if (name=="K") return loadSpectralImage(filename);  // not currently supported
      if (name=="data") return readSpectralImage(filename); // need different handling for spatial image
      qDebug()<<"unsupported volume type: "<<name;
//      return false;
    }
    catch (const nlohmann::json::exception& e)
    {
        qDebug()<< e.what();
    }
    catch (...)
    {
      qDebug()<<"unknown exception!";
    }
  }
  return readSpectralImage(filename);
}

bool DRWindow::readSpectralImage()
{
  QString filename = QFileDialog::getOpenFileName(this,tr("Load Spectral Image"),currentDatapathQS(),
                                                  tr("3D Image Volume Files (*.img *.nii *.img.gz *.nii.gz)"),0,
                                                  QFileDialog::Options());
  return readSpectralImage(filename.toStdString());
}

void DRWindow::on_action_LoadCustomColormap_triggered()
{
  QString filename = QFileDialog::getOpenFileName(this,tr("Colormap Lookup Table"),currentDatapathQS(),
                                                  tr("LUT Files (*.lut *.txt)"),0,
                                                  QFileDialog::Options());
  loadCustomLUT(filename.toStdString());
}


bool DRWindow::readSpectralImage(const std::string &filename)
{
  WaitCursor waitCursor(this);
  if (!std::filesystem::exists(filename))
  {
    status(filename+" does not exist!");
    return false;
  }
  status("loading spectral image "+filename);
  auto vTmp=VolNBase::load(filename);
  std::unique_ptr<VolND<float32>> vFloat;
  if (!vTmp)
  {
    status("unable to load "+filename);
    return false;
  }
  if (auto *v=dynamic_cast<VolND<float> *>(vTmp.get()))
  {
    vTmp.release(); vFloat=std::unique_ptr<VolND<float>>(v);
  }
  else
  {
    vFloat=copyToFloat(vTmp.get());
    if (vFloat)
      vFloat->filename=vTmp->filename;
  }
  if (!vFloat)
  {
    status("unable to load "+filename);
    return false;
  }
  addToRecentFileList(filename);
  status("loaded spectroscopic image "+filename);
  std::ostringstream ostr;
  ostr<<"loaded component spectral image "<<filename
     <<" with shape "<<vFloat->nx()<<'x'
     <<vFloat->ny()<<'x'
     <<vFloat->nz()<<'x'
     <<vFloat->nu()<<'x'<<vFloat->nv();
  status(ostr.str());

  auto sidecarfile=findSidecar(filename);
  if (!sidecarfile.empty())
  {
    if (spectralImageInfo.parse(sidecarfile))
    {
      if (spectralImageInfo.spectralFirst())
      {
        vSpectralSlices=std::move(vFloat);
        vSpectrum=reorderAsSpatial(vSpectralSlices.get());
      }
      else
      {
        vSpectrum=std::move(vFloat);
        vSpectralSlices=reorderAsSpectral(vSpectrum.get());
      }
    }
    else
    {
      vSpectrum=std::move(vFloat);
      vSpectralSlices=reorderAsSpectral(vSpectrum.get());
    }
  }
  else
  {
    spectralImageInfo=SpectralImageInfo();
    vSpectrum=std::move(vFloat);
    vSpectralSlices=reorderAsSpectral(vSpectrum.get());
  }
  vIntegratedSpatialImage=integrateOverSpectra(vSpectrum.get());
  vAverageSpectralImage=averageOverSpatialDims(vSpectrum.get());

  maxFullSpectrum=vSpectrum->maxvalue();
  maxFullSpectrumSlices=vSpectralSlices->maxvalue();
  maxIntegratedSpatialImage=vIntegratedSpatialImage->maxvalue();

  imageState.brightness=maxFullSpectrum;
  imageState.overlay1Brightness=maxFullSpectrum;
  setWindowTitle(filename.c_str());
  if (relaxationToolboxForm) relaxationToolboxForm->setSpectralImageText(filename.c_str());
  currentVolumePosition=glm::ivec3(vSpectrum->nx()/2,vSpectrum->ny()/2,vSpectrum->nz()/2);
  currentSpectralPosition=UVPoint(vSpectrum->nu()/2,vSpectrum->nv()/2);
  volumeExtent=glm::vec3(vSpectrum->nx()*vSpectrum->rx(),vSpectrum->ny()*vSpectrum->ry(),vSpectrum->nz()*vSpectrum->rz());
  reloadSSOs();
  updateComponentVolumes();
  relaxationToolboxForm->updateAxisLabelInfo();
  if (relaxationToolboxForm) relaxationToolboxForm->updatePosition();
  resetViews();
  return true;
}

bool DRWindow::readComponentMask()
{
  QString filename = QFileDialog::getOpenFileName(this,tr("Load Component Mask"),currentDatapathQS(),
                                                  tr("3D Image Volume Files (*.img *.nii *.img.gz *.nii.gz)"),0,
                                                  QFileDialog::Options());
  return readComponentMask(filename.toStdString());
}

bool DRWindow::readComponentMask(const std::string &filename)
{
  WaitCursor waitCursor(this);
  if (!std::filesystem::exists(filename))
  {
    qDebug()<<filename<<" does not exist!";
    return false;
  }
  status("loading component mask "+filename);
  vComponentMask=std::make_unique<VolND<uint8_t>>();
  vComponentMask->read(filename);
  if (!vComponentMask) return false;
  addToRecentFileList(filename);
  status("loaded component mask "+filename);
  updateComponentVolumes();

  if (relaxationToolboxForm)
  {
    relaxationToolboxForm->setComponentMaskText(filename.c_str());
    relaxationToolboxForm->enableComponents(true);
    relaxationToolboxForm->updateSliders();
  }
  updateViews();
  return true;
}

void DRWindow::dropEvent(QDropEvent *event)
{
  if (event->mimeData()->hasUrls())
  {
    QList<QUrl> list = event->mimeData()->urls();
    for (QList<QUrl>::iterator i=list.begin(); i!=list.end();i++)
    {
      readFile(i->toLocalFile().toStdString());
    }
  }
}
