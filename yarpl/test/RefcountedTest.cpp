// Copyright 2004-present Facebook. All Rights Reserved.

#include <memory>
#include <vector>

#include <gtest/gtest.h>

#include "yarpl/Refcounted.h"

namespace yarpl {

TEST(RefcountedTest, ObjectCountsAreMaintained) {
  std::vector<std::unique_ptr<Refcounted>> v;
  for (std::size_t i = 0; i < 16; ++i) {
    v.push_back(std::make_unique<Refcounted>());
    EXPECT_EQ(1U, v[i]->count()); // no references.
  }

  v.resize(11);
}

TEST(RefcountedTest, ReferenceCountingWorks) {
  auto first = make_ref<Refcounted>();
  EXPECT_EQ(1U, first->count());

  auto second = first;

  EXPECT_EQ(second.get(), first.get());
  EXPECT_EQ(2U, first->count());

  auto third = std::move(second);
  EXPECT_EQ(nullptr, second.get());
  EXPECT_EQ(third.get(), first.get());
  EXPECT_EQ(2U, first->count());

  // second was already moved from, above.
  second.reset();
  EXPECT_EQ(nullptr, second.get());
  EXPECT_EQ(2U, first->count());

  auto fourth = third;
  EXPECT_EQ(3U, first->count());

  fourth.reset();
  EXPECT_EQ(nullptr, fourth.get());
  EXPECT_EQ(2U, first->count());
}


class MyRefInside : public virtual Refcounted, public yarpl::enable_get_ref {
public:
  MyRefInside() {
    auto r = this->ref_from_this(this);
  }

  auto a_const_method() const {
    return ref_from_this(this);
  }
};

TEST(RefcountedTest, CanCallGetRefInCtor) {
  auto r = make_ref<MyRefInside>();
  auto r2 = r->a_const_method();
  EXPECT_EQ(r, r2);
}

} // yarpl
