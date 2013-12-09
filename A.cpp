#include <iostream>
#include "A.h"
#include "B.h"

using namespace std;

void foo(int _something)
{
   cout << _something << "\n";
}

int main(int argc, char *argv[])
{
   B *b;
   
   foo ( 123 ); // So we don't get that pesky defined-not-used warning
   b = new B ();
   b->display ( 123 );
}
