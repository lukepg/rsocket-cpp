// Copyright 2004-present Facebook. All Rights Reserved.

#pragma once

#include <functional>
#include <memory>
#include <yarpl/Flowable_Subscriber.h>

namespace yarpl {
namespace flowable {

template <typename T> class Flowable {
 public:
  explicit Flowable(
      std::function<void(std::unique_ptr<Subscriber<T>>)> onSubscribe);

  static std::unique_ptr<Flowable<T>>
  create(std::function<void(std::unique_ptr<Subscriber<T>>)> onSubscribe);

  void subscribe(std::unique_ptr<Subscriber<T>>);

 private:
  std::function<void(std::unique_ptr<Subscriber<T>>)> onSubscribe_;
};

/* ****************************************** */
/* implementation here because of templates */
/* https://isocpp.org/wiki/faq/templates#templates-defn-vs-decl */
/* ****************************************** */

template <typename T>
Flowable<T>::Flowable(std::function<void(std::unique_ptr<Subscriber<T>>)> os)
    : onSubscribe_(std::move(os)) {}

template <typename T>
std::unique_ptr<Flowable<T>> Flowable<T>::create(
    std::function<void(std::unique_ptr<Subscriber<T>>)> onSubscribe) {
  return std::make_unique<Flowable>(std::move(onSubscribe));
}

template <typename T>
void Flowable<T>::subscribe(std::unique_ptr<Subscriber<T>> o) {
  // when subscribed to, invoke the `onSubscribe` function
  onSubscribe_(std::move(o));
}

} // flowable namespace
} // yarpl namespace
