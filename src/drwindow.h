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

#ifndef DRWINDOW_H
#define DRWINDOW_H

#include <QMainWindow>
#include <volnbase.h>
#include <spectroscopicshaderobject.h>
#include <imagestate.h>
#include <colormap.h>
#include <uvpoint.h>
#include <QMenu>
#include <spectralimageinfo.h>

class SpectrumGLWidget;
class RelaxationToolboxForm;
typedef uint32_t *LUTPtr;

QT_BEGIN_NAMESPACE
namespace Ui { class DRWindow; }
QT_END_NAMESPACE

class DRWindow : public QMainWindow
{
  Q_OBJECT

public:
  DRWindow(QWidget *parent = nullptr);
  enum ViewModes {
    FourUpIntegratedSpectra,
    FourUpIndividualSpectra,
    FourUpComponent,
    SixUpComposite
  };
  ViewModes viewMode=FourUpIntegratedSpectra;
  ~DRWindow();
  void status(const std::string &msg);
  void refresh();
  void updateComponentSelection();
  void initWindowGrid();
  void reloadSSOs();
  void reloadComponentSSOs();
  void toggleSpectralOverlay();
  bool setViewMode(ViewModes viewMode, bool update=false);
  void zoomIn();
  void zoomOut();
  glm::ivec3 worldToVoxel(const glm::vec3 &worldCoordinate) const;
  void mouseTrack(glm::vec3 worldClickPoint);
  std::string coordInfoU();
  std::string coordInfoV();
// menu

  QAction *inMenu(QMenu *menu, std::string item);
//  void changeLUT(uint32_t **lutTarget, uint32_t *newLUT);
  bool changeLUT(uint32_t *&LUTptr, const uint32_t *newLUT);
  void connectLUTMenuItem(QMenu *menu, std::string lutName, uint32_t **lutTarget, LUTPtr newLUT);
  void connectImageLUTMenuItem(QMenu *menu, std::string lutName, const LUTPtr lut);
  void connectSpectrumLUTMenuItem(QMenu *menu, std::string lutName, const LUTPtr lut);
  void createLUTMenus();
  void makeSpectralPopupMenu(QPoint point);
  QMenu* contextMenu=nullptr;
  ColorMap lutList;

  int showComponent();
  void updateComponentVolumes();
  int nComponents();
  float spotlightRadius();
  float gridwidth();
  void createMenus();
  void toggleSingleWindow(SpectrumGLWidget *requestingWindow);
  std::vector<QAction *> recentFileActions;
  std::vector<SpectrumGLWidget *> spectrumWindows;
  QString shortName(const QString &filename);
  QString shortName(const std::string &filename);
  void resetViews();
  void updateViews();
  std::string findSidecar(std::string niftiFile) const;
  std::string currentDatapath();
  std::string currentShaderpath() { return shaderpath; }
  QString currentDatapathQS() { return currentDatapath().c_str(); }
  QString currentShaderpathQS() { return currentShaderpath().c_str(); }
  SpectroscopicShaderObject volumeShaderObject;
  auto *componentMask() { return vComponentMask.get(); }
  VolND<float> *spectrum() { return vSpectrum.get(); }
  const VolND<float> *spectrum() const { return vSpectrum.get(); }
  VolND<float> *avgSpectrum() { return vAverageSpectralImage.get(); }
  void syncWindows(const SpectrumGLWidget *source);
  float spectrumScale();
  float imageScale();

  void loadSSO(SpectroscopicShaderObject &vso);
  void loadComponentMasktoSSO(SpectroscopicShaderObject &vso);
  void loadComponentMasktoSSOs();
  void loadLocalSSO(SpectrumGLWidget *window, int component);
  void loadComponentSSO(SpectroscopicShaderObject &vso);
  void updateSliceSSOs();
  bool useLocalSSOs() { return localSSOs; }

