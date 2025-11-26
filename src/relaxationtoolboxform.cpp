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

#include "relaxationtoolboxform.h"
#include <QMouseEvent>
#include "ui_relaxationtoolboxform.h"
#include <QDebug>
#include <QStyle>
#include <drwindow.h>
#include <volnd.h>
#include <volnd_t.h>
#include <colorops.h>
#include <waitcursor.h>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QGestureEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QFileInfo>

typedef uint16_t uint16;

QString shortName(const QString &filename)
{
  const QFileInfo info(filename);
  return info.fileName();
}

RelaxationToolboxForm::RelaxationToolboxForm(DRWindow *drWindow, QWidget *parent)
    : QWidget(parent), ui(new Ui::RelaxationToolboxForm)
    , drWindow(drWindow)
{
  ui->setupUi(this);
  ui->uAxisLabel->setText("u");
  ui->vAxisLabel->setText("v");
  ui->spectralImageWidget->relaxationToolboxForm=this;
  ui->spectralImageWidget->drWindow=drWindow;
  ui->colorbarWidget->drWindow=drWindow;
  ui->uPlotWidget->relaxationToolboxForm=this;
  ui->uPlotWidget->drWindow=drWindow;
  ui->vPlotWidget->relaxationToolboxForm=this;
  ui->vPlotWidget->drWindow=drWindow;
  ui->loadSpectralNIIButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogOpenButton));
  ui->vPlotWidget->setVisible(false);
  ui->uPlotWidget->setVisible(false);
  ui->vPlotWidget->vertical=true;
  ui->colorCheckBox->setChecked(ui->vPlotWidget->plotColor);
  ui->nContoursSpinBox->blockSignals(true);
  ui->nContoursSpinBox->setValue(nContours);
  ui->nContoursSpinBox->blockSignals(false);
  ui->interpolateContoursCheckBox->setChecked(true);
  ui->contourCheckBox->setChecked(false);
  ui->showSpectrumImageCheckBox->setChecked(true);
  ui->integrate3DCheckBox->hide();
  ui->showContourTickLabelsCheckBox->hide();
  ui->spotlightRadiusSlider->setRange(1,100);
  ui->spotlightRadiusSlider->setValue(10);
  ui->uaxisBlankSpace->hide();
  ui->vaxisBlankSpace->hide();
  updateColorbar();
  enableComponents(false);
}

void RelaxationToolboxForm::updateColorbar()
{
  ui->colorbarWidget->setLUT(drWindow->spectrumLUT());
}

void RelaxationToolboxForm::enableComponents(bool state)
{
  ui->componentSlider->setDisabled(!state);
  ui->componentButton->setDisabled(!state);
  ui->compositeViewButton->setDisabled(!state);
}

void RelaxationToolboxForm::setComponentsOn(bool state)
{
//  if (drWindow->vComponentMask)
  ui->componentButton->setChecked(state);
}

void RelaxationToolboxForm::selectComponent(int n)
{
  activeComponent=(n>=0 && n<(int)drWindow->nComponents()) ? n : -1;
  ui->componentSlider->setValue(activeComponent>0 ? activeComponent:0);
  ui->componentLabel->setText(QString::number(activeComponent>0 ? activeComponent+1:1));
  ui->componentButton->setChecked(true);
  drWindow->updateComponentSelection();
  refresh();
}

bool RelaxationToolboxForm::integration()
{
  return (ui) ? ui->integrate3DCheckBox->isChecked() : false;
}

void RelaxationToolboxForm::dragEnterEvent(QDragEnterEvent *event)
{
  if (event->mimeData()->hasFormat("text/uri-list"))
  {
    event->acceptProposedAction();
  }
}

void RelaxationToolboxForm::dropEvent(QDropEvent *event)
{
  if (event->mimeData()->hasUrls())
  {
    QList<QUrl> list = event->mimeData()->urls();
    for (QList<QUrl>::iterator i=list.begin(); i!=list.end();i++)
    {
      drWindow->readFile(i->toLocalFile().toStdString());
    }
  }
}

RelaxationToolboxForm::~RelaxationToolboxForm()
{
  delete ui;
}

inline float cleaner(const float f)
{
  return (f>1e-16) ? f : 0;
}

