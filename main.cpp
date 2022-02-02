#include "llzdb.cpp"
#include <iostream>

int main()
{
    LDTemplate tt("D:\\llzdb\\test.xml");
    cout << tt.GetTemplatestring() << endl;

    string file = "./tt.llzdb";
    string m2 = ReadData(file, 2);
    string m3 = ReadLastData(file);
    cout << m2 << endl
         << endl
         << m3 << endl;
    return 0;
}