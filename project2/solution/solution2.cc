#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include "type.h"
#include "IR.h"
#include "IRPrinter.h"
#include "IRMutator.h"
#include "IRVisitor.h"
#include "JsonParser.h"

using std::string;
using std::vector;
using std::map;
using std::make_pair;

using namespace Boost::Internal;

const char *filename = "./cases/case";
const char *outputfile = "./kernels/grad_case";

map<string, Expr> IndexList;		// Expr for Index

vector<string> vUsedIndex;				// for every S ::= LHS = RHS, get all index used in this S into this vector, for
									//  later use in the progress building Loopnest.

Case c;

char curAlphabet = 'a', curNum = '0';		// used for Index replace

Group handleP(Case c);
Stmt handleS(string s);
Expr handleRHS(string s);
Expr handleTRef(string s);
Expr handleSRef(string s);
Expr handleIdExpr(string s);
Expr handleSelect(string s);
Expr handleCond(string s);
void split(const string &s, vector<string>& sv, const char flag);


Group handleP(Case c) {
	vector<string> vStmt;		// string of stmts later for handleS()
	vector<string> vStmt00;		// string of stmts, handled bypass temporate variables below
	split(string(c.kernel), vStmt00, ';');
	vector<Stmt> Loop_Nest;

	vector<string> sv;
	string L, R, tmp, cur;

	for (int i = 0; i < vStmt00.size(); ++i) {
		int equalloc = vStmt00[i].find('=');
		L = vStmt00[i].substr(0, equalloc);
		R = vStmt00[i].substr(equalloc + 1);
		R = "+" + R;
		int pre = 0, dep = 0;
		bool first = true;
		for (size_t j = 1; j < R.size(); ++j) {
			switch (R[j]) {
		//	case '<':
				case '(':case '[':
				++dep;		break;
		//	case '>':
				case ')':case ']':
				--dep;		break;
			case '+':case '-':
				if (dep == 0) {
				        if (first)
				        {
				        first = false;
					pre = j;
				        cur = L + "=" + R.substr(pre + 1, j - pre - 1);
				        }
					else {
					cur = L + "=" + L + R.substr(pre, j - pre);
					pre = j;
					vStmt.push_back(cur);}
				}
				break;
			default:
				break;
			}
		}
		if (first)
		{
		    first = false;
		}else{
		cur = L + "=" + L + R.substr(pre, R.size() - pre);
		vStmt.push_back(cur);}
	}

	for (size_t i = 0; i < vStmt.size(); i++) {

		vector<Expr> vIndex;
		Stmt stmt = handleS(vStmt[i]);

		for (size_t j = 0; j < vUsedIndex.size(); j++) {
			vIndex.push_back(IndexList[vUsedIndex[j]]);
		}
		Loop_Nest.push_back(LoopNest::make(vIndex, { stmt }));
	}


	vector<Expr> inputs, outputs;

	for (size_t i = 0; i < c.tins.size(); i++) {
		Expr tmp;
		if (c.tins[i].tensor_type == SCALAR)
			tmp = Var::make(c.type, c.tins[i].name, { 0 }, { 1 });
		else {
			vector<Expr> vtmp;
			for (size_t j = 0; j < c.tins[i].dim_size.size(); j++)
				vtmp.push_back(0);			// just a placeholder
			tmp = Var::make(c.type, c.tins[i].name, vtmp, c.tins[i].dim_size);
		}

		inputs.push_back(tmp);
	}

	for (size_t i = 0; i < c.touts.size(); i++) {
		// if the current Var is in ins and outs simultaneously, just put it in the ins.
		bool inIns = false;
		for (size_t j = 0; j < c.tins.size(); j++) {
			if (!strcmp(c.touts[i].name, c.tins[j].name)) {
				inIns = true;	break;
			}
		}
		if (inIns) continue;


		Expr tmp;
		if (c.touts[i].tensor_type == SCALAR)
			tmp = Var::make(c.type, c.touts[i].name, { 0 }, { 1 });
		else {
			vector<Expr> vtmp;
			for (size_t j = 0; j < c.touts[i].dim_size.size(); j++)
				vtmp.push_back(0);			// just a placeholder
			tmp = Var::make(c.type, c.touts[i].name, vtmp, c.touts[i].dim_size);
		}

		outputs.push_back(tmp);
	}

	return Kernel::make(c.name, inputs, outputs, Loop_Nest, KernelType::CPU);

}

struct IndicesInS {
	string index;	// such as "i//16"
	bool inLHS;
	string replacedIndex = "";
	int replacedLowerbound = 0;
	int replacedUpperbound;
};

struct replaced {
	vector<string> name;
};

map<string, replaced> Replaced;
map<string, string> ReplaceResult;

Stmt handleS(string s)
{
	Replaced.clear();
	ReplaceResult.clear();
	vUsedIndex.clear();
	int equalloc = s.find('=');

	string L = s.substr(0, equalloc);
	string R = s.substr(equalloc + 1);
	// processing operators within LHS's index
	// scan all indexes in LHS, and process 
	// examples:
	// dA<2,16>[i//16,i%16]=dA<2,16>[i//16,i%16]+dB<32>[i]*(1);
	// dB<10,8>[i+1,j]=dB<10,8>[i+1,j]+dA<8,8>[i,j]*(((1))*(3.0)/((3.0)*(3.0)));
	// dB<2,16,7,7>[n,c,p+r,q+s]=dB<2,16,7,7>[n,c,p+r,q+s]+dA<2,8,5,5>[n,k,p,q]*(C<8,16,3,3>[k,c,r,s]);

	bool operatorInLHS = false;
	vector<IndicesInS> Indices;
	// According to syntax, there should be only one '[' and ']' in LHS.
	int i = L.find('['), lastSeparator = i;
	while (true) {
		if (L[i] == ',' || L[i] == ']') {
			string nowIndex = L.substr(lastSeparator + 1, i - lastSeparator - 1);
			bool foundInIndices = false;
			for (int j = 0; j < (int)Indices.size(); j++) {
				if (Indices[j].index == nowIndex) {
					Indices[j].inLHS = true;
					foundInIndices = true;
					break;
				}
			}
			if (!foundInIndices) {
				IndicesInS tmp;
				tmp.index = nowIndex;
				tmp.inLHS = true;
				Indices.push_back(tmp);
			}
			lastSeparator = i;
			if (L[i] == ']') break;
		}
		i++;
	}
	int RLen = R.length();
	for (i = R.find('['); i != R.npos && i < RLen; i++) {
		while (i < RLen && R[i] != '[') i++;
		if (i < RLen) {
			lastSeparator = i;
			while (true) {
				if (R[i] == ',' || R[i] == ']') {
					string nowIndex = R.substr(lastSeparator + 1, i - lastSeparator - 1);
					bool foundInIndices = false;
					for (int j = 0; j < (int)Indices.size(); j++) {
						if (Indices[j].index == nowIndex) {
							foundInIndices = true;
							break;
						}
					}
					if (!foundInIndices) {
						IndicesInS tmp;
						tmp.index = nowIndex;
						tmp.inLHS = false;
						Indices.push_back(tmp);
					}
					lastSeparator = i;
					if (R[i] == ']') break;
				}
				i++;
			}
		}
	}


	// now all indices in IndicesInS.
	// this "for" handles indexes in form Id op IntV.
	//  replacedIndex will be named 'a~z' + '0~9'.
	//  for Id op Id, select stmt may be used.
	

	for (i = 0; i < Indices.size(); i++) {
		if (Indices[i].inLHS == false) continue;
		string curId; char op = 0; int IntV = -1;
		string &curIndex = Indices[i].index;
		int curlen = curIndex.length();
		for (int j = 0; j < curlen; j++) {
			char cur = curIndex[j];
			if (cur == '+' || cur == '/' || cur == '%') {
				op = cur;
				if (cur == '/') j++;
				if (curIndex[j + 1] >= '0' && curIndex[j + 1] <= '9') {
					IntV = atoi(curIndex.substr(j + 1).c_str());
					curId = curIndex.substr(0, (cur == '/' ? j - 1: j));
				}
				break;
			}
		}
		if (IntV != -1) {
			Indices[i].replacedIndex = string(1, curAlphabet) + string(1, curNum);
			if (++curAlphabet == ('z' + 1)) { curAlphabet = 'a'; curNum++; }
			switch (op) {
			case '+': {
				Indices[i].replacedLowerbound = IntV;
				Indices[i].replacedUpperbound = c.finalbound[curId.c_str()] + IntV;
				auto tmpit = Replaced.find(curId);
				if (tmpit != Replaced.end()) {
					tmpit->second.name.push_back(Indices[i].replacedIndex + "-" + std::to_string(IntV));
				}
				else {
					Replaced[curId] = replaced();
					Replaced[curId].name.push_back(Indices[i].replacedIndex + "-" + std::to_string(IntV));
				}
				break; }
			case '/': {
				Indices[i].replacedUpperbound = c.finalbound[curId.c_str()] / IntV;
				auto tmpit = Replaced.find(curId);
				if (tmpit != Replaced.end()) {
					tmpit->second.name.push_back(Indices[i].replacedIndex + "*" + std::to_string(IntV));
				}
				else {
					Replaced[curId] = replaced();
					Replaced[curId].name.push_back(Indices[i].replacedIndex + "*" + std::to_string(IntV));
				}
				break; }
			case '%': {
				Indices[i].replacedUpperbound = IntV;
				auto tmpit = Replaced.find(curId);
				if (tmpit != Replaced.end()) {
					tmpit->second.name.push_back(Indices[i].replacedIndex);
				}
				else {
					Replaced[curId] = replaced();
					Replaced[curId].name.push_back(Indices[i].replacedIndex);
				}
				break; }
			}

		}
		else Indices[i].replacedIndex = Indices[i].index;
	}


	for (auto it = Replaced.begin(); it != Replaced.end(); it++) {
		string curName = it->first, curResult = it->second.name[0];
		for (int j = 1; j < (int)(it->second.name.size()); j++) {
			curResult += "+" + it->second.name[j];
		}
		ReplaceResult[curName] = curResult;
	}

	// now replace all indices in kernel string.
	string newKernel = s;

	for (i = 0; i < Indices.size(); i++) {
		if (!Indices[i].inLHS || Indices[i].index == Indices[i].replacedIndex) continue;
		int substrLoc;
		Expr Domtemp = Dom::make(int_type, Indices[i].replacedLowerbound, Indices[i].replacedUpperbound);
		Expr Indextemp = Index::make(int_type, Indices[i].replacedIndex.c_str(), Domtemp, IndexType::Spatial);
		IndexList.insert(make_pair(Indices[i].replacedIndex, Indextemp));
		while ((substrLoc = newKernel.find(Indices[i].index)) != newKernel.npos)
			newKernel.replace(substrLoc, Indices[i].index.length(), Indices[i].replacedIndex);
	}
	for (auto it = ReplaceResult.begin(); it != ReplaceResult.end(); it++) {
		int substrLoc;
		while ((substrLoc = newKernel.find(it->first)) != newKernel.npos) {
			if (isalnum(newKernel[substrLoc - 1]) || isalnum(newKernel[substrLoc + it->first.length()])) continue;			// not an index id
			newKernel.replace(substrLoc, it->first.length(), "(" + it->second + ")");
		}
	}

	equalloc = newKernel.find('=');
	L = newKernel.substr(0, equalloc);
	R = newKernel.substr(equalloc + 1);
	Expr LHS = handleTRef(L);
	Expr RHS = handleRHS(R);

	return Move::make(LHS, RHS, MoveType::MemToMem);		// seems MoveType doesn't matter
}