template <class T>
void copyAvgSpectralSlice(SliceT<float> &slice, const VolND<T> *vol)
// TODO : make faster copy using strides & pointers
{
  if (!vol) {
    return;
  }
  bool sanitize=true;
  const auto du = vol->nx();
  const auto dv = vol->ny();
  auto *src=vol->slice(0);
  slice.setsize(du,dv);
  if (sanitize)
  {
    for (size_t v=0;v<dv;v++)
      for (size_t u=0;u<du;u++)
        slice(u,v)=cleaner(*src++);
  }
  else
  {
    for (size_t v=0;v<dv;v++)
      for (size_t u=0;u<du;u++)
        slice(u,v)=(*src++);
  }
}


template <class S, class T>
void copySpectralSlice(SliceT<S> &slice, const VolND<T> *vol, int n)
// TODO : make faster copy using strides & pointers
{
  if (!vol) {
    return;
  }
  bool sanitize=true;
  const auto du = vol->nx();
  const auto dv = vol->ny();
  auto *p=vol->slice(n);
  slice.setsize(du,dv);
  if (sanitize)
  {
    for (size_t v=0;v<dv;v++)
      for (size_t u=0;u<du;u++)
        slice(u,v)=cleaner(*p++);
  }
  else
  {
    for (size_t v=0;v<dv;v++)
      for (size_t u=0;u<du;u++)
        slice(u,v)=(*p++);
  }
}

template <class T>
void getSpectralSlice(SliceT<float> &slice, const VolND<T> &vol, int x, int y, int z)
// TODO : make faster copy using strides & pointers
{
  bool sanitize=true;
  const auto du = vol.nu();
  const auto dv = vol.nv();
  slice.setsize(du,dv);
  if (sanitize)
  {
    for (size_t v=0;v<dv;v++)
      for (size_t u=0;u<du;u++)
        slice(u,v)=cleaner(vol(x,y,z,u,v));
  }
  else
  {
    for (size_t v=0;v<dv;v++)
      for (size_t u=0;u<du;u++)
        slice(u,v)=vol(x,y,z,u,v);
  }
}

template <class T, class Operation>
void RelaxationToolboxForm::renderPointSpectrum(DSImage &bitmap, SliceT<T> &slice, Operation operation) // TODO: needs 3d point
{
  const auto du = slice.cx;
  const auto dv = slice.cy;
  bitmap.resize(du,dv);
  QRgb* pixel = reinterpret_cast<QRgb*>(bitmap.image.bits());
  for (size_t v=0;v<dv;v++)
    for (size_t u=0;u<du;u++)
    {
      operation(*pixel++,slice(u,dv-1-v));
    }
}

void RelaxationToolboxForm::togglePlots()
{
  bool on=ui->uPlotWidget->isVisible();
  if (on)
  {
    ui->uPlotWidget->plotColor^=true;
    ui->vPlotWidget->plotColor^=true;
    ui->vPlotWidget->darkBackground=ui->darkBackGroundCheckBox->isChecked();
    ui->uPlotWidget->darkBackground=ui->darkBackGroundCheckBox->isChecked();
    if (ui->vPlotWidget->plotColor)
    {
      ui->uPlotWidget->setVisible(false);
      ui->vPlotWidget->setVisible(false);
      ui->axisPlotCheckBox->setChecked(false);
      ui->colorCheckBox->setChecked(true);
    }
    else
      ui->colorCheckBox->setChecked(false);
  }
  else
  {
    ui->uPlotWidget->setVisible(true);
    ui->vPlotWidget->setVisible(true);
    ui->axisPlotCheckBox->setChecked(true);
  }
  update();
}

double readValue(VolNBase *v, const int x, const int y, const int z, const int t)
{
  if (v)
  {
    switch (v->typeID())
    {
      case SILT::Uint16  : return (*static_cast<VolND<uint16>  *>(v))(x,y,z,t); break;
      case SILT::Sint16  : return (*static_cast<VolND<sint16>  *>(v))(x,y,z,t); break;
      case SILT::Float32 : return (*static_cast<VolND<float32> *>(v))(x,y,z,t); break;
      case SILT::Float64 : return (*static_cast<VolND<float64> *>(v))(x,y,z,t); break;
      default: break;
    }
  }
  return 0;
}

void RelaxationToolboxForm::selectSpectralPointF(float u, float v)
{
  if (drWindow)
    drWindow->moveToSpectralPoint(u,v);
}

void RelaxationToolboxForm::on_loadSpectralNIIButton_clicked()
{
  drWindow->readSpectralImage();
}

void RelaxationToolboxForm::updateSliders()
{
  if (drWindow->spectrum())
  {
    ui->zSliceSlider->setRange(0,drWindow->spectrum()->nz()-1);
    ui->zSliceSlider->setValue(drWindow->currentVolumePosition.z);
    ui->zSliceEdit->setText(QString::number(drWindow->currentVolumePosition.z));
  }
  if (drWindow->componentMask())
  {
    int activeComponent=0;
    ui->componentSlider->setRange(0,drWindow->componentMask()->nz()-1);
    activeComponent=std::clamp<int>(activeComponent,0,drWindow->componentMask()->nz()-1);
    ui->componentSlider->setValue(activeComponent);
    ui->componentLabel->setText(QString::number(activeComponent+1));
  }
}


std::unique_ptr<VolND<float32>> reorder(const VolND<float> *vIn);



bool RelaxationToolboxForm::event(QEvent *event)
{
  // qDebug()<<"main event: "<<event->type();
  switch (event->type())
  {
    case QEvent::TouchUpdate:
    {
      // auto *touch = dynamic_cast<QTouchEvent*>(event);
      // if (touch) { qDebug()<<"I have the touch"; }
      break;
    }
    case QEvent::Wheel:
    {
      auto *wheel = dynamic_cast<QWheelEvent*>(event);
      if (wheel)
      {
        // qDebug()<<"I have the wheel: "<<wheel->angleDelta()<<" "<<wheel->pixelDelta();
        float scale=float(1.0f-wheel->pixelDelta().y()/100.0f);
        auto b=drWindow->imageState.overlay1Brightness;
        if (wheel->modifiers()&Qt::AltModifier)
        {
          b*=scale;
        }
        else if (wheel->modifiers()&Qt::ShiftModifier)
        {
        }
        else
        {
          b*=scale;
        }
        drWindow->imageState.overlay1Brightness=b;
//        setImageScale(b);
        updateLUT();
      }
      break;
    }
    case QEvent::NativeGesture:
    {    
      auto gesture = dynamic_cast<QNativeGestureEvent*>(event);
      if (gesture)
      {
         qDebug()<<"(gestures wildly) "<<gesture->gestureType()<<" @ "<<gesture->position()<<" : "<<gesture->value();
        switch (gesture->gestureType())
        {
        case Qt::BeginNativeGesture: gesturing=true; gestureScale=1.0f; break;
        case Qt::EndNativeGesture:   gesturing=false; ui->spotlightRadiusSlider->setValue(int(spotlightRadius()));
            gestureScale=1.0f;
          break;
        case Qt::ZoomNativeGesture:
        {
          {
//            auto n=ui->spotlightRadiusSlider->value();
//            if (gesture->value())
              ui->spotlightRadiusLabel->setText(QString::number(int(spotlightRadius())));
              gestureScale*=(1+gesture->value());
//            else
//              scaling-=n;
            qDebug()<<gestureScale;
//            if (gesture->value())
//              on_spotlightRadiusSlider_sliderMoved(int(scaling));
//            else
//              on_spotlightRadiusSlider_sliderMoved(n--);
            refresh();
            drWindow->updateViews();
          }
        }
        case Qt::SmartZoomNativeGesture:
          {
//            auto n=ui->spotlightRadiusSlider->value();
//            qDebug()<<n;
//            if (gesture->value())
//              on_spotlightRadiusSlider_sliderMoved(n+10);
//            else
//              on_spotlightRadiusSlider_sliderMoved(n-10);
//            refresh();
          }
          // if (gesture->value())
          //   glViewController.properties.zoom *= 0.5f;
          // else
          //   glViewController.properties.zoom /= 0.5f;
          // update();
          break;
        case Qt::RotateNativeGesture:
        {
          // glm::vec3 R(0,0,1);
          // if (shaderdata)
          //   R=glm::vec3(shaderdata->eye.x,shaderdata->eye.y,shaderdata->eye.z);
          // glViewController.rotateAroundVector(-float(gesture->value()),R);
          // update();
        }
        break;
        default:
          break;
        }
      }
    }
    break;
    case QEvent::Gesture:
    {
      auto gesture = dynamic_cast<QGestureEvent*>(event);
      if (gesture)
      {
        //          qDebug()<<"(gestures mildly) "<<gesture;//-> //gestureType()<<" @ "<<gesture->pos()<<" : "<<gesture->value();
      }
    }
    break;
    default:
      //    qDebug()<<"(HUH) "<<event;//<<gesture-> //gestureType()<<" @ "<<gesture->pos()<<" : "<<gesture->value();
      break;
  }
  return QWidget::event(event);
}

