#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <map>

using namespace std;

class Process {
public:
    int id;
    bool isForeground;
    int waitTime = 0; // Added wait time for each process for simulation

    Process(int id, bool isForeground) : id(id), isForeground(isForeground) {}
};

class StackNode {
public:
    list<shared_ptr<Process>> processes;
    shared_ptr<StackNode> next;
    shared_ptr<StackNode> prev;

    StackNode() : next(nullptr), prev(nullptr) {}
};

class DynamicQueue {
private:
    mutex mtx;
    shared_ptr<StackNode> head;
    shared_ptr<StackNode> tail;
    map<int, int> waitQueue; // Map to simulate Wait Queue
    atomic<int> processCount{ 0 };

public:
    DynamicQueue() {
        head = make_shared<StackNode>();
        tail = head;
    }

    void enqueue(shared_ptr<Process> process) {
        lock_guard<mutex> lock(mtx);
        shared_ptr<StackNode> newNode = make_shared<StackNode>();
        newNode->processes.push_back(process);
        if (process->isForeground) {
            if (head != tail || head->processes.empty()) {
                newNode->next = head;
                head->prev = newNode;
                head = newNode;
            }
            else {
                head->processes.push_back(process);
            }
        }
        else {
            if (tail->processes.empty()) {
                tail->processes.push_back(process);
            }
            else {
                newNode->prev = tail;
                tail->next = newNode;
                tail = newNode;
            }
        }
        processCount++;
        waitQueue[process->id] = process->waitTime; // Simulate initial wait time
    }

    void printQueue() {
        lock_guard<mutex> lock(mtx);
        cout << "Running: [" << processCount.load() << "]\n";
        cout << "---------------------------\n";
        cout << "DQ: ";

        auto printProcesses = [](const list<shared_ptr<Process>>& processes) {
            for (auto& p : processes) {
                cout << p->id << (p->isForeground ? "F " : "B ") << " ";
            }
            };

        if (tail) {
            for (auto current = tail; current != nullptr; current = current->prev) {
                if (current->prev) {
                    printProcesses(current->processes);
                    cout << "(bottom)\n";
                    cout << "P => ";
                }
                else {
                    printProcesses(current->processes);
                    cout << "(top)\n";
                }
            }
        }

        cout << "---------------------------\n";
        cout << "WQ: ";
        for (auto& wp : waitQueue) {
            cout << "[" << wp.first << ":" << wp.second << "] ";
        }
        cout << "\n...\n";
    }
};

int main() {
    auto dq = make_shared<DynamicQueue>();
    dq->enqueue(make_shared<Process>(0, true));  // Foreground
    dq->enqueue(make_shared<Process>(1, false)); // Background
    this_thread::sleep_for(chrono::seconds(1));
    dq->printQueue();

    dq->enqueue(make_shared<Process>(2, false));
    dq->enqueue(make_shared<Process>(3, true));
    dq->enqueue(make_shared<Process>(4, false));
    dq->enqueue(make_shared<Process>(5, true));
    this_thread::sleep_for(chrono::seconds(1));
    dq->printQueue();

    return 0;
}



