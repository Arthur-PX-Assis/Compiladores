#ifndef CSMALL_H
#define CSMALL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TAM_LEXEMA 128

typedef enum{
    TOK_INT, TOK_FLOAT, TOK_IF, TOK_ELSE, TOK_WHILE, TOK_PRINT,
    TOK_IDENTIFIER, TOK_INT_CONST, TOK_FLOAT_CONST,
    TOK_PLUS, TOK_MINUS, TOK_MULT, TOK_DIV,
    TOK_ASSIGN, TOK_LT, TOK_LE, TOK_GT, TOK_GE, TOK_EQ, TOK_NE,
    TOK_LPAREN, TOK_RPAREN, TOK_LBRACE, TOK_RBRACE, TOK_SEMICOLON,
    TOK_EOF, TOK_INVALID
}TokenType;

typedef struct{
    TokenType tipo;
    char lexema[TAM_LEXEMA];
    int linha;
}Token;

typedef struct{
    FILE *arquivo;
    int linha;
}CodigoFonte;

typedef enum{
    TIPO_INT,
    TIPO_FLOAT,
    TIPO_ERRO
}TipoDado;

typedef struct EntradaSimbolo{
    char lexema[TAM_LEXEMA];
    TipoDado tipo;
    int linha;
    struct EntradaSimbolo *prox;
}EntradaSimbolo;

typedef struct TabelaSimbolos{
    EntradaSimbolo *entradas;
    struct TabelaSimbolos *anterior;
}TabelaSimbolos;

typedef struct{
    CodigoFonte *codigo;
    Token tokenAtual;
    TabelaSimbolos *tabelaAtual;
    int temErro;
}Parser;

// Arquivo e lookahead
int abrirCodigoFonte(CodigoFonte *codigo, const char *nomeArquivo);
int lerCaractere(CodigoFonte *codigo);
int olharCaractere(CodigoFonte *codigo);
void fecharCodigoFonte(CodigoFonte *codigo);
void devolverCaractere(CodigoFonte *codigo, int caractere);

// Tokens
void imprimirToken(Token token);
const char *nomeToken(TokenType tipo);

// Erros
void erroLexico(int linha);
void erroSintatico(int linha, TokenType esperado, TokenType encontrado);
void erroSemantico(int linha, const char *mensagem);

// Lexer
Token proximoToken(CodigoFonte *codigo);

// Tabela de símbolos
int inserirSimbolo(TabelaSimbolos *tabela, const char *lexema, TipoDado tipo, int linha);
TabelaSimbolos *criarTabela(TabelaSimbolos *anterior);
TabelaSimbolos *destruirTabela(TabelaSimbolos *tabela);
EntradaSimbolo *buscarSimbolo(TabelaSimbolos *tabela, const char *lexema);

// Parser e análise semântica
int analisarPrograma(Parser *parser);
void avancarEntrada(Parser *parser);
void casar(Parser *parser, TokenType esperado);
void Programa(Parser *parser);
void ListaDecl(Parser *parser);
void Declaracao(Parser *parser);
void ListaCmd(Parser *parser);
void Comando(Parser *parser);
void Atribuicao(Parser *parser);
void Print(Parser *parser);
void If(Parser *parser);
void While(Parser *parser);
void Bloco(Parser *parser);
void Condicao(Parser *parser);
TipoDado Tipo(Parser *parser);
TipoDado Expressao(Parser *parser);
TipoDado ExprLinha(Parser *parser, TipoDado tipoEsq);
TipoDado Termo(Parser *parser);
TipoDado TermoLinha(Parser *parser, TipoDado tipoEsq);
TipoDado Fator(Parser *parser);

#endif