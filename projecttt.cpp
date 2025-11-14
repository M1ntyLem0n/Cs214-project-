#include <iostream>
#include <fstream>
#include <unordered_map>
#include <stack>
#include <string>
#include <iomanip>
#include <algorithm> // for max
using namespace std;

// ================================
// Node for linked list (task)
// ================================
struct Node {
    int id;
    string description;
    int priority;   // 1..5
    bool completed;
    Node* next;
};

// ================================
// Container for all program data
// (avoids global variables)
// ================================
struct ToDoData {
    Node* head;
    unordered_map<int, Node*> taskMap;
    stack<Node*> deletedStack; // stores copies of deleted tasks for undo
    int nextId;
    int totalTasks;

    ToDoData() : head(nullptr), nextId(1), totalTasks(0) {}
};

// ================================
// Utility: free all nodes in linked list
// and clear map and stack (to avoid memory leak)
// ================================
void freeAll(ToDoData& data) {
    Node* cur = data.head;
    while (cur) {
        Node* tmp = cur;
        cur = cur->next;
        delete tmp;
    }
    data.head = nullptr;
    data.taskMap.clear();

    // clear stack nodes
    while (!data.deletedStack.empty()) {
        delete data.deletedStack.top();
        data.deletedStack.pop();
    }
    data.totalTasks = 0;
}

// ================================
// Add a new task (to end of linked list)
// ================================
void addTask(ToDoData& data, const string& desc, int priority = 1) {
    if (desc.empty()) {
        cout << "✗ Description cannot be empty!\n";
        return;
    }
    int pr = priority;
    if (pr < 1 || pr > 5) pr = 1;

    Node* newNode = new Node{ data.nextId, desc, pr, false, nullptr };

    if (data.head == nullptr) {
        data.head = newNode;
    }
    else {
        Node* cur = data.head;
        while (cur->next) cur = cur->next;
        cur->next = newNode;
    }

    data.taskMap[data.nextId] = newNode;
    cout << "✓ Task added with ID: " << data.nextId << "\n";
    data.nextId++;
    data.totalTasks++;
}

// ================================
// Edit a task by ID (description & priority)
// ================================
void editTask(ToDoData& data, int id) {
    auto it = data.taskMap.find(id);
    if (it == data.taskMap.end()) {
        cout << "✗ Task not found!\n";
        return;
    }
    Node* t = it->second;
    cout << "Current description: " << t->description << "\n";
    cout << "Enter new description (leave empty to keep): ";
    string newDesc;
    getline(cin, newDesc);
    if (!newDesc.empty()) t->description = newDesc;

    cout << "Current priority: " << t->priority << "\n";
    cout << "Enter new priority (1-5, 0 to keep): ";
    int newPrio;
    if (!(cin >> newPrio)) { // handle non-int input
        cin.clear();
        string dummy;
        getline(cin, dummy);
        cout << "Invalid input. Priority unchanged.\n";
    }
    else {
        cin.ignore(); // remove newline
        if (newPrio >= 1 && newPrio <= 5) t->priority = newPrio;
        else if (newPrio != 0) cout << "Invalid priority. Keeping old value.\n";
    }

    cout << "✓ Task " << id << " updated.\n";
}

// ================================
// Delete task by ID (push a copy to stack for undo)
// ================================
void deleteTask(ToDoData& data, int id) {
    auto it = data.taskMap.find(id);
    if (it == data.taskMap.end()) {
        cout << "✗ Task not found!\n";
        return;
    }

    Node* cur = data.head;
    Node* prev = nullptr;
    while (cur && cur->id != id) {
        prev = cur;
        cur = cur->next;
    }
    if (!cur) return; // safety

    // detach from list
    if (!prev) data.head = cur->next;
    else prev->next = cur->next;

    // push a copy to deletedStack (so we can restore later)
    Node* copyNode = new Node{ cur->id, cur->description, cur->priority, cur->completed, nullptr };
    data.deletedStack.push(copyNode);

    // remove from map and free node
    data.taskMap.erase(id);
    delete cur;
    data.totalTasks--;

    cout << "✓ Task deleted (you can undo it).\n";
}

