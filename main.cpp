/*
 * 1. Linked List - For sequential storage and traversal of tasks
 * 2. Hash Table (unordered_map) - For O(1) lookup by task ID
 */

#include <iostream>
#include <string>
#include <unordered_map>
#include <iomanip>
using namespace std;

// Node class for the linked list
class Node {
public:
    int taskId;
    string description;
    int priority;
    bool completed;
    Node* next;
    
    // Constructor
    Node(int id, string desc, int prio) {
        taskId = id;
        description = desc;
        priority = prio;
        completed = false;
        next = nullptr;
    }
};

// To-Do List class using Linked List and Hash Table
class ToDoList {
private:
    Node* head;                           
    unordered_map<int, Node*> hashTable;  
    int taskCounter;                      
    int size;                             
    
public:
    // Constructor
    ToDoList() {
        head = nullptr;
        taskCounter = 1;
        size = 0;
    }
    
    // Destructor to free memory
    ~ToDoList() {
        Node* current = head;
        while (current != nullptr) {
            Node* temp = current;
            current = current->next;
            delete temp;
        }
    }
    
    // Add a new task to the list
    void addTask(string description, int priority = 1) {
        if (description.empty()) {
            cout << "\n✗ Task description cannot be empty!\n";
            return;
        }
        
        if (priority < 1 || priority > 5) {
            cout << "\n✗ Priority must be between 1 and 5. Setting to 1.\n";
            priority = 1;
        }
        
        int taskId = taskCounter;
        Node* newNode = new Node(taskId, description, priority);
        
        // Add to hash table for O(1) lookup
        hashTable[taskId] = newNode;
        
        // Add to linked list (at the end)
        if (head == nullptr) {
            head = newNode;
        } else {
            Node* current = head;
            while (current->next != nullptr) {
                current = current->next;
            }
            current->next = newNode;
        }
        
        taskCounter++;
        size++;
        cout << "\n✓ Task added successfully with ID: " << taskId << "\n";
    }
    
    // Delete a task by ID
    void deleteTask(int taskId) {
        if (hashTable.find(taskId) == hashTable.end()) {
            cout << "\n✗ Task ID " << taskId << " not found!\n";
            return;
        }
        
        // Remove from linked list
        if (head->taskId == taskId) {
            Node* temp = head;
            head = head->next;
            delete temp;
        } else {
            Node* current = head;
            while (current->next != nullptr && current->next->taskId != taskId) {
                current = current->next;
            }
            if (current->next != nullptr) {
                Node* temp = current->next;
                current->next = current->next->next;
                delete temp;
            }
        }
        
        // Remove from hash table
        hashTable.erase(taskId);
        size--;
        cout << "\n✓ Task " << taskId << " deleted successfully!\n";
    }
    
    // Mark a task as completed using hash table lookup (O(1))
    void markComplete(int taskId) {
        if (hashTable.find(taskId) != hashTable.end()) {
            if (hashTable[taskId]->completed) {
                cout << "\n⚠ Task " << taskId << " is already marked as complete!\n";
            } else {
                hashTable[taskId]->completed = true;
                cout << "\n✓ Task " << taskId << " marked as complete!\n";
            }
        } else {
            cout << "\n✗ Task ID " << taskId << " not found!\n";
        }
    }
    
    // Mark a task as incomplete
    void markIncomplete(int taskId) {
        if (hashTable.find(taskId) != hashTable.end()) {
            if (!hashTable[taskId]->completed) {
                cout << "\n⚠ Task " << taskId << " is already marked as incomplete!\n";
            } else {
                hashTable[taskId]->completed = false;
                cout << "\n✓ Task " << taskId << " marked as incomplete!\n";
            }
        } else {
            cout << "\n✗ Task ID " << taskId << " not found!\n";
        }
    }
    
    // Search for a task by ID using hash table - O(1) complexity
    void searchTask(int taskId) {
        if (hashTable.find(taskId) != hashTable.end()) {
            Node* node = hashTable[taskId];
            string status = node->completed ? "✓ Completed" : "○ Pending";
            
            cout << "\n" << string(60, '=') << "\n";
            cout << "TASK DETAILS\n";
            cout << string(60, '=') << "\n";
            cout << "Task ID: " << node->taskId << "\n";
            cout << "Description: " << node->description << "\n";
            cout << "Priority: " << node->priority << "\n";
            cout << "Status: " << status << "\n";
            cout << string(60, '=') << "\n\n";
        } else {
            cout << "\n✗ Task ID " << taskId << " not found!\n";
        }
    }
    
    // Display all tasks by traversing the linked list
    void displayAll() {
        if (head == nullptr) {
            cout << "\n📋 No tasks in the list! Add some tasks to get started.\n\n";
            return;
        }
        
        cout << "\n" << string(70, '=') << "\n";
        cout << setw(40) << "TO-DO LIST\n";
        cout << string(70, '=') << "\n";
        cout << "Total tasks: " << size << "\n\n";
        
        Node* current = head;
        while (current != nullptr) {
            string status = current->completed ? "✓" : "○";
            cout << status << " [ID: " << setw(2) << current->taskId << "] " 
                 << setw(40) << left << current->description 
                 << " (Priority: " << current->priority << ")\n";
            current = current->next;
        }
        cout << string(70, '=') << "\n\n";
    }
    
