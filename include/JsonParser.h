#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <vector>
#include <cstring>
#include <string>
#include <map>
#include <sstream>
#include "type.h"

using std::string;
using std::vector;
using std::map;
using std::make_pair;


enum TENSORTYPE { TENSOR, SCALAR, NUM_OF_TENSORTYPE, };

using namespace Boost::Internal;

Type int_type = Type::int_scalar(32);
Type float_type = Type::float_scalar(32);

void handleSpaces(string &s) {
	for (size_t i = 0; i < s.length(); i++) {
		if (s[i] == ' ') {
			s.erase(i, 1);
			i--;
		}
	}
}

// Get rid of all flag chars and split into several strings into "sv".
void split(const string &s, vector<string>& sv, const char flag) {
	sv.clear();
	std::istringstream iss(s);
	string temp;

	while (std::getline(iss, temp, flag))
		sv.push_back(temp);
	return;
}

string parseQuote(char* source, int st, int &ed) {
	while (source[st] != '\"' && source[st] != '\n' && source[st] != 0) st++;
	if (source[st] == '\n' || source[st] == 0) return string();
	ed = ++st;
	while (source[ed] != '\"') ed++;
	return string(source, st, ed - st);
}

// returns string in range [begin, end].
string genString(char *name, int begin, int end) {
	return string(name + begin, end - begin + 1);
	/*    string s="";
		for(int i=begin;i<=end;i++)
			s+=*(name+i);
		return s;
		*/
}

class IndexBound {
public:
	char name[16];
	//	vector<string> lowerbound;		// Seems that in Project 1, all lowerbound could be considered 0
	int upperbound;

	IndexBound() {
		memset(name, 0, sizeof(name));
	}
	IndexBound(const char *_name, int _ub) :upperbound(_ub) {
		memcpy(name, _name, sizeof(_name));
	}
	bool operator == (const IndexBound &i) {
		return (strcmp(name, i.name) == 0 && upperbound == i.upperbound);
	}
};

class Tensor {
public:
	char name[16];
	TENSORTYPE tensor_type;
	Type type;
	vector<size_t> dim_size;
	Tensor() : dim_size(vector<size_t>()) {
		memset(name, 0, sizeof(name));
	}
	Tensor(const Tensor &T) {
		tensor_type = T.tensor_type;
		type = T.type;
		dim_size = T.dim_size;
		memcpy(name, T.name, sizeof(name));
	}
	friend std::ostream& operator<< (std::ostream &o, const Tensor &T) {
		if (T.tensor_type == TENSOR) o << "Tensor";
		else o << "Scalar";
		o << " name: " << T.name << std::endl;

		if (T.type.is_float()) o << "Type: float" << std::endl;
		else o << "Type: int" << std::endl;
		if (T.tensor_type == TENSOR) {
			o << "Dimension: ";
			for (size_t i = 0; i < T.dim_size.size(); i++)
				o << T.dim_size[i] << " ";
			o << std::endl;
		}
		o << std::endl;
		return o;
	}
};

class Case {
public:
	char name[32];
	vector<string> ins, outs;
	vector<Tensor> tins, touts;
	Type type;
	char kernel[1024];
	// here saves tensor_names prefixed by "d" i.e. A -> dA
	vector<string> grad_tos;
	vector<IndexBound> indexbound;
	map<string, int> finalbound;

	Case() :ins(vector<string>()), outs(vector<string>()), grad_tos(vector<string>()) {
		memset(name, 0, sizeof(name));
		memset(kernel, 0, sizeof(kernel));
	}
	Case(const char *filename) :ins(vector<string>()), outs(vector<string>()), grad_tos(vector<string>()) {
		memset(name, 0, sizeof(name));
		memset(kernel, 0, sizeof(kernel));

		char buf[1024];

		FILE* file = fopen(filename, "r");
		if (!file) {
			printf("OPEN ERROR!\n");		// return;
			throw "file not exist";
		}

		while (!feof(file)) {
			char* tmp;
			int itmp = 0;
			fgets(buf, 1024, file);

			if ((tmp = strstr(buf, "\"name\"")) != NULL) {
				tmp += 7;	// length of String "name":
				string stmp = parseQuote(tmp, 0, itmp);
				memcpy(name, stmp.c_str(), stmp.length());

			}
			else if ((tmp = strstr(buf, "\"ins\"")) != NULL) {
				tmp += 6;	// length of String "ins":
				int insidx = 0;
				while (true) {
					string stmp = parseQuote(tmp, insidx, itmp);
					if (stmp.length()) {
						ins.push_back(stmp);
						insidx = itmp + 1;
					}
					else break;
				}

			}
			else if ((tmp = strstr(buf, "\"outs\"")) != NULL) {
				tmp += 7;	// length of String "outs":
				int insidx = 0;
				while (true) {
					string stmp = parseQuote(tmp, insidx, itmp);
					if (stmp.length()) {
						outs.push_back(stmp);
						insidx = itmp + 1;
					}
					else break;
				}
			}
			else if ((tmp = strstr(buf, "\"data_type\"")) != NULL) {
				if (strstr(tmp, "\"int\"")) {
					type = int_type;
				}
				else if (strstr(tmp, "\"float\"")) {
					type = float_type;
				}
			}
			else if ((tmp = strstr(buf, "\"kernel\"")) != NULL) {
				tmp += 9;	// length of String "kernel":
				string stmp = parseQuote(tmp, 0, itmp);
				handleSpaces(stmp);
				memcpy(kernel, stmp.c_str(), stmp.length());
				//	printf("%s\n", kernel);

			}
			else if ((tmp = strstr(buf, "\"grad_to\"")) != NULL) {
				tmp += 10;	// length of String "grad_to":
				int insidx = 0;
				while(true) {
					string stmp = parseQuote(tmp, insidx, itmp);
					if (stmp.length()) {
						grad_tos.push_back("d" + stmp);
						insidx = itmp + 1;
					}
					else break;
				}
			}


		}
		fclose(file);
		parseTensor();
		parseIndex();
		for (size_t i = 0; i < indexbound.size(); i++) {
			handleIndexBound(indexbound[i]);
		}

	}
	void print() {
		printf("\nname: %s\n", name);
		printf("ins: ");
		for (size_t i = 0; i < ins.size(); i++) {
			printf("%s ", ins[i].c_str());
		}printf("\n");
		printf("outs: ");
		for (size_t i = 0; i < outs.size(); i++) {
			printf("%s ", outs[i].c_str());
		}printf("\n");
		if (type.is_int()) printf("type: INT\n");
		else printf("type: FLOAT\n");
		printf("kernel: %s\n", kernel);
	}

	void parseTensor() {
		int tmp = ins.size();
		char *tmpptr;
		for (int i = 0; i < tmp; i++) {
			Tensor Ttmp;
			Ttmp.type = type;
			tmpptr = strstr(kernel, (ins[i] + "<").c_str());
			while (tmpptr > kernel && *(tmpptr - 1) == 'd')
			    tmpptr = strstr(tmpptr + 1,  (ins[i] + "<").c_str());
			//	if (!tmpptr) { printf("??????\n"); return; }
			while (*tmpptr != '<') tmpptr++;
			tmpptr++;
			while (*tmpptr != '>') {
				Ttmp.dim_size.push_back(atoi(tmpptr));
				while (*tmpptr != ',' && *tmpptr != '>') tmpptr++;
				if (*tmpptr == ',') tmpptr++;
			}
			if (Ttmp.dim_size.size() == 1 && Ttmp.dim_size[0] == 1) Ttmp.tensor_type = SCALAR;
			else Ttmp.tensor_type = TENSOR;
			memcpy(Ttmp.name, ins[i].c_str(), ins[i].length());
		//	std::cout << Ttmp;
			tins.push_back(Ttmp);
		}

		tmp = outs.size();
		for (int i = 0; i < tmp; i++) {
			Tensor Ttmp;
			Ttmp.type = type;
			tmpptr = strstr(kernel, (outs[i] + "<").c_str());
			//	if (!tmpptr) { printf("??????\n"); return; }
			while (*tmpptr != '<') tmpptr++;
			tmpptr++;
			while (*tmpptr != '>') {
				Ttmp.dim_size.push_back(atoi(tmpptr));
				while (*tmpptr != ',' && *tmpptr != '>') tmpptr++;
				if (*tmpptr == ',') tmpptr++;
			}
			if (Ttmp.dim_size.size() == 1 && Ttmp.dim_size[0] == 1) Ttmp.tensor_type = SCALAR;
			else Ttmp.tensor_type = TENSOR;
			memcpy(Ttmp.name, outs[i].c_str(), outs[i].length());
		//	std::cout << Ttmp;
			touts.push_back(Ttmp);
		}
	}

	void handleIndexBound(IndexBound &indexbound) {		//将表示为id的index和bound加入finalbound
		int flag = 0;
		int length = 0;
		for (int i = 0; i < 16; i++) {
			if (indexbound.name[i] != 0)
				length++;
			else break;
		}
		for (int i = 0; i < length; i++) {
			if (indexbound.name[i] == '(')
				return;
			if (indexbound.name[i] == ')')
				return;
			if (indexbound.name[i] == '+') {
				
				return;
			}
			if (indexbound.name[i] == '-') {
				return;
			}
			if (indexbound.name[i] == '*') {
				return;
			}
			if (indexbound.name[i] == '/') {
				return;
			}
			if (indexbound.name[i] == '%') {
				return;
			}
			if (indexbound.name[i] == ')') {
				return;
			}
			if (i == length - 1) {
				string s = genString(indexbound.name, 0, length - 1);
				map<string, int>::iterator it = finalbound.find(s);
				if (it == finalbound.end())
					finalbound.insert(make_pair(s, indexbound.upperbound));
				else if (it->second < indexbound.upperbound)
				{
					finalbound.erase(it);
					finalbound.insert(make_pair(s, indexbound.upperbound));
				}
				return;
			}
		}
	}

	void parseIndex() {
		vector<string> VTMP;
		VTMP.insert(VTMP.end(), ins.begin(), ins.end());
		VTMP.insert(VTMP.end(), outs.begin(), outs.end());

		for (size_t idx = 0; idx < VTMP.size(); idx++){
			char* tmp = kernel;
			while ((tmp = strstr(tmp, (VTMP[idx] + "<").c_str())) != NULL) {
				tmp = strstr(tmp, "<");
				char* tmp2 = strstr(tmp, ">");
				if (*(tmp2 + 1) == '[') {	// a tensor
					string CList(tmp + 1, tmp2 - tmp - 1);
					tmp = tmp2 + 1;
					tmp2 = strstr(tmp, "]");
					string AList(tmp + 1, tmp2 - tmp - 1);
					vector<string> vCList, vAList;
					split(CList, vCList, ',');
					split(AList, vAList, ',');
					for (size_t i = 0; i < vCList.size(); i++) {
						IndexBound Itmp(vAList[i].c_str(), atoi(vCList[i].c_str()));
						int flag = 0;
						for (size_t j = 0; j < indexbound.size(); j++) {
							if (Itmp == indexbound[j]) { flag = 1; break; }
						}
						if (!flag)
							indexbound.push_back(IndexBound(vAList[i].c_str(), atoi(vCList[i].c_str())));
					}
				}

				tmp++;		// turn to next finding
			}
		}
		
	/*	printf("IndexBounds: \n");
		for (int i = 0; i < indexbound.size(); i++) {
			printf("%s<%d\n", indexbound[i].name, indexbound[i].upperbound);
		}*/
	}

};

