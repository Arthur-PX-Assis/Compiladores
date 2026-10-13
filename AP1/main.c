#include <stdio.h>
#include <stdlib.h>
#include "csmall.h"

int main(int argc, char **argv){
    // Checa se passou o arquivo no terminal
    if (argc != 2){
        fprintf(stderr, "Uso: %s arquivo.csmall\n", argv[0]);
        return EXIT_FAILURE;
    }

    CodigoFonte codigo;
    if (!abrirCodigoFonte(&codigo, argv[1])) return EXIT_FAILURE; // argv[1] é o nome da entrada

    Parser parser;
    parser.codigo = &codigo;

    if (analisarPrograma(&parser)) printf("Compilação concluída com sucesso! Nenhum erro encontrado.\n");
    else printf("A compilação falhou por conta de erros no código-fonte.\n");

    fecharCodigoFonte(&codigo);
    return EXIT_SUCCESS;
}