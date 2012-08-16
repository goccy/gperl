#include <gperl.hpp>
#include "gen_token_decl.cpp"
using namespace std;

GPerlToken::GPerlToken(string data_, int idx_) : data(data_), idx(idx_), indent(0)
{
	type = Undefined;
}

GPerlTokenizer::GPerlTokenizer(void)
{
}

vector<GPerlToken *> *GPerlTokenizer::tokenize(char *script)
{
	size_t i = 0;
	char token[MAX_TOKEN_SIZE] = {0};
	int token_idx = 0;
	vector<GPerlToken*> *tokens = new vector<GPerlToken *>();
	bool isStringStarted = false;
	bool escapeFlag = false;
	bool mdOperationFlag = false;/*multi or div operation flag*/
	size_t script_size = strlen(script) + 1;
	char num_buffer[2] = {0};
	while (script[i] != EOL) {
		//DBG_PL("[%c]", script[i]);
		switch (script[i]) {
		case '\"':
			if (isStringStarted) {
				//token[token_idx] = script[i];
				//DBG_PL("token = [%s]", token);
				GPerlToken *t = new GPerlToken(string(token));
				t->info = getTokenInfo("String", NULL);
				tokens->push_back(t);
				memset(token, 0, MAX_TOKEN_SIZE);
				token_idx = 0;
				isStringStarted = false;
			} else {
				isStringStarted = true;
			}
			escapeFlag = false;
			break;
		case ' ': case '\t':
			if (isStringStarted) {
				token[token_idx] = script[i];
				token_idx++;
				break;
			}
			if (token[0] != EOL) {
				//DBG_PL("token = [%s]", token);
				tokens->push_back(new GPerlToken(string(token)));
				memset(token, 0, MAX_TOKEN_SIZE);
				token_idx = 0;
			}
			escapeFlag = false;
			break;
		case '\\':
			escapeFlag = true;
			break;
		case 'n':
			if (escapeFlag) {
				token[token_idx] = '\n';
			} else {
				token[token_idx] = 'n';
			}
			token_idx++;
			escapeFlag = false;
			break;
		case '*': case '/' : {
			char tmp[2] = {0};
			if (isStringStarted) {
				token[token_idx] = script[i];
				token_idx++;
				break;
			}
			if (token[0] != EOL) {
				//DBG_PL("***** token = [%s]", token);
				if (atoi(token) != 0) {
					//----previous token is number---
					//insert LEFT CURLY BRACE
					//DBG_PL("token = [(]");
					tmp[0] = '(';
					tokens->push_back(new GPerlToken(string(tmp)));
					mdOperationFlag = true;//to insert RIGHT CURLY BRACE
				}
				tokens->push_back(new GPerlToken(string(token)));
				memset(token, 0, MAX_TOKEN_SIZE);
			} else if (!mdOperationFlag && atoi(cstr(tokens->back()->data)) != 0) {
				//----previous token is number---
				//previous token is white space
				//insert LEFT CURLY BRACE
				//DBG_PL("token = [(]");
				tmp[0] = '(';
				tokens->insert(tokens->end()-1, new GPerlToken(string(tmp)));
				mdOperationFlag = true;//to insert RIGHT CURLY BRACE
			}
			//DBG_PL("token = [%c]", script[i]);
			tmp[0] = script[i];
			tokens->push_back(new GPerlToken(string(tmp)));
			token_idx = 0;
			escapeFlag = false;
			break;
		}
		case '-':
			if (!isStringStarted && i + 1 < script_size) {
				num_buffer[0] = script[i + 1];
				if (atoi(num_buffer) > 0) {
					//negative number
					token[token_idx] = script[i];
					token_idx++;
					break;
				}
				num_buffer[0] = 0;
			}
			//through
		case ',': case ':': case ';': case '=': case '+':
		case '<': case '>': case '&': case '.':
		case '(': case ')': case '{': case '}':
		case '[': case ']': {
			if (isStringStarted) {
				token[token_idx] = script[i];
				token_idx++;
				break;
			}
			if (script[i] == '.' && ('0' <= token[0] && token[0] <= '9')) {
				token[token_idx] = script[i];
				token_idx++;
				break;
			}
			if (token[0] != EOL) {
				//DBG_PL("token = [%s]", token);
				if (token[0] == '@' && script[i] == '{') {
					tokens->push_back(new GPerlToken("@{"));
					memset(token, 0, MAX_TOKEN_SIZE);
					token_idx = 0;
					escapeFlag = false;
					break;
				} else {
					tokens->push_back(new GPerlToken(string(token)));
					memset(token, 0, MAX_TOKEN_SIZE);
				}
			}
			char tmp[2] = {0};
			if (mdOperationFlag &&
				(script[i] == ',' || script[i] == '+' || script[i] == '-' || script[i] == ';')) {
				//insert RIGHT CURLY BRACE
				//DBG_PL("token = [)]");
				tmp[0] = ')';
				tokens->push_back(new GPerlToken(string(tmp)));
				mdOperationFlag = false;
			}
			if ((i + 1 < script_size) &&
				((script[i] == '<' || script[i] == '>' || script[i] == '=' || script[i] == '.' ||
				  script[i] == '+' || script[i] == '-' || script[i] == '*' || script[i] == '/') &&
				 (script[i + 1] == '='))) {
					//DBG_PL("token = [%c=]", script[i]);
					tmp[0] = script[i];
					tokens->push_back(new GPerlToken(string(tmp) + "="));
					i++;
			} else if ((i + 1 < script_size) &&
					   (script[i] == '<' && script[i + 1] == '<') ||
					   (script[i] == '>' && script[i + 1] == '>')) {
				tmp[0] = script[i];
				tmp[1] = script[i + 1];
				tokens->push_back(new GPerlToken(string(tmp)));
				i++;
			} else if ((i + 1 < script_size) && script[i] == '+' && script[i + 1] == '+') {
				tokens->push_back(new GPerlToken("++"));
				i++;
			} else if ((i + 1 < script_size) && script[i] == '-' && script[i + 1] == '-') {
				tokens->push_back(new GPerlToken("--"));
				i++;
			} else {
				//DBG_PL("token = [%c]", script[i]);
				tmp[0] = script[i];
				tokens->push_back(new GPerlToken(string(tmp)));
			}
			token_idx = 0;
			escapeFlag = false;
			break;
		}
		case '!': {
			if (isStringStarted) {
				token[token_idx] = script[i];
				token_idx++;
				break;
			}
			if (token[0] != EOL) {
				//DBG_PL("token = [%s]", token);
				tokens->push_back(new GPerlToken(string(token)));
				memset(token, 0, MAX_TOKEN_SIZE);
			}
			char tmp[2] = {0};
			if (i + 1 < script_size && script[i + 1] == '=') {
				//DBG_PL("token = [%c=]", script[i]);
				tmp[0] = script[i];
				tokens->push_back(new GPerlToken(string(tmp) + "="));
				i++;
			} else {
				//DBG_PL("token = [%c]", script[i]);
				tmp[0] = script[i];
				tokens->push_back(new GPerlToken(string(tmp)));
			}
			token_idx = 0;
			escapeFlag = false;
			break;
		}
		case '0': case '1': case '2': case '3': case '4': case '5':
		case '6': case '7': case '8': case '9': {
			token[token_idx] = script[i];
			token_idx++;
			escapeFlag = false;
			break;
		}
		case '\n':
			escapeFlag = false;
			break;
		default:
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
			data == ";"     ||
			data == ","     || data == ","     ||
			data == "&"     || data == "("     ||
			data == ")"     || data == "{"     ||
			data == "}"     || data == "["     ||
			data == "]"     || data == "@{"    ||
			data == "%{"    || data == "!"     ||
			data == "<<"    || data == ">>"    ||
			data == "print" || data == "push"  ||
			data == "ref"   || data == "undef" ||
			data == "if"    || data == "else"  ||
			data == "elsif" || data == "unless"||
			data == "my"    || data == "sub"   ||
			data == "shift" || data == "while" ||
			data == "for"   || data == "foreach" ||
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
		} else if (search(vardecl_list, t->data)) {
			if (t->data.find("@") != string::npos) {
				t->info = getTokenInfo("ArrayVar", NULL);
				cur_type = ArrayVar;
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
		} else if (t->data == "0" || atoi(cstr(t->data)) != 0) {
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
			cur_type = 0;
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
