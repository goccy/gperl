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
		fprintf(stderr, "[%c]\n", script[i]);
		switch (script[i]) {
		case '\"':
			if (isStringStarted) {
				//token[token_idx] = script[i];
				fprintf(stderr, "token = [%s]\n", token);
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
		case '*': case '/' : {
			char tmp[2] = {0};
			if (isStringStarted) {
				token[token_idx] = script[i];
				token_idx++;
				break;
			}
			if (token[0] != EOL) {
				fprintf(stderr, "***** token = [%s]\n", token);
				if (atoi(token) != 0) {
					//----previous token is number---
					//insert LEFT CURLY BRACE
					DBG_P("token = [(]");
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
				DBG_P("token = [(]");
				tmp[0] = '(';
				tokens->insert(tokens->end()-1, new Token(string(tmp)));
				mdOperationFlag = true;//to insert RIGHT CURLY BRACE
			}
			fprintf(stderr, "token = [%c]\n", script[i]);
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
				fprintf(stderr, "token = [%s]\n", token);
				tokens->push_back(new Token(string(token)));
				memset(token, 0, MAX_TOKEN_SIZE);
			}
			char tmp[2] = {0};
			if (mdOperationFlag &&
				(script[i] == ',' || script[i] == '+' || script[i] == '-' || script[i] == ';')) {
				//insert RIGHT CURLY BRACE
				DBG_P("token = [)]");
				tmp[0] = ')';
				tokens->push_back(new Token(string(tmp)));
				mdOperationFlag = false;
			}
			if (i + 1 < script_size &&
				(script[i] == '<' || script[i] == '>' || script[i] == '=') &&
				script[i + 1] == '=') {
				fprintf(stderr, "token = [%c=]\n", script[i]);
				tmp[0] = script[i];
				tokens->push_back(new Token(string(tmp) + "="));
				i++;
			} else {
				fprintf(stderr, "token = [%c]\n", script[i]);
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
				fprintf(stderr, "token = [%s]\n", token);
				tokens->push_back(new Token(string(token)));
				memset(token, 0, MAX_TOKEN_SIZE);
			}
			char tmp[2] = {0};
			if (i + 1 < script_size && script[i + 1] == '=') {
				fprintf(stderr, "token = [%c=]\n", script[i]);
				tmp[0] = script[i];
				tokens->push_back(new Token(string(tmp) + "="));
				i++;
			} else {
				fprintf(stderr, "token = [%c]\n", script[i]);
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
	case RightBrace:
		fprintf(stderr, "RightBrace");
		break;
	case LeftBrace:
		fprintf(stderr, "LeftBrace");
		break;
	case IfStmt:
		fprintf(stderr, "IfStmt");
		break;
	case ElseStmt:
		fprintf(stderr, "ElseStmt");
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
	case Function:
		fprintf(stderr, "Function");
		break;
	case Call:
		fprintf(stderr, "Call");
		break;
	case Shift:
		fprintf(stderr, "Shift");
		break;
	case Argument:
		fprintf(stderr, "Argument");
		break;
	case Return:
		fprintf(stderr, "Return");
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
	case IfStmt:
		return "IfStmt";
	case ElseStmt:
		return "ElseStmt";
	case Comma:
		return "Comma";
	case SemiColon:
		return "SemiColon";
	case RightParenthesis:
		return "RightParenthesis";
	case LeftParenthesis:
		return "LeftParenthesis";
	case RightBrace:
		return "RightBrace";
	case LeftBrace:
		return "LeftBrace";
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
	case Greater:
		return "Greater";
	case Less:
		return "Less";
	case GreaterEqual:
		return "GreaterEqual";
	case LessEqual:
		return "LessEqual";
	case EqualEqual:
		return "EqualEqual";
	case NotEqual:
		return "NotEqual";
	case Function:
		return "Function";
	case Call:
		return "Call";
	case Shift:
		return "Shift";
	case Argument:
		return "Argument";
	case Return:
		return "Return";
	default:
		break;
	}
	return "";
}

void GPerlTokenizer::annotateTokens(vector<Token *> *tokens)
{
	vector<Token *>::iterator it = tokens->begin();
	vector<string> vardecl_list;
	vector<string> funcdecl_list;
	int cur_type = 0;
	while (it != tokens->end()) {
		Token *t = (Token *)*it;
		if (t->data == "my") {
			t->type = VarDecl;
			cur_type = VarDecl;
		} else if  (t->data == "+") {
			t->type = Operator;
			cur_type = Operator;
		} else if (t->data == "-") {
			t->type = Operator;
			cur_type = Operator;
		} else if (t->data == "*") {
			t->type = Operator;
			cur_type = Operator;
		} else if (t->data == "/") {
			t->type = Operator;
			cur_type = Operator;
		} else if (t->data == "<") {
			t->type = Operator;
			cur_type = Operator;
		} else if (t->data == ">") {
			t->type = Operator;
			cur_type = Operator;
		} else if (t->data == "<=") {
			t->type = Operator;
			cur_type = Operator;
		} else if (t->data == ">=") {
			t->type = Operator;
			cur_type = Operator;
		} else if (t->data == "==") {
			t->type = Operator;
			cur_type = Operator;
		} else if (t->data == "!=") {
			t->type = Operator;
			cur_type = Operator;
		} else if (cur_type == VarDecl && t->data.find("$") != string::npos) {
			t->type = LocalVar;
			vardecl_list.push_back(t->data);
			cur_type = LocalVar;
		} else if (t->data == "=") {
			t->type = Assign;
			cur_type = Assign;
		} else if (t->data == ";") {
			t->type = SemiColon;
			cur_type = SemiColon;
		} else if (t->data == ",") {
			t->type = Comma;
		} else if (t->data == "(") {
			t->type = LeftParenthesis;
		} else if (t->data == ")") {
			t->type = RightParenthesis;
		} else if (t->data == "{") {
			t->type = LeftBrace;
		} else if (t->data == "}") {
			t->type = RightBrace;
		} else if (t->data == "print") {
			t->type = PrintDecl;
			cur_type = PrintDecl;
		} else if (t->data == "if") {
			t->type = IfStmt;
			cur_type = IfStmt;
		} else if (t->data == "else") {
			t->type = ElseStmt;
			cur_type = ElseStmt;
		} else if (t->data == "sub") {
			t->type = FunctionDecl;
			cur_type = FunctionDecl;
		} else if (t->data == "shift") {
			t->type = Shift;
			cur_type = Shift;
		} else if (t->data == "return") {
			t->type = Return;
			cur_type = Return;
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
