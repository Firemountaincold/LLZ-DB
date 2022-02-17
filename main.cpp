#include "llzdb.cpp"
#include <iostream>

int main()
{
    LDTemplate tt("D:\\llzdb\\test.xml");
    cout << tt.GetTemplatestring() << endl;

    string file = "tt.llzdb";
    string m2 = ReadAllData(file);
    string m3 = ReadLastData(file);
    cout << m2 << endl
         << m3 << endl
         << endl;
    return 0;
}