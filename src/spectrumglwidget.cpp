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

#include <QtOpenGL>
#include <QMouseEvent>
#include "spectrumglwidget.h"
#include "drwindow.h"
#include <glm/glm.hpp>
#include <checkglerror.h>
#include <QKeyEvent>
#include <relaxationtoolboxform.h>
#include <uvpoint.h>
#include <QApplication>
#include <memory>

void SpectrumGLWidget::setBaseColor(glm::vec3 color)
{
  basecolor=color;
  const size_t n=componentLUT.size();
  for (size_t i=0;i<n;i++)
  {
    uint32_t r8=std::clamp<uint32_t>(basecolor.x*i,0,0xFF),
        g8=std::clamp<uint32_t>(basecolor.y*i,0,0xFF),
        b8=std::clamp<uint32_t>(basecolor.z*i,0,0xFF);
    componentLUT[i]=((r8<<16)&0xFF0000)|((g8<<8)&0xFF00)|(b8&0xFF);
  }
  colorBar->setLUT(&componentLUT[0]);
}

SpectrumGLWidget::SpectrumGLWidget(QWidget* parent, Qt::WindowFlags f) : QOpenGLWidget(parent,f)
{
  colorBar=std::make_unique<ColorBar>(this);
  spectrumColorBar=std::make_unique<ColorBar>(this);
  setBaseColor(basecolor);
  colorBar->hide();
  spectrumColorBar->hide();
}

void SpectrumGLWidget::resizeGL(int w, int h)
{
  viewController.setup(w,h);
}

float SpectrumGLWidget::currentScale()
{
  switch (drWindow->viewMode)
  {
    case DRWindow::FourUpIndividualSpectra:
      return drWindow->maxFullSpectrum;
    case DRWindow::FourUpIntegratedSpectra:
      return drWindow->maxIntegratedSpatialImage;
    case DRWindow::FourUpComponent:
      if (showComponent>=0 && showComponent<(int)drWindow->maxComponentIntensities.size())
        return drWindow->maxComponentIntensities[showComponent];
      else
        qDebug()<<"shouldn't get here : FourUpComponent "<<showComponent<<drWindow->maxComponentIntensities.size();
      break;
    case DRWindow::SixUpComposite:
      if (showComposite>0||showComponent>=0)
      {
        if (drWindow->maxComponentIntensities.size()>0)
          return drWindow->maxComponentIntensities[0];
      }
      else return drWindow->maxIntegratedSpatialImage;
  }
  return drWindow->maxFullSpectrum; // shouldn't reach this
}

void SpectrumGLWidget::initVolumeShader(DSShaderProgram *shader)
{
  if (!drWindow) return;
  if (!shader) return;
  const auto *volume=drWindow->spectrum();
  if (!volume) return;
  shader->bind();CHECK_GL
  UVPoint uv=drWindow->currentSpectralPosition;
  bool showSpectralOverlay=drWindow->showSpectralOverlay;
  std::ostringstream info;
  switch (drWindow->viewMode)
  {
    case DRWindow::FourUpIndividualSpectra:
      info<<"Spectroscopic Image\n"<<drWindow->coordInfoU()<<","<<drWindow->coordInfoV();
      break;
    case DRWindow::FourUpIntegratedSpectra:
      info<<"Spectroscopic Image\n(integrated)";
      break;
    case DRWindow::FourUpComponent:
      if (showComponent>=0)
      {
        info<<"Component "<<showComponent;
      }
      else
      {
        qDebug()<<"warning: should not be in component mode!";
      }
      break;
    case DRWindow::SixUpComposite:
      if (showComponent>=0)
      {
        info<<"Component "<<showComponent;
        useBasecolor=true;
      }
      else if (showComposite>0)
      {
        info<<"Composite Image";
      }
      else
      {
        info<<"Spectroscopic Image";
      }
      break;
  }
  if (drWindow->showSpectralOverlay)
  {
    if (this->showUPlot)
    {
      info<<"\nplotting spectrum for "<<drWindow->coordInfoV();
    }
    if (this->showVPlot)
    {
      info<<"\nplotting spectrum for "<<drWindow->coordInfoU();
    }
  }
  titleText=info.str().c_str();
  int useBasecolor=(drWindow->viewMode==DRWindow::SixUpComposite && showComponent>=0) ? 1 : 0;
  shader->setUniformValue("useBasecolor",useBasecolor);
  glm::vec4 weights(0.25f,0.25f,0.25f,0.25f);
  if (drWindow->viewMode==DRWindow::SixUpComposite && showComposite>0)
  {
    if (drWindow->spectrumWindows.size()>=4)
    {
      weights.x = 1.0f/drWindow->windowgrid(0,0)->viewController.scaleY();
      weights.y = 1.0f/drWindow->windowgrid(0,1)->viewController.scaleY();
      weights.z = 1.0f/drWindow->windowgrid(1,0)->viewController.scaleY();
      weights.w = 1.0f/drWindow->windowgrid(1,1)->viewController.scaleY();
    }
  }
  glUniform3f(shader->uniformLocation("basecolor"), basecolor.x,basecolor.y,basecolor.z);CHECK_GL
  glUniform4f(shader->uniformLocation("componentWeights"), weights.x,weights.y,weights.z,weights.w);CHECK_GL
  shader->setUniformValue("matrix", projectionMatrix*modelMatrix);
  shader->setUniformValue("spotlightRadius", drWindow->spotlightRadius());CHECK_GL
  shader->setUniformValue("showComposite", showComposite);CHECK_GL
  shader->setUniformValue("showComponent", showComponent);CHECK_GL
  shader->setUniformValue("ncomponents", (int)drWindow->nComponents());CHECK_GL
  shader->setUniformValue("scale1Max", currentScale()*viewController.scaleY());CHECK_GL
  shader->setUniformValue("scale2Max", drWindow->spectrumScale());CHECK_GL
  glUniform3i(shader->uniformLocation("voxeldimensions"), GLint(volume->nx()),GLint(volume->ny()),GLint(volume->nz()));CHECK_GL
  glUniform3f(shader->uniformLocation("voxelresolution"), GLfloat(volume->rx()),GLfloat(volume->ry()),GLfloat(volume->rz()));CHECK_GL
  glUniform2f(shader->uniformLocation("spectraldimensions"), volume->nu(),volume->nv());CHECK_GL
  shader->setUniformValue("gridwidth",drWindow->gridwidth());CHECK_GL
  glUniform2f(shader->uniformLocation("spectralcoord"), uv.u,uv.v);CHECK_GL
  shader->setUniformValue("showSpectra", showSpectralOverlay&&showSpectra);CHECK_GL
  shader->setUniformValue("showUPlot", showSpectralOverlay&&showUPlot);CHECK_GL
  shader->setUniformValue("showVPlot", showSpectralOverlay&&showVPlot);CHECK_GL
}