// ================================
// Undo last delete (restore node to head)
// ================================
void undoDelete(ToDoData& data) {
    if (data.deletedStack.empty()) {
        cout << "⚠ No deleted task to undo.\n";
        return;
    }
    Node* restored = data.deletedStack.top();
    data.deletedStack.pop();

    // insert at head for simplicity
    restored->next = data.head;
    data.head = restored;

    data.taskMap[restored->id] = restored;
    data.totalTasks++;
    // ensure nextId stays greater than any id
    data.nextId = max(data.nextId, restored->id + 1);

    cout << "↩ Task " << restored->id << " restored.\n";
}

// ================================
// Search task by ID and print details
// ================================
void searchTask(ToDoData& data, int id) {
    auto it = data.taskMap.find(id);
    if (it == data.taskMap.end()) {
        cout << "✗ Task not found!\n";
        return;
    }
    Node* t = it->second;
    cout << "\n--- Task Found ---\n";
    cout << "ID: " << t->id << "\n";
    cout << "Description: " << t->description << "\n";
    cout << "Priority: " << t->priority << "\n";
    cout << "Status: " << (t->completed ? "Done" : "Pending") << "\n";
    cout << "------------------\n";
}

// ================================
// Mark complete / incomplete
// ================================
void markComplete(ToDoData& data, int id, bool done) {
    auto it = data.taskMap.find(id);
    if (it == data.taskMap.end()) {
        cout << "✗ Task not found!\n";
        return;
    }
    it->second->completed = done;
    cout << "✓ Task " << id << (done ? " completed.\n" : " marked incomplete.\n");
}

// ================================
// Print all tasks (traverse linked list)
// ================================
void printAll(ToDoData& data) {
    if (!data.head) {
        cout << "📋 No tasks available.\n";
        return;
    }
    cout << "\n" << string(60, '=') << "\n";
    cout << setw(35) << "TO-DO LIST\n";
    cout << string(60, '=') << "\n";
    Node* cur = data.head;
    while (cur) {
        cout << (cur->completed ? "✓ " : "○ ")
            << "[ID:" << cur->id << "] "
            << left << setw(30) << cur->description
            << " (P:" << cur->priority << ")\n";
        cur = cur->next;
    }
    cout << string(60, '=') << "\n";
}

// ================================
// Save tasks to file (tasks.txt)
// format per line: id|description|priority|completed
// ================================
void saveToFile(ToDoData& data, const string& filename = "tasks.txt") {
    ofstream fout(filename);
    if (!fout.is_open()) {
        cout << "✗ Unable to open file for saving.\n";
        return;
    }
    Node* cur = data.head;
    while (cur) {
        // Replace any '|' in description to avoid parse issues (simple sanitize)
        string desc = cur->description;
        size_t pos;
        while ((pos = desc.find('|')) != string::npos) desc.replace(pos, 1, " "); // remove pipe
        fout << cur->id << "|" << desc << "|" << cur->priority << "|" << (cur->completed ? 1 : 0) << "\n";
        cur = cur->next;
    }
    fout.close();
    cout << "✓ Tasks saved to '" << filename << "'.\n";
}

// ================================
// Load tasks from file (tasks.txt)
// expects same format used in saveToFile
// ================================
void loadFromFile(ToDoData& data, const string& filename = "tasks.txt") {
    ifstream fin(filename);
    if (!fin.is_open()) {
        // no file yet — not an error
        return;
    }
    string line;
    while (getline(fin, line)) {
        if (line.empty()) continue;
        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);
        size_t p3 = line.find('|', p2 + 1);
        if (p1 == string::npos || p2 == string::npos || p3 == string::npos) continue; // malformed line

        int id = stoi(line.substr(0, p1));
        string desc = line.substr(p1 + 1, p2 - p1 - 1);
        int prio = stoi(line.substr(p2 + 1, p3 - p2 - 1));
        int done = stoi(line.substr(p3 + 1));

        Node* n = new Node{ id, desc, prio, (done != 0), nullptr };
        if (data.head == nullptr) data.head = n;
        else {
            Node* cur = data.head;
            while (cur->next) cur = cur->next;
            cur->next = n;
        }
        data.taskMap[id] = n;
        data.nextId = max(data.nextId, id + 1);
        data.totalTasks++;
    }
    fin.close();
}

