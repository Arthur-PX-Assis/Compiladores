#include "csmall.h"

const char *nomeToken(TokenType tipo){
    switch (tipo){
        case TOK_INT: return "INT";
        case TOK_FLOAT: return "FLOAT";
        case TOK_IF: return "IF";
        case TOK_ELSE: return "ELSE";
        case TOK_WHILE: return "WHILE";
        case TOK_PRINT: return "PRINT";
        case TOK_IDENTIFIER: return "IDENTIFIER";
        case TOK_INT_CONST: return "INT_CONST";
        case TOK_FLOAT_CONST: return "FLOAT_CONST";
        case TOK_PLUS: return "PLUS";
        case TOK_MINUS: return "MINUS";
        case TOK_MULT: return "MULT";
        case TOK_DIV: return "DIV";
        case TOK_ASSIGN: return "ASSIGN";
        case TOK_LT: return "LT";
        case TOK_LE: return "LE";
        case TOK_GT: return "GT";
        case TOK_GE: return "GE";
        case TOK_EQ: return "EQ";
        case TOK_NE: return "NE";
        case TOK_LPAREN: return "LPAREN";
        case TOK_RPAREN: return "RPAREN";
        case TOK_LBRACE: return "LBRACE";
        case TOK_RBRACE: return "RBRACE";
        case TOK_SEMICOLON: return "SEMICOLON";
        case TOK_EOF: return "EOF";
        case TOK_INVALID: return "INVALID";
        default: return "UNKNOWN";
    }
}

void imprimirToken(Token token){
    printf("%-14s %-20s linha %d\n", nomeToken(token.tipo), token.lexema, token.linha);
}

int abrirCodigoFonte(CodigoFonte *codigo, const char *nomeArquivo){
    codigo->arquivo = fopen(nomeArquivo, "r");
    codigo->linha = 1;

    if (codigo->arquivo == NULL){
        fprintf(stderr, "Não foi possível abrir o arquivo: %s\n", nomeArquivo);
        return 0;
    }
    return 1;
}

void fecharCodigoFonte(CodigoFonte *codigo){
    if (codigo->arquivo != NULL){
        fclose(codigo->arquivo);
        codigo->arquivo = NULL;
    }
}

int lerCaractere(CodigoFonte *codigo){
    int caractere = fgetc(codigo->arquivo);
    if (caractere == '\n') codigo->linha++;
    return caractere;
}

void devolverCaractere(CodigoFonte *codigo, int caractere){
    if (caractere == EOF) return;
    ungetc(caractere, codigo->arquivo);
    if (caractere == '\n') codigo->linha--;
}

int olharCaractere(CodigoFonte *codigo){
    int caractere = lerCaractere(codigo);
    devolverCaractere(codigo, caractere);
    return caractere;
}

void erroLexico(int linha){
    fprintf(stderr, "Erro léxico na linha %d.\n", linha);
}

void erroSintatico(int linha, TokenType esperado, TokenType encontrado){
    fprintf(stderr, "Erro sintático na linha %d: esperado %s, encontrado %s.\n",
            linha, nomeToken(esperado), nomeToken(encontrado));
}

void erroSemantico(int linha, const char *mensagem){
    fprintf(stderr, "Erro semântico na linha %d: %s\n", linha, mensagem);
}

