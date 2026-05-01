#include <iostream>
#include <algorithm>

using namespace std;

struct Node {
    int data;
    Node *left, *right;
    Node(int val) : data(val), left(nullptr), right(nullptr) {}
};

class BST {
private:
    Node* root;

    // --- Private Helper Functions (Internal Logic) ---

    Node* insert(Node* node, int val) {
        if (!node) return new Node(val);
        if (val < node->data) node->left = insert(node->left, val);
        else if (val > node->data) node->right = insert(node->right, val);
        return node;
    }

    bool search(Node* node, int val) {
        if (!node) return false;
        if (node->data == val) return true;
        return (val < node->data) ? search(node->left, val) : search(node->right, val);
    }

    Node* remove(Node* node, int val) {
        if (!node) return nullptr;
        if (val < node->data) node->left = remove(node->left, val);
        else if (val > node->data) node->right = remove(node->right, val);
        else {
            if (!node->left) {
                Node* temp = node->right;
                delete node;
                return temp;
            } else if (!node->right) {
                Node* temp = node->left;
                delete node;
                return temp;
            }
            Node* temp = findMin(node->right);
            node->data = temp->data;
            node->right = remove(node->right, temp->data);
        }
        return node;
    }

    Node* findMin(Node* node) {
        while (node && node->left) node = node->left;
        return node;
    }

    Node* findMax(Node* node) {
        while (node && node->right) node = node->right;
        return node;
    }

    // Traversal Helpers
    void inOrder(Node* node) {
        if (!node) return;
        inOrder(node->left);
        cout << node->data << " ";
        inOrder(node->right);
    }

    void preOrder(Node* node) {
        if (!node) return;
        cout << node->data << " ";
        preOrder(node->left);
        preOrder(node->right);
    }

    void postOrder(Node* node) {
        if (!node) return;
        postOrder(node->left);
        postOrder(node->right);
        cout << node->data << " ";
    }

    void clear(Node* node) {
        if (!node) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

public:
    BST() : root(nullptr) {}
    ~BST() { clear(root); } // auto clear

    // --- Public API (Accessible from main) ---

    void insert(int val) { root = insert(root, val); }

    bool search(int val) { return search(root, val); }

    void remove(int val) { root = remove(root, val); }

    int getMin() {
        Node* res = findMin(root);
        return res ? res->data : -1; // empty tree
    }

    int getMax() {
        Node* res = findMax(root);
        return res ? res->data : -1;
    }

    void printInOrder() { inOrder(root); cout << endl; }
    void printPreOrder() { preOrder(root); cout << endl; }
    void printPostOrder() { postOrder(root); cout << endl; }
};

int main() {
    BST tree;
    int values[] = {37, 24, 42, 7, 2, 40, 45, 32, 120};

    for (int v : values) tree.insert(v);

    cout << "Pre-order: ";  tree.printPreOrder();
    cout << "In-order: ";   tree.printInOrder();
    cout << "Post-order: "; tree.printPostOrder();

    cout << "\nMin Value: " << tree.getMin();
    cout << "\nMax Value: " << tree.getMax();

    cout << "\n\nSearching for 40: " << (tree.search(40) ? "Found" : "Not Found");

    cout << "\n\nDeleting 24 (Node with children)..." << endl;
    tree.remove(24);

    cout << "In-order after deletion: ";
    tree.printInOrder();

    return 0;
}