void SpectrumGLWidget::renderSpectrum(SpectroscopicShaderObject &volumeShaderObject)
{
  auto shader = spectrumShader.get();
  if (!shader) return;
  auto *volume=drWindow->spectrum();
  if (!volume) return;
  initializeOpenGLFunctions();
  bool smoothSlices=false;
  volumeShaderObject.showXYPlane = true;
  volumeShaderObject.showXZPlane = true;
  volumeShaderObject.showYZPlane = true;
  CHECK_GL
  glm::vec3 p(drWindow->currentVolumePosition.x*volume->rx(),
              drWindow->currentVolumePosition.y*volume->ry(),
              drWindow->currentVolumePosition.z*volume->rz());
  volumeShaderObject.generateOrthogonalSlicePlanes(p);
  CHECK_GL
  initVolumeShader(shader);
  CHECK_GL
  volumeShaderObject.setTextures(*shader,smoothSlices);    CHECK_GL
  volumeShaderObject.draw(false);  CHECK_GL
}

void SpectrumGLWidget::renderSpectrum()
{
  this->initializeOpenGLFunctions();
  if (!drWindow) return;
  auto vSpectrum=drWindow->spectrum();
  if (!vSpectrum) return;
  if (!spectrumShader)
  {
    qDebug()<<objectName()<<"no shader available!";
    return;
  }
  CHECK_GL
  if (drWindow->useLocalSSOs())
  {
    renderSpectrum(localSSO); CHECK_GL
  }
  else
  {
    renderSpectrum(drWindow->volumeShaderObject); CHECK_GL
  }
}

void SpectrumGLWidget::loadShaders()
{
  if (!drWindow) return;
  std::string shaderpath=drWindow->currentShaderpath();
  DSShaderProgram::loadShader(spectrumShader,shaderpath+"spectrum.vert",shaderpath+"spectrum.frag","");
  CHECK_GL
}

void SpectrumGLWidget::initializeGL()
{
  if (!drWindow) return;
  initializeOpenGLFunctions();
  QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
  f->glClearColor(0,0,0,0);
  loadShaders();
  return;
}

void SpectrumGLWidget::updateImageColorbar()
{
  if (useBasecolor)
  {
    setBaseColor(basecolor);
  }
  else
    colorBar->setLUT(drWindow->imageLUT());
}

void SpectrumGLWidget::updateSpectrumColorbar()
{
  spectrumColorBar->setLUT(drWindow->spectrumLUT());
}

