#include <iostream>
#include "Stack.h"

using namespace std;

int main()
{
    Stack s;

    s.pop();
    s.push(2);
    s.push(4);
    s.push(6);
    s.push(8);
    s.push(10);
    s.push(12);

    cout<<endl;

    cout << "Top element: " << s.peek() << endl;
    cout<<endl;

            if (s.isFull())
        cout << "Stack is Full" << endl;
    else
        cout << "Stack is NOT Full" << endl;

            if (s.isEmpty())
        cout << "Stack is Empty" << endl;
    else
        cout << "Stack is NOT Empty" << endl;
        cout<<endl;

    s.pop();
    cout << "After pop, Top element: " << s.peek() << endl;
    cout<<endl;

    s.pop();
    s.pop();
    s.pop();
    s.pop();
    cout<<"After 4 pop \n \n";

    if (s.isEmpty())
        cout << "Stack is Empty" << endl;
    else
        cout << "Stack is NOT Empty" << endl;

        if (s.isFull())
        cout << "Stack is Full" << endl;
    else
        cout << "Stack is NOT Full" << endl;
        cout<<endl;


    s.display();
    cout<<endl;

    return 0;
}