Expr handleRHS(string s) {
	int inBracket = 0, slen = s.length();
	Expr binaryLeft, binaryRight;
	char binaryOperator = 0;

	for (int i = 0; i < slen; i++) {	// handling expressions with binary operator "+" and "-"
		if (s[i] == '(' || s[i] == '[')
			inBracket++;
		else if (s[i] == ')' || s[i] == ']')
			inBracket--;
		else if (s[i] == '+' || s[i] == '-') {
			if (inBracket)	continue;
			string sLeft = s.substr(0, i);
			string sRight = s.substr(i + 1);
			binaryOperator = s[i];
			binaryLeft = handleRHS(sLeft);	binaryRight = handleRHS(sRight);
			break;
		}
	}
	if (!binaryOperator) {
		for (int i = 0; i < slen; i++) {	// handling expressions with binary operator
			if (s[i] == '(' || s[i] == '[')
				inBracket++;
			else if (s[i] == ')' || s[i] == ']')
				inBracket--;
			else if (s[i] == '*' || s[i] == '/' || s[i] == '%') {
				// operator "//" not handled here, considered as "/"
				if (inBracket)	continue;
				string sLeft = s.substr(0, i);
				string sRight;
				if (s[i] == '/' && s[i + 1] == '/')
					sRight = s.substr(i + 2);
				else sRight = s.substr(i + 1);
				binaryOperator = s[i];
				binaryLeft = handleRHS(sLeft);	binaryRight = handleRHS(sRight);
				break;
			}
		}
	}
	if (binaryOperator) {
		BinaryOpType opType;
		switch (binaryOperator) {
		case '+':	opType = BinaryOpType::Add;	break;
		case '-':	opType = BinaryOpType::Sub;	break;
		case '*':	opType = BinaryOpType::Mul;	break;
		case '/':	opType = BinaryOpType::Div;	break;
		case '%':	opType = BinaryOpType::Mod;	break;
		}
		return Binary::make(c.type, opType, binaryLeft, binaryRight);
	}
	else {			// for RHS ::= (RHS) | TRef | SRef | Const | Select
		if (s[0] == '(') {					// RHS ::= (RHS)
			return Unary::make(c.type, UnaryOpType::Bracket, handleRHS(s.substr(1, slen - 2)));		// get rid of ()
		}
		else if (s[0] >= '0' && s[0] <= '9') {		// RHS ::= Const
			bool isFloat = (s.find('.') != s.npos);
			if (isFloat)
				return FloatImm::make(float_type, atof(s.c_str()));
			else return IntImm::make(int_type, atoi(s.c_str()));
		}
		else if (s[0] == 's') {				// RHS ::= Select
			return handleSelect(s);
		}
		else {
			bool isScalar = (s.find('[') == s.npos);
			if (isScalar)
				return handleSRef(s);
			else return handleTRef(s);
		}
	}
}

Expr handleTRef(string s) {

	int lindex = s.find('<'), rindex = s.find('>');
	string name = s.substr(0, lindex);
	string CList = s.substr(lindex + 1, rindex - lindex - 1);
	vector<string> vCList;
	split(CList, vCList, ',');

	vector<size_t> shape;
	for (size_t i = 0; i < vCList.size(); i++) {
		shape.push_back(atoi(vCList[i].c_str()));
	}

	lindex = s.find('[');
	rindex = s.find(']');
	string AList = s.substr(lindex + 1, rindex - lindex - 1);
	vector<string> vAList;
	split(AList, vAList, ',');

	vector<Expr> args;

	for (size_t i = 0; i < vAList.size(); i++) {
		Expr tmp = handleIdExpr(vAList[i]);
		args.push_back(tmp);
	}

	return Var::make(c.type, name.c_str(), args, shape);
}

Expr handleSRef(string s) {
	int tmpidx = s.find('<');
	string name = s.substr(0, tmpidx);
	return Var::make(c.type, name.c_str(), { 0 }, { 1 });
}

Expr handleIdExpr(string s) {

	Expr binaryLeft, binaryRight;		// the left/Right operating num of binary operators

	char binaryOperator = 0;
	int inBracket = 0;
	Expr Id;						// for "IdExpr ::= Id"
	for (size_t i = 0; i < s.length(); i++) {	// handle "+" and "-" firstly
		if (s[i] == '(')
			inBracket++;
		else if (s[i] == ')')
			inBracket--;
		else if (s[i] == '+' || s[i] == '-') {
			if (inBracket) continue;
			binaryOperator = s[i];
			binaryLeft = handleIdExpr(s.substr(0, i));
			binaryRight = handleIdExpr(s.substr(i + 1));
			break;
		}
	}

	if (!binaryOperator) {
		for (size_t i = 0; i < s.length(); i++) {
			if (s[i] == '(')
				inBracket++;
			else if (s[i] == ')')
				inBracket--;
			else if (s[i] == '*' || (s[i] == '/' && s[i + 1] == '/') || s[i] == '%') {
				if (inBracket) continue;
				binaryOperator = s[i];
				binaryLeft = handleIdExpr(s.substr(0, i));
				int tmp = (s[i] == '/' ? i + 2 : i + 1);
				binaryRight = handleIdExpr(s.substr(tmp));
				break;
			}
		}
	}

	if (binaryOperator) {
		BinaryOpType opType;
		switch (binaryOperator) {
		case '+':	opType = BinaryOpType::Add;	break;
		case '-':	opType = BinaryOpType::Sub;	break;
		case '*':	opType = BinaryOpType::Mul;	break;
		case '/':	opType = BinaryOpType::Div;	break;
		case '%':	opType = BinaryOpType::Mod;	break;
		}
		return Binary::make(int_type, opType, binaryLeft, binaryRight);
	}
	else {			// handling IdExpr ::= Id | (IdExpr) | IntV 
					//  IdExpr ::= IntV doesn't appear in the syntax, but may appear
					//  in situation such as (Id + IntV + Id),
					//  since the code handling "+" above progresses from left to right
		if (s[0] >= '0' && s[0] <= '9') {	// IdExpr ::= IntV
			return IntImm::make(int_type, atoi(s.c_str()));
		}
		else if (s[0] == '(') {				// IdExpr ::= (IdExpr)
			return Unary::make(int_type, UnaryOpType::Bracket, handleIdExpr(s.substr(1, s.length() - 2)));
		}
		else {		// IdExpr ::= Id
			for (auto it = IndexList.begin(); it != IndexList.end(); it++) {
				if (it->first == s) {
					Id = it->second;
					break;
				}
			}
			bool currentIndexUsed = false;
			for (size_t i = 0; i < vUsedIndex.size(); i++) {
				if (vUsedIndex[i] == s) {
					currentIndexUsed = true;	break;
				}
			}
			if (!currentIndexUsed && s.size() > 0)
				vUsedIndex.push_back(s);
			return Id;

		}
	}

}

