#pragma once

#include <memory>

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QThread>

#ifdef QCOM2
#define EGL_EGLEXT_PROTOTYPES
#define EGL_NO_X11
#define GL_TEXTURE_EXTERNAL_OES 0x8D65
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <drm/drm_fourcc.h>
#endif

#include "cereal/visionipc/visionipc_client.h"
#include "system/camerad/cameras/camera_common.h"
#include "selfdrive/ui/ui.h"

class CameraViewWidget : public QOpenGLWidget, protected QOpenGLFunctions {
  Q_OBJECT

public:
  using QOpenGLWidget::QOpenGLWidget;
  explicit CameraViewWidget(std::string stream_name, VisionStreamType stream_type, bool zoom, QWidget* parent = nullptr);
  ~CameraViewWidget();
  void setStreamType(VisionStreamType type) { stream_type = type; }
  void setBackgroundColor(const QColor &color) { bg = color; }

signals:
  void clicked();
  void vipcThreadConnected(VisionIpcClient *);
  void vipcThreadFrameReceived(VisionBuf *);

protected:
  void paintGL() override;
  void initializeGL() override;
  void resizeGL(int w, int h) override { updateFrameMat(w, h); }
  void showEvent(QShowEvent *event) override;
  void hideEvent(QHideEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override { emit clicked(); }
  virtual void updateFrameMat(int w, int h);
  void vipcThread();

  bool zoomed_view;
  VisionBuf *latest_frame = nullptr;
  GLuint frame_vao, frame_vbo, frame_ibo;
  GLuint textures[2];
  mat4 frame_mat;
  std::unique_ptr<QOpenGLShaderProgram> program;
  QColor bg = QColor("#000000");

#ifdef QCOM2
  EGLDisplay egl_display;
  std::map<int, EGLImageKHR> egl_images;
#endif

  std::string stream_name;
  int stream_width = 0;
  int stream_height = 0;
  int stream_stride = 0;
  std::atomic<VisionStreamType> stream_type;
  QThread *vipc_thread = nullptr;

protected slots:
  void vipcConnected(VisionIpcClient *vipc_client);
  void vipcFrameReceived(VisionBuf *vipc_client);
};
