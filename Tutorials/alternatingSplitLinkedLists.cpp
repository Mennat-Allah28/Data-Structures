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

void AlternatingSplit(Node*& L, Node*& L1, Node*& L2) {
    L1 = nullptr;
    L2 = nullptr;

    if (L == nullptr) return;

    Node* current = L;
    Node* tail1 = nullptr;
    Node* tail2 = nullptr;
    int count = 1;

    while (current != nullptr) {
        Node* nextNode = current->next;
        current->next = nullptr;

        if (count % 2 == 1) {
            if (L1 == nullptr) {
                L1 = current;
                tail1 = current;
            } else {
                tail1->next = current;
                tail1 = current;
            }
        } else {
            if (L2 == nullptr) {
                L2 = current;
                tail2 = current;
            } else {
                tail2->next = current;
                tail2 = current;
            }
        }

        current = nextNode;
        count++;
    }

    L = nullptr;
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
    // Create the original list: 1 -> 2 -> 3 -> 4 -> 5 -> 6 -> 7 -> 8
    Node* L = nullptr;
    insertAtEnd(L, 1);
    insertAtEnd(L, 2);
    insertAtEnd(L, 3);
    insertAtEnd(L, 4);
    insertAtEnd(L, 5);
    insertAtEnd(L, 6);
    insertAtEnd(L, 7);
    insertAtEnd(L, 8);

    cout << "Original List L: ";
    displayList(L);

    Node* L1 = nullptr;
    Node* L2 = nullptr;

    AlternatingSplit(L, L1, L2);

    cout << "\nAfter AlternatingSplit:" << endl;
    cout << "L1 (odd positions): ";
    displayList(L1);

    cout << "L2 (even positions): ";
    displayList(L2);

    cout << "L (original list): ";
    displayList(L);

    deleteList(L1);
    deleteList(L2);

    return 0;
}
