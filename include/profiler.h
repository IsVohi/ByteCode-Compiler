#ifndef COMPILER_PROFILER_H
#define COMPILER_PROFILER_H

#include "common.h"
#include <chrono>
#include <cstdint>
#include <iostream>
#include <unordered_map>

/**
 * Profiler for collecting VM execution statistics.
 * Tracks opcode execution counts and timing.
 */
class Profiler {
public:
  Profiler() = default;

  /**
   * Called by VM on each instruction execution
   */
  void onExecute(Opcode op) {
    opcodeCounts_[static_cast<uint8_t>(op)]++;
    totalInstructions_++;
  }

  /**
   * Start timing
   */
  void startTiming() { startTime_ = std::chrono::high_resolution_clock::now(); }

  /**
   * Stop timing
   */
  void stopTiming() { endTime_ = std::chrono::high_resolution_clock::now(); }

  /**
   * Get elapsed time in milliseconds
   */
  double getElapsedMs() const {
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
        endTime_ - startTime_);
    return duration.count() / 1000.0;
  }

  /**
   * Get total instruction count
   */
  uint64_t getTotalInstructions() const { return totalInstructions_; }

  /**
   * Get count for a specific opcode
   */
  uint64_t getOpcodeCount(Opcode op) const {
    auto it = opcodeCounts_.find(static_cast<uint8_t>(op));
    return it != opcodeCounts_.end() ? it->second : 0;
  }

  /**
   * Print statistics to output stream
   */
  void dump(std::ostream &os = std::cout) const {
    os << "=== Profiler Statistics ===" << std::endl;
    os << "Total instructions: " << totalInstructions_ << std::endl;
    os << "Elapsed time: " << getElapsedMs() << " ms" << std::endl;
    os << "Opcode counts:" << std::endl;

    for (const auto &[opcode, count] : opcodeCounts_) {
      os << "  " << opcode_to_string(static_cast<Opcode>(opcode)) << ": "
         << count << std::endl;
    }
  }

  /**
   * Reset all statistics
   */
  void reset() {
    opcodeCounts_.clear();
    totalInstructions_ = 0;
  }

private:
  std::unordered_map<uint8_t, uint64_t> opcodeCounts_;
  uint64_t totalInstructions_ = 0;
  std::chrono::high_resolution_clock::time_point startTime_;
  std::chrono::high_resolution_clock::time_point endTime_;
};

#endif // COMPILER_PROFILER_H
