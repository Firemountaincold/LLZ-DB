#include "llzdb.h"

string readLine(string filename, int line)
{
    //读取文件的指定一行
    ifstream text(filename, ios::in);

    int c = 0;
    while (!text.eof())
    {
        string inbuf;
        text.seekg(0, ios::cur);
        getline(text, inbuf);
        if (c == line - 1)
        {
            text.close();
            return inbuf;
        }
        c++;
    }
    text.close();
    return "NULL";
}

long ReadTimestamp(string DataString)
{
    //读取一行中的时间戳
    int c = DataString.find(":");
    return atol(DataString.substr(0, c).c_str());
}

int GetTimestampLine(LDTemplate temp, string filename, long timestamp)
{
    //根据时间戳获取应该插入的位置
    ifstream rf(filename);
    long ts;
    int line = 3;
    string a = readLine(filename, line);
    while (a != "NULL")
    {
        ts = ReadTimestamp(a);
        if (ts >= timestamp)
        {
            rf.close();
            return line;
        }
        line++;
        a = readLine(filename, line);
    }
    rf.close();
    return line;
}

string GetPrintData(string data)
{
    //把一行文件数据转化为一行打印用数据
    if (data.find(":") < data.size())
        data.replace(data.find(":"), 1, "   ");
    while (true)
    {
        if (data.find("@") < data.size())
            data.replace(data.find("@"), 1, "  ");
        else
            break;
    }
    return data;
}

int CreateAsTemplate(LDTemplate temp, string filename)
{
    //根据模板创建文件
    fstream afile(filename, fstream::out);
    afile.close();
    fstream cfile(filename);
    if (cfile.is_open())
    {
        cfile << temp.GetMagicString() << endl;
        cfile << temp.GetNameString() << endl;
        cfile.close();
        return 1;
    }
    return 0;
}

int SaveAsTemplate(LDTemplate temp, string filename, long timestamp, string *measuments)
{
    //为文件添加数据
    rename(filename.c_str(), (filename + "cp").c_str());
    ifstream rfile;
    ofstream wfile;
    rfile.open((filename + "cp"), ios::in);
    wfile.open(filename, ios::out | ios::trunc);
    int line = GetTimestampLine(temp, (filename + "cp"), timestamp);
    int i = 1;
    bool k = false;
    rfile.seekg(0, ios::beg);
    while (!rfile.eof())
    {
        if (i == line)
        {
            wfile << timestamp << ":";
            for (int j = 0; j < temp.count; j++)
            {
                wfile << measuments[j] << "@";
            }
            wfile << endl;
            k = true;
        }
        else
        {
            string tem;
            getline(rfile, tem, '\n');
            if (tem != "")
                wfile << tem << endl;
        }
        i++;
    }
    if (!k)
    {
        wfile << timestamp << ":";
        for (int j = 0; j < temp.count; j++)
        {
            wfile << measuments[j] << "@";
        }
        wfile << endl;
    }
    rfile.close();
    wfile.close();
    remove((filename + "cp").c_str());
    return 0;
}

LDTemplate ReadTemplate(string filename)
{
    //读取文件模板
    ifstream rfile;
    rfile.open(filename);
    string magicstring, namestring;
    getline(rfile, magicstring);
    getline(rfile, namestring);
    LDTemplate temp(magicstring, namestring);
    return temp;
}

string ReadData(string filename, int line)
{
    //读取指定的一行数据
    string str = "";
    ifstream rfile;
    rfile.open(filename);
    LDTemplate temp = ReadTemplate(filename);
    str += temp.GetPrintName();
    str += "\n";
    int i = 1;
    while (!rfile.eof())
    {
        string tem;
        getline(rfile, tem, '\n');
        if (i == line + 2)
            str += GetPrintData(tem);
        i++;
    }
    return str;
}

string ReadLastData(string filename)
{
    //读取最新的一行数据
    string str = "";
    ifstream rfile;
    rfile.open(filename);
    LDTemplate temp = ReadTemplate(filename);
    str += temp.GetPrintName();
    str += "\n";
    string te = "";
    while (!rfile.eof())
    {
        string tem;
        getline(rfile, tem, '\n');
        if (tem != "")
            te = GetPrintData(tem);
    }
    str += te;
    return str;
}
