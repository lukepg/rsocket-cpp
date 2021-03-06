// Copyright 2004-present Facebook. All Rights Reserved.

#pragma once

#include <deque>

#include "rsocket/RSocketStats.h"
#include "rsocket/ResumeManager.h"

namespace folly {
class IOBuf;
}

namespace rsocket {

class RSocketStateMachine;
class FrameTransport;

class WarmResumeManager : public ResumeManager {
 public:
  explicit WarmResumeManager(
      std::shared_ptr<RSocketStats> stats,
      size_t capacity = DEFAULT_CAPACITY)
      : stats_(std::move(stats)), capacity_(capacity) {}
  ~WarmResumeManager();

  void trackReceivedFrame(
      size_t frameLength,
      FrameType frameType,
      StreamId streamId,
      size_t consumerAllowance) override;

  void trackSentFrame(
      const folly::IOBuf& serializedFrame,
      FrameType frameType,
      StreamId streamId,
      size_t consumerAllowance) override;

  void resetUpToPosition(ResumePosition position) override;

  bool isPositionAvailable(ResumePosition position) const override;

  void sendFramesFromPosition(
      ResumePosition position,
      FrameTransport& transport) const override;

  ResumePosition firstSentPosition() const override {
    return firstSentPosition_;
  }

  ResumePosition lastSentPosition() const override {
    return lastSentPosition_;
  }

  ResumePosition impliedPosition() const override {
    return impliedPosition_;
  }

  // No action to perform for WarmResumeManager
  void onStreamOpen(StreamId, RequestOriginator, std::string, StreamType)
      override{};

  // No action to perform for WarmResumeManager
  void onStreamClosed(StreamId) override{};

  const StreamResumeInfos& getStreamResumeInfos() override {
    LOG(FATAL) << "Not Implemented for Warm Resumption";
  }

  StreamId getLargestUsedStreamId() override {
    LOG(FATAL) << "Not Implemented for Warm Resumption";
  }

  size_t size() {
    return size_;
  }

 protected:
  void addFrame(const folly::IOBuf&, size_t);
  void evictFrame();

  // Called before clearing cached frames to update stats.
  void clearFrames(ResumePosition position);

  std::shared_ptr<RSocketStats> stats_;

  // Start position of the send buffer queue
  ResumePosition firstSentPosition_{0};
  // End position of the send buffer queue
  ResumePosition lastSentPosition_{0};
  // Inferred position of the rcvd frames
  ResumePosition impliedPosition_{0};

  std::deque<std::pair<ResumePosition, std::unique_ptr<folly::IOBuf>>> frames_;

  constexpr static size_t DEFAULT_CAPACITY = 1024 * 1024; // 1MB
  const size_t capacity_;
  size_t size_{0};
};
}
