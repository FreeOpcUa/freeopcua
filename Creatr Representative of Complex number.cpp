#include <iostream>
#include <cmath>
using namespace std;
int r,i,num;
class complex
{ int real,img; public: complexnum(int ,int ); void display();
void addreal(); void addimg();
void subreal();
void subimg();
};
complex::complexnum(int r,int i)
{
real=r;
img=i;
}
void complex::display()
{
char ch;
if(img>=0)
ch='+';
else
ch='-';
cout<<" "<<real<<ch<<abs(img)<<"i"<<endl;
}
void complex::addreal()
{
cout<<"Enter the no to add:\n";
cin>>num;
real=r+num;
}
void complex::addimg()
{
cout<<"Enter the no to add:\n";
cin>>num;
img=i+num;
}
void complex::subreal()
{
cout<<"Enter the no to sub:\n";
cin>>num;
real=r-num;
}
void complex::subimg()
{
cout<<"Enter the no to sub:\n";
cin>>num;
img=i-num;
}
int main()
{
complex c;
cout<<"Enter the real part of complex number:\n"; cin>>r;
cout<<"Enter the imaginary part of complex number:\n";
cin>>i;
c.complexnum (r,i);
int ch;
do
{
cout<<" What do you want to do:\n";
cout<<" 1.Display complex number\n 2.Add a real number\n 3.Subtract a real number\n 4.Add a imaginary number\n 5.Subtract a imaginary number\n 6.Exit\n";
cin>>ch;
switch(ch)
{
case 1:cout<<" The complex number is:\n";
c.display();
break;
case 2:c.addreal();
cout<<"result:\n";
c.display();
break;
case 3:c.subreal();
cout<<"result:\n";
c.display();
break;
case 4:c.addimg();
cout<<"result:\n";
c.display();
break;
case 5:c.subimg();
cout<<"result:\n";
c.display();
break;
case 6: cout<<"Exiting now";
break;
default:cout<<"Invalid Choice\n";
}
}while(ch!=6);
return 0;
}
