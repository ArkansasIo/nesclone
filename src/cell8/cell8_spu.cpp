#include "nex8/spu/cell8_spu.hpp"

#include <algorithm>

namespace nex8::spu {

Cell8Spu::Cell8Spu(std::uint8_t id, Role role) : id_(id), role_(role) {}

bool Cell8Spu::loadTask(Task task) {
    if (task.code.empty() || task.code.size() > localStore_.size()) return false;
    std::copy(task.code.begin(), task.code.end(), localStore_.begin());
    task.complete = false;
    queue_.push_back(std::move(task));
    if (state_ == State::idle) state_ = State::ready;
    return true;
}

bool Cell8Spu::start() {
    if (queue_.empty() || state_ == State::running) return false;
    currentIndex_ = queue_.size() - 1;
    pc_ = 0;
    accumulator_ = 0;
    state_ = State::running;
    return true;
}

void Cell8Spu::tick(std::uint32_t cycles) noexcept {
    if (state_ != State::running || queue_.empty()) return;
    const auto* task = currentTask();
    if (!task) { state_ = State::fault; return; }
    // Stub executor: tasks advance their local program counter; a future SPU
    // ISA decoder will replace this scheduling skeleton.
    const auto amount = static_cast<std::size_t>(cycles);
    pc_ = static_cast<std::uint16_t>(std::min<std::size_t>(localStoreBytes, static_cast<std::size_t>(pc_) + amount));
    if (pc_ >= task->code.size()) {
        queue_[currentIndex_].complete = true;
        state_ = State::idle;
    }
}
void Cell8Spu::halt() noexcept { state_ = State::halted; }
const Task* Cell8Spu::currentTask() const noexcept {
    return currentIndex_ < queue_.size() ? &queue_[currentIndex_] : nullptr;
}

} // namespace nex8::spu