Token proximoToken(CodigoFonte *codigo){
    Token token;
    token.lexema[0] = '\0';

    while (1){
        int c = lerCaractere(codigo);

        if (c == EOF){
            token.tipo = TOK_EOF;
            strcpy(token.lexema, "EOF");
            token.linha = codigo->linha;
            return token;
        }

        if (isspace(c)) continue;

        token.linha = codigo->linha;

        // Comentários e divisão
        if (c == '/'){
            int prox = olharCaractere(codigo);
            if (prox == '/'){
                lerCaractere(codigo);
                while (1){
                    int cc = lerCaractere(codigo);
                    if (cc == '\n' || cc == EOF) break;
                }
                continue;
            }else{
                token.tipo = TOK_DIV;
                strcpy(token.lexema, "/");
                return token;
            }
        }

        // Identificadores e palavras reservadas
        if (isalpha(c) || c == '_'){
            int pos = 0;
            token.lexema[pos++] = (char)c;

            while (isalnum(olharCaractere(codigo)) || olharCaractere(codigo) == '_'){
                if (pos < TAM_LEXEMA - 1) token.lexema[pos++] = (char)lerCaractere(codigo);
                else lerCaractere(codigo);
            }

            token.lexema[pos] = '\0';

            if (strcmp(token.lexema, "int") == 0) token.tipo = TOK_INT;
            else if (strcmp(token.lexema, "float") == 0) token.tipo = TOK_FLOAT;
            else if (strcmp(token.lexema, "if") == 0) token.tipo = TOK_IF;
            else if (strcmp(token.lexema, "else") == 0) token.tipo = TOK_ELSE;
            else if (strcmp(token.lexema, "while") == 0) token.tipo = TOK_WHILE;
            else if (strcmp(token.lexema, "print") == 0) token.tipo = TOK_PRINT;
            else token.tipo = TOK_IDENTIFIER;

            return token;
        }

        // Constantes numéricas
        if (isdigit(c)){
            int pos = 0;
            token.lexema[pos++] = (char)c;
            int ehFloat = 0;

            while (isdigit(olharCaractere(codigo)) || (olharCaractere(codigo) == '.' && !ehFloat)){
                int prox = lerCaractere(codigo);
                if (prox == '.') ehFloat = 1;
                if (pos < TAM_LEXEMA - 1) token.lexema[pos++] = (char)prox;
            }

            token.lexema[pos] = '\0';
            token.tipo = ehFloat ? TOK_FLOAT_CONST : TOK_INT_CONST;
            return token;
        }

        // Operadores e delimitadores
        switch (c){
            case '+':
                token.tipo = TOK_PLUS;
                strcpy(token.lexema, "+");
                return token;
            
            case '-':
                token.tipo = TOK_MINUS;
                strcpy(token.lexema, "-");
                return token;
            
            case '*':
                token.tipo = TOK_MULT;
                strcpy(token.lexema, "*");
                return token;
            
            case '=':
                if (olharCaractere(codigo) == '='){
                    lerCaractere(codigo);token.tipo = TOK_EQ;
                    strcpy(token.lexema, "==");
                }else{
                    token.tipo = TOK_ASSIGN;
                    strcpy(token.lexema, "=");
                }
                return token;
            
            case '<':
                if (olharCaractere(codigo) == '='){
                    lerCaractere(codigo);
                    token.tipo = TOK_LE;
                    strcpy(token.lexema, "<=");
                }else{
                    token.tipo = TOK_LT;
                    strcpy(token.lexema, "<");
                }
                return token;
            
            case '>':
                if (olharCaractere(codigo) == '='){
                    lerCaractere(codigo);
                    token.tipo = TOK_GE;
                    strcpy(token.lexema, ">=");
                }else{
                    token.tipo = TOK_GT;
                    strcpy(token.lexema, ">");
                }
                return token;
            
            case '!':
                if (olharCaractere(codigo) == '='){
                    lerCaractere(codigo);
                    token.tipo = TOK_NE;
                    strcpy(token.lexema, "!=");
                    return token;
                }
                break;
            
            case '(':
                token.tipo = TOK_LPAREN;
                strcpy(token.lexema, "(");
                return token;
            
            case ')':
                token.tipo = TOK_RPAREN;
                strcpy(token.lexema, ")");
                return token;
            
            case '{':
                token.tipo = TOK_LBRACE;
                strcpy(token.lexema, "{");
                return token;
            
            case '}':
                token.tipo = TOK_RBRACE;
                strcpy(token.lexema, "}");
                return token;
            
            case ';':
                token.tipo = TOK_SEMICOLON;
                strcpy(token.lexema, ";");
                return token;
        }

        erroLexico(token.linha);
        token.tipo = TOK_INVALID;
        token.lexema[0] = (char)c;
        token.lexema[1] = '\0';
        return token;
    }
}

