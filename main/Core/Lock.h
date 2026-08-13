#pragma once

#include <mutex>

// In a previous life this was a Win32 binary semaphore doing a very
// convincing mutex impression (complete with spin-wait). It has since
// transcended to become an actual mutex.
typedef std::lock_guard<std::mutex> CLock;
