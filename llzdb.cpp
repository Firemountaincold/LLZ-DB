#include "llzdb.h"

int byteToBinary(char byteBuf[], int byteLen, char binaryBuf[], int binaryLen)
{
    //字节流转二进制字符流
    if (binaryLen < 8 * byteLen)
        return -1;
    int charBufIndex = 0;
    for (int i = 0; i < byteLen; ++i)
    {
        for (int j = 7; j >= 0; --j)
        {
            if (byteBuf[i] >> j & 0x1)
            {
                binaryBuf[charBufIndex++] = '1';
            }
            else
            {
                binaryBuf[charBufIndex++] = '0';
            }
        }
    }
    return 0;
}

int binaryToByte(char binaryBuf[], int binaryLen, char byteBuf[], int byteLen)
{
    //二进制字符流转字节流
    if (8 * byteLen < binaryLen)
        return -1;
    char tmpStr[9] = "";
    int byteBufIndex = 0;
    for (int i = 0; i < binaryLen; i = i + 8)
    {
        strncpy(tmpStr, binaryBuf + i, 8);
        for (int j = 0; j < 8; ++j)
        {
            //byteBuf[byteBufIndex] += ((tmpStr[j] - '0') << (7 - j));
            byteBuf[byteBufIndex] = byteBuf[byteBufIndex] << 1;
            char tmp = tmpStr[j] - '0';
            byteBuf[byteBufIndex] |= tmp;
        }
        ++byteBufIndex;
    }
    return 0;
}

int FileByteToBinary(string filename)
{
    //文本文件转化为二进制
    int length;
    char *buffer;
    char *binarybuffer;
    string filecopy = filename + "cp";
    rename(filename.c_str(), filecopy.c_str());

    fstream rfile;
    rfile.open(filecopy, fstream::in | fstream::binary);
    if (!rfile)
    {
        return -1;
    }
    //获取长度
    rfile.seekg(0, ios::end);
    length = rfile.tellg();
    rfile.seekg(0, ios::beg);
    //声明缓存
    buffer = new char[length];
    binarybuffer = new char[8 * length];
    //成块读取
    rfile.read(buffer, length);
    rfile.close();
    //转换并写回
    byteToBinary(buffer, length, binarybuffer, 8 * length);
    fstream wfile(filename, fstream::out | fstream::binary);
    wfile.write(binarybuffer, 8 * length);
    //删除缓存
    delete[] buffer, binarybuffer;
    //关闭对象
    wfile.close();
    rfile.close();
    remove(filecopy.c_str());
    return length;
}

int FileBinaryToByte(string filename)
{
    //二进制转化为文本文件
    int length;
    char *buffer;
    char *bytebuffer;
    string filecopy = filename + "cp";
    rename(filename.c_str(), filecopy.c_str());

    fstream rfile;
    rfile.open(filecopy, fstream::in | fstream::binary);
    if (!rfile)
    {
        return -1;
    }
    //获取长度
    rfile.seekg(0, ios::end);
    length = rfile.tellg();
    rfile.seekg(0, ios::beg);
    //声明缓存
    buffer = new char[length];
    bytebuffer = new char[length / 8];
    //成块读取
    rfile.read(buffer, length);
    rfile.close();
    //转换并写回
    binaryToByte(buffer, length, bytebuffer, length / 8);
    fstream wfile(filename, fstream::out | fstream::binary);
    wfile.write(bytebuffer, length / 8);
    //删除缓存
    delete[] buffer, bytebuffer;
    //关闭对象
    wfile.close();
    rfile.close();
    remove(filecopy.c_str());
    return length;
}

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
    FileBinaryToByte(filename);
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
    FileByteToBinary(filename);
    return 0;
}

LDTemplate ReadTemplate(string filename)
{
    //读取文件模板
    FileBinaryToByte(filename);
    ifstream rfile;
    rfile.open(filename);
    string magicstring, namestring;
    getline(rfile, magicstring);
    getline(rfile, namestring);
    LDTemplate temp(magicstring, namestring);
    rfile.close();
    FileByteToBinary(filename);
    return temp;
}

string ReadData(string filename, int line)
{
    //读取指定的一行数据
    FileBinaryToByte(filename);
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
    rfile.close();
    FileByteToBinary(filename);
    return str;
}

string ReadAllData(string filename)
{
    //读取全部数据
    FileBinaryToByte(filename);
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
        if (i >= 3 && tem != "")
        {
            str += GetPrintData(tem);
            str += "\n";
        }
        i++;
    }
    rfile.close();
    FileByteToBinary(filename);
    return str;
}

string ReadLastData(string filename)
{
    FileBinaryToByte(filename);
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
    rfile.close();
    FileByteToBinary(filename);
    return str;
}