void avancarEntrada(Parser *parser){
    parser->tokenAtual = proximoToken(parser->codigo);
}

void casar(Parser *parser, TokenType esperado){
    if (parser->tokenAtual.tipo == esperado) avancarEntrada(parser);
    else{
        erroSintatico(parser->tokenAtual.linha, esperado, parser->tokenAtual.tipo);
        parser->temErro = 1;
    }
}

TabelaSimbolos *criarTabela(TabelaSimbolos *anterior){
    TabelaSimbolos *tabela = (TabelaSimbolos *)malloc(sizeof(TabelaSimbolos));
    tabela->entradas = NULL;
    tabela->anterior = anterior;
    return tabela;
}

TabelaSimbolos *destruirTabela(TabelaSimbolos *tabela){
    TabelaSimbolos *anterior = tabela->anterior;
    EntradaSimbolo *atual = tabela->entradas;

    while (atual != NULL){
        EntradaSimbolo *temp = atual;
        atual = atual->prox;
        free(temp);
    }

    free(tabela);
    return anterior;
}

int inserirSimbolo(TabelaSimbolos *tabela, const char *lexema, TipoDado tipo, int linha){
    EntradaSimbolo *atual = tabela->entradas;

    while (atual != NULL){
        if (strcmp(atual->lexema, lexema) == 0) return 0;
        atual = atual->prox;
    }

    EntradaSimbolo *novo = (EntradaSimbolo *)malloc(sizeof(EntradaSimbolo));
    strcpy(novo->lexema, lexema);
    novo->tipo = tipo;
    novo->linha = linha;
    novo->prox = tabela->entradas;
    tabela->entradas = novo;
    return 1;
}

EntradaSimbolo *buscarSimbolo(TabelaSimbolos *tabela, const char *lexema){
    TabelaSimbolos *escopoAtual = tabela;
    while (escopoAtual != NULL){
        EntradaSimbolo *e = escopoAtual->entradas;

        while (e != NULL){
            if (strcmp(e->lexema, lexema) == 0) return e;
            e = e->prox;
        }

        escopoAtual = escopoAtual->anterior;
    }
    return NULL;
}

void Programa(Parser *parser){
    ListaDecl(parser);
    ListaCmd(parser);
}

void ListaDecl(Parser *parser){
    while (parser->tokenAtual.tipo == TOK_INT || parser->tokenAtual.tipo == TOK_FLOAT) Declaracao(parser);
}

void Declaracao(Parser *parser){
    int linhaDef = parser->tokenAtual.linha;
    TipoDado t = Tipo(parser);
    
    if (parser->tokenAtual.tipo == TOK_IDENTIFIER){
        char id[TAM_LEXEMA];
        strcpy(id, parser->tokenAtual.lexema);
        casar(parser, TOK_IDENTIFIER);

        if (!inserirSimbolo(parser->tabelaAtual, id, t, linhaDef)){
            char msg[256];
            snprintf(msg, sizeof(msg), "Variável '%s' já declarada neste escopo.", id);
            erroSemantico(linhaDef, msg);
            parser->temErro = 1;
        }

        casar(parser, TOK_SEMICOLON);
    }else{
        erroSintatico(parser->tokenAtual.linha, TOK_IDENTIFIER, parser->tokenAtual.tipo);
        parser->temErro = 1;
    }
}

TipoDado Tipo(Parser *parser){
    if (parser->tokenAtual.tipo == TOK_INT){
        casar(parser, TOK_INT);
        return TIPO_INT;
    }else if (parser->tokenAtual.tipo == TOK_FLOAT){
        casar(parser, TOK_FLOAT);
        return TIPO_FLOAT;
    }else{
        erroSintatico(parser->tokenAtual.linha, TOK_INT, parser->tokenAtual.tipo);
        parser->temErro = 1;
        return TIPO_ERRO;
    }
}

