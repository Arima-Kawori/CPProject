#include <cstdio>
#include <cstring>
#include <cctype>
#include "json/json.h"
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
using namespace std;

char instr[10][50] = {"./cases/case1.json", 
	          "./cases/case2.json",
                  "./cases/case3.json",
                  "./cases/case4.json",
                  "./cases/case5.json",
                  "./cases/case6.json",
                  "./cases/case7.json",
                  "./cases/case8.json",
                  "./cases/case9.json",
                  "./cases/case10.json"};

char outstr[10][50] = {"./cases/case1_graded.json", 
	             "./cases/case2_graded.json",
                     "./cases/case3_graded.json",
                     "./cases/case4_graded.json",
                     "./cases/case5_graded.json",
                     "./cases/case6_graded.json",
                     "./cases/case7_graded.json",
                     "./cases/case8_graded.json",
                     "./cases/case9_graded.json",
                     "./cases/case10_graded.json"};

map<string, bool> used;
map<string, vector<long unsigned int> > shape;
map<string, int> extents;

void get_shape(string ker)
{
    int len = ker.size();
    for (int i = 0; i < len; i++)
    {
        if (ker[i] == '<')
	{
	    int j = i - 1;
	    string now = "";
	    while (j >= 0 && isalnum(ker[j]))
	    {
		now = ker[j] + now;
		j--;
	    }
	    vector<long unsigned int> nowshape;
	    j = i + 1;
	    int nownum = 0;
	    while (ker[j] != '>')
	    {
		if (isdigit(ker[j]))
	            nownum = nownum * 10 + (ker[j] - '0');
		if (ker[j] == ',')
		{
		    nowshape.push_back(nownum);
		    nownum = 0;
		}
		j++;
	    }
	    nowshape.push_back(nownum);
	    shape[now] = nowshape;
	    if (ker[j] == '>' && j + 1 < len)
	    {
		if (ker[j + 1] != '[')
	            continue;
		j++;
		int cnt = 0;
		while (true)
		{
		string nowid = "";
		bool ok = true;
		j++;
		while (ker[j] != ',' && ker[j] != ']')
		{
		    if (nowid == "" && !isalpha(ker[j]))
			ok = false;
		    if (nowid != "" && !isalnum(ker[j]))
			ok = false;
		    nowid = nowid + ker[j];
		    j++;
		}
		if (ok)
		{
		    if (extents.count(nowid))
		        if (extents[nowid] < nowshape[cnt])
			    extents[nowid] = nowshape[cnt];
			else
			    ;
		    else
			extents[nowid] = nowshape[cnt];
		}
		cnt++;
		if (ker[j] == ']')
		    break;
		}
	    }
	}
    }
}
string name, ins[10], outs[10], data_type, ker, grad_to[10];
string now_grad;
int now_pos;
string final_ker;

