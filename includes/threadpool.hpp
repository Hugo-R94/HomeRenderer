// ThreadPool.h - version minimale
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>

// ThreadPool corrigé
class ThreadPool {
public:
    ThreadPool(int n) : stop(false), activeTasks(0) {
        for (int i = 0; i < n; i++)
            workers.emplace_back([this]() {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(mtx);
                        cv.wait(lock, [this]{ return stop || !tasks.empty(); });
                        if (stop && tasks.empty()) return;
                        task = std::move(tasks.front());
                        tasks.pop();
                        activeTasks++;  // <-- increment avant exécution
                    }
                    task();
                    {
                        std::lock_guard<std::mutex> lock(mtx);
                        activeTasks--;  // <-- decrement après exécution
                    }
                    cv_done.notify_all();
                }
            });
    }

    void enqueue(std::function<void()> task) {
        { std::lock_guard<std::mutex> lock(mtx); tasks.push(std::move(task)); }
        cv.notify_one();
    }

    void wait() {
        std::unique_lock<std::mutex> lock(mtx);
        cv_done.wait(lock, [this]{ return tasks.empty() && activeTasks == 0; });
    }

    ~ThreadPool() {
        { std::lock_guard<std::mutex> lock(mtx); stop = true; }
        cv.notify_all();
        for (auto& w : workers) w.join();
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex mtx;
    std::condition_variable cv, cv_done;
    std::atomic<int> activeTasks;
    bool stop;
};