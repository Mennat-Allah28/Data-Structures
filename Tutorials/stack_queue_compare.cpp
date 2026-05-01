#include <iostream>
#include <stack>
#include <queue>
using namespace std;

bool SameOrder(stack<char> s, queue<char> q)
{
    if (s.size() != q.size())
        return false;

    stack<char> temp;

    while (!s.empty())
    {
        temp.push(s.top());
        s.pop();
    }

    while (!temp.empty())
    {
        if (temp.top() != q.front())
            return false;

        temp.pop();
        q.pop();
    }

    return true;
}
int main()
{
    stack<char> s,S;
    queue<char> q;

    string name = "Mennat-allah Gamal - 91240786 ";

    for(char c : name)
    {
        s.push(c);
        S.push(c);
        q.push(c);
    }

    cout<<"Stack (Bottom to Top): ";
    stack<char> temp ;
    while (!s.empty())
    {
        temp.push(s.top());
        s.pop();
    }
    while(!temp.empty())
    {
        cout<<temp.top()<<" ";
        temp.pop();
    }

    cout<<endl;

    cout<<"Queue (Front to Back): ";
    queue<char> tempq = q;
    while(!tempq.empty())
    {
        cout<<tempq.front()<<" ";
        tempq.pop();
    }

    cout<<endl;

    if(SameOrder(S,q))
        cout<<"Same Order"<<endl;
    else
        cout<<"Not Same Order"<<endl;
    cout<<endl;

    //test2
    stack<char> s2;
    queue<char> q2;

    s2.push('A');
    s2.push('B');
    s2.push('C');

    q2.push('A');
    q2.push('C');
    q2.push('B');

    cout<<"char (Bottom to Top) : ABC ";
    cout<<endl;
    cout<<"Queue (Front to Back) : ACB ";
    cout<<endl;

    if(SameOrder(s2,q2))
        cout<<"Same Order"<<endl;
    else
        cout<<"Not Same Order"<<endl;
    cout<<endl;

}