void ListaCmd(Parser *parser){
    while (parser->tokenAtual.tipo == TOK_IDENTIFIER ||
           parser->tokenAtual.tipo == TOK_PRINT ||
           parser->tokenAtual.tipo == TOK_IF ||
           parser->tokenAtual.tipo == TOK_WHILE ||
           parser->tokenAtual.tipo == TOK_LBRACE) Comando(parser);
}

void Comando(Parser *parser){
    switch (parser->tokenAtual.tipo){
        case TOK_IDENTIFIER: 
            Atribuicao(parser);
            break;
        case TOK_PRINT: 
            Print(parser);
            break;
        case TOK_IF: 
            If(parser);
            break;
        case TOK_WHILE: 
            While(parser);
            break;
        case TOK_LBRACE: 
            Bloco(parser);
            break;
        default:
            erroSintatico(parser->tokenAtual.linha, TOK_IDENTIFIER, parser->tokenAtual.tipo);
            parser->temErro = 1;
            break;
    }
}

void Atribuicao(Parser *parser){
    int linhaAttr = parser->tokenAtual.linha;
    char id[TAM_LEXEMA];
    strcpy(id, parser->tokenAtual.lexema);
    casar(parser, TOK_IDENTIFIER);

    EntradaSimbolo *sym = buscarSimbolo(parser->tabelaAtual, id);
    TipoDado tipoVar = TIPO_ERRO;

    if (sym == NULL){
        char msg[256];
        snprintf(msg, sizeof(msg), "Variável '%s' não declarada.", id);
        erroSemantico(linhaAttr, msg);
        parser->temErro = 1;
    }else tipoVar = sym->tipo;

    casar(parser, TOK_ASSIGN);
    TipoDado tipoExpr = Expressao(parser);
    casar(parser, TOK_SEMICOLON);

    if (tipoVar != TIPO_ERRO && tipoExpr != TIPO_ERRO)
        if (tipoVar != tipoExpr){
            char msg[256];
            snprintf(msg, sizeof(msg), "Incompatibilidade de tipos na atribuição para '%s'.", id);
            erroSemantico(linhaAttr, msg);
            parser->temErro = 1;
        }
}

void Print(Parser *parser){
    casar(parser, TOK_PRINT);
    casar(parser, TOK_LPAREN);
    Expressao(parser);
    casar(parser, TOK_RPAREN);
    casar(parser, TOK_SEMICOLON);
}

void If(Parser *parser){
    casar(parser, TOK_IF);
    casar(parser, TOK_LPAREN);
    Condicao(parser);
    casar(parser, TOK_RPAREN);
    Bloco(parser);

    if (parser->tokenAtual.tipo == TOK_ELSE){
        casar(parser, TOK_ELSE);
        Bloco(parser);
    }
}

void While(Parser *parser){
    casar(parser, TOK_WHILE);
    casar(parser, TOK_LPAREN);
    Condicao(parser);
    casar(parser, TOK_RPAREN);
    Bloco(parser);
}

void Bloco(Parser *parser){
    casar(parser, TOK_LBRACE);
    parser->tabelaAtual = criarTabela(parser->tabelaAtual);

    ListaDecl(parser);
    ListaCmd(parser);

    casar(parser, TOK_RBRACE);
    parser->tabelaAtual = destruirTabela(parser->tabelaAtual);
}

void Condicao(Parser *parser){
    int linhaCond = parser->tokenAtual.linha;
    TipoDado t1 = Expressao(parser);

    if (parser->tokenAtual.tipo == TOK_LT || parser->tokenAtual.tipo == TOK_LE ||
        parser->tokenAtual.tipo == TOK_GT || parser->tokenAtual.tipo == TOK_GE ||
        parser->tokenAtual.tipo == TOK_EQ || parser->tokenAtual.tipo == TOK_NE){
        
        avancarEntrada(parser);
        TipoDado t2 = Expressao(parser);

        if (t1 == TIPO_ERRO || t2 == TIPO_ERRO) parser->temErro = 1;
    }else{
        erroSemantico(linhaCond, "Condição inválida ou sem operador relacional.");
        parser->temErro = 1;
    }
}

