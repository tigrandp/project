#include "threading/mutex.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace threading {
namespace {

TEST(HierarchicalMutexTest, ViolatesLockHierarchy) {}

TEST(HierarchicalMutexTest, RestoresLockAfterUnlock) {}

TEST(HierarchicalMutexTest, MultipleThreadsConfflicts) {}

}  // namespace
}  // namespace threading
