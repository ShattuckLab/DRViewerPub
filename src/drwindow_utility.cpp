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
#include <QFileInfo>
#include <drsuitesettings.h>
typedef uint8_t uint8;
typedef double float64;

QString DRWindow::shortName(const std::string &filename)
{
  const QFileInfo info(filename.c_str());
  return info.fileName();
}

QString DRWindow::shortName(const QString &filename)
{
  const QFileInfo info(filename);
  return info.fileName();
}

std::string DRWindow::currentDatapath() {
  auto rd=DRSuiteSettings::mostRecentDirectory();
  if (!rd.empty()) datapath=rd;
  return datapath;
}

void DRWindow::addToRecentFileList(const std::string &filename)
{
  DRSuiteSettings::addRecentFile(filename);
  QFileInfo fi(filename.c_str());
  DRSuiteSettings::setMostRecentDirectory(fi.absoluteFilePath().toStdString());
  updateRecentFileActions();
}

std::unique_ptr<VolND<float32>> reorderAsSpectral(const VolND<float> *vIn)
{
  if (!vIn) return nullptr;
  auto vOut=std::make_unique<VolND<float32>>();
  vOut->reshape(vIn->hdr);
  const auto nx=vIn->nx();
  const auto ny=vIn->ny();
  const auto nz=vIn->nz();
  const auto nu=vIn->nu();
  const auto nv=vIn->nv();
  vOut->res[0]=vIn->res[3];
  vOut->res[1]=vIn->res[4];
  vOut->res[2]=vIn->res[0];
  vOut->res[3]=vIn->res[1];
  vOut->res[4]=vIn->res[2];
  for (int i=0;i<5;i++) vOut->hdr.pixdim[i+1]=vOut->res[i];

  vOut->hdr.dim[1]=vOut->dim[0]=nu;
  vOut->hdr.dim[2]=vOut->dim[1]=nv;
  vOut->hdr.dim[3]=vOut->dim[2]=nx;
  vOut->hdr.dim[4]=vOut->dim[3]=ny;
  vOut->hdr.dim[5]=vOut->dim[4]=nz;
  auto &vD=(*vOut);
  auto &vS=(*vIn);

  for (size_t u=0;u<nu;u++)
    for (size_t v=0;v<nv;v++)
      for (size_t z=0;z<nz;z++)
        for (size_t y=0;y<ny;y++)
          for (size_t x=0;x<nx;x++)
            vD(u,v,x,y,z)=vS(x,y,z,u,v);
  vOut->filename=vIn->filename+" (spectral reorder)";
  return vOut;
}

std::unique_ptr<VolND<float32>> reorderAsSpatial(const VolND<float> *vIn)
{
  if (!vIn) return nullptr;
  auto vOut=std::make_unique<VolND<float32>>();
  vOut->reshape(vIn->hdr); // to allocate the vector
  const auto nu=vIn->nx();
  const auto nv=vIn->ny();
  const auto nx=vIn->nz();
  const auto ny=vIn->nu();
  const auto nz=vIn->nv();
  vOut->res[0]=vIn->res[2];
  vOut->res[1]=vIn->res[3];
  vOut->res[2]=vIn->res[4];
  vOut->res[3]=vIn->res[0];
  vOut->res[4]=vIn->res[1];
  for (int i=0;i<5;i++) vOut->hdr.pixdim[i+1]=vOut->res[i];
  vOut->hdr.dim[1]=vOut->dim[0]=nx;
  vOut->hdr.dim[2]=vOut->dim[1]=ny;
  vOut->hdr.dim[3]=vOut->dim[2]=nz;
  vOut->hdr.dim[4]=vOut->dim[3]=nu;
  vOut->hdr.dim[5]=vOut->dim[4]=nv;
  auto &vD=(*vOut);
  auto &vS=(*vIn);

  for (size_t u=0;u<nu;u++)
    for (size_t v=0;v<nv;v++)
      for (size_t z=0;z<nz;z++)
        for (size_t y=0;y<ny;y++)
          for (size_t x=0;x<nx;x++)
            vD(x,y,z,u,v)=vS(u,v,x,y,z);
  return vOut;
}

std::unique_ptr<VolND<float32>> averageOverSpectra(const VolND<float> *vIn)
{
  auto vOut=std::make_unique<VolND<float32>>();
  auto hdr=vIn->hdr;
  hdr.dim[4]=1;
  hdr.dim[5]=1;
  vOut->reshape(hdr);
  const auto nx=vIn->nx();
  const auto ny=vIn->ny();
  const auto nz=vIn->nz();
  const auto nu=vIn->nu();
  const auto nv=vIn->nv();
  auto &vD=(*vOut);
  auto &vS=(*vIn);

  for (size_t z=0;z<nz;z++)
    for (size_t y=0;y<ny;y++)
      for (size_t x=0;x<nx;x++)
      {
        float s=0;
        for (size_t u=0;u<nu;u++)
          for (size_t v=0;v<nv;v++)
            s+=vS(x,y,z,u,v);
        vD(x,y,z)=s/(nu*nv);
      }
  return vOut;
}


std::unique_ptr<VolND<float32>> integrateOverSpectra(const VolND<float> *vIn)
{
  auto vOut=std::make_unique<VolND<float32>>();
  auto hdr=vIn->hdr;
  hdr.dim[4]=1;
  hdr.dim[5]=1;
  vOut->reshape(hdr);
  const auto nx=vIn->nx();
  const auto ny=vIn->ny();
  const auto nz=vIn->nz();
  const auto nu=vIn->nu();
  const auto nv=vIn->nv();
  auto &vD=(*vOut);
  auto &vS=(*vIn);

// could be optimized
  for (size_t z=0;z<nz;z++)
    for (size_t y=0;y<ny;y++)
      for (size_t x=0;x<nx;x++)
      {
        float s=0;
        for (size_t u=0;u<nu;u++)
          for (size_t v=0;v<nv;v++)
            s+=vS(x,y,z,u,v);
        vD(x,y,z)=s;
      }
  return vOut;
}

std::unique_ptr<VolND<float32>> maskedAverageOverSpectra2(const VolND<float> *vIn, const VolND<uint8> *vMask)
{
  auto vOut=std::make_unique<VolND<float32>>();
  auto hdr=vIn->hdr;
  auto ncomp=vMask->nz();
  hdr.dim[4]=ncomp;
  hdr.dim[5]=1;
  vOut->reshape(hdr);
  qDebug()<<"maskedAverageOverSpectra: output will be "<<vOut->nx()<<','<<vOut->ny()<<','<<vOut->nz();
  qDebug()<<"spectral dims:     "<<vIn->nu()<<','<<vOut->nv();
  qDebug()<<"spectral mask dims:"<<vOut->nx()<<','<<vOut->ny()<<','<<vOut->nz();
  const size_t nx=vIn->nx();
  const size_t ny=vIn->ny();
  const size_t nz=vIn->nz();
  const size_t nu=vIn->nu();
  const size_t nv=vIn->nv();
  auto &vD=(*vOut);
  auto &vS=(*vIn);
// could be optimized
  for (size_t n=0;n<ncomp;n++)
    for (size_t z=0;z<nz;z++)
      for (size_t y=0;y<ny;y++)
        for (size_t x=0;x<nx;x++)
        {
          float s=0;
          const uint8_t *m=vMask->slice(n);
          for (size_t u=0;u<nu;u++)
            for (size_t v=0;v<nv;v++)
              s+=(*m++) ? vS(x,y,z,u,v) : 0;
          vD(x,y,z,n)=s/(nu*nv);
        }
  return vOut;
}

std::unique_ptr<VolND<float32>> integrationOverMaskedSpectra(const VolND<float> *vIn, const VolND<uint8_t> *vMask)
{
  auto vOut=std::make_unique<VolND<float32>>();
  auto hdr=vIn->hdr;
  auto ncomp=vMask->nz();
  hdr.dim[4]=ncomp;
  hdr.dim[5]=1;
  vOut->reshape(hdr);
  qDebug()<<"maskedAverageOverSpectra: output will be "<<vOut->nx()<<','<<vOut->ny()<<','<<vOut->nz();
  qDebug()<<"spectral dims:     "<<vIn->nu()<<','<<vOut->nv();
  qDebug()<<"spectral mask dims:"<<vOut->nx()<<','<<vOut->ny()<<','<<vOut->nz();
  const size_t nx=vIn->nx();
  const size_t ny=vIn->ny();
  const size_t nz=vIn->nz();
  const size_t nu=vIn->nu();
  const size_t nv=vIn->nv();
  auto &vD=(*vOut);
  auto &vS=(*vIn);
// could be optimized
  for (size_t n=0;n<ncomp;n++)
    for (size_t z=0;z<nz;z++)
      for (size_t y=0;y<ny;y++)
        for (size_t x=0;x<nx;x++)
        {
          float s=0;
          const uint8_t *m=vMask->slice(n);
          for (size_t u=0;u<nu;u++)
            for (size_t v=0;v<nv;v++)
              s+=(*m++) ? vS(x,y,z,u,v) : 0;
          vD(x,y,z,n)=s;
        }
  return vOut;
}

std::unique_ptr<VolND<float32>> maskedAverageOverSpectra(const VolND<float> *vIn, const VolND<uint8_t> *vMask, int nComponent)
{
  auto vOut=std::make_unique<VolND<float32>>();
  auto hdr=vIn->hdr;
  hdr.dim[4]=1;
  hdr.dim[5]=1;
  vOut->reshape(hdr);
  qDebug()<<"maskedAverageOverSpectra: output will be "<<vOut->nx()<<','<<vOut->ny()<<','<<vOut->nz();
  qDebug()<<"spectral dims:     "<<vIn->nu()<<','<<vOut->nv();
  qDebug()<<"spectral mask dims:"<<vOut->nx()<<','<<vOut->ny()<<','<<vOut->nz();
  const auto nx=vIn->nx();
  const auto ny=vIn->ny();
  const auto nz=vIn->nz();
  const auto nu=vIn->nu();
  const auto nv=vIn->nv();
  auto &vD=(*vOut);
  auto &vS=(*vIn);
// could be optimized
  for (size_t z=0;z<nz;z++)
    for (size_t y=0;y<ny;y++)
      for (size_t x=0;x<nx;x++)
      {
        float s=0;
        const uint8_t *m=vMask->slice(nComponent);
        for (size_t u=0;u<nu;u++)
          for (size_t v=0;v<nv;v++)
            s+=(*m++) ? vS(x,y,z,u,v) : 0;
//            s+=vS(x,y,z,u,v);
        vD(x,y,z)=s/(nu*nv);
      }
  return vOut;
}

template <class T>
std::unique_ptr<VolND<float32>> copyToFloat(const VolND<T> *vIn)
{
  auto vScaled=std::make_unique<VolND<float32>>();
  vScaled->reshape(vIn->hdr);
  auto sz=vScaled->size();
  auto d=vScaled->begin();
  auto s=vIn->cbegin();
  for (size_t i=0;i<sz;i++) d[i]=s[i];
  return vScaled;
}

std::unique_ptr<VolND<float32>> copyToFloat(const VolNBase *vIn)
{
  switch (vIn->typeID())
  {
  //    case SILT::Float32: return copyToFloat(dynamic_cast<const VolND<float32> *>(vIn));
    case SILT::Sint16: return copyToFloat(dynamic_cast<const VolND<int16_t> *>(vIn));
    case SILT::Uint16: return copyToFloat(dynamic_cast<const VolND<uint16_t> *>(vIn));
    case SILT::Float64: return copyToFloat(dynamic_cast<const VolND<float64> *>(vIn));
    default:
      qDebug()<<"unsupported conversion"<<vIn->datatypeName().c_str();
  }
  return nullptr;
}

std::unique_ptr<VolND<float32>> averageOverSpatialDims(const VolND<float> *vIn)
{
  auto vOut=std::make_unique<VolND<float32>>();
  auto hdr=vIn->hdr;
  hdr.dim[1]=vIn->nu();
  hdr.dim[2]=vIn->nv();
  hdr.dim[3]=1;
  hdr.dim[4]=1;
  hdr.dim[5]=1;
  for (size_t i=0;i<=5;i++) hdr.pixdim[i]=1;
  vOut->reshape(hdr);
  const size_t nx=vIn->nx();
  const size_t ny=vIn->ny();
  const size_t nz=vIn->nz();
  const size_t nu=vIn->nu();
  const size_t nv=vIn->nv();
  auto &vD=(*vOut);
  size_t nvox=nx*ny*nz;
  for (size_t u=0;u<nu;u++)
    for (size_t v=0;v<nv;v++)
    {
      vD(u,v,0)=std::accumulate(vIn->volume(u,v),vIn->volume(u,v)+nvox,0.0f)/float(nvox);
    }
  return vOut;
}
