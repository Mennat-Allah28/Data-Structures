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

void displayList(Node* head) {
    Node* current = head;
    while (current != nullptr) {
        cout << current->data;
        if (current->next != nullptr) cout << " -> ";
        current = current->next;
    }
    cout << endl;
}

void AlternatingMerge(Node*& L, Node*& L1, Node*& L2) {
    L = nullptr;

    if (L1 == nullptr && L2 == nullptr) return;

    Node* ptr1 = L1;
    Node* ptr2 = L2;
    Node* tail = nullptr;

    if (ptr1 != nullptr) {
        L = ptr1;
        tail = ptr1;
        ptr1 = ptr1->next;
        tail->next = nullptr;
    }
    else if (ptr2 != nullptr) {
        L = ptr2;
        tail = ptr2;
        ptr2 = ptr2->next;
        tail->next = nullptr;
    }

    while (ptr1 != nullptr || ptr2 != nullptr) {
        if (ptr2 != nullptr) {
            tail->next = ptr2;
            tail = ptr2;
            ptr2 = ptr2->next;
            tail->next = nullptr;
        }

        if (ptr1 != nullptr) {
            tail->next = ptr1;
            tail = ptr1;
            ptr1 = ptr1->next;
            tail->next = nullptr;
        }
    }

    L1 = nullptr;
    L2 = nullptr;
}

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
    Node* L1 = nullptr;
    insertAtEnd(L1, 1);
    insertAtEnd(L1, 3);
    insertAtEnd(L1, 5);
    insertAtEnd(L1, 7);

    Node* L2 = nullptr;
    insertAtEnd(L2, 2);
    insertAtEnd(L2, 4);
    insertAtEnd(L2, 6);
    insertAtEnd(L2, 8);

    Node* L = nullptr;

    cout << "Before AlternatingMerge:" << endl;
    cout << "L1: ";
    displayList(L1);
    cout << "L2: ";
    displayList(L2);
    cout << "L: ";
    displayList(L);

    AlternatingMerge(L, L1, L2);

    cout << "\nAfter AlternatingMerge:" << endl;
    cout << "L (merged): ";
    displayList(L);


    deleteList(L);

    return 0;
}