TipoDado Expressao(Parser *parser){
    TipoDado tTermo = Termo(parser);
    return ExprLinha(parser, tTermo);
}

TipoDado ExprLinha(Parser *parser, TipoDado tipoEsq){
    if (parser->tokenAtual.tipo == TOK_PLUS || parser->tokenAtual.tipo == TOK_MINUS){
        avancarEntrada(parser);
        TipoDado tDir = Termo(parser);
        TipoDado tipoRes;

        if (tipoEsq == TIPO_ERRO || tDir == TIPO_ERRO) tipoRes = TIPO_ERRO;
        else if (tipoEsq == TIPO_FLOAT || tDir == TIPO_FLOAT) tipoRes = TIPO_FLOAT;
        else tipoRes = TIPO_INT;
        return ExprLinha(parser, tipoRes);
    }
    return tipoEsq;
}

TipoDado Termo(Parser *parser){
    TipoDado tFator = Fator(parser);
    return TermoLinha(parser, tFator);
}

TipoDado TermoLinha(Parser *parser, TipoDado tipoEsq){
    if (parser->tokenAtual.tipo == TOK_MULT || parser->tokenAtual.tipo == TOK_DIV){
        avancarEntrada(parser);
        TipoDado tDir = Fator(parser);
        TipoDado tipoRes;

        if (tipoEsq == TIPO_ERRO || tDir == TIPO_ERRO) tipoRes = TIPO_ERRO;
        else if (tipoEsq == TIPO_FLOAT || tDir == TIPO_FLOAT) tipoRes = TIPO_FLOAT;
        else tipoRes = TIPO_INT;
        return TermoLinha(parser, tipoRes);
    }
    return tipoEsq;
}

TipoDado Fator(Parser *parser){
    int linhaFator = parser->tokenAtual.linha;

    if (parser->tokenAtual.tipo == TOK_IDENTIFIER){
        char id[TAM_LEXEMA];
        strcpy(id, parser->tokenAtual.lexema);
        casar(parser, TOK_IDENTIFIER);

        EntradaSimbolo *sym = buscarSimbolo(parser->tabelaAtual, id);
        
        if (sym == NULL){
            char msg[256];
            snprintf(msg, sizeof(msg), "Variável '%s' não declarada.", id);
            erroSemantico(linhaFator, msg);
            parser->temErro = 1;
            return TIPO_ERRO;
        }
        return sym->tipo;
    }else if (parser->tokenAtual.tipo == TOK_INT_CONST){
        casar(parser, TOK_INT_CONST);
        return TIPO_INT;
    }else if (parser->tokenAtual.tipo == TOK_FLOAT_CONST){
        casar(parser, TOK_FLOAT_CONST);
        return TIPO_FLOAT;
    }else if (parser->tokenAtual.tipo == TOK_LPAREN){
        casar(parser, TOK_LPAREN);
        TipoDado t = Expressao(parser);
        casar(parser, TOK_RPAREN);
        return t;
    }else{
        erroSintatico(linhaFator, TOK_IDENTIFIER, parser->tokenAtual.tipo);
        parser->temErro = 1;
        return TIPO_ERRO;
    }
}

int analisarPrograma(Parser *parser){
    parser->tabelaAtual = criarTabela(NULL);
    parser->temErro = 0;

    avancarEntrada(parser);
    Programa(parser);

    if (parser->tokenAtual.tipo != TOK_EOF && !parser->temErro){
        erroSintatico(parser->tokenAtual.linha, TOK_EOF, parser->tokenAtual.tipo);
        parser->temErro = 1;
    }

    parser->tabelaAtual = destruirTabela(parser->tabelaAtual);
    return !parser->temErro;
}