#include <gperl.hpp>
#include <cassert>
#include "gen_token_decl.cpp"
using namespace std;

GPerlToken::GPerlToken(string data_, int idx_) : data(data_), idx(idx_), indent(0)
{
	type = Undefined;
}

GPerlTokenizer::GPerlTokenizer(void)
  : isStringStarted(false), escapeFlag(false),
	mdOperationFlag(false), commentFlag(false),
	token_idx(0), max_token_size(0)
{
}

void GPerlTokenizer::scanDoubleQuote(GPerlTokens *tks)
{
	if (isStringStarted) {
		GPerlToken *t = new GPerlToken(string(token));
		t->info = getTokenInfo("String", NULL);
		tks->push_back(t);
		memset(token, 0, max_token_size);
		token_idx = 0;
		isStringStarted = false;
	} else {
		isStringStarted = true;
	}
	escapeFlag = false;
}

void GPerlTokenizer::scanEscapeChar(GPerlTokens *tks, char ch)
{
	if (isStringStarted) {
		token[token_idx] = ch;
		token_idx++;
		return;
	}
	if (token[0] != EOL) {
		tks->push_back(new GPerlToken(string(token)));
		memset(token, 0, max_token_size);
		token_idx = 0;
	}
	escapeFlag = false;
}

void GPerlTokenizer::scanNewLineKeyword(void)
{
	if (escapeFlag) {
		token[token_idx] = '\n';
	} else {
		token[token_idx] = 'n';
	}
	token_idx++;
	escapeFlag = false;
}

void GPerlTokenizer::scanTabKeyword(void)
{
	if (escapeFlag) {
		token[token_idx] = '\t';
	} else {
		token[token_idx] = 't';
	}
	token_idx++;
	escapeFlag = false;
}

void GPerlTokenizer::scanMDOperator(GPerlTokens *tks, char op)
{
	char tmp[2] = {0};
	if (isStringStarted) {
		token[token_idx] = op;
		token_idx++;
		return;
	}
	if (token[0] != EOL) {
		if (atoi(token) != 0) {
			//----previous token is number---
			//insert LEFT CURLY BRACE
			//DBG_PL("token = [(]");
			tmp[0] = '(';
			tks->push_back(new GPerlToken(string(tmp)));
			mdOperationFlag = true;//to insert RIGHT CURLY BRACE
		}
		tks->push_back(new GPerlToken(string(token)));
		memset(token, 0, max_token_size);
	} else if (!mdOperationFlag && atoi(cstr(tks->back()->data)) != 0) {
		//----previous token is number---
		//previous token is white space
		//insert LEFT CURLY BRACE
		//DBG_PL("token = [(]");
		tmp[0] = '(';
		tks->insert(tks->end()-1, new GPerlToken(string(tmp)));
		mdOperationFlag = true;//to insert RIGHT CURLY BRACE
	}
	tmp[0] = op;
	tks->push_back(new GPerlToken(string(tmp)));
	token_idx = 0;
	escapeFlag = false;
}

bool GPerlTokenizer::scanNegativeNumber(char number)
{
	char num_buffer[2] = {0};
	if (!isStringStarted) {
		num_buffer[0] = number;
		if (atoi(num_buffer) > 0) {
			//negative number
			token[token_idx] = '-';
			token_idx++;
			return true;
		}
	}
	return false;
}

int GPerlTokenizer::scanSymbol(GPerlTokens *tks, char symbol, char next_ch)
{
	int ret = 0;
	if (isStringStarted) {
		token[token_idx] = symbol;
		token_idx++;
		return ret;
	}
	if (symbol == '.' && ('0' <= token[0] && token[0] <= '9')) {
		//double value
		token[token_idx] = symbol;
		token_idx++;
		return ret;
	}
	if (token[0] != EOL) {
		if (token[0] == '@' && symbol == '{') {
			tks->push_back(new GPerlToken("@{"));
			memset(token, 0, max_token_size);
			token_idx = 0;
			escapeFlag = false;
			return ret;
		} else {
			tks->push_back(new GPerlToken(string(token)));
			memset(token, 0, max_token_size);
		}
	}
	char tmp[3] = {0};
	if (mdOperationFlag &&
		(symbol == ',' || symbol == '+' ||
		 symbol == '-' || symbol == ';')) {
		//insert RIGHT CURLY BRACE
		//DBG_PL("token = [)]");
		tmp[0] = ')';
		tks->push_back(new GPerlToken(string(tmp)));
		mdOperationFlag = false;
	}
	if (next_ch == '=') {
		switch (symbol) {
		case '<': case '>': case '=': case '.': case '!':
		case '+': case '-': case '*': case '/':
			tmp[0] = symbol;
			tks->push_back(new GPerlToken(string(tmp) + "="));
			ret = 1;
			break;
		default:
			break;
		}
	} else if ((symbol == '<' && next_ch == '<') ||
			   (symbol == '>' && next_ch == '>') ||
			   (symbol == '+' && next_ch == '+') ||
			   (symbol == '=' && next_ch == '>') ||
			   (symbol == '-' && next_ch == '-')) {
		tmp[0] = symbol;
		tmp[1] = next_ch;
		tks->push_back(new GPerlToken(string(tmp)));
		ret = 1;
	} else {
		tmp[0] = symbol;
		tks->push_back(new GPerlToken(string(tmp)));
	}
	token_idx = 0;
	escapeFlag = false;
	return ret;
}

void GPerlTokenizer::scanSymbol(GPerlTokens *tks, char symbol)
{
	if (isStringStarted) {
		token[token_idx] = symbol;
		token_idx++;
		return;
	}
	if (symbol == '.' && ('0' <= token[0] && token[0] <= '9')) {
		//double value
		token[token_idx] = symbol;
		token_idx++;
		return;
	}
	if (token[0] != EOL) {
		if (token[0] == '@' && symbol == '{') {
			tks->push_back(new GPerlToken("@{"));
			memset(token, 0, max_token_size);
			token_idx = 0;
			escapeFlag = false;
			return;
		} else {
			tks->push_back(new GPerlToken(string(token)));
			memset(token, 0, max_token_size);
		}
	}
	char tmp[2] = {0};
	if (mdOperationFlag &&
		(symbol == ',' || symbol == '+' ||
		 symbol == '-' || symbol == ';')) {
		//insert RIGHT CURLY BRACE
		//DBG_PL("token = [)]");
		tmp[0] = ')';
		tks->push_back(new GPerlToken(string(tmp)));
		mdOperationFlag = false;
	}
	tmp[0] = symbol;
	tks->push_back(new GPerlToken(string(tmp)));
	memset(token, 0, max_token_size);
	token_idx = 0;
	escapeFlag = false;
}

#define isSKIP() commentFlag
#define NEXT() (*(src + i++))

static GPerlToken *parseNumber(GPerlTokenizer *tkn, char *src, size_t &i)
{
	char *begin = src + i;
	int c = NEXT();
	GPerlToken *token = NULL;
	assert((c == '.' || ('0' <= c && c <= '9')) && "It do not seem as Number");
	bool isFloat = false;
	/*
	 * DIGIT  = 0-9
	 * DIGITS = DIGIT | DIGIT DIGITS
	 * INT    = DIGIT | DIGIT1-9 DIGITS
	 * FLOAT  = INT
	 *        | INT FRAC
	 *        | INT EXP
	 *        | INT FRAC EXP
	 * FRAC   = "." digits
	 * EXP    = E digits
	 * E      = 'e' | 'e+' | 'e-' | 'E' | 'E+' | 'E-'
	 */
	if (c == '0') {
		c = NEXT();
	}
	else if ('1' <= c && c <= '9') {
		for (; '0' <= c && c <= '9' && c != EOL; c = NEXT()) {}
	}
	if (c != '.' && c != 'e' && c != 'E') {
		goto L_emit;
	}
	if (c == '.') {
		isFloat = true;
		for (c = NEXT(); '0' <= c && c <= '9' && c != EOL; c = NEXT()) {}
	}
	if (c == 'e' || c == 'E') {
		isFloat = true;
		c = NEXT();
		if (c == '+' || c == '-') {
			c = NEXT();
		}
		for (; '0' <= c && c <= '9' && c != EOL; c = NEXT()) {}
	}
	L_emit:;
	i -= 1;
	token = new GPerlToken(string(begin, src+i));
	token->info = isFloat ?
		tkn->getTokenInfo("Double", NULL) : tkn->getTokenInfo("Int", NULL);
	return token;
}
#undef NEXT

GPerlTokens *GPerlTokenizer::tokenize(char *script)
{
	size_t i = 0;
	size_t script_size = strlen(script) + 1;
	max_token_size = script_size;
	token = (char *)safe_malloc(max_token_size);
	GPerlTokens *tokens = new GPerlTokens();
	while (script[i] != EOL) {
		switch (script[i]) {
		case '\"':
			if (isSKIP()) break;
			scanDoubleQuote(tokens);
			break;
		case '#':
			if (isSKIP()) break;
			if (isStringStarted) {
				token[token_idx] = script[i];
				token_idx++;
				break;
			}
			while (script[i] != '\n' && i < script_size) {i++;}
			break;
		case ' ': case '\t':
			if (isSKIP()) break;
			scanEscapeChar(tokens, script[i]);
			break;
		case '\\':
			if (isSKIP()) break;
			if (i + 1 < script_size && script[i + 1] == '&') {
				tokens->push_back(new GPerlToken(string("\\&")));
				i++;
			} else {
				escapeFlag = true;
			}
			break;
		case 'n':
			if (isSKIP()) break;
			scanNewLineKeyword();
			break;
		case 't':
			if (isSKIP()) break;
			scanTabKeyword();
			break;
		case '*': case '/' :
			if (isSKIP()) break;
			scanMDOperator(tokens, script[i]);
			break;
		case '-':
			if (isSKIP()) break;
			if (i + 1 < script_size &&
				scanNegativeNumber(script[i + 1])) {
				break;
			}
			//fall through
		case '=':
			if (0 < i && i + 1 < script_size &&
				script[i - 1] == '\n' && isalnum(script[i + 1])) {
				//multi-line comment flag
				if (i + 3 < script_size &&
					script[i + 1] == 'c' && script[i + 2] == 'u' && script[i + 3] == 't')  {
					DBG_PL("commentFlag => OFF");
					i += 3;
					commentFlag = false;
					break;
				} else {
					DBG_PL("commentFlag => ON");
					commentFlag = true;
				}
			}
			//fall through
		case ',': case ':': case ';': case '+':
		case '<': case '>': case '&': case '.': case '!':
		case '(': case ')': case '{': case '}':
		case '[': case ']': {
			if (isSKIP()) break;
			if (i + 1 < script_size) {
				i += scanSymbol(tokens, script[i], script[i + 1]);
			} else {
				scanSymbol(tokens, script[i]);
			}
			break;
		}
		case '\n':
			if (isSKIP()) break;
			escapeFlag = false;
			break;
		case '0': case '1': case '2': case '3': case '4': case '5':
		case '6': case '7': case '8': case '9':
			if (isSKIP()) break;
			if (isStringStarted) {
				token[token_idx] = script[i];
				token_idx++;
				break;
			}
			if (token_idx == 0) {
				GPerlToken *tk = parseNumber(this, script, i);
				tokens->push_back(tk);
				escapeFlag = false;
				continue;
			}
		default:
			if (isSKIP()) break;
			token[token_idx] = script[i];
			token_idx++;
			escapeFlag = false;
			break;
		}
		i++;
	}
	return tokens;
}

void GPerlTokenizer::dump(vector<GPerlToken *> *tokens)
{
	vector<GPerlToken *>::iterator it = tokens->begin();
	while (it != tokens->end()) {
		GPerlToken *t = (GPerlToken *)*it;
		(void)t;
		DBG_PL("[%s] TYPE:%s", cstr(t->data), t->info.name);
		it++;
	}
}

GPerlTokenInfo GPerlTokenizer::getTokenInfo(const char *name, const char *data)
{
	int i = 0;
	if (name) {
		size_t nsize = strlen(name);
		while (true) {
			GPerlT type = decl_tokens[i].type;
			const char *token_name = decl_tokens[i].name;
			size_t tsize = strlen(token_name);
			if (nsize == tsize && !strncmp(token_name, name, nsize)) {
				return decl_tokens[i];
			} else if (type == Undefined) {
				return decl_tokens[i];
			}
			i++;
		}
	} else if (data) {
		size_t dsize = strlen(data);
		while (true) {
			GPerlT type = decl_tokens[i].type;
			const char *token_data = decl_tokens[i].data;
			size_t tsize = strlen(token_data);
			if (dsize == tsize && !strncmp(token_data, data, dsize)) {
				return decl_tokens[i];
			} else if (type == Undefined) {
				break;
			}
			i++;
		}
	}
	return decl_tokens[i];
}