    // Display only pending tasks
    void displayPending() {
        cout << "\n" << string(70, '=') << "\n";
        cout << setw(40) << "PENDING TASKS\n";
        cout << string(70, '=') << "\n";
        
        Node* current = head;
        bool found = false;
        int count = 0;
        
        while (current != nullptr) {
            if (!current->completed) {
                cout << "○ [ID: " << setw(2) << current->taskId << "] " 
                     << setw(40) << left << current->description 
                     << " (Priority: " << current->priority << ")\n";
                found = true;
                count++;
            }
            current = current->next;
        }
        
        if (!found) {
            cout << "🎉 No pending tasks! Great job!\n";
        } else {
            cout << "\nTotal pending tasks: " << count << "\n";
        }
        cout << string(70, '=') << "\n\n";
    }
    
    // Display only completed tasks
    void displayCompleted() {
        cout << "\n" << string(70, '=') << "\n";
        cout << setw(40) << "COMPLETED TASKS\n";
        cout << string(70, '=') << "\n";
        
        Node* current = head;
        bool found = false;
        int count = 0;
        
        while (current != nullptr) {
            if (current->completed) {
                cout << "✓ [ID: " << setw(2) << current->taskId << "] " 
                     << setw(40) << left << current->description 
                     << " (Priority: " << current->priority << ")\n";
                found = true;
                count++;
            }
            current = current->next;
        }
        
        if (!found) {
            cout << "No completed tasks yet!\n";
        } else {
            cout << "\nTotal completed tasks: " << count << "\n";
        }
        cout << string(70, '=') << "\n\n";
    }
    
    // Display statistics about tasks
    void getStatistics() {
        if (size == 0) {
            cout << "\n📊 No tasks to show statistics for!\n\n";
            return;
        }
        
        int completed = 0;
        Node* current = head;
        while (current != nullptr) {
            if (current->completed) completed++;
            current = current->next;
        }
        
        int pending = size - completed;
        double completionRate = (size > 0) ? (completed * 100.0 / size) : 0.0;
        
        cout << "\n" << string(60, '=') << "\n";
        cout << setw(35) << "TASK STATISTICS\n";
        cout << string(60, '=') << "\n";
        cout << "Total Tasks:       " << size << "\n";
        cout << "Completed Tasks:   " << completed << "\n";
        cout << "Pending Tasks:     " << pending << "\n";
        cout << fixed << setprecision(1);
        cout << "Completion Rate:   " << completionRate << "%\n";
        cout << string(60, '=') << "\n\n";
    }
};

// Function to print program header
void printHeader() {
    cout << "\n" << string(70, '=') << "\n";
    cout << setw(45) << "TO-DO LIST MANAGEMENT SYSTEM\n";
    cout << setw(48) << "Using Linked List & Hash Table Data Structures\n";
    cout << string(70, '=') << "\n";
}

// Main function
int main() {
    ToDoList todo;
    
    printHeader();
    
    // Add some sample tasks for demonstration
    cout << "\n💡 Tip: Sample tasks have been added for demonstration!\n";
    todo.addTask("Complete data structures assignment", 5);
    todo.addTask("Study for midterm exam", 4);
    todo.addTask("Buy groceries", 2);
    
    int choice, taskId, priority;
    string description, priorityInput;
    
    while (true) {
        cout << "\n" << string(70, '-') << "\n";
        cout << setw(40) << "MAIN MENU\n";
        cout << string(70, '-') << "\n";
        cout << "1.  Add Task\n";
        cout << "2.  Delete Task\n";
        cout << "3.  Mark Task as Complete\n";
        cout << "4.  Mark Task as Incomplete\n";
        cout << "5.  Search Task by ID\n";
        cout << "6.  Display All Tasks\n";
        cout << "7.  Display Pending Tasks\n";
        cout << "8.  Display Completed Tasks\n";
        cout << "9.  Show Statistics\n";
        cout << "10. Exit\n";
        cout << string(70, '-') << "\n";
        
        cout << "\n👉 Enter your choice (1-10): ";
        cin >> choice;
        cin.ignore(); // Clear input buffer
        
        switch (choice) {
            case 1:
                cout << "\n--- ADD NEW TASK ---\n";
                cout << "Enter task description: ";
                getline(cin, description);
                cout << "Enter priority (1-5, default is 1): ";
                getline(cin, priorityInput);
                priority = priorityInput.empty() ? 1 : stoi(priorityInput);
                todo.addTask(description, priority);
                break;
                
            case 2:
                cout << "\n--- DELETE TASK ---\n";
                cout << "Enter task ID to delete: ";
                cin >> taskId;
                todo.deleteTask(taskId);
                break;
                
            case 3:
                cout << "\n--- MARK TASK AS COMPLETE ---\n";
                cout << "Enter task ID to mark complete: ";
                cin >> taskId;
                todo.markComplete(taskId);
                break;
                
            case 4:
                cout << "\n--- MARK TASK AS INCOMPLETE ---\n";
                cout << "Enter task ID to mark incomplete: ";
                cin >> taskId;
                todo.markIncomplete(taskId);
                break;
                
            case 5:
                cout << "\n--- SEARCH TASK ---\n";
                cout << "Enter task ID to search: ";
                cin >> taskId;
                todo.searchTask(taskId);
                break;
                
            case 6:
                todo.displayAll();
                break;
                
            case 7:
                todo.displayPending();
                break;
                
            case 8:
                todo.displayCompleted();
                break;
                
            case 9:
                todo.getStatistics();
                break;
                
            case 10:
                cout << "\n" << string(70, '=') << "\n";
                cout << setw(42) << "Thank you for using To-Do List! 👋\n";
                cout << setw(43) << "Stay productive and organized! 📝\n";
                cout << string(70, '=') << "\n\n";
                return 0;
                
            default:
                cout << "\n✗ Invalid choice! Please enter a number between 1-10.\n";
        }
    }
    
    return 0;
}