//2-2.alarm clock
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
//#include <fstream>
//#include <sstream>
//#include <queue>
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
//        auto proc = find_if(fgProcesses.begin(), fgProcesses.end(), [&pid](const auto& p) { return p->id == pid; });
//        if (proc != fgProcesses.end()) {
//            waitQueue[pid] = *proc;
//            fgProcesses.erase(proc);
//        }
//        else {
//            proc = find_if(bgProcesses.begin(), bgProcesses.end(), [&pid](const auto& p) { return p->id == pid; });
//            if (proc != bgProcesses.end()) {
//                waitQueue[pid] = *proc;
//                bgProcesses.erase(proc);
//            }
//        }
//        waitQueue[pid]->remainingTime = seconds;
//    }
//
//    void wakeUpProcesses() {
//        lock_guard<mutex> lock(mtx);
//        for (auto it = waitQueue.begin(); it != waitQueue.end();) {
//            if (--it->second->remainingTime <= 0) {
//                enqueue(it->second);
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
//        cout << "DQ: ";
//        cout << "(bottom) ";
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
//    thread shellThread(shellProcess, ref(dq), 5); // Simulate shell processing
//    thread monitorThread(monitorProcess, ref(dq), 10);
//
//    shellThread.join();
//    monitorThread.detach();
//
//    return 0;
//}





