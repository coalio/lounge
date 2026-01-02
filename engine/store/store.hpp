#pragma once

#include <atomic>
#include <functional>
#include <mutex>
#include <utility>
#include <vector>

namespace engine::store {

template <typename State, typename Action>
class Store {
public:
    using Reducer = std::function<State(const State&, const Action&)>;
    using Subscriber = std::function<void(const State&)>;

    explicit Store(State initial_state, Reducer reducer)
        : state_{std::move(initial_state)},
          reducer_{std::move(reducer)} {}

    Store(const Store&) = delete;
    auto operator=(const Store&) -> Store& = delete;
    Store(Store&&) = delete;
    auto operator=(Store&&) -> Store& = delete;
    ~Store() = default;

    auto state() const -> State {
        std::lock_guard lock(mutex_);
        return state_;
    }

    auto subscribe(Subscriber subscriber) -> std::size_t {
        const auto token = next_token_.fetch_add(1);
        std::lock_guard lock(mutex_);
        subscribers_.push_back({token, std::move(subscriber)});
        return token;
    }

    void unsubscribe(std::size_t token) {
        std::lock_guard lock(mutex_);
        subscribers_.erase(
            std::remove_if(
                subscribers_.begin(),
                subscribers_.end(),
                [token](const SubscriberEntry& entry) { return entry.token == token; }
            ),
            subscribers_.end()
        );
    }

    void dispatch(const Action& action) {
        State next;
        {
            std::lock_guard lock(mutex_);
            next = reducer_(state_, action);
            state_ = next;
        }

        notify(next);
    }

private:
    struct SubscriberEntry {
        std::size_t token{0};
        Subscriber subscriber{};
    };

    void notify(const State& snapshot) {
        std::vector<SubscriberEntry> subs_copy;
        {
            std::lock_guard lock(mutex_);
            subs_copy = subscribers_;
        }
        for (const auto& entry : subs_copy) {
            if (entry.subscriber) {
                entry.subscriber(snapshot);
            }
        }
    }

    mutable std::mutex mutex_{};
    State state_{};
    Reducer reducer_{};
    std::vector<SubscriberEntry> subscribers_{};
    std::atomic<std::size_t> next_token_{1};
};

}  // namespace engine::store