// ================================
// Print tasks to a separate output file for submission (output.txt)
// ================================
void printToOutputFile(ToDoData& data, const string& filename = "output.txt") {
    ofstream fout(filename);
    if (!fout.is_open()) {
        cout << "✗ Unable to open output file.\n";
        return;
    }
    Node* cur = data.head;
    while (cur) {
        fout << (cur->completed ? "[✓] " : "[ ] ")
            << cur->id << " - " << cur->description
            << " (P:" << cur->priority << ")\n";
        cur = cur->next;
    }
    fout.close();
    cout << "✓ Output written to '" << filename << "'.\n";
}

// ================================
// Simple menu display
// ================================
void showMenu() {
    cout << "\n========== TO-DO LIST MENU ==========\n";
    cout << "1. Add Task\n";
    cout << "2. Edit Task\n";
    cout << "3. Delete Task\n";
    cout << "4. Undo Delete (restore last deleted)\n";
    cout << "5. Mark Complete\n";
    cout << "6. Mark Incomplete\n";
    cout << "7. Search Task by ID\n";
    cout << "8. Show All Tasks\n";
    cout << "9. Save & Exit (also write output.txt)\n";
    cout << "====================================\n";
    cout << "Choose: ";
}

// ================================
// Main program
// ================================
int main() {
    ToDoData data;
    // Load from file at start
    loadFromFile(data, "tasks.txt");

    // Example: if you want to add sample tasks uncomment below:
    // addTask(data, "Sample task 1", 3);
    // addTask(data, "Sample task 2", 2);

    while (true) {
        showMenu();
        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            string dummy;
            getline(cin, dummy);
            cout << "Invalid input. Try again.\n";
            continue;
        }
        cin.ignore(); // remove newline before getline uses

        if (choice == 1) {
            cout << "Enter description: ";
            string desc;
            getline(cin, desc);
            cout << "Enter priority (1-5, default 1): ";
            int p;
            if (!(cin >> p)) { cin.clear(); string d; getline(cin, d); p = 1; }
            cin.ignore();
            addTask(data, desc, p);
        }
        else if (choice == 2) {
            cout << "Enter Task ID to edit: ";
            int id; cin >> id; cin.ignore();
            editTask(data, id);
        }
        else if (choice == 3) {
            cout << "Enter Task ID to delete: ";
            int id; cin >> id; cin.ignore();
            deleteTask(data, id);
        }
        else if (choice == 4) {
            undoDelete(data);
        }
        else if (choice == 5) {
            cout << "Enter Task ID to mark complete: ";
            int id; cin >> id; cin.ignore();
            markComplete(data, id, true);
        }
        else if (choice == 6) {
            cout << "Enter Task ID to mark incomplete: ";
            int id; cin >> id; cin.ignore();
            markComplete(data, id, false);
        }
        else if (choice == 7) {
            cout << "Enter Task ID to search: ";
            int id; cin >> id; cin.ignore();
            searchTask(data, id);
        }
        else if (choice == 8) {
            printAll(data);
        }
        else if (choice == 9) {
            saveToFile(data, "tasks.txt");
            printToOutputFile(data, "output.txt");
            cout << "Goodbye!\n";
            break;
        }
        else {
            cout << "Invalid choice. Enter 1..9.\n";
        }
    }

    // free memory before exit
    freeAll(data);
    return 0;
}