void RelaxationToolboxForm::updatePosition()
{
  updateSpectrumImage(drWindow->currentVolumePosition.x,drWindow->currentVolumePosition.y,drWindow->currentVolumePosition.z);
  updateInfo();
  update();
}

QString makeAxisLabel(AxisInfo &info)
{
  return info.units.empty() ? info.name.c_str() : (info.name +" ("+info.units+")").c_str() ;
}

void RelaxationToolboxForm::updateAxisLabelInfo()
{
  spectralAxes=std::vector<AxisInfo>();
  auto &axes=drWindow->spectralImageInfo.axes;
  for (size_t i=0;i<drWindow->spectralImageInfo.axes.size();i++)
  {
    if (axes[i].dimtype==AxisInfo::DimType::Spectral)
    {
      spectralAxes.push_back(axes[i]);
    }
  }
  for (size_t i=0;i<spectralAxes.size();i++)
    qDebug()<<"spectral axis: "<<i<<spectralAxes[i].name;
  switch (spectralAxes.size())
  {
    case 0:
      uAxisValues={};
      vAxisValues={};
      ui->uAxisLabel->setText("u");
      ui->vAxisLabel->setText("v"); break;
      break; // no axis info
    case 1:
      uAxisValues=spectralAxes[0].samples;
      vAxisValues={};
      ui->uAxisLabel->setText(makeAxisLabel(spectralAxes[0]));
      ui->vAxisLabel->setText("v"); break;
    case 2:
    default:
      uAxisValues=spectralAxes[0].samples;
      vAxisValues=spectralAxes[1].samples;
      ui->uAxisLabel->setText(makeAxisLabel(spectralAxes[0]));
      ui->vAxisLabel->setText(makeAxisLabel(spectralAxes[1])); break;
  }
  ui->uaxisBlankSpace->setVisible(uAxisValues.size()>2);
  ui->vaxisBlankSpace->setVisible(vAxisValues.size()>2);
}

void RelaxationToolboxForm::updateInfo()
{
  ui->infoLabel->hide();
  {
    std::ostringstream ostr;
    ostr<<"\n["
        <<drWindow->currentVolumePosition.x<<","
        <<drWindow->currentVolumePosition.y<<","
        <<drWindow->currentVolumePosition.z<<","
        <<drWindow->currentSpectralPosition.u<<","
        <<drWindow->currentSpectralPosition.v<<"]";
    ostr<<"\n"<<coordInfoU(drWindow->currentSpectralPosition.u);
    ostr<<", "<<coordInfoV(drWindow->currentSpectralPosition.v);
    ui->positionInfo->setText(ostr.str().c_str());
  }
}

QString formatAxisLabel(float value)
{
  return QString::number(value,'g',4);
}


void stencilOutline(DSImage &stencilBitmap, const DSImage &labelBitmap)
{
  const uint32_t stencilColor = 0xFF00FF00; // include alpha channel
  const uint32_t backgroundColor = 0x0;
  stencilBitmap.resize(labelBitmap.size());
  const uint32_t *src = reinterpret_cast<const uint32_t *>(labelBitmap.image.bits());
  uint32_t *dst = reinterpret_cast<uint32_t *>(stencilBitmap.image.bits());
  const int cx = stencilBitmap.size().width();
  const int cy = stencilBitmap.size().height();
  if (cy>0)
    for (int x=0;x<cx;x++) *dst++ = (*src++!=0) ? stencilColor : backgroundColor;
  for (int y=1;y<cy-1;y++)
  {
    *dst++ = (*src++!=0) ? stencilColor : backgroundColor;
    for (int x=1;x<cx-1;x++)
    {
      const uint32_t v = *src;
      *dst++ = ((v!=0) && ((src[-1]!=v)||(src[+1]!=v)||(src[-cx]!=v)||(src[+cx]!=v))) ? stencilColor : backgroundColor;
      src++;
    }
    *dst++ = (*src++!=0) ? stencilColor : backgroundColor;
  }
  if (cy>0)
    for (int x=0;x<cx;x++) *dst++ = (*src++!=0) ? stencilColor : backgroundColor;
}

//spotlightRadiusLabel

