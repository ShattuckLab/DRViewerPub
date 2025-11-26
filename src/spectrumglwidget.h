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

#ifndef SPECTRUMGLWIDGET_H
#define SPECTRUMGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_1_Core>
#include <QOpenGLFunctions_4_1_Compatibility>
#include <QObject>
#include <QWidget>
#include <spectroscopicshaderobject.h>
#include <dsshaderprogram.h>
#include <QOpenGLBuffer>
#include <spectrumviewcontroller.h>
#include <colormap.h>
#include <colorbar.h>
#include <array>

class DRWindow;

class SpectrumGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_1_Core
{
  Q_OBJECT
public:
  class VAO {
  public:
    VAO() : vao(0), vbo(0), ibo(0) {}
    GLuint vao;
    GLuint vbo;
    GLuint ibo;
  };
  class ShaderData;
  SpectrumGLWidget(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
  std::unique_ptr<ColorBar> colorBar,spectrumColorBar;
  void setBaseColor(glm::vec3 color);
  QString titleText;
  void attach(DRWindow *drWindow) { this->drWindow=drWindow; }
  void loadShaders();
  int showSpectra=0;
  int showUPlot=0;
  int showVPlot=0;
  int showComposite=0;
  float currentScale();
  SpectrumViewController viewController;
  SpectroscopicShaderObject localSSO;
  glm::vec3 getOpenGLPosition(const QPoint &p);
  int showComponent=-1;
  glm::vec3 basecolor=glm::vec3(1.0f,1.0f,1.0f);
  float localBrightness=1;
  void updateImageColorbar();
  void updateSpectrumColorbar();
  bool useBasecolor=false;
protected:
  void renderSpectrum(SpectroscopicShaderObject &vso);
  void renderSpectrum();
  void updateProjectionMatrix();
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int width, int height) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
  void initVolumeShader(DSShaderProgram *shader);

  bool event(QEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent * event) override;
  QMatrix4x4 projectionMatrix;
  QMatrix4x4 modelMatrix;
  std::array<uint32_t,256> componentLUT;
private:
  QPoint clickPoint;
  glm::vec3 worldClickPoint;
  unsigned int uboCommonBlock=0;
  std::unique_ptr<DSShaderProgram> spectrumShader;
  VAO testTriangle;
  DRWindow *drWindow=nullptr;
};

#endif // SPECTRUMGLWIDGET_H