Expr handleSelect(string s) {
	// for Select ::= select(Cond, VTrue, VFalse), get rid of 
	//  "select(" and ")", and then separate into 3 strings by ','
	int slen = s.length();
	s = s.substr(7, slen - 8);
	int comma1 = s.find(','), comma2;
	string sCond = s.substr(0, comma1);
	int inBracket = 0;
	for (int i = comma1 + 1; i < slen; i++) {
		if (s[i] == '<' || s[i] == '(' || s[i] == '[')
			inBracket++;
		else if (s[i] == '>' || s[i] == ')' || s[i] == ']')
			inBracket--;
		else if (s[i] == ',' && !inBracket) {
			comma2 = i; break;
		}
	}
	string sVTrue = s.substr(comma1 + 1, comma2 - comma1 - 1);
	string sVFalse = s.substr(comma2 + 1);
	Expr Cond = handleCond(sCond);
	Expr VTrue = handleRHS(sVTrue);
	Expr VFalse = handleRHS(sVFalse);
	return Select::make(c.type, Cond, VTrue, VFalse);

}

Expr handleCond(string s) {
	// by reading examples given in run2.cc, the Cond syntax
	//  is given as follows:
	//	Cond ::= Cond && Cond
	//		| (IdExpr >= IntV)
	//		| (IdExpr < IntV)
	int andOperator = -1;
	int slen = s.length();
	for (int i = 0; i < slen; i++) {
		if (s[i] == '&' && s[i + 1] == '&') {		// "&&" found!
			andOperator = i;
			break;
		}
	}
	if (andOperator != -1) {	// for Cond ::= Cond && Cond
		string sLeft = s.substr(0, andOperator);
		string sRight = s.substr(andOperator + 2);
		Expr Left = handleCond(sLeft), Right = handleCond(sRight);
		return Binary::make(int_type, BinaryOpType::And, Left, Right);
	}
	else {			// for Cond ::= (IdExpr >= IntV) | (IdExpr < IntV)
		bool GEOperator = (s.find('>') != s.npos);
		int oploc = GEOperator ? s.find('>') : s.find('<');
		CompareOpType op = GEOperator ? CompareOpType::GE : CompareOpType::LT;
		string sIdExpr = s.substr(1, oploc - 1),
			sIntV = s.substr(GEOperator ? oploc + 2 : oploc + 1, s.length() - 1 - (GEOperator ? oploc + 2 : oploc + 1));
		Expr IdExpr = handleIdExpr(sIdExpr),
			IntV = IntImm::make(int_type, atoi(sIntV.c_str()));
		return Unary::make(int_type, UnaryOpType::Bracket,
			Compare::make(int_type, op, IdExpr, IntV));
	}
}