void RelaxationToolboxForm::updateSpectrumImage(int x, int y, int z)
{
  if (!drWindow->spectrum()) return;
  if (showAverageSpectrum()&& drWindow->avgSpectrum())
  {
    copyAvgSpectralSlice(spectralSlice,drWindow->avgSpectrum());
  }
  else
    getSpectralSlice(spectralSlice,*(drWindow->spectrum()),x,y,z);

  auto &bitmap=ui->spectralImageWidget->spectrumBitmap;
  bitmap.resize(spectralSlice.cx,spectralSlice.cy);
  if (bitmap.npix()==0) return;

  ui->spectralImageWidget->plotSpectrumImage=ui->showSpectrumImageCheckBox->isChecked();
  if (ui->showSpectrumImageCheckBox->isChecked())
  {
    RangeLUTOp op(drWindow->spectrumLUT(),0,drWindow->spectrumScale());
    renderPointSpectrum(bitmap,spectralSlice,op);
  }
  auto *vComp=dynamic_cast<VolND<uint8_t> *>(drWindow->componentMask());
  ui->spectralImageWidget->plotComponent=false;
  ui->spectralImageWidget->plotComposite=false;
  if (vComp)
  {
    if (ui->componentButton->isChecked())
    {
      ui->spectralImageWidget->plotComponent=true;
      RangeLUTOp op(ColorMap::greenLUT,0,1);
      SliceT<uint8_t> componentSlice;
      activeComponent=std::clamp<int>(activeComponent,0,vComp->nz()-1);
      copySpectralSlice(componentSlice,vComp,activeComponent);
      renderPointSpectrum(ui->spectralImageWidget->componentBitmap,componentSlice,op);
      stencilOutline(ui->spectralImageWidget->componentOutlineBitmap,ui->spectralImageWidget->componentBitmap);
    }
    else
    {
      ui->spectralImageWidget->plotComposite=true;
      DSImage composite;
      for (size_t component=0;component<vComp->nz();component++)
      {
        RangeLUTOp op(ColorMap::greenLUT,0,1);
        SliceT<uint8_t> componentSlice;
        activeComponent=std::clamp<int>(activeComponent,0,vComp->nz()-1);
        copySpectralSlice(componentSlice,vComp,activeComponent);
        renderPointSpectrum(ui->spectralImageWidget->componentBitmap,componentSlice,op);
        stencilOutline(ui->spectralImageWidget->componentOutlineBitmap,ui->spectralImageWidget->componentBitmap);
      }
    }
  }
  ui->spectralImageWidget->plotContours=ui->contourCheckBox->isChecked();
  if (ui->spectralImageWidget->plotContours)
  {
    contourLevels.resize(nContours);
    for (int i=0;i<nContours;i++)
    {
      contourLevels[i].value=i*(drWindow->spectrumScale())/float(nContours-1);
      MarchingSquares::march(contourLevels[i].contours,spectralSlice,contourLevels[i].value,ui->interpolateContoursCheckBox->isChecked());
    }
  }
  ui->colorbarWidget->setImageScale(drWindow->spectrumScale());
  ui->spectralImageWidget->darkBackground=ui->darkBackGroundCheckBox->isChecked();
  ui->spectralImageWidget->update();
}

std::string RelaxationToolboxForm::coordInfoU(int u)
{
  std::ostringstream ostr;
  if (spectralAxes.size()>0)
  {
    if (u>=0 && u < (int)spectralAxes[0].samples.size())
    {
      ostr<<spectralAxes[0].name<<"="<<spectralAxes[0].samples[u]<<" "<<spectralAxes[0].units;
    }
    else ostr<<"<?>";
  }
  else ostr<<"u="<<u;
  return ostr.str();
}

std::string RelaxationToolboxForm::coordInfoV(int v)
{
  std::ostringstream ostr;
  if (spectralAxes.size()>0)
  {
    if (v>=0 && v < (int)spectralAxes[1].samples.size())
    {
      ostr<<spectralAxes[1].name<<"="<<spectralAxes[1].samples[v]<<" "<<spectralAxes[1].units;
    }
    else ostr<<"<?>";
  }
  else ostr<<"v="<<v;
  return ostr.str();
}


bool RelaxationToolboxForm::showAverageSpectrum()
{
  return ui->showAverageSpectrumCheckBox->isChecked();
}

bool RelaxationToolboxForm::showContours()
{
  return ui->contourCheckBox->isChecked();
}

