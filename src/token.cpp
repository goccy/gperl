#include <gperl.hpp>

using namespace std;

Token::Token(string data_, int idx_) : data(data_), idx(idx_)
{
	type = 0;
}

GPerlTokenizer::GPerlTokenizer(void)
{
}

vector<Token *> *GPerlTokenizer::tokenize(char *script)
{
	int i = 0;
	char token[MAX_TOKEN_SIZE] = {0};
	int token_idx = 0;
	vector<Token*> *tokens = new vector<Token *>();
	bool isStringStarted = false;
	bool escapeFlag = false;
	while (script[i] != EOL) {
		fprintf(stderr, "[%c]\n", script[i]);
		switch (script[i]) {
		case '\"':
			if (isStringStarted) {
				//token[token_idx] = script[i];
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
		case ' ':
			if (isStringStarted) {
				token[token_idx] = script[i];
				token_idx++;
				break;
			}
			if (token[0] != EOL) {
				fprintf(stderr, "token = [%s]\n", token);
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
		case ',': case ':': case ';': case '=': case '+': case '-':
		case '(': case ')': {
			if (isStringStarted) {
				token[token_idx] = script[i];
				token_idx++;
				break;
			}
			if (token[0] != EOL) {
				fprintf(stderr, "token = [%s]\n", token);
				tokens->push_back(new Token(string(token)));
				memset(token, 0, MAX_TOKEN_SIZE);
			}
			fprintf(stderr, "token = [%c]\n", script[i]);
			char tmp[2] = {0};
			tmp[0] = script[i];
			tokens->push_back(new Token(string(tmp)));
			token_idx = 0;
			escapeFlag = false;
			break;
		}
		case '0': case '1': case '2': case '3': case '4': case '5':
		case '6': case '7': case '8': case '9': {
			fprintf(stderr, "token = [%c]\n", script[i]);
			char tmp[2] = {0};
			tmp[0] = script[i];
			Token *t = new Token(string(tmp));
			t->type = Int;
			tokens->push_back(t);
			token_idx = 0;
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
		fprintf(stderr, "data = [%s]", t->data.c_str());
		fprintf(stderr, " TYPE:");
		dumpType(t);
		fprintf(stderr, "\n");
		it++;
	}
}

void GPerlTokenizer::dumpType(Token *token)
{
	switch (token->type) {
	case Undefined:
		fprintf(stderr, "Undefined");
		break;
	case FunctionDecl:
		fprintf(stderr, "FunctionDecl");
		break;
	case FieldDecl:
		fprintf(stderr, "FieldDecl");
		break;
	case TypeRef:
		fprintf(stderr, "TypeRef");
		break;
	case LabelRef:
		fprintf(stderr, "LabelRef");
		break;
	case Assign:
		fprintf(stderr, "Assign");
		break;
	case VarDecl:
		fprintf(stderr, "VarDecl");
		break;
	case PrintDecl:
		fprintf(stderr, "PrintDecl");
		break;
	case Comma:
		fprintf(stderr, "Comma");
		break;
	case SemiColon:
		fprintf(stderr, "SemiColon");
		break;
	case RightParenthesis:
		fprintf(stderr, "RightParenthesis");
		break;
	case LeftParenthesis:
		fprintf(stderr, "LeftParenthesis");
		break;
	case Var:
		fprintf(stderr, "Var");
		break;
	case Int:
		fprintf(stderr, "Int");
		break;
	case Float:
		fprintf(stderr, "Float");
		break;
	case String:
		fprintf(stderr, "String");
		break;
	case Object:
		fprintf(stderr, "Object");
		break;
	case Operator:
		fprintf(stderr, "Operator");
		break;
	case LocalVar:
		fprintf(stderr, "LocalVar");
		break;
	case GlobalVar:
		fprintf(stderr, "GlobalVar");
		break;
	default:
		break;
	}
}

const char *GPerlTokenizer::getTypeName(GPerlTypes type)
{
	switch (type) {
	case Undefined:
		return "Undefined";
	case FunctionDecl:
		return "FunctionDecl";
	case FieldDecl:
		return "FieldDecl";
	case TypeRef:
		return "TypeRef";
	case LabelRef:
		return "LabelRef";
	case Assign:
		return "Assign";
	case VarDecl:
		return "VarDecl";
	case PrintDecl:
		return "PrintDecl";
	case Comma:
		return "Comma";
	case SemiColon:
		return "SemiColon";
	case RightParenthesis:
		return "RightParenthesis";
	case LeftParenthesis:
		return "LeftParenthesis";
	case Var:
		return "Var";
	case Int:
		return "Int";
	case Float:
		return "Float";
	case String:
		return "String";
	case Object:
		return "Object";
	case Operator:
		return "Operator";
	case LocalVar:
		return "LocalVar";
	case GlobalVar:
		return "GlobalVar";
		break;
	default:
		break;
	}
	return "";
}

void GPerlTokenizer::annotateTokens(vector<Token *> *tokens)
{
	vector<Token *>::iterator it = tokens->begin();
	vector<string> vardecl_list;
	int cur_type = 0;
	while (it != tokens->end()) {
		Token *t = (Token *)*it;
		if (t->data == "my") {
			t->type = VarDecl;
			cur_type = VarDecl;
		} else if  (t->data == "+") {
			t->type = Operator;
			cur_type = Operator;
		} else if (cur_type == VarDecl && t->data.find("$") != -1) {
			t->type = LocalVar;
			vardecl_list.push_back(t->data);
			cur_type = LocalVar;
		} else if (t->data == "=") {
			t->type = Assign;
			cur_type = Assign;
		} else if (t->data == ";") {
			t->type = SemiColon;
		} else if (t->data == ",") {
			t->type = Comma;
		} else if (t->data == "(") {
			t->type = LeftParenthesis;
		} else if (t->data == ")") {
			t->type = RightParenthesis;
		} else if (t->data == "print") {
			t->type = PrintDecl;
			cur_type = PrintDecl;
		} else if (search(vardecl_list, t->data)) {
			t->type = Var;
			cur_type = Var;
		} else {
			cur_type = 0;
		}
		it++;
	}
}

bool GPerlTokenizer::search(vector<string> &list, string target)
{
	bool ret = false;
	vector<string>::iterator it = find(list.begin(), list.end(), target);
	if (it != list.end()){
		ret = true;
	}
	return ret;
}