string getgrad(int st, int ed)
{
    int cnt1 = 0, cnt2 = 0;
    bool lowlev = false, highlev = false;
    int oploc = -1;
    for (int i = st; i <= ed; i++)
    {
        if (ker[i] == '(')
            cnt1++;
        else if (ker[i] == ')')
            cnt1--;
        else if (ker[i] == '[')
            cnt2++;
        else if (ker[i] == ']')
            cnt2--;
        else
        {
            if (ker[i] == '+' || ker[i] == '-')
                if (cnt1 == 0 && cnt2 == 0)
                {
                    lowlev = true;
                    oploc = i;
                    break;
                }
        }
    }
    if (!lowlev)
    {
    for (int i = st; i <= ed; i++)
    {
        if (ker[i] == '(')
            cnt1++;
        else if (ker[i] == ')')
            cnt1--;
        else if (ker[i] == '[')
            cnt2++;
        else if (ker[i] == ']')
            cnt2--;
        else
        {
            if (ker[i] == '*' || ker[i] == '/')
                if (cnt1 == 0 && cnt2 == 0)
                {
                    highlev = true;
                    oploc = i;
                    break;
                }
        }
    }
    }
    if (lowlev)
    {
        if (ker[oploc] == '+')
        {
            if (getgrad(st, oploc - 1) == "0" && getgrad(oploc + 1, ed) == "0")
                return "0";
            else if (getgrad(st, oploc - 1) != "0" && getgrad(oploc + 1, ed) == "0")
                return getgrad(st, oploc - 1);
            else if (getgrad(st, oploc - 1) == "0" && getgrad(oploc + 1, ed) != "0")
                return getgrad(oploc + 1, ed);
            return getgrad(st, oploc - 1) + "+" + getgrad(oploc + 1, ed);
        }
            
        else if (ker[oploc] == '-')
        {
            if (getgrad(st, oploc - 1) == "0" && getgrad(oploc + 1, ed) == "0")
                return "0";
            else if (getgrad(st, oploc - 1) != "0" && getgrad(oploc + 1, ed) == "0")
                return getgrad(st, oploc - 1);
            return getgrad(st, oploc - 1) + "+" + getgrad(oploc + 1, ed);
        }
    }
    else if (highlev)
    {
        if (ker[oploc] == '*')
        {
            if (getgrad(st, oploc - 1) == "0" && getgrad(oploc + 1, ed) == "0")
                return "0";
            else if (getgrad(st, oploc - 1) != "0" && getgrad(oploc + 1, ed) == "0")
            {
                if (getgrad(st, oploc - 1) == "1")
                    return ker.substr(oploc + 1, ed - oploc);
                else 
                    return "(" + getgrad(st, oploc - 1) + ")*(" + ker.substr(oploc + 1, ed - oploc) + ")";
            }                
            else if (getgrad(st, oploc - 1) == "0" && getgrad(oploc + 1, ed) != "0")
            {
                if (getgrad(oploc + 1, ed) == "1")
                    return ker.substr(st, oploc - st);
                else 
                    return "(" + ker.substr(st, oploc - st) + ")*(" + getgrad(oploc + 1, ed) + ")";
            }
            if (getgrad(st, oploc - 1) == "1" && getgrad(oploc + 1, ed) == "1")
                return "1";
            else if (getgrad(st, oploc - 1) != "1" && getgrad(oploc + 1, ed) == "1")
                return "(" + getgrad(st, oploc - 1) + ")*(" + ker.substr(oploc + 1, ed - oploc) + ")+" + ker.substr(st, oploc - st);                
            else if (getgrad(st, oploc - 1) == "1" && getgrad(oploc + 1, ed) != "1")
                return ker.substr(oploc + 1, ed - oploc) + "+(" + ker.substr(st, oploc - st) + ")*(" + getgrad(oploc + 1, ed) + ")";
            return "(" + getgrad(st, oploc - 1) + ")*(" + ker.substr(oploc + 1, ed - oploc) + ")+(" + ker.substr(st, oploc - st) + ")*(" + getgrad(oploc + 1, ed) + ")";
        }
        if (ker[oploc] == '/')
        {
            if (getgrad(st, oploc - 1) == "0" && getgrad(oploc + 1, ed) == "0")
                return "0";
            else if (getgrad(st, oploc - 1) != "0" && getgrad(oploc + 1, ed) == "0")
            {
                if (getgrad(st, oploc - 1) == "1")
                    return ker.substr(oploc + 1, ed - oploc) + "/((" + ker.substr(oploc + 1, ed - oploc) + ")*(" + ker.substr(oploc + 1, ed - oploc) + "))";
                else 
                    return "(" + getgrad(st, oploc - 1) + ")*(" + ker.substr(oploc + 1, ed - oploc) + ")" + "/((" + ker.substr(oploc + 1, ed - oploc) + ")*(" + ker.substr(oploc + 1, ed - oploc) + "))";
            }                
            else if (getgrad(st, oploc - 1) == "0" && getgrad(oploc + 1, ed) != "0")
            {
                if (getgrad(oploc + 1, ed) == "1")
                    return "(-(" + ker.substr(st, oploc - st) + "))" + "/((" + ker.substr(oploc + 1, ed - oploc) + ")*(" + ker.substr(oploc + 1, ed - oploc) + "))";
                else 
                    return "(-((" + ker.substr(st, oploc - st) + ")*(" + getgrad(oploc + 1, ed) + ")))" + "/((" + ker.substr(oploc + 1, ed - oploc) + ")*(" + ker.substr(oploc + 1, ed - oploc) + "))";
            }
        }
    }
    else
    {
        if (ker[st] == '(' && ker[ed] == ')')
            return "(" + getgrad(st + 1, ed - 1) + ")";
        if (isdigit(ker[st]))
            return "0";
        else
        {
            if (st == now_pos)
                return "1";
            else
                return "0";
        }
    }
}

