﻿//2-2.alarm clock
//#include <iostream>
//#include <list>
//#include <memory>
//#include <mutex>
//#include <thread>
//#include <vector>
//#include <atomic>
//#include <chrono>
//#include <condition_variable>
//#include <map>
//#include <algorithm>
//#include <sstream>
//
//using namespace std;
//
//class Process {
//public:
//    int id;
//    bool isForeground;
//    string command;
//    bool promoted = false;
//    int remainingTime = 0; // for wait queue
//
//    Process(int id, bool isForeground, const string& command)
//        : id(id), isForeground(isForeground), command(command), promoted(false) {}
//
//    string toString() const {
//        stringstream ss;
//        ss << "[" << id << (isForeground ? "F" : "B");
//        if (promoted) ss << "*";
//        ss << "]";
//        return ss.str();
//    }
//};
//
//class DynamicQueue {
//private:
//    mutex mtx;
//    condition_variable cv;
//    list<shared_ptr<Process>> fgProcesses;
//    list<shared_ptr<Process>> bgProcesses;
//    map<int, shared_ptr<Process>> waitQueue;
//    atomic<int> processCount{ 0 };
//    atomic<int> bgCount{ 0 };
//
//public:
//    void enqueue(shared_ptr<Process> process) {
//        lock_guard<mutex> lock(mtx);
//        if (process->isForeground) {
//            fgProcesses.push_back(process);
//        }
//        else {
//            bgProcesses.push_back(process);
//            bgCount++;
//        }
//        processCount++;
//    }
//
//    void simulateSleep(int pid, int seconds) {
//        lock_guard<mutex> lock(mtx);
//        auto it = find_if(fgProcesses.begin(), fgProcesses.end(), [&pid](const auto& p) { return p->id == pid; });
//        if (it != fgProcesses.end()) {
//            waitQueue[pid] = *it;
//            fgProcesses.erase(it);
//        }
//        else {
//            it = find_if(bgProcesses.begin(), bgProcesses.end(), [&pid](const auto& p) { return p->id == pid; });
//            if (it != bgProcesses.end()) {
//                waitQueue[pid] = *it;
//                bgProcesses.erase(it);
//                bgCount--;
//            }
//        }
//        waitQueue[pid]->remainingTime = seconds;
//    }
//
//    void wakeUpProcesses() {
//        lock_guard<mutex> lock(mtx);
//        for (auto it = waitQueue.begin(); it != waitQueue.end();) {
//            if (--it->second->remainingTime <= 0) {
//                it->second->isForeground ? fgProcesses.push_back(it->second) : bgProcesses.push_back(it->second);
//                if (!it->second->isForeground) bgCount++;
//                it = waitQueue.erase(it);
//            }
//            else {
//                ++it;
//            }
//        }
//    }
//
//    void printQueue() {
//        lock_guard<mutex> lock(mtx);
//        cout << "Running: [" << bgCount.load() << "B]\n";
//        cout << "---------------------------\n";
//        cout << "DQ: (bottom) ";
//        for (const auto& proc : bgProcesses) {
//            cout << proc->toString() << " ";
//        }
//        cout << "\nP => ";
//        for (const auto& proc : fgProcesses) {
//            cout << proc->toString() << " ";
//        }
//        cout << "(top)\n";
//        cout << "---------------------------\n";
//        cout << "WQ: ";
//        for (const auto& wp : waitQueue) {
//            cout << wp.second->toString() << " - Remaining Time: " << wp.second->remainingTime << "s; ";
//        }
//        cout << "\n...\n";
//    }
//};
//
//void shellProcess(DynamicQueue& dq, int interval) {
//    vector<string> commands = { "alarm clock", "todo list", "email check" };
//    int processId = 2; // Starting from 2, since 0 and 1 are taken by shell and monitor
//    for (const string& line : commands) {
//        shared_ptr<Process> proc = make_shared<Process>(processId++, true, line);
//        dq.enqueue(proc);
//        this_thread::sleep_for(chrono::seconds(interval));
//        dq.printQueue();
//    }
//}
//
//void monitorProcess(DynamicQueue& dq, int interval) {
//    while (true) {
//        this_thread::sleep_for(chrono::seconds(interval));
//        dq.wakeUpProcesses();
//        dq.printQueue();
//    }
//}
//
//int main() {
//    DynamicQueue dq;
//    shared_ptr<Process> shellProc = make_shared<Process>(0, true, "shell");
//    shared_ptr<Process> monitorProc = make_shared<Process>(1, false, "monitor");
//
//    dq.enqueue(shellProc);
//    dq.enqueue(monitorProc);
//
//    thread shellThread(shellProcess, ref(dq), 1); // Simulate shell processing
//    thread monitorThread(monitorProcess, ref(dq), 2);
//
//    shellThread.join();
//    monitorThread.detach();
//
//    return 0;
//}


