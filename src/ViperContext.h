#pragma once

#include "essential.h"
#include "viper/ViPER.h"
#include <chrono>
#include <cstddef>
#include <string>
#include <vector>

class ViperContext {
public:
    enum class DisableReason : int32_t {
        UNKNOWN = -1,
        NONE = 0,
        INVALID_FRAME_COUNT,
        INVALID_SAMPLING_RATE,
        INVALID_CHANNEL_COUNT,
        INVALID_FORMAT,
    };

    ViperContext();

    int32_t handleCommand(
        uint32_t cmdCode,
        uint32_t cmdSize,
        void *pCmdData,
        uint32_t *replySize,
        void *pReplyData
    );
    int32_t process(audio_buffer_t *inBuffer, audio_buffer_t *outBuffer);

private:
    effect_config_t config;
    DisableReason disableReason;
    std::string disableReasonMessage;

    // Processing buffer
    std::vector<float> buffer;
    size_t bufferFrameCount;

    // Viper
    bool enabled;
    uint64_t lastStreamingFrames = 0;
    ViPER viper_;

    // Stream discontinuity detection
    std::chrono::steady_clock::time_point lastProcessTime;
    bool hasProcessed;
    uint32_t fadeInRemaining;

    static void copyBufferConfig(buffer_config_t *dest, buffer_config_t *src);
    void handleSetConfig(effect_config_t *newConfig);
    int32_t handleGetParam(
        effect_param_t *pCmdParam, effect_param_t *pReplyParam, uint32_t *pReplySize
    int32_t HandleSetParam(effect_param_t *cmd_param, void *reply_data);
    );

    void setDisableReason(DisableReason reason);
    void setDisableReason(DisableReason reason, std::string message);
};
