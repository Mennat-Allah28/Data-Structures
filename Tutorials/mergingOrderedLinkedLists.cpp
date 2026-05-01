#include <iostream>
using namespace std;

struct Node {
    int data;
    Node* next;

    Node(int val) : data(val), next(nullptr) {}
};

Node* createNode(int val) {
    return new Node(val);
}

void displayList(Node* head) {
    Node* current = head;
    while (current != nullptr) {
        cout << current->data;
        if (current->next != nullptr) cout << " -> ";
        current = current->next;
    }
    cout << endl;
}

void insertAtEnd(Node*& head, int val) {
    Node* newNode = createNode(val);
    if (head == nullptr) {
        head = newNode;
        return;
    }
    Node* temp = head;
    while (temp->next != nullptr) {
        temp = temp->next;
    }
    temp->next = newNode;
}

// Function to create a new list by merging L1 and L2 without modifying them
// Returns head of the new merged list L3
Node* mergeToNewList(Node* L1, Node* L2) {
    if (L1 == nullptr) {
        // Create a copy of L2
        if (L2 == nullptr) return nullptr;

        Node* L3 = nullptr;
        Node* tail = nullptr;
        Node* current = L2;

        while (current != nullptr) {
            Node* newNode = createNode(current->data);
            if (L3 == nullptr) {
                L3 = newNode;
                tail = newNode;
            } else {
                tail->next = newNode;
                tail = newNode;
            }
            current = current->next;
        }
        return L3;
    }

    if (L2 == nullptr) {
        Node* L3 = nullptr;
        Node* tail = nullptr;
        Node* current = L1;

        while (current != nullptr) {
            Node* newNode = createNode(current->data);
            if (L3 == nullptr) {
                L3 = newNode;
                tail = newNode;
            } else {
                tail->next = newNode;
                tail = newNode;
            }
            current = current->next;
        }
        return L3;
    }

    Node* L3 = nullptr;
    Node* tail = nullptr;
    Node* ptr1 = L1;
    Node* ptr2 = L2;

    while (ptr1 != nullptr && ptr2 != nullptr) {
        Node* newNode;
        if (ptr1->data <= ptr2->data) {
            newNode = createNode(ptr1->data);
            ptr1 = ptr1->next;
        } else {
            newNode = createNode(ptr2->data);
            ptr2 = ptr2->next;
        }

        if (L3 == nullptr) {
            L3 = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
    }

    while (ptr1 != nullptr) {
        Node* newNode = createNode(ptr1->data);
        tail->next = newNode;
        tail = newNode;
        ptr1 = ptr1->next;
    }

    while (ptr2 != nullptr) {
        Node* newNode = createNode(ptr2->data);
        tail->next = newNode;
        tail = newNode;
        ptr2 = ptr2->next;
    }

    return L3;
}

// Function to merge L2 into L1 and make L2 empty (reusing nodes, no new node creation)
void mergeIntoFirst(Node*& L1, Node*& L2) {
    if (L2 == nullptr) return;
    if (L1 == nullptr) {
        L1 = L2;
        L2 = nullptr;
        return;
    }

    Node* ptr1 = L1;
    Node* ptr2 = L2;
    Node* prev1 = nullptr;

    while (ptr1 != nullptr && ptr2 != nullptr) {
        if (ptr2->data <= ptr1->data) {
            Node* next2 = ptr2->next;

            if (prev1 == nullptr) {
                ptr2->next = ptr1;
                L1 = ptr2;
            } else {
                prev1->next = ptr2;
                ptr2->next = ptr1;
            }

            prev1 = ptr2;
            ptr2 = next2;
        } else {
            prev1 = ptr1;
            ptr1 = ptr1->next;
        }
    }

    if (ptr2 != nullptr) {
        if (prev1 != nullptr) {
            prev1->next = ptr2;
        } else {
            L1 = ptr2;
        }
    }

    L2 = nullptr;
}

// Function to delete entire linked list (for cleanup)
void deleteList(Node*& head) {
    Node* current = head;
    while (current != nullptr) {
        Node* temp = current;
        current = current->next;
        delete temp;
    }
    head = nullptr;
}

int main() {
    // Create first ordered list L1: 1 -> 2 -> 5 -> 7
    Node* L1 = nullptr;
    insertAtEnd(L1, 1);
    insertAtEnd(L1, 2);
    insertAtEnd(L1, 5);
    insertAtEnd(L1, 7);

    // Create second ordered list L2: 2 -> 4 -> 6 -> 8
    Node* L2 = nullptr;
    insertAtEnd(L2, 2);
    insertAtEnd(L2, 4);
    insertAtEnd(L2, 6);
    insertAtEnd(L2, 8);

    cout << "Original Lists:" << endl;
    cout << "L1: ";
    displayList(L1);
    cout << "L2: ";
    displayList(L2);

    // Case a: Merge into new list L3
    Node* L3 = mergeToNewList(L1, L2);
    cout << "\nCase a: Merge to new list L3" << endl;
    cout << "L1 (unchanged): ";
    displayList(L1);
    cout << "L2 (unchanged): ";
    displayList(L2);
    cout << "L3 (merged): ";
    displayList(L3);

    // Create new lists for case b
    Node* L1b = nullptr;
    insertAtEnd(L1b, 1);
    insertAtEnd(L1b, 2);
    insertAtEnd(L1b, 5);
    insertAtEnd(L1b, 7);

    Node* L2b = nullptr;
    insertAtEnd(L2b, 2);
    insertAtEnd(L2b, 4);
    insertAtEnd(L2b, 6);
    insertAtEnd(L2b, 8);

    cout << "\nCase b: Merge into L1 and empty L2" << endl;
    cout << "Before merging:" << endl;
    cout << "L1: ";
    displayList(L1b);
    cout << "L2: ";
    displayList(L2b);

    // Case b:merged list in L1 and make L2 empty
    mergeIntoFirst(L1b, L2b);

    cout << "After merging:" << endl;
    cout << "L1: ";
    displayList(L1b);
    cout << "L2: ";
    displayList(L2b);

    deleteList(L1);
    deleteList(L2);
    deleteList(L3);
    deleteList(L1b);

    return 0;
}