void RelaxationToolboxForm::updateLUT()
{
  updateSpectrumImage(drWindow->currentVolumePosition.x,drWindow->currentVolumePosition.y,drWindow->currentVolumePosition.z);
  ui->colorbarWidget->setLUT(drWindow->spectrumLUT());
  ui->colorbarWidget->setImageScale(drWindow->spectrumScale());
  if (showContours())
  {
    ui->colorbarWidget->showCustomTicks=true;
    ui->colorbarWidget->customTicks.clear();
    for (auto &c : contourLevels) ui->colorbarWidget->customTicks.push_back(c.value);
  }
  else
  {
    ui->colorbarWidget->showCustomTicks=false;
  }
  update();
}


void RelaxationToolboxForm::drawVAxisTick(QPainter &painter, const QPoint &p, const float value, bool blank)
{
  const int tickwidth=10;
  const auto s=QString::number(value,'g',4);
  const auto textbox=painter.fontMetrics().boundingRect(s);
  const auto position=QPoint(p.x()-textbox.width()-tickwidth-2,p.y()+textbox.height()/2-2);
  if (blank)
  {
    painter.fillRect(QRect(textbox.topLeft(),textbox.bottomRight()+QPoint(4,2)).translated(position-QPoint(4,2)),QBrush(Qt::gray, Qt::SolidPattern));
  }
  painter.drawText(position,s);
  painter.drawLine(p.x()-tickwidth,p.y(),p.x(),p.y());
}

void RelaxationToolboxForm::drawHAxisTick(QPainter &painter, const QPoint &p, const float value, bool blank)
{
  const int tickheight=10;
  const auto s=QString::number(value,'g',4);
  const auto textbox=painter.fontMetrics().boundingRect(s);
  const auto position=QPoint(p.x()-textbox.width()/2,p.y()+textbox.height()+tickheight+2);
  if (blank)
  {
    painter.fillRect(QRect(textbox.topLeft(),textbox.bottomRight()+QPoint(8,4)).translated(position-QPoint(4,2)),QBrush(Qt::gray, Qt::SolidPattern));
  }
  painter.drawText(position,s);
  painter.drawLine(p.x(),p.y(),p.x(),p.y()+tickheight);
}

void RelaxationToolboxForm::paintEvent(QPaintEvent *ev)
{
  auto *vSpectral=drWindow->spectrum();
  float nu=(vSpectral) ? vSpectral->nv() : 0;
  float nv=(vSpectral) ? vSpectral->nv() : 0;
  auto origin=ui->spectralImageWidget->mapTo(this,ui->spectralImageWidget->spectrumToScreenCoordinates(UVPoint(0,0)));
  auto maxpoint=ui->spectralImageWidget->mapTo(this,ui->spectralImageWidget->spectrumToScreenCoordinates(UVPoint(nu-1,nv-1)));
  auto center=ui->spectralImageWidget->mapTo(this,ui->spectralImageWidget->spectrumToScreenCoordinates(UVPoint(nu/2,nv/2)));

  QWidget::paintEvent(ev);
  const auto tick=ui->spectralImageWidget->spectrumToScreenCoordinates(drWindow->currentSpectralPosition);
  QPainter painter(this);

  auto corner=ui->spectralImageWidget->mapTo(this,ui->spectralImageWidget->rect().bottomLeft());
  const auto nUValues=uAxisValues.size();
  if (nUValues>0)
  {
    drawHAxisTick(painter,QPoint(origin.x(),corner.y()),uAxisValues.front());
    if (nUValues>1) drawHAxisTick(painter,QPoint(maxpoint.x(),corner.y()),uAxisValues.back());
    if (nUValues>2) drawHAxisTick(painter,QPoint(center.x(),corner.y()),uAxisValues[nUValues/2]);
  }
  else
  {
    drawHAxisTick(painter,QPoint(origin.x(),corner.y()),0);
    if (nu>1) drawHAxisTick(painter,QPoint(maxpoint.x(),corner.y()),nu-1);
  }
  const auto nVValues=vAxisValues.size();
  if (nVValues>0)
  {
    drawVAxisTick(painter,QPoint(corner.x(),origin.y()),
                  vAxisValues.size()>0 ? vAxisValues.front() : 0);
    if (nVValues>1) drawVAxisTick(painter,QPoint(corner.x(),maxpoint.y()),vAxisValues.back());
    if (nVValues>2) drawVAxisTick(painter,QPoint(corner.x(),center.y()),vAxisValues[nVValues/2]);
  }
  else
  {
    drawVAxisTick(painter,QPoint(corner.x(),origin.y()),0);
    if (nv>1) drawVAxisTick(painter,QPoint(corner.x(),maxpoint.y()),nv-1);
  }
  if (vSpectral)
  {
    auto cursorTick=ui->spectralImageWidget->mapTo(this,tick);
    QPen pen(Qt::white, 0.5, Qt::DashLine);
    painter.setPen(pen);
    if (drWindow->currentSpectralPosition.u>=0 && drWindow->currentSpectralPosition.u < uAxisValues.size())
      drawHAxisTick(painter,QPoint(cursorTick.x(),corner.y()),uAxisValues[drWindow->currentSpectralPosition.u],true);
    if (drWindow->currentSpectralPosition.v>=0 && drWindow->currentSpectralPosition.v < vAxisValues.size())
      drawVAxisTick(painter,QPoint(corner.x(),cursorTick.y()),vAxisValues[drWindow->currentSpectralPosition.v],true);
  }

}