//2-1.Dynamic Queueing
//#include <iostream>
//#include <queue>
//#include <list>
//#include <thread>
//#include <mutex>
//#include <condition_variable>
//#include <atomic>
//#include <chrono>
//#include <vector>
//#include <cmath>
//#include <unordered_map>
//
//using namespace std;
//
//class Process {
//public:
//    int id;
//    bool isForeground;
//
//    Process(int id, bool isForeground) : id(id), isForeground(isForeground) {}
//};
//
//class DynamicQueue {
//private:
//    vector<list<Process>> queues;
//    list<Process> waitQueue;
//    mutex mtx;
//    condition_variable cv;
//    atomic<bool> running;
//    int threshold;
//    size_t pIndex; // Promote index
//
//public:
//    DynamicQueue(int numQueues, int threshold) : running(true), threshold(threshold), pIndex(0) {
//        queues.resize(numQueues);
//    }
//
//    void enqueue(Process process) {
//        unique_lock<mutex> lock(mtx);
//        if (process.isForeground) {
//            queues.back().push_back(process);
//        }
//        else {
//            queues.front().push_back(process);
//        }
//        split_and_merge();
//        cv.notify_all();
//    }
//
//    Process dequeue() {
//        unique_lock<mutex> lock(mtx);
//        cv.wait(lock, [this] { return !queues.back().empty() || !queues.front().empty(); });
//
//        Process process(-1, false);
//        if (!queues.back().empty()) {
//            process = queues.back().front();
//            queues.back().pop_front();
//            if (queues.back().empty() && queues.size() > 1) {
//                queues.pop_back();
//            }
//        }
//        else if (!queues.front().empty()) {
//            process = queues.front().front();
//            queues.front().pop_front();
//        }
//
//        promote();
//        return process;
//    }
//
//    void promote() {
//        if (queues[pIndex].empty()) {
//            pIndex = (pIndex + 1) % queues.size();
//            return;
//        }
//
//        Process process = queues[pIndex].front();
//        queues[pIndex].pop_front();
//
//        size_t nextIndex = (pIndex + 1) % queues.size();
//        queues[nextIndex].push_back(process);
//
//        if (queues[pIndex].empty() && queues.size() > 1) {
//            queues.erase(queues.begin() + pIndex);
//        }
//
//        pIndex = (pIndex + 1) % queues.size();
//    }
//
//    void split_and_merge() {
//        for (size_t i = 0; i < queues.size(); ++i) {
//            if (queues[i].size() > threshold) {
//                auto it = queues[i].begin();
//                advance(it, queues[i].size() / 2);
//
//                list<Process> newList;
//                newList.splice(newList.begin(), queues[i], queues[i].begin(), it);
//                if (i + 1 < queues.size()) {
//                    queues[i + 1].splice(queues[i + 1].end(), newList);
//                }
//                else {
//                    queues.push_back(newList);
//                }
//            }
//        }
//    }
//
//    void print_status() {
//        unique_lock<mutex> lock(mtx);
//        cout << "Running: [1B]" << endl;  // Always print "Running: [1B]"
//        cout << "---------------------------" << endl;
//        cout << "DQ: ";
//        for (auto& process : queues.front()) {
//            cout << process.id << (process.isForeground ? "F " : "B ");
//        }
//        cout << "(bottom)";
//        cout << endl;
//
//        cout << "P => ";
//        for (auto& process : queues.back()) {
//            cout << process.id << (process.isForeground ? "F " : "B ");
//        }
//        cout << "(top)";
//        cout << endl;
//
//        cout << "WQ: ";
//        for (auto& process : waitQueue) {
//            cout << process.id << (process.isForeground ? "F" : "B") << " ";
//        }
//        cout << endl;
//        cout << "---------------------------" << endl;
//    }
//
//    void add_to_wait_queue(Process process) {
//        unique_lock<mutex> lock(mtx);
//        waitQueue.push_back(process);
//    }
//
//    void remove_from_wait_queue(Process process) {
//        unique_lock<mutex> lock(mtx);
//        waitQueue.remove_if([&process](const Process& p) { return p.id == process.id; });
//    }
//
//    bool is_running() const {
//        return running;
//    }
//
//    void stop() {
//        running = false;
//        cv.notify_all();
//    }
//};
//
//void fg_process(DynamicQueue& dq) {
//    for (int i = 0; i < 10; ++i) { // Simulating 10 FG processes
//        Process p(i, true);
//        dq.enqueue(p);
//        this_thread::sleep_for(chrono::seconds(1));
//        dq.add_to_wait_queue(p);  // Simulate process moving to wait queue
//    }
//    dq.stop();
//}
//
//void bg_monitor(DynamicQueue& dq) {
//    while (dq.is_running()) {
//        dq.print_status();
//        this_thread::sleep_for(chrono::seconds(1));
//    }
//}
//
//int main() {
//    const int numQueues = 2;
//    const int threshold = 3;
//    DynamicQueue dq(numQueues, threshold);
//
//    thread fg_thread(fg_process, ref(dq));
//    thread bg_thread(bg_monitor, ref(dq));
//
//    fg_thread.join();
//    bg_thread.join();
//
//    return 0;
//}

// 2-3	CLI
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <regex>
#include <functional>

using namespace std;

mutex mtx;
condition_variable cv;
queue<function<void()>> fgTasks;  // Foreground tasks
atomic<bool> quit(false);
atomic<int> bgTaskCount(0);  // Count of background tasks

// Utility functions
int gcd(int a, int b) {
    while (b != 0) {
        int t = b;
        b = a % b;
        a = t;
    }
    return a;
}

int countPrimes(int limit) {
    vector<bool> sieve(limit + 1, true);
    int count = 0;
    for (int p = 2; p <= limit; p++) {
        if (sieve[p]) {
            count++;
            for (int multiple = 2 * p; multiple <= limit; multiple += p)
                sieve[multiple] = false;
        }
    }
    return count;
}

long long sum(int n) {
    long long total = 0;
    for (int i = 1; i <= n; i++) {
        total = (total + i) % 1000000;
    }
    return total;
}

void echo(const string& message) {
    cout << message << endl;
}

void dummy() {}

// Task execution
void executeTask(const string& command, const vector<string>& args, bool isBackground) {
    auto task = [command, args]() {
        if (command == "echo" && !args.empty()) {
            echo(args[0]);
        }
        else if (command == "dummy") {
            dummy();
        }
        else if (command == "gcd" && args.size() >= 2) {
            cout << "GCD: " << gcd(stoi(args[0]), stoi(args[1])) << endl;
        }
        else if (command == "prime" && !args.empty()) {
            cout << "Primes count: " << countPrimes(stoi(args[0])) << endl;
        }
        else if (command == "sum" && !args.empty()) {
            cout << "Sum: " << sum(stoi(args[0])) << endl;
        }
        };

    if (isBackground) {
        bgTaskCount++;
        thread bgThread([task]() {
            task();
            bgTaskCount--;
            });
        bgThread.detach();
        cout << "Running: [" << bgTaskCount.load() << "B]" << endl;
    }
    else {
        lock_guard<mutex> lock(mtx);
        fgTasks.push(task);
        cv.notify_one();
    }
}

// Foreground task processor
void processForegroundTasks() {
    while (!quit) {
        function<void()> task;
        {
            unique_lock<mutex> lock(mtx);
            cv.wait(lock, [] { return !fgTasks.empty() || quit; });
            if (quit && fgTasks.empty()) break;
            task = fgTasks.front();
            fgTasks.pop();
        }
        task();
    }
    cout << "prompt> ...";
}

int main() {
    ifstream file("commands.txt");
    string line;

    // Start the foreground task processor
    thread fgThread(processForegroundTasks);

    // Command parsing and dispatching
    regex commandPattern(R"((\&)?(\w+)(?:\s+(\S+))*)");
    while (getline(file, line)) {
        smatch matches;
        if (regex_match(line, matches, commandPattern)) {
            bool isBackground = matches[1].matched;
            string command = matches[2];
            vector<string> args;
            if (matches[3].matched) {
                istringstream argStream(matches[3]);
                string arg;
                while (argStream >> arg) {
                    args.push_back(arg);
                }
            }
            executeTask(command, args, isBackground);
        }
        this_thread::sleep_for(chrono::seconds(1)); // Simulate command interval
    }

    // Clean up
    quit = true;
    cv.notify_all();
    fgThread.join();
    return 0;
}








