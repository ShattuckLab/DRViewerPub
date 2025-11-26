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

#ifndef RELAXATIONTOOLBOXFORM_H
#define RELAXATIONTOOLBOXFORM_H

#include <QWidget>
#include <volnd.h>
#include <uvpoint.h>
#include <marchingsquares.h>
#include <drwindow.h>

class DRWindow;
class QPaintEvent;
class PlotWidget;
class ImageWidget;
class ColorBar;
class SpectralImageWidget;
class DSImage;

namespace Ui {
class RelaxationToolboxForm;
}

class RelaxationToolboxForm : public QWidget
{
  friend PlotWidget;
  friend ImageWidget;
  friend ColorBar;
  friend SpectralImageWidget;
  Q_OBJECT

  public:
  explicit RelaxationToolboxForm(DRWindow *drWindow, QWidget *parent = nullptr);
  std::vector<AxisInfo> spectralAxes;
  ~RelaxationToolboxForm();
  std::string coordInfoU(int u);
  std::string coordInfoV(int v);
  void updateColorbar();
  void updateAxisLabelInfo();
  void selectComponent(int n);
  void enableComponents(bool state=true);
  void setComponentsOn(bool state=true);
  float spotlightRadius();
  void setSpectralImageText(const QString &filename);
  void setComponentMaskText(const QString &filename);
  bool showAverageSpectrum();
  bool showContours();
  void refresh();
  void togglePlots();
  void dropEvent(QDropEvent *event) override;
  void dragEnterEvent(QDragEnterEvent *event) override;
  UVPoint position() { return drWindow->currentSpectralPosition; }
  void advanceV();
  void rescale();
  void updateInfo();
  void updatePosition();
  void updateLUT();
  void selectSpectralPoint(int w, int z);
  void selectSpectralPointF(float u, float v);
  bool integration();
  void integrateSpectra();
  void integrateOverVolume();
  void updateSliders();
  int showComponent();
  template <class T> void loadVolume(VolND<T> *vSource, UVPoint &uv);
  template <class T, class Operation> void renderPointSpectrum(DSImage &bitmap, SliceT<T> &slice, Operation operation); // always float? always the same slicemap? always the same op?
//  template <class Operation> void renderSpectrumLabels(DSImage &bitmap, Operation operation);
//  void mouseMoveTo(const glm::ivec3 &position, QMouseEvent * event);
  void paintEvent(QPaintEvent *ev) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent * event) override;
  bool event(QEvent *event) override;
  // void keyPressEvent(QKeyEvent *event) override;
  // void keyReleaseEvent(QKeyEvent *event) override;
  private slots:
  void on_loadSpectralNIIButton_clicked();
  void updateSpectrumImage(int x, int y, int z);

  void on_axisPlotCheckBox_clicked(bool checked);

  void on_colorCheckBox_clicked(bool checked);

  void on_integrate3DCheckBox_clicked(bool checked);

  void on_contourCheckBox_clicked(bool checked);

  void on_showSpectrumImageCheckBox_clicked(bool checked);

  void on_interpolateContoursCheckBox_clicked(bool checked);

  void on_darkBackGroundCheckBox_clicked(bool checked);

  void on_showContourTickLabelsCheckBox_clicked(bool checked);

  void on_nContoursSpinBox_valueChanged(int arg1);

  void on_zSliceSlider_sliderMoved(int position);

  void on_zSliceEdit_editingFinished();

  void on_showAverageSpectrumCheckBox_toggled(bool checked);

  void on_loadMaskButton_clicked();

  void on_componentSlider_sliderMoved(int position);

  void on_spotlightButton_toggled(bool checked);

  void on_spotlightRadiusSlider_sliderReleased();

  void on_spotlightRadiusSlider_valueChanged(int value);
  void on_componentButton_toggled(bool checked);

  void on_componentSlider_sliderPressed();

  void on_spotlightRadiusSlider_sliderPressed();

  void on_componentSlider_sliderReleased();

  void on_compositeViewButton_clicked();

private:
  bool gesturing=false;
  float gestureScale=1.0f;
  void drawHAxisTick(QPainter &painter, const QPoint &p, const float value, bool blank=false);
  void drawVAxisTick(QPainter &painter, const QPoint &p, const float value, bool blank=false);
  Ui::RelaxationToolboxForm *ui;
  DRWindow *drWindow=nullptr;
public:
  std::vector<float> uAxisValues;
  std::vector<float> vAxisValues;
  std::vector<std::string> axisLabels;
  std::vector<ContourLevel> contourLevels;
public:
  SliceT<float32> spectralSlice;
  int tracking=0;
  int nContours=7;
  int activeComponent=0;
};

#endif // RELAXATIONTOOLBOXFORM_H