void RelaxationToolboxForm::mousePressEvent(QMouseEvent *event)
{
  if (event->buttons() & Qt::RightButton)
  {
    drWindow->makeSpectralPopupMenu(event->globalPosition().toPoint());
  }
  else
  {
    auto *vSpectral=drWindow->spectrum();
    if (!vSpectral) return;
    auto spectralPosition=ui->spectralImageWidget->mapFromParent(event->pos());
    if (spectralPosition.x( )< 0)// && spectralPosition.x()>=-20)
    {
      if (spectralPosition.y()>=0 && spectralPosition.y()<=ui->spectralImageWidget->rect().bottom())
      {
        tracking=-1; // for y axis
        mouseMoveEvent(event);
      }
    }
    else if (spectralPosition.y( )>ui->spectralImageWidget->rect().bottom() && spectralPosition.x()>=0 && spectralPosition.x()<=ui->spectralImageWidget->rect().right())
    {
      tracking=1; // for y axis
      mouseMoveEvent(event);
    }
  }
}


void RelaxationToolboxForm::mouseReleaseEvent(QMouseEvent * /*event*/)
{
  tracking=0;
  releaseKeyboard();
  if (drWindow) drWindow->updateViews();
}

void RelaxationToolboxForm::mouseMoveEvent(QMouseEvent *event)
{
  auto vSpectral=drWindow->spectrum();
  if (!vSpectral) return;
  if (!tracking ) return;
  auto spectralPosition=ui->spectralImageWidget->mapFromParent(event->pos());
  if (tracking<0)
  {
    auto v=(1-ui->spectralImageWidget->mapToSpectrum(QPoint(0,std::clamp(spectralPosition.y(),0,ui->spectralImageWidget->rect().bottom()-1))).v)*vSpectral->nv();
    if (v>=vSpectral->nv()) v=vSpectral->nv()-1;
    updatePosition();
    update();

  }
  else if (tracking>0)
  {
    const auto dx=vSpectral->nv();
    auto u=static_cast<int>(ui->spectralImageWidget->mapToSpectrum(QPoint(spectralPosition.x(),0)).u*dx);
    if (u<0) u=0;
    if (u>static_cast<int>(dx-1)) u=static_cast<int>(dx-1);
    updatePosition();
    update();
  }
}

void RelaxationToolboxForm::on_axisPlotCheckBox_clicked(bool checked)
{
  ui->vPlotWidget->setVisible(checked);
  ui->uPlotWidget->setVisible(checked);
  update();
}

void RelaxationToolboxForm::refresh()
{
  updatePosition();
}

void RelaxationToolboxForm::on_colorCheckBox_clicked(bool checked)
{
  ui->uPlotWidget->plotColor=checked;
  ui->vPlotWidget->plotColor=checked;
  if (ui->uPlotWidget->isVisible() || ui->vPlotWidget->isVisible())
  {
    repaint();
  }
}


void RelaxationToolboxForm::on_integrate3DCheckBox_clicked(bool /*checked*/)
{
  if (drWindow) drWindow->updateViews();
}


void RelaxationToolboxForm::on_contourCheckBox_clicked(bool /*checked*/)
{
  refresh();
}


void RelaxationToolboxForm::on_showSpectrumImageCheckBox_clicked(bool /*checked*/)
{
  refresh();
}


void RelaxationToolboxForm::on_interpolateContoursCheckBox_clicked(bool /*checked*/)
{
  refresh();
}


void RelaxationToolboxForm::on_darkBackGroundCheckBox_clicked(bool /*checked*/)
{
  ui->vPlotWidget->darkBackground=ui->darkBackGroundCheckBox->isChecked();
  ui->uPlotWidget->darkBackground=ui->darkBackGroundCheckBox->isChecked();
  ui->spectralImageWidget->darkBackground=ui->darkBackGroundCheckBox->isChecked();
  refresh();
}


