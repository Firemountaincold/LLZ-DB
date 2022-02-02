#include <cstdio>
#include <cstdlib>
#include <typeinfo>
#include <string>
#include <fstream>
#include "tinyxml/tinyxml.h"
#include "tinyxml/tinystr.h"
#include "tinyxml/tinyxml.cpp"
#include "tinyxml/tinystr.cpp"
#include "tinyxml/tinyxmlerror.cpp"
#include "tinyxml/tinyxmlparser.cpp"

using namespace std;

enum LDDataType
{
    //数据类型枚举
    BOOLEAN = 0,
    INT32,
    INT64,
    FLOAT,
    DOUBLE,
    TEXT,
    NULLTYPE
};

string lddatatype[7] =
    {
        //与枚举对应的字符串数组
        "BOOLEAN",
        "INT32",
        "INT64",
        "FLOAT",
        "DOUBLE",
        "TEXT",
        "NULLTYPE"};

class LDTemplate
{
    //模板类
public:
    string name;
    int count;
    string measuments[100];
    LDDataType datatype[100];

    LDTemplate(string tname, int tcount, string *tmeasuments, LDDataType *tdatatype)
    {
        //用参数构建模板对象
        name = tname;
        count = tcount;
        for (int i = 0; i < tcount; i++)
        {
            measuments[i] = tmeasuments[i];
            datatype[i] = tdatatype[i];
        }
    }

    LDTemplate(string xmlpath)
    {
        //从xml文件读取模板对象
        TiXmlDocument doc;
        if (!doc.LoadFile(xmlpath.c_str()))
            return;
        TiXmlElement *pElem;
        pElem = doc.RootElement();
        name = string(pElem->FirstAttribute()->Value());
        TiXmlElement *nodeElem = pElem->FirstChildElement();
        count = 0;
        for (; nodeElem != NULL; nodeElem = nodeElem->NextSiblingElement())
        {
            TiXmlHandle node(nodeElem);
            TiXmlElement *dtElem = node.FirstChild("datatype").Element();
            measuments[count] = nodeElem->Attribute("mname");
            datatype[count] = (LDDataType)(atoi(dtElem->GetText()));
            count++;
        }
    }

    LDTemplate(string magicstring, string namestring)
    {
        //从文件的字符串读取模板对象
        count = atoi(magicstring.substr(5, 2).c_str());
        int c = namestring.find(":");
        name = namestring.substr(0, c);
        int p = namestring.find("@");
        for (int i = 0; i < count; i++)
        {
            measuments[i] = namestring.substr(c + 1, p - 1 - c);
            c = p;
            p = namestring.substr(c + 1).find("@") + c + 1;
            datatype[i] = (LDDataType)atoi(magicstring.substr(8 + i, 1).c_str());
        }
    }

    ~LDTemplate() {}

    string GetTemplatestring()
    {
        //获取模板描述字符串
        string str = "Template: ";
        str += name;
        str += "\n";
        for (int i = 0; i < count; i++)
        {
            str += "measument ";
            str += measuments[i];
            str += " : ";
            str += lddatatype[datatype[i]];
            str += "\n";
        }
        return str;
    }

    string GetMagicString()
    {
        //获取魔术字符串，用于文件第一行
        char temp[] = {'0', '0', '0', '0', '0', (char)('0' + count / 10), (char)('0' + count % 10), '0'};
        string str(temp);
        for (int i = 0; i < count; i++)
        {
            str += (char)('0' + datatype[i]);
        }
        str += '&';
        return str;
    }

    string GetNameString()
    {
        //获取名称字符串，用于文件第二行
        string str = "";
        str += name;
        str += ":";
        for (int i = 0; i < count; i++)
        {
            str += measuments[i];
            str += "@";
        }
        return str;
    }

    string GetPrintName()
    {
        //获取打印用名称字符串
        string str = "";
        str += "timestamp   ";
        for (int i = 0; i < count; i++)
        {
            str += measuments[i];
            str += "  ";
        }
        return str;
    }
};