#include <gperl.hpp>
#include <cassert>
#include "gen_token_decl.cpp"
using namespace std;

GPerlToken::GPerlToken(string data_, int idx_) :
    data(data_), idx(idx_), indent(0), token_num(0), total_token_num(0)
{
	type = Undefined;
    stype = Value;
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
		} else if (token[0] == '%' && symbol == '{') {
			tks->push_back(new GPerlToken("%{"));
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
			   (symbol == '-' && next_ch == '-') ||
			   (symbol == '-' && next_ch == '>') ||
               (symbol == '&' && next_ch == '&') ||
               (symbol == '|' && next_ch == '|')) {
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
		} else if (token[0] == '%' && symbol == '{') {
			tks->push_back(new GPerlToken("%{"));
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
			} else if (i + 1 < script_size && script[i + 1] == '@') {
				tokens->push_back(new GPerlToken(string("#@")));
				i++;
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
			//case '*': case '/' :
			//if (isSKIP()) break;
			//scanMDOperator(tokens, script[i]);
			//break;
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
		case '<': case '>': case '&': case '|':
        case '.': case '!': case '*': case '/':
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
			if (token_idx > 0) {
				tokens->push_back(new GPerlToken(token));
				memset(token, 0, max_token_size);
				token_idx = 0;
			}
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
			if (token_idx == 0 || (token_idx == 1 && token[0] == '-')) {
				GPerlToken *tk = parseNumber(this, script, i);
				if (token[0] == '-') tk->data = "-" + tk->data;
				tokens->push_back(tk);
				memset(token, 0, max_token_size);
				token_idx = 0;
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

bool GPerlTokenizer::isReservedKeyword(std::string word)
{
	for (int i = 0; decl_tokens[i].type != Undefined; i++) {
		if (word == decl_tokens[i].data) {
			return true;
		}
	}
	return false;
}

void GPerlTokenizer::annotateTokens(vector<GPerlToken *> *tokens)
{
	vector<GPerlToken *>::iterator it = tokens->begin();
	vector<string> vardecl_list;
	vector<string> funcdecl_list;
	vector<string> pkgdecl_list;
	int cur_type = 0;
	while (it != tokens->end()) {
		GPerlToken *t = (GPerlToken *)*it;
		string data = t->data;
		if (t->info.type == String) {
			cur_type = 0;
			it++;
			continue;
		}
		//DBG_PL("TOKEN = [%s]", cstr(data));
		if (isReservedKeyword(data)) {
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
		} else if (cur_type == Package) {
			t->info = getTokenInfo("Class", NULL);
			pkgdecl_list.push_back(t->data);
		} else if (search(pkgdecl_list, t->data)) {
			t->info = getTokenInfo("Class", NULL);
		} else {
			t->info = getTokenInfo("Key", NULL);
			cur_type = Key;
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

typedef vector<GPerlToken *>::iterator GPerlTokenPos;

GPerlToken::GPerlToken(vector<GPerlToken *> *tokens) : stype(Value)
{
    size_t size = tokens->size();
    GPerlTokenPos pos = tokens->begin();
    this->tks = (GPerlToken **)safe_malloc(size * PTR_SIZE);
    this->token_num = size;
    for (size_t i = 0; i < size; i++) {
        GPerlToken *t = (GPerlToken *)*pos;
        this->tks[i] = t;
        this->total_token_num += t->total_token_num > 1 ? t->total_token_num : 1;
        pos++;
    }
}

void GPerlTokenizer::prepare(vector<GPerlToken *> *tokens)
{
    pos = tokens->begin();
}

GPerlToken *GPerlTokenizer::parseSyntax(GPerlToken *start_token, vector<GPerlToken *> *tokens)
{
    GPerlT prev_type = Undefined;
    GPerlTokenPos end_pos = tokens->end();
    vector<GPerlToken *> *new_tokens = new vector<GPerlToken *>();
    GPerlTokenPos intermediate_pos = pos;
    if (start_token) {
        new_tokens->push_back(start_token);
        intermediate_pos--;
    }
    while (pos != end_pos) {
        GPerlToken *t = (GPerlToken *)(*pos);
        GPerlT type = t->info.type;
        switch (type) {
        case LeftBracket: {
            pos++;
            GPerlToken *syntax = parseSyntax(t, tokens);
            syntax->stype = Expr;
            new_tokens->push_back(syntax);
            break;
        }
        case LeftParenthesis: {
            pos++;
            GPerlToken *syntax = parseSyntax(t, tokens);
            syntax->stype = Expr;
            new_tokens->push_back(syntax);
            break;
        }
        case LeftBrace: {
            pos++;
            GPerlToken *syntax = parseSyntax(t, tokens);
            if (prev_type == Pointer) {
                syntax->stype = Expr;
            } else {
                syntax->stype = BlockStmt;
            }
            new_tokens->push_back(syntax);
            intermediate_pos = pos;
            break;
        }
        case RightBracket: case RightBrace: case RightParenthesis:
            new_tokens->push_back(t);
            return new GPerlToken(new_tokens);
            break;
        case SemiColon: {
            size_t k = pos - intermediate_pos;
            //DBG_PL("stmt_token_num = [%lu]", k);
            //DBG_PL("pos = [%s], intermediate_pos = [%s]", (*pos)->info.name, (*intermediate_pos)->info.name);
            //DBG_PL("new_tokens_num = [%d]", new_tokens->size());
            vector<GPerlToken *> *stmt = new vector<GPerlToken *>();
            for (size_t j = 0; j < k - 1; j++) {
                GPerlToken *tk = new_tokens->back();
                //DBG_PL("stype = [%d], total_token_num = [%d]", tk->stype, tk->total_token_num);
                j += (tk->total_token_num > 0) ? tk->total_token_num - 1 : 0;
                stmt->insert(stmt->begin(), tk);
                new_tokens->pop_back();
            }
            stmt->push_back(t);
            //DBG_PL("last_token = [%s]", new_tokens->back()->info.name);
            GPerlToken *stmt_ = new GPerlToken(stmt);
            stmt_->stype = Stmt;
            new_tokens->push_back(stmt_);
            intermediate_pos = pos;
            break;
        }
        default:
            new_tokens->push_back(t);
            break;
        }
        prev_type = type;
        pos++;
    }
    return new GPerlToken(new_tokens);
}

void GPerlTokenizer::dumpSyntax(GPerlToken *syntax, int indent)
{
    size_t tk_n = syntax->token_num;
    for (size_t i = 0; i < tk_n; i++) {
        GPerlToken *tk = syntax->tks[i];
        for (int j = 0; j < indent; j++) {
            DBG_P("----------------");
        }
        switch (tk->stype) {
        case Expr:
            DBG_PL("Expr |");
            indent++;
            dumpSyntax(tk, indent);
            indent--;
            break;
        case Stmt:
            DBG_PL("Stmt |");
            indent++;
            dumpSyntax(tk, indent);
            indent--;
            break;
        case BlockStmt:
            DBG_PL("BlockStmt |");
            indent++;
            dumpSyntax(tk, indent);
            indent--;
            break;
        default:
            DBG_PL("%-12s", syntax->tks[i]->info.name);
            break;
        }
    }
}

void GPerlTokenizer::insertParenthesis(vector<GPerlToken *> *tokens)
{
	int pcount = 0;
	vector<GPerlToken *>::iterator it = tokens->begin();
	while (it != tokens->end()) {
		GPerlToken *t = (GPerlToken *)*it;
		DBG_PL("=====(%s)=====", ((GPerlToken *)*it)->info.name);
		switch (t->info.type) {
		case Call: case CodeVar: case BuiltinFunc: {
			GPerlToken *next_token = (GPerlToken *)*(it+1);
			if (next_token->info.type != LeftParenthesis) {
				GPerlToken *token = new GPerlToken("(");
				token->info = getTokenInfo(NULL, "(");
				tokens->insert(it+1, token);
				it++;
				pcount++;
				if (next_token->info.type == Comma) {
					token = new GPerlToken(")");
					token->info = getTokenInfo(NULL, ")");
					DBG_PL("*****(%s)******", ((GPerlToken *)*it+2)->info.name);
					tokens->insert(it+1, token);
					pcount--;
					it++;
				}
			}
			break;
		}
		case SemiColon: case LeftBrace: /* TODO: map, grep */
			for (; pcount > 0; pcount--) {
				GPerlToken *token = new GPerlToken(")");
				token->info = getTokenInfo(NULL, ")");
				DBG_PL("******(%s)******", ((GPerlToken *)*it)->info.name);
				tokens->insert(it, token);
				it++;
			}
			break;
		default:
			break;
		}
		it++;
	}
	{
		vector<GPerlToken *>::iterator debug_it = tokens->begin();
		while (debug_it != tokens->end()) {
			DBG_PL("(%s)", ((GPerlToken *)*debug_it)->info.name);
			debug_it++;
		}
	}
	pcount = 0;
	it = tokens->begin();
	bool insertFlag = false;
	int func_pcount = 0;
	while (it != tokens->end()) {
		GPerlToken *t = (GPerlToken *)*it;
		if (func_pcount > 0) {
			if (t->info.type == LeftParenthesis) {
				func_pcount++;
			} else if (t->info.type == RightParenthesis) {
				func_pcount--;
				if (func_pcount == 0) {
					GPerlToken *token = new GPerlToken(")");
					token->info = getTokenInfo(NULL, ")");
					tokens->insert(it+1, token);
				}
			}
		}
		switch (t->info.type) {
		case IsNot: {
			GPerlToken *next_token = (GPerlToken *)*(it+1);
			if (next_token->info.type != LeftParenthesis) {
				GPerlToken *token = new GPerlToken("(");
				token->info = getTokenInfo(NULL, "(");
				tokens->insert(it+1, token);
				if (next_token->info.kind == GPerlKind::Function) {
					func_pcount++;
					it+=3;
					insertFlag = true;
				} else {
					token = new GPerlToken(")");
					token->info = getTokenInfo(NULL, ")");
					tokens->insert(it+3, token);
				}
			}
			break;
		}
		default:
			break;
		}
		it++;
	}
	vector<GPerlToken *>::iterator debug_it = tokens->begin();
	while (debug_it != tokens->end()) {
		DBG_PL("(%s)", ((GPerlToken *)*debug_it)->info.name);
		debug_it++;
	}
}