void RelaxationToolboxForm::on_showContourTickLabelsCheckBox_clicked(bool /*checked*/)
{
  ui->colorbarWidget->showCustomTicks      =ui->showContourTickLabelsCheckBox->isChecked();
  ui->colorbarWidget->showContourTickLabels=ui->showContourTickLabelsCheckBox->isChecked();
  refresh();
}

void RelaxationToolboxForm::setComponentMaskText(const QString &filename)
{
  ui->maskFilenameLabel->setText(shortName(filename));
  ui->maskFilenameLabel->setToolTip(filename);
}


void RelaxationToolboxForm::setSpectralImageText(const QString &filename)
{
  ui->filenameLabel->setText(shortName(filename));
  ui->filenameLabel->setToolTip(filename);
}

void RelaxationToolboxForm::on_nContoursSpinBox_valueChanged(int arg1)
{
  nContours=arg1;
  refresh();
}

void RelaxationToolboxForm::on_zSliceSlider_sliderMoved(int position)
{
  drWindow->moveToSlice(position);
}


void RelaxationToolboxForm::on_zSliceEdit_editingFinished()
{
  int slice=std::atoi(ui->zSliceEdit->displayText().toStdString().c_str());
  drWindow->moveToSlice(slice);
}


void RelaxationToolboxForm::on_showAverageSpectrumCheckBox_toggled(bool /*checked*/)
{
  refresh();
}


void RelaxationToolboxForm::on_loadMaskButton_clicked()
{
    drWindow->readComponentMask();
}

void RelaxationToolboxForm::on_componentSlider_sliderMoved(int position)
{
  selectComponent(position);
}

int RelaxationToolboxForm::showComponent()
{
  return (ui->componentButton->isChecked()) ? ui->componentSlider->value() : -1;
}

float RelaxationToolboxForm::spotlightRadius()
{
  return ui->spotlightButton->isChecked() ?
        gestureScale*ui->spotlightRadiusSlider->value() : 0.0f;
}

void RelaxationToolboxForm::on_spotlightButton_toggled(bool /*checked*/)
{
  refresh();
  drWindow->updateViews();
}

void RelaxationToolboxForm::on_spotlightRadiusSlider_sliderReleased()
{
  ui->spotlightRadiusLabel->setText(QString::number(ui->spotlightRadiusSlider->value()));
  refresh();
  drWindow->updateViews();
}

void RelaxationToolboxForm::on_spotlightRadiusSlider_valueChanged(int /*value*/)
{
  ui->spotlightRadiusLabel->setText(QString::number(ui->spotlightRadiusSlider->value()));
  refresh();
  drWindow->updateViews();
}


void RelaxationToolboxForm::on_componentButton_toggled(bool /*checked*/)
{
  refresh();
//  drWindow->reloadSSOs();
  drWindow->updateViews();
}


void RelaxationToolboxForm::on_componentSlider_sliderPressed()
{
    ui->componentButton->setChecked(true);
    activeComponent=std::clamp<int>(ui->componentSlider->value(),0,drWindow->componentMask() ? drWindow->componentMask()->nz()-1 : 0);
    selectComponent(activeComponent);
}


void RelaxationToolboxForm::on_spotlightRadiusSlider_sliderPressed()
{
  ui->spotlightButton->setChecked(true);
}


void RelaxationToolboxForm::on_componentSlider_sliderReleased()
{
  activeComponent=std::clamp<int>(ui->componentSlider->value(),0,drWindow->componentMask() ? drWindow->componentMask()->nz()-1 : 0);
  ui->componentSlider->setValue(activeComponent);
  ui->componentLabel->setText(QString::number(activeComponent));
  qDebug()<<"active comp:"<<activeComponent;
  refresh();
  drWindow->updateViews();
}


void RelaxationToolboxForm::on_compositeViewButton_clicked()
{
  switch (drWindow->viewMode)
  {
    case DRWindow::FourUpIndividualSpectra:
    case DRWindow::FourUpIntegratedSpectra:
    case DRWindow::FourUpComponent:
      drWindow->setViewMode(DRWindow::SixUpComposite,true); break;
    case DRWindow::SixUpComposite:
    default:
      drWindow->setViewMode(DRWindow::FourUpIntegratedSpectra,true); break;
  }
}