extern void generate();

int main() {
        generate();

	for (int i = 1; i <= 10; i++) {
		string curFile(filename);
		//curFile += std::to_string(i) + ".json";
		curFile += std::to_string(i) + "_graded.json";
		string curOutput(outputfile);
		curOutput += std::to_string(i) + ".cc";
		try {
			c = Case(curFile.c_str());

		}
		catch (...) {
			printf("File %s does not exist\n", curFile.c_str());
			continue;
		}

		IndexList.clear();

		for (auto it = c.finalbound.begin(); it != c.finalbound.end(); it++) {

			Expr Domtemp = Dom::make(int_type, 0, it->second);
			Expr Indextemp = Index::make(int_type, it->first, Domtemp, IndexType::Spatial);
			IndexList.insert(make_pair(it->first, Indextemp));
		}

		Group kernel = handleP(c);

		// printer
		IRPrinter printer;
		std::string code = printer.print(kernel);
		
		string tmptmp("#include \"../run2.h\"\n\n");
		
		for (size_t i = 0; i < c.tins.size(); i++) {
			Tensor &cur = c.tins[i];
			tmptmp += "static ";
			tmptmp += cur.type.is_float()?"float tmp_":"int tmp_";
			tmptmp += cur.name;
			if (cur.tensor_type == TENSOR) {
				tmptmp += "[";
				for (size_t j = 0; j < cur.dim_size.size(); j++) {
					tmptmp += std::to_string(cur.dim_size[j]);
					if (j != cur.dim_size.size() - 1)
						tmptmp += "][";
				}
				tmptmp += "]";
			}
			tmptmp += ";\n";
		}
		
		for (size_t i = 0; i < c.touts.size(); i++) {
			bool inIns = false;
			for (size_t j = 0; j < c.tins.size(); j++) {
				if (!strcmp(c.touts[i].name, c.tins[j].name)) {
					inIns = true;	break;
				}
			}
			if (inIns) continue;

			Tensor &cur = c.touts[i];
			tmptmp += "static ";
			tmptmp += cur.type.is_float()?"float tmp_":"int tmp_";
			tmptmp += cur.name;
			if (cur.tensor_type == TENSOR) {
				tmptmp += "[";
				for (size_t j = 0; j < cur.dim_size.size(); j++) {
					tmptmp += std::to_string(cur.dim_size[j]);
					if (j != cur.dim_size.size() - 1)
						tmptmp += "][";
				}
				tmptmp += "]";
			}
			tmptmp += ";\n";
		}
		
		code = tmptmp + code;
		FILE* file = fopen(curOutput.c_str(), "w");

		fputs(code.c_str(), file);
		fclose(file);
		//	std::cout << code;
	}
}