  void dropEvent(QDropEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
  void dragEnterEvent(QDragEnterEvent *event) override;

  void moveToSpectralPoint(float u, float v);
  void moveToSlice(int z);

 // file handling
  bool readFile(const std::string &filename);
  bool readSpectralImage();
  bool readSpectralImage(const std::string &filename);
//  bool readSpatialMask(const std::string &filename);
//  bool readSpectralMask(const std::string &filename);
  bool readComponentMask();
  bool readComponentMask(const std::string &filename);

  void updateRecentFileActions();
  void addToRecentFileList(const std::string &filename);

  uint32_t *spectrumLUT() {return spectLUT; }
  uint32_t *imageLUT() {return imLUT; }

  glm::ivec3 currentVolumePosition;
  UVPoint currentSpectralPosition;
  glm::vec3 volumeExtent=glm::vec3(1,1,1);
  ImageState imageState;
  bool showCursor=true;
  bool showSpectralOverlay=true;
  SpectrumGLWidget *grid(const int x, const int y);
  SpectrumGLWidget *windowgrid(int row, int col) { return spectrumWindows[col+row*gridCols]; }
  bool showImageInfo=false;
  SpectralImageInfo spectralImageInfo;
  void reloadShaders();
  bool loadCustomLUT(std::string lutFilename);
  bool solitaryMode=true;

  float maxFullSpectrum=0.0f;
  float maxFullSpectrumSlices=0.0f;
  float maxIntegratedSpatialImage=0.0f; // integrated over entire spectrum at each voxel
  float maxAverageSpectralImage=0.0f; // averaged over space
  std::vector<float> maxComponentIntensities;

private slots:
  void changeImageLUT();

  void changeSpectrumLUT();

  void openRecentFile();

  void on_action_Open_triggered();

  void on_action_Exit_triggered();

  void on_action_Toggle_View2_toggled(bool arg1);

  void on_action_Toggle_View3_toggled(bool arg1);

  void on_action_Toggle_View4_toggled(bool arg1);

  void on_action_SyncViews_toggled(bool arg1);

  void on_actionIntegrated_over_spectra_triggered();

  void on_actionToggleGrid_toggled(bool arg1);

  void on_actionNavigator_triggered();

  void on_actionShow_Spectral_triggered();

  void on_actionAutoscale_triggered();

  void on_action_Next_Component_triggered();

  void on_action_Previous_Component_triggered();

  void on_action_Sync_Intensity_triggered();

  void on_action_Image_Info_toggled(bool arg1);

  void on_action_Next_Slice_triggered();

  void on_action_Previous_Slice_triggered();

  void on_action_Reset_View_triggered();

  void on_action_LoadCustomColormap_triggered();

  void on_action_ViewAverageSpectroscopicImage_triggered();

private:
  template <class Form>
  bool attachFormToDockWidget(Form *&form, QDockWidget *dockWidget, std::string menu, std::string tooltip);
  Ui::DRWindow *ui;
  RelaxationToolboxForm *relaxationToolboxForm=nullptr;
  std::unique_ptr<VolND<float>> vSpectrum, vSpectralSlices, vIntegratedSpatialImage, vAverageSpectralImage, vComponentMaps;
  std::unique_ptr<VolND<uint8_t>> vSpectralMask, vSpatialMask, vComponentMask;
  std::string datapath;
  std::string shaderpath;
  bool synchronizeViews=true;
  bool syncBrightness=true;
  bool localSSOs=false;
  int gridRows=2;
  int gridCols=3;
  uint32_t *imLUT=ColorMap::greyLUT; // temporary to test if other colorbars are hard-coded
  uint32_t *spectLUT=ColorMap::jetLUT; // temporary to test if other colorbars are hard-coded
};

typedef float float32;
std::unique_ptr<VolND<float32>> reorderAsSpatial(const VolND<float> *vIn);
std::unique_ptr<VolND<float32>> reorderAsSpectral(const VolND<float> *vIn);
std::unique_ptr<VolND<float32>> averageOverSpectra(const VolND<float> *vIn);
std::unique_ptr<VolND<float32>> maskedAverageOverSpectra(const VolND<float> *vIn, const VolND<uint8_t> *vMask, int nComponent);
std::unique_ptr<VolND<float32>> copyToFloat(const VolNBase *vIn);
std::unique_ptr<VolND<float32>> averageOverSpatialDims(const VolND<float> *vIn);
std::unique_ptr<VolND<float32>> maskedAverageOverSpectra2(const VolND<float> *vIn, const VolND<uint8_t> *vMask);
std::unique_ptr<VolND<float32>> integrateOverSpectra(const VolND<float> *vIn);
std::unique_ptr<VolND<float32>> integrationOverMaskedSpectra(const VolND<float> *vIn, const VolND<uint8_t> *vMask);

#endif // DRWINDOW_H
