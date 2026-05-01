#include <iostream>

using namespace std;

struct student
{
    int id;
    string name;
    float grade;
};
student*findTopStudent(student arr[],int size)
{
    int MaxIndex=0;
    for(int i=1;i<size;i++)
        {
            if(arr[i].grade>arr[MaxIndex].grade)
            MaxIndex=i;
        }
    return &arr[MaxIndex];
};
student*findLowestStudent(student arr[],int size)
{
    int leastIndex=0;
    for(int i=1;i<size;i++)
        {
            if(arr[i].grade<arr[leastIndex].grade)
            leastIndex=i;
        }
    return &arr[leastIndex];
};

int main()
{
    student students[3]={
        {1,"Menna",90},
        {2,"Mariem",80},
        {3,"Malk",70},
    };

    student*topStudent=findTopStudent(students,3);
    student*leastStudent=findLowestStudent(students,3);

    cout<<"Top Student : \n";
    cout<<"ID : \t"<<topStudent->id<<endl;
    cout<<"Name : \t"<<topStudent->name<<endl;
    cout<<"Grade : "<<topStudent->grade<<endl;

    cout<<endl;
    cout<<"Lowest Student : \n";
    cout<<"ID : \t"<<leastStudent->id<<endl;
    cout<<"Name : \t"<<leastStudent->name<<endl;
    cout<<"Grade : "<<leastStudent->grade<<endl;
    return 0;
}