void SpectrumGLWidget::updateProjectionMatrix()
{
  const auto aspect = viewController.aspectRatio();
  float xShiftScreen = -static_cast<float>(devicePixelRatio())*viewController.shift.x; // shift is in normalized coords
  float yShiftScreen = -static_cast<float>(devicePixelRatio())*viewController.shift.y;
  projectionMatrix.setToIdentity();
  float zm=1.0f/viewController.zoom;
  projectionMatrix.ortho(zm*(xShiftScreen-aspect),zm*(xShiftScreen+aspect),zm*(yShiftScreen-1),zm*(yShiftScreen+1),-1000,1000);
  modelMatrix.setToIdentity();
  if (drWindow->spectrum())
  {
    auto center=(drWindow->useLocalSSOs()) ? localSSO.center() :
                                             drWindow->volumeShaderObject.center();
    modelMatrix.translate(-center.x,-center.y,-center.z);
  }
}

void SpectrumGLWidget::paintGL()
{
  if (!drWindow) return;
  QPainter painter(this);
  painter.beginNativePainting();
  initializeOpenGLFunctions();
  updateProjectionMatrix();
  CHECK_GL
  const qreal retinaScale = devicePixelRatio();
  glViewport(0, 0, width() * retinaScale, height() * retinaScale); CHECK_GL
  glClear(GL_COLOR_BUFFER_BIT); CHECK_GL
  if (drWindow->spectrum())
  {
    renderSpectrum();
  }
  painter.endNativePainting();
  if (drWindow->showImageInfo && drWindow->spectrum())
  {
    glDisable(GL_DEPTH_TEST);
    painter.setPen(Qt::white);
    QFont font = QApplication::font();
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawText(0, 0, width(), height(), Qt::AlignLeft| Qt::AlignTop | Qt::TextWordWrap, titleText);
    painter.drawText(0, 0, width(), height(), Qt::AlignRight| Qt::AlignTop| Qt::TextWordWrap,
                     QString("z=")+QString::number(drWindow->currentVolumePosition.z));
    if (!showComposite)
    {
      colorBar->align=Qt::AlignRight;
      colorBar->showTicks=false;
      colorBar->paintRegion(painter,QRect(width()-25,height()/2,20,height()/2),0,
                            currentScale()*viewController.scaleY()); // TODO : flag for using this if in composite mode
    }
    if (drWindow->showSpectralOverlay)
    if (showSpectra||showUPlot||showVPlot)
    {
      spectrumColorBar->align=Qt::AlignLeft;
      spectrumColorBar->showTicks=false;
      spectrumColorBar->paintRegion(painter,QRect(0,height()/2,20,height()/2),0,drWindow->spectrumScale());
    }
    glEnable(GL_DEPTH_TEST);
  }
  painter.end();
}

glm::vec3 SpectrumGLWidget::getOpenGLPosition(const QPoint &p)
{
  makeCurrent();
  GLint winX = GLint(p.x()*devicePixelRatio());
  GLint winY = GLint((height() - p.y())*devicePixelRatio());
  GLfloat winZ=0; // for orthoview / flat surface, we don't need z-plane and can skip reading pixels!
//  glReadPixels( winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );
  auto point=QVector3D(GLfloat(winX), GLfloat(winY), winZ).unproject(modelMatrix,projectionMatrix,QRect(0, 0, GLint(width()*devicePixelRatio()), GLint(height()*devicePixelRatio())));
  return glm::vec3(point.x(), point.y(), point.z());
}

void SpectrumGLWidget::keyPressEvent(QKeyEvent *event)
{
  qDebug()<<event->key();
}

void SpectrumGLWidget::mousePressEvent(QMouseEvent *event)
{
  clickPoint = event->position().toPoint();
  if (event->buttons() & Qt::RightButton)
  {
    drWindow->makeSpectralPopupMenu(event->globalPosition().toPoint());
  }
  else
  {
    if (!viewController.mouseDown(clickPoint))
    {
      if (event->buttons() & Qt::LeftButton & !event->modifiers())
      {
        drWindow->mouseTrack(getOpenGLPosition(event->position().toPoint()));
      }
    }
  }
}

void SpectrumGLWidget::mouseDoubleClickEvent(QMouseEvent * event)
{
  if (event->modifiers()&Qt::ControlModifier)
  {
    drWindow->toggleSingleWindow(this);
  }
}

void SpectrumGLWidget::mouseMoveEvent(QMouseEvent *event)
{
  clickPoint = event->position().toPoint();
  worldClickPoint=getOpenGLPosition(clickPoint);
  if (event->buttons())
  {
    if (viewController.mouseMoveT(event->pos(),event->buttons(),event->modifiers()))
    {
      drWindow->syncWindows(this);
    }
    else if (event->buttons() & Qt::LeftButton & !event->modifiers())
    {
      drWindow->mouseTrack(getOpenGLPosition(event->position().toPoint()));
      update();
    }
  }
  else
  {
  }
}