string getnow()
{
   printf("%s\n", ker.c_str());
    int i = 0;
    while (ker[i] != '=') i++;
    string nowindex = "";
    for (int j = now_pos + 1; ker[j] != ']'; j++)
        nowindex = nowindex + ker[j];
    nowindex = nowindex + "]";
    return "d" + now_grad + nowindex + "=d" + now_grad + nowindex + "+d" + ker.substr(0, i) + "*(" + getgrad(i + 1, ker.size() - 2) + ")";
}

void grad()
{
    final_ker = "";
    string ls = "";
    bool inCList = false;
    for (int i = 0; i < ker.size(); i++)
    {
        if (ker[i] == '<')
            inCList = true;
        if (!inCList)
            ls = ls + ker[i];
        if (ker[i] == '>')
            inCList = false;
    }
    ker = ls;
    for (int i = 0; grad_to[i] != ""; i++)
    {
        now_grad = grad_to[i];
        for (int j = 0; j < ker.size(); j++)
            if (ker[j] == grad_to[i][0])
            {
                now_pos = j;
                final_ker = final_ker + getnow() + ";";
            }
    }
}

void work(int id)
{
    freopen(instr[id], "r", stdin);
    freopen(outstr[id], "w", stdout);
    char input[1000];
    int len = 0;
    while ((input[len] = getchar()) != EOF)
	len++;
    Json::Reader reader;
    Json::Value value;
    if (reader.parse(input, value))
    {
	used.clear();
	shape.clear();
	extents.clear();
	for (int i = 0; i < 10; i++)
	    ins[i] = outs[i] = grad_to[i] = "";
	name = value["name"].asString();
        for (int i = 0; i < value["ins"].size(); i++)
        {
        	ins[i] = value["ins"][i].asString();
            used[ins[i]] = 0;
        }
	    
	for (int i = 0; i < value["outs"].size(); i++)
	    outs[i] = value["outs"][i].asString();
	data_type = value["data_type"].asString();
	ker = value["kernel"].asString();
	string ls = "";
	for (int i = 0; i < ker.size(); i++)
	    if (ker[i] != ' ')
	        ls = ls + ker[i];
	ker = ls;
	for (int i = 0; i < value["grad_to"].size(); i++)
	    grad_to[i] = value["grad_to"][i].asString();
        get_shape(ker);
	Json::Value root;
	root["name"] = Json::Value(name);
	for (int i = 0; i < value["grad_to"].size(); i++)
	    root["outs"].append("d" + grad_to[i]);
	root["data_type"] = Json::Value(data_type);
	grad();
	string ke = "";
	for (int i = 0; i < final_ker.size(); i++)
	{
	    ke = ke + final_ker[i];
	    if (final_ker[i] >= 'A' && final_ker[i] <= 'Z')
	    {
	        if (used.count(final_ker.substr(i, 1)) && (i > 0 && final_ker[i - 1] != 'd'))
	            used[final_ker.substr(i, 1)] = true;
	        string nshape = "<";
	        vector<long unsigned int> sp = shape[final_ker.substr(i, 1)];
	        for (int j = 0; j < sp.size(); j++)
	            nshape = nshape + to_string(sp[j]) + ",";
	        nshape[nshape.size() - 1] = '>';
	        ke = ke + nshape;
	    }
	}
	root["kernel"] = ke;
	for (int i = 0; i < value["ins"].size(); i++)
	    if (used[ins[i]])
	        root["ins"].append(ins[i]);
	for (int i = 0; i < value["outs"].size(); i++)
	    root["ins"].append("d" + outs[i]);
	Json::StyledWriter sw;
	cout << sw.write(root) << endl << endl;
    }
    fclose(stdin);
    fclose(stdout);
}

void generate() {
    for (int i = 0; i <= 9; i++)
	work(i);
}
