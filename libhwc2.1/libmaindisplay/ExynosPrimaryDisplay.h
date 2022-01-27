/*
 * Copyright (C) 2012 The Android Open Source Project
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
#ifndef EXYNOS_PRIMARY_DISPLAY_H
#define EXYNOS_PRIMARY_DISPLAY_H

#include <map>

#include "../libdevice/ExynosDisplay.h"

class ExynosPrimaryDisplay : public ExynosDisplay {
    public:
        /* Methods */
        ExynosPrimaryDisplay(uint32_t index, ExynosDevice *device);
        ~ExynosPrimaryDisplay();
        virtual void setDDIScalerEnable(int width, int height);
        virtual int getDDIScalerMode(int width, int height);
        virtual int32_t SetCurrentPanelGammaSource(const displaycolor::DisplayType type,
                                                   const PanelGammaSource& source) override;
        virtual PanelGammaSource GetCurrentPanelGammaSource() const override {
            return currentPanelGammaSource;
        }

        virtual int32_t setLhbmState(bool enabled);

        virtual bool getLhbmState();
        virtual void notifyLhbmState(bool enabled);
        virtual void setEarlyWakeupDisplay();
        virtual void setExpectedPresentTime(uint64_t timestamp);
        virtual uint64_t getPendingExpectedPresentTime();
        virtual void applyExpectedPresentTime();
        virtual int32_t setDisplayIdleTimer(const int32_t timeoutMs) override;

        virtual void initDisplayInterface(uint32_t interfaceType);
        virtual int32_t doDisplayConfigInternal(hwc2_config_t config) override;

        virtual int setMinIdleRefreshRate(const int fps) override;
        virtual int setRefreshRateThrottleNanos(const int64_t delayNs,
                                                const DispIdleTimerRequester requester) override;
        virtual void dump(String8& result) override;
        virtual void updateAppliedActiveConfig(const hwc2_config_t newConfig,
                                               const int64_t ts) override;

    protected:
        /* setPowerMode(int32_t mode)
         * Descriptor: HWC2_FUNCTION_SET_POWER_MODE
         * Parameters:
         *   mode - hwc2_power_mode_t and ext_hwc2_power_mode_t
         *
         * Returns HWC2_ERROR_NONE or the following error:
         *   HWC2_ERROR_UNSUPPORTED when DOZE mode not support
         */
        virtual int32_t setPowerMode(int32_t mode) override;
        virtual bool getHDRException(ExynosLayer* __unused layer);
        virtual int32_t setActiveConfigInternal(hwc2_config_t config, bool force) override;
        virtual int32_t getActiveConfigInternal(hwc2_config_t* outConfig) override;
    public:
        // Prepare multi resolution
        ResolutionInfo mResolutionInfo;
        std::string getPanelSysfsPath(const displaycolor::DisplayType& type);

    private:
        static constexpr const char* kDisplayCalFilePath = "/mnt/vendor/persist/display/";
        static constexpr const char* kPanelGammaCalFilePrefix = "gamma_calib_data";
        enum PanelGammaSource currentPanelGammaSource = PanelGammaSource::GAMMA_DEFAULT;

        bool checkLhbmMode(bool status, nsecs_t timoutNs);

        hwc2_config_t mPendActiveConfig = UINT_MAX;
        bool mFirstPowerOn = true;

        int32_t applyPendingConfig();
        int32_t setPowerOn();
        int32_t setPowerOff();
        int32_t setPowerDoze(hwc2_power_mode_t mode);
        void firstPowerOn();
        int32_t setDisplayIdleTimerEnabled(const bool enabled);
        int32_t getDisplayIdleTimerEnabled(bool& enabled);

        // LHBM
        FILE* mLhbmFd;
        bool mLhbmOn;
        bool mLhbmChanged;

        std::mutex lhbm_mutex_;
        std::condition_variable lhbm_cond_;

        FILE* mEarlyWakeupDispFd;
        static constexpr const char* kWakeupDispFilePath =
                "/sys/devices/platform/1c300000.drmdecon/early_wakeup";

        CtrlValue<uint64_t> mExpectedPresentTime;

        void calculateTimeline(hwc2_config_t config,
                               hwc_vsync_period_change_constraints_t* vsyncPeriodChangeConstraints,
                               hwc_vsync_period_change_timeline_t* outTimeline) override;
        std::mutex mIdleRefreshRateThrottleMutex;
        int mMinIdleRefreshRate;
        int64_t mRefreshRateDelayNanos;
        int64_t mLastRefreshRateAppliedNanos;
        hwc2_config_t mAppliedActiveConfig;

        bool mDisplayIdleTimerEnabled;
        int64_t mDisplayIdleTimerNanos[toUnderlying(DispIdleTimerRequester::MAX)];
};

#endif
