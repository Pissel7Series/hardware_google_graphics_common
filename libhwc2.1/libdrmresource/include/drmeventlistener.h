/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_DRM_EVENT_LISTENER_H_
#define ANDROID_DRM_EVENT_LISTENER_H_

#include <sys/epoll.h>

#include <map>

#include "autofd.h"
#include "worker.h"

namespace android {

constexpr uint32_t kDefaultVsyncPeriodNanoSecond = 16666666;
constexpr int32_t kDefaultRefreshRateFrequency = 60;

class DrmDevice;

class DrmEventHandler {
 public:
  DrmEventHandler() {
  }
  virtual ~DrmEventHandler() {
  }

  virtual void handleEvent(uint64_t timestamp_us) = 0;
};

class DrmHistogramEventHandler {
public:
    DrmHistogramEventHandler() {}
    virtual ~DrmHistogramEventHandler() {}

    virtual void handleHistogramEvent(uint32_t crtc_id, void *) = 0;
};

class DrmHistogramChannelEventHandler {
   public:
    DrmHistogramChannelEventHandler() {}
    virtual ~DrmHistogramChannelEventHandler() {}

    virtual void handleHistogramChannelEvent(void *) = 0;
};

class DrmContextHistogramEventHandler {
 public:
  DrmContextHistogramEventHandler() {}
  virtual ~DrmContextHistogramEventHandler() {}

  virtual void handleContextHistogramEvent(void *) = 0;
};

class DrmTUIEventHandler {
 public:
  DrmTUIEventHandler() {
  }
  virtual ~DrmTUIEventHandler() {
  }

  virtual void handleTUIEvent() = 0;
};

class DrmPanelIdleEventHandler {
 public:
  DrmPanelIdleEventHandler() {}
  virtual ~DrmPanelIdleEventHandler() {}

  virtual void handleIdleEnterEvent(char const *event) = 0;
};

class DrmSysfsEventHandler {
 public:
  DrmSysfsEventHandler() {}
  virtual ~DrmSysfsEventHandler() {}

  virtual void handleSysfsEvent() = 0;
  virtual int getFd() = 0;
};

class DrmPropertyUpdateHandler {
 public:
  DrmPropertyUpdateHandler() {}
  virtual ~DrmPropertyUpdateHandler() {}

  virtual void handleDrmPropertyUpdate(unsigned connector_id, unsigned prop_id) = 0;
};

class DrmEventListener : public Worker {
  static constexpr const char kTUIStatusPath[] = "/sys/devices/platform/exynos-drm/tui_status";
  static const uint32_t maxFds = 4;

 public:
  DrmEventListener(DrmDevice *drm);
  virtual ~DrmEventListener();

  int Init();

  void RegisterHotplugHandler(const std::shared_ptr<DrmEventHandler> &handler);
  void UnRegisterHotplugHandler(const std::shared_ptr<DrmEventHandler> &handler);
  void RegisterHistogramHandler(const std::shared_ptr<DrmHistogramEventHandler> &handler);
  void UnRegisterHistogramHandler(const std::shared_ptr<DrmHistogramEventHandler> &handler);
  void RegisterHistogramChannelHandler(
      const std::shared_ptr<DrmHistogramChannelEventHandler> &handler);
  void UnRegisterHistogramChannelHandler(
      const std::shared_ptr<DrmHistogramChannelEventHandler> &handler);
  void RegisterContextHistogramHandler(
      const std::shared_ptr<DrmContextHistogramEventHandler> &handler);
  void UnRegisterContextHistogramHandler(
      const std::shared_ptr<DrmContextHistogramEventHandler> &handler);
  void RegisterTUIHandler(const std::shared_ptr<DrmTUIEventHandler> &handler);
  void UnRegisterTUIHandler(const std::shared_ptr<DrmTUIEventHandler> &handler);
  void RegisterPanelIdleHandler(const std::shared_ptr<DrmPanelIdleEventHandler> &handler);
  void UnRegisterPanelIdleHandler(const std::shared_ptr<DrmPanelIdleEventHandler> &handler);
  int RegisterSysfsHandler(std::shared_ptr<DrmSysfsEventHandler> handler);
  int UnRegisterSysfsHandler(int sysfs_fd);
  void RegisterPropertyUpdateHandler(const std::shared_ptr<DrmPropertyUpdateHandler> &handler);
  void UnRegisterPropertyUpdateHandler(const std::shared_ptr<DrmPropertyUpdateHandler> &handler);

  bool IsDrmInTUI();

  static void FlipHandler(int fd, unsigned int sequence, unsigned int tv_sec, unsigned int tv_usec,
                          void *user_data);

 protected:
  virtual void Routine();

 private:
  void UEventHandler();
  void DRMEventHandler();
  void TUIEventHandler();
  void SysfsEventHandler(int fd);

  UniqueFd epoll_fd_;
  UniqueFd uevent_fd_;
  UniqueFd tuievent_fd_;

  DrmDevice *drm_;
  std::shared_ptr<DrmEventHandler> hotplug_handler_;
  std::shared_ptr<DrmHistogramEventHandler> histogram_handler_;
  std::shared_ptr<DrmHistogramChannelEventHandler> histogram_channel_handler_;
  std::shared_ptr<DrmContextHistogramEventHandler> context_histogram_handler_;
  std::shared_ptr<DrmTUIEventHandler> tui_handler_;
  std::shared_ptr<DrmPanelIdleEventHandler> panel_idle_handler_;
  std::shared_ptr<DrmPropertyUpdateHandler> drm_prop_update_handler_;
  std::mutex mutex_;
  std::map<int, std::shared_ptr<DrmSysfsEventHandler>> sysfs_handlers_;
};

}  // namespace android

#endif
