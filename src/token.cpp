#include <gperl.hpp>
#include <token.hpp>
using namespace std;

Token::Token(string data_, int idx_) : data(data_), idx(idx_)
{
	type = Undefined;
}

GPerlTokenizer::GPerlTokenizer(void)
{
}

vector<Token *> *GPerlTokenizer::tokenize(char *script)
{
	size_t i = 0;
	char token[MAX_TOKEN_SIZE] = {0};
	int token_idx = 0;
	vector<Token*> *tokens = new vector<Token *>();
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
				Token *t = new Token(string(token));
				t->type = String;
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
				tokens->push_back(new Token(string(token)));
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
					tokens->push_back(new Token(string(tmp)));
					mdOperationFlag = true;//to insert RIGHT CURLY BRACE
				}
				tokens->push_back(new Token(string(token)));
				memset(token, 0, MAX_TOKEN_SIZE);
			} else if (!mdOperationFlag && atoi(cstr(tokens->back()->data)) != 0) {
				//----previous token is number---
				//previous token is white space
				//insert LEFT CURLY BRACE
				//DBG_PL("token = [(]");
				tmp[0] = '(';
				tokens->insert(tokens->end()-1, new Token(string(tmp)));
				mdOperationFlag = true;//to insert RIGHT CURLY BRACE
			}
			//DBG_PL("token = [%c]", script[i]);
			tmp[0] = script[i];
			tokens->push_back(new Token(string(tmp)));
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
		case '<': case '>':
		case '(': case ')': case '{': case '}': {
			if (isStringStarted) {
				token[token_idx] = script[i];
				token_idx++;
				break;
			}
			if (token[0] != EOL) {
				//DBG_PL("token = [%s]", token);
				tokens->push_back(new Token(string(token)));
				memset(token, 0, MAX_TOKEN_SIZE);
			}
			char tmp[2] = {0};
			if (mdOperationFlag &&
				(script[i] == ',' || script[i] == '+' || script[i] == '-' || script[i] == ';')) {
				//insert RIGHT CURLY BRACE
				//DBG_PL("token = [)]");
				tmp[0] = ')';
				tokens->push_back(new Token(string(tmp)));
				mdOperationFlag = false;
			}
			if (i + 1 < script_size &&
				(script[i] == '<' || script[i] == '>' || script[i] == '=') &&
				script[i + 1] == '=') {
				//DBG_PL("token = [%c=]", script[i]);
				tmp[0] = script[i];
				tokens->push_back(new Token(string(tmp) + "="));
				i++;
			} else {
				//DBG_PL("token = [%c]", script[i]);
				tmp[0] = script[i];
				tokens->push_back(new Token(string(tmp)));
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
				tokens->push_back(new Token(string(token)));
				memset(token, 0, MAX_TOKEN_SIZE);
			}
			char tmp[2] = {0};
			if (i + 1 < script_size && script[i + 1] == '=') {
				//DBG_PL("token = [%c=]", script[i]);
				tmp[0] = script[i];
				tokens->push_back(new Token(string(tmp) + "="));
				i++;
			} else {
				//DBG_PL("token = [%c]", script[i]);
				tmp[0] = script[i];
				tokens->push_back(new Token(string(tmp)));
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

void GPerlTokenizer::dump(vector<Token *> *tokens)
{
	vector<Token *>::iterator it = tokens->begin();
	while (it != tokens->end()) {
		Token *t = (Token *)*it;
		DBG_PL("[%s] TYPE:%s", cstr(t->data), getTypeName(t->type));
		it++;
	}
}

void GPerlTokenizer::dumpType(Token *token)
{
	DBG_P("%s", TypeName(token->type));
}

const char *GPerlTokenizer::getTypeName(GPerlT type)
{
	return TypeName(type);
}

GPerlT GPerlTokenizer::getType(const char *name)
{
	int i = 0;
	size_t nsize = strlen(name);
	while (true) {
		const char *type_name = decl_token_types[i].str;
		GPerlT type = decl_token_types[i].type;
		size_t tsize = strlen(type_name);
		if (nsize == tsize && !strncmp(type_name, name, nsize)) {
			return type;
		} else if (type == Undefined) {
			return Undefined;
		}
		i++;
	}
}

void GPerlTokenizer::annotateTokens(vector<Token *> *tokens)
{
	vector<Token *>::iterator it = tokens->begin();
	vector<string> vardecl_list;
	vector<string> funcdecl_list;
	int cur_type = 0;
	while (it != tokens->end()) {
		Token *t = (Token *)*it;
		string data = t->data;
		if (data == "+"     || data == "-"   || data == "*"     || data == "/"  ||
			data == "<"     || data == ">"   || data == "<="    || data == ">=" ||
			data == "=="    || data == "!="  || data == "="     ||
			data == ";"     || data == ","   || data == ","     ||
			data == "("     || data == ")"   || data == "{"     || data == "}"  ||
			data == "print" || data == "if"  || data == "else"  ||
			data == "my"    || data == "sub" || data == "shift" ||
			data == "return") {
			GPerlT type = getType(cstr(data));
			t->type = type;
			cur_type = type;
		} else if (cur_type == VarDecl && t->data.find("$") != string::npos) {
			t->type = LocalVar;
			vardecl_list.push_back(t->data);
			cur_type = LocalVar;
		} else if (search(vardecl_list, t->data)) {
			t->type = Var;
			cur_type = Var;
		} else if (t->data.find("$") != string::npos) {
			t->type = GlobalVar;
			vardecl_list.push_back(t->data);
			cur_type = GlobalVar;
		} else if (t->data == "0" || atoi(cstr(t->data)) != 0) {
			t->type = Int;
			cur_type = Int;
		} else if (cur_type == FunctionDecl) {
			t->type = Function;
			cur_type = Function;
			funcdecl_list.push_back(t->data);
		} else if (search(funcdecl_list, t->data)) {
			t->type = Call;
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