void GPerlTokenizer::annotateTokens(vector<GPerlToken *> *tokens)
{
	vector<GPerlToken *>::iterator it = tokens->begin();
	vector<string> vardecl_list;
	vector<string> funcdecl_list;
	int cur_type = 0;
	while (it != tokens->end()) {
		GPerlToken *t = (GPerlToken *)*it;
		string data = t->data;
		if (data == "+"     || data == "-"     ||
			data == "*"     || data == "/"     ||
			data == "<"     || data == ">"     ||
			data == "<="    || data == ">="    ||
			data == "=="    || data == "!="    ||
			data == "="     || data == "+="    ||
			data == "-="    || data == "*="    ||
			data == "/="    || data == ".="    ||
			data == "++"    || data == "--"    ||
			data == ";"     || data == "=>"    ||
			data == ","     || data == ","     ||
			data == "&"     || data == "\\&"   ||
			data == "("     ||
			data == ")"     || data == "{"     ||
			data == "}"     || data == "["     ||
			data == "]"     || data == "@{"    ||
			data == "%{"    || data == "!"     ||
			data == "<<"    || data == ">>"    ||
			data == "print" || data == "push"  ||
			data == "ref"   || data == "undef" ||
			data == "keys"  || data == "values" ||
			data == "if"    || data == "else"  ||
			data == "elsif" || data == "unless"||
			data == "my"    || data == "sub"   ||
			data == "shift" || data == "while" ||
			data == "for"   || data == "foreach" ||
			data == "$_"    || data == "map"   ||
			data == "@_"    || data == "@ARGV" ||
			data == "return") {
			DBG_PL("TOKEN = [%s]", cstr(data));
			t->info = getTokenInfo(NULL, cstr(data));
			cur_type = t->info.type;
		} else if (cur_type == VarDecl && t->data.find("$") != string::npos) {
			t->info = getTokenInfo("LocalVar", NULL);
			vardecl_list.push_back(t->data);
			cur_type = LocalVar;
		} else if (cur_type == VarDecl && t->data.find("@") != string::npos) {
			t->info = getTokenInfo("LocalArrayVar", NULL);
			vardecl_list.push_back(t->data);
			cur_type = LocalArrayVar;
		} else if (cur_type == VarDecl && t->data.find("%") != string::npos) {
			t->info = getTokenInfo("LocalHashVar", NULL);
			vardecl_list.push_back(t->data);
			cur_type = LocalHashVar;
		} else if (search(vardecl_list, t->data)) {
			if (t->data.find("@") != string::npos) {
				t->info = getTokenInfo("ArrayVar", NULL);
				cur_type = ArrayVar;
			} else if (t->data.find("%") != string::npos) {
				t->info = getTokenInfo("HashVar", NULL);
				cur_type = HashVar;
			} else {
				t->info = getTokenInfo("Var", NULL);
				cur_type = Var;
			}
		} else if (t->data.find("$") != string::npos) {
			t->info = getTokenInfo("GlobalVar", NULL);
			vardecl_list.push_back(t->data);
			cur_type = GlobalVar;
		} else if (t->data.find("@") != string::npos) {
			t->info = getTokenInfo("GlobalArrayVar", NULL);
			vardecl_list.push_back(t->data);
			cur_type = GlobalArrayVar;
		} else if (t->data.find("%") != string::npos) {
			t->info = getTokenInfo("GlobalHashVar", NULL);
			vardecl_list.push_back(t->data);
			cur_type = GlobalHashVar;
		} else if (t->info.type == Double) {
			cur_type = Double;
		} else if (t->info.type == Int) {
			cur_type = Int;
		} else if (t->data == "0" || atoi(cstr(t->data)) != 0) {
			if (t->info.type == String) {
				cur_type = 0; it++;
				continue;
			}
			if (t->data.find(".") != string::npos) {
				t->info = getTokenInfo("Double", NULL);
				cur_type = Double;
			} else {
				t->info = getTokenInfo("Int", NULL);
				cur_type = Int;
			}
		} else if (cur_type == FunctionDecl) {
			t->info = getTokenInfo("Function", NULL);
			cur_type = Function;
			funcdecl_list.push_back(t->data);
		} else if (search(funcdecl_list, t->data)) {
			t->info = getTokenInfo("Call", NULL);
			cur_type = Call;
		} else {
			//string
			if (t->info.type != String) {
				t->info = getTokenInfo("Key", NULL);
				cur_type = Key;
			} else {
				cur_type = 0;
			}
		}
		it++;
	}
}

bool GPerlTokenizer::search(vector<string> list, string target)
{
	bool ret = false;
	vector<string>::iterator it = find(list.begin(), list.end(), target);
	if (it != list.end()){
		ret = true;
	}
	return ret;
}
