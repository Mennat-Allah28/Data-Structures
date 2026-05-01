#include <iostream>
#include <stack>
using namespace std;

void collapse(stack<int>& st) {
    stack<int> temp;

    while (!st.empty()) {
        int x1 = st.top();
        st.pop();
        if(!st.empty()){
                int x2 = st.top();
                st.pop();
                temp.push(x1+x2);
        }else{
        temp.push(x1);
        }
    }

    while (!temp.empty()) {
        st.push(temp.top());
        temp.pop();
    }
}

int main() {
    stack<int> s1;

    s1.push(7);
    s1.push(2);
    s1.push(8);
    s1.push(9);
    s1.push(4);
    s1.push(13);
    s1.push(7);
    s1.push(1);
    s1.push(9);

    collapse(s1);

    while (!s1.empty()) {
        cout << s1.top() << " , ";
        s1.pop();
    }

    return 0;
}
