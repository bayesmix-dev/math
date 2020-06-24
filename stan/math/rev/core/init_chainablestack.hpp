#ifndef STAN_MATH_REV_CORE_INIT_CHAINABLESTACK_HPP
#define STAN_MATH_REV_CORE_INIT_CHAINABLESTACK_HPP

#include <stan/math/rev/core/chainablestack.hpp>

#include <tbb/task_scheduler_observer.h>

#include <mutex>
#include <unordered_map>
#include <utility>
#include <thread>
#include <tuple>

namespace stan {
namespace math {

/**
 * TBB observer object which is a callback hook called whenever the
 * TBB scheduler adds a new thread to the TBB managed threadpool. This
 * hook ensures that each worker thread has an initialized AD tape
 * ready for use.
 *
 * Refer to https://software.intel.com/en-us/node/506314 for details
 * on the observer concept.
 */
class ad_tape_observer final : public tbb::task_scheduler_observer {
  using stack_ptr = std::unique_ptr<const ChainableStack>;
  using ad_map = std::unordered_map<std::thread::id, stack_ptr>;

 public:
  ad_tape_observer() : tbb::task_scheduler_observer(), thread_tape_map_() {
    on_scheduler_entry(true);  // register current process
    observe(true);             // activates the observer
  }

  ~ad_tape_observer() { observe(false); }

  inline void on_scheduler_entry(bool worker) {
    std::lock_guard<std::mutex> thread_tape_map_lock(thread_tape_map_mutex_);
    if (thread_tape_map_.find(thread_id) == thread_tape_map_.end()) {
      thread_tape_map_.emplace(std::piecewise_constuct, std::forward_as_tuple(thread_id),
			 std::forward_as_tuple(std::make_unique<const ChainableStack>()));
    }
  }

  inline void on_scheduler_exit(bool worker) {
    std::lock_guard<std::mutex> thread_tape_map_lock(thread_tape_map_mutex_);
    auto elem = thread_tape_map_.find(std::this_thread::get_id());
    if (elem != thread_tape_map_.end()) {
      thread_tape_map_.erase(elem);
    }
  }

 private:
  ad_map thread_tape_map_;
  std::mutex thread_tape_map_mutex_;
};

namespace {

ad_tape_observer global_observer;

}  // namespace
}  // namespace math
}  // namespace stan

#endif
