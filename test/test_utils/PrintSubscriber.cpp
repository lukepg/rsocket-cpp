// Copyright 2004-present Facebook. All Rights Reserved.

#include "PrintSubscriber.h"
#include <folly/Memory.h>
#include <folly/io/IOBufQueue.h>
#include <glog/logging.h>

namespace rsocket {

PrintSubscriber::~PrintSubscriber() {
  LOG(INFO) << "~PrintSubscriber " << this;
}

void PrintSubscriber::onSubscribe(
    yarpl::Reference<yarpl::flowable::Subscription> subscription) noexcept {
  LOG(INFO) << "PrintSubscriber " << this << " onSubscribe";
  subscription->request(std::numeric_limits<int32_t>::max());
}

void PrintSubscriber::onNext(Payload element) noexcept {
  LOG(INFO) << "PrintSubscriber " << this << " onNext " << element;
}

void PrintSubscriber::onComplete() noexcept {
  LOG(INFO) << "PrintSubscriber " << this << " onComplete";
}

void PrintSubscriber::onError(folly::exception_wrapper ex) noexcept {
  LOG(INFO) << "PrintSubscriber " << this << " onError " << ex;
}
}
