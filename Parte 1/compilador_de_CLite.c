/*
Eric Akio Uchiyamada    RA: 10395287
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
//##################### INICIO DEFINICOES mini Analisador Lexico ##########################

//tokens
typedef enum {
    ERRO,
    IDENTIFICADOR,
    NUMERO,
    OP_SOMA,
    OP_MULT,
    OP_SUBT,
    OP_DIV,
    BOOL,
    ELSE,
    FALSE,
    IF,
    INT,
    MAIN,
    PRINTF,
    SCANF,
    TRUE,
    VOID,
    WHILE,
    MENOR,
    MENOR_IGUAL,
    IGUAL,
    DIFERENTE,
    MAIOR,
    MAIOR_IGUAL,
    OU,
    EH,
    VIRGULA,
    PONTO_VIRGULA,
    ABRE_CHAVE,
    FECHA_CHAVE,
    ABRE_PAR,
    FECHA_PAR,
    ATRIBUICAO,
    COMENTARIO_LINHA,
    INICIO_COMENTARIO_BLOCO,
    FIM_COMENTARIO_BLOCO,
    EOS
}TAtomo;

typedef struct{
    TAtomo atomo;
    int linha;
    int atributo_numero;
    char atributo_ID[16];
} TInfoAtomo;

//linha = informação global
int linha = 1;

//booleano para ver se esta em comentario ou nao
int comentario_linha = 0, comentario_bloco = 0;

char *buffer = NULL;

TInfoAtomo reconhece_id();
TInfoAtomo reconhece_num();
TInfoAtomo reconhece_palavra_reservada();
TInfoAtomo reconhece_op_relacional();
TInfoAtomo reconhece_igual();
TInfoAtomo reconhece_barra();
TInfoAtomo reconhece_aritmetico();
TInfoAtomo reconhece_pontuacao();
TInfoAtomo reconhece_op_logica();
TInfoAtomo reconhece_chave_par();
TInfoAtomo obter_atomo(void);

void erro_lexico(char* msg){
    printf("Erro lexico: na linha %d: %s\n",linha,msg);
    exit(1);
}
//##################### FIM DEFINICOES mini Analisador Lexico ##########################

//##################### INICIO DEFINICOES Analisador Sintatico ##########################
TAtomo lookahead;
TInfoAtomo infoAtomo;
char *strAtomo[] = {"ERRO", "IDENTIFICADOR", "NUMERO", "OP_SOMA", "OP_MULT", "OP_SUBT", "OP_DIV", "BOOL",
                    "ELSE", "FALSE", "IF", "INT", "MAIN", "PRINTF", "SCANF", "TRUE",
                    "VOID", "WHILE","MENOR", "MENOR_IGUAL", "IGUAL", "DIFERENTE", "MAIOR", "MAIOR_IGUAL",
                    "OU", "E", "VIRGULA", "PONTO_VIRGULA", "ABRE_CHAVE", "FECHA_CHAVE", "ABRE_PAR",
                    "FECHA_PAR", "ATRIBUICAO", "COMENTARIO_LINHA", "INICIO_COMENTARIO_BLOCO", 
                    "FIM_COMENTARIO_BLOCO", "EOS"};

void consome(TAtomo atomo);

void programa();
void declaracoes(); void declaracao();
void tipo();
void lista_variavel();
void comandos(); void comando();
void bloco_comandos();
void atribuicao();
void comando_if();
void comando_while();
void comando_entrada();
void comando_saida();
void expressao();
void expressao_logica();
void expressao_relacional();
void op_relacional();
void expressao_adicao();
void expressao_multi();
void operando();


void erro_sintatico(char* msg){
    printf("Erro sintatico: na linha %d: %s\n",linha,msg);
    exit(1);
}
//##################### FIM DEFINICOES Analisador Sintatico ##########################
int main(int nArgs, char *ArgStr[]){

    if(nArgs < 2){
        printf("\n%s: fatal error: no input\nuse: %s input.txt\n\n", ArgStr[0], ArgStr[0]);
        exit(2);
    }

    FILE *arquivo;
    long numero_bytes;
    char *nome_arquivo = ArgStr[1];

    arquivo = fopen(nome_arquivo,"r");//abrir arquivo
    if(arquivo == NULL){
        printf("Erro ao abrir o arquivo.\n");
        exit(1);
    }
    
    fseek(arquivo, 0L, SEEK_END);//ir ate o fim do arquivo
    numero_bytes = ftell(arquivo);//obtem tamanho do arquivo

    rewind(arquivo);//retorna para o comeco do arquivo
    
    //alocacao dinamica do buffer
    buffer = (char *)calloc(numero_bytes, sizeof(char));
    if(buffer == NULL){
        printf("Erro ao alocar memoria.\n");
        exit(1);
    }

    //ler o conteudo do arquivo e atribuindo para buffer
    fread(buffer, sizeof(char), numero_bytes, arquivo);
    fclose(arquivo);


    printf("\nAnalisando: \n%s\n",buffer);
    printf("===============\n");
    infoAtomo = obter_atomo(); //obtem primeiro atomo
    lookahead = infoAtomo.atomo;
    programa();
    printf("\nAnalise sintatica finalizada com sucesso.\n");
    printf("\nfim da analise\n");
    

    //free(buffer); nao sei porque o programa quebra quando descomento o free
    return 0;
}

//##################### INICIO mini Analisador Lexico ##########################
//analisador lexico
TInfoAtomo obter_atomo(void){
    TInfoAtomo infoAtomo;
    infoAtomo.atomo = ERRO;

    //descarto caracere delimitador
    while(*buffer == '\n' || *buffer == ' ' || *buffer == '\t' || *buffer == '\r'){
        if(*buffer == '\n' && comentario_linha){ //conta as linhas
            linha++;
            comentario_linha = 0;//determina flag como false
        }
        else if(*buffer == '\n' && comentario_bloco){//conta as linhas
            linha++;
        }
        else if(*buffer == '\n'){//conta as linhas
            linha++;
        }
        
        buffer++;
    }

    
    //se termina o bloco de comentario
    if(*buffer == '*' && *(buffer+1) == '/' && comentario_bloco){
        buffer++; buffer++;
        comentario_bloco = 0; //determina flag como false
        infoAtomo.linha = linha;

        infoAtomo.atomo = FIM_COMENTARIO_BLOCO;
        printf("%d# %s\n",infoAtomo.linha,strAtomo[infoAtomo.atomo]);
        return infoAtomo;
    }

    //fim do arquivo
    if(*buffer == EOF){
        infoAtomo.atomo = EOS;
        infoAtomo.linha = linha;
        printf("%d# %s\n",infoAtomo.linha,strAtomo[infoAtomo.atomo]);
        return infoAtomo;
    }

    //caso seja uma palavra reservada
    if(islower(*buffer)){
        infoAtomo = reconhece_palavra_reservada();
        infoAtomo.linha = linha;
        if(!comentario_linha && !comentario_bloco){
            printf("%d# %s\n",infoAtomo.linha,strAtomo[infoAtomo.atomo]);
        }
        return infoAtomo;
    }

    //outros casos
    switch(*buffer){
        case '/':
            infoAtomo = reconhece_barra();
            infoAtomo.linha = linha;

            //reconhece comentario de linha
            if(infoAtomo.atomo == COMENTARIO_LINHA && !comentario_bloco){
                printf("%d# %s\n",infoAtomo.linha,strAtomo[infoAtomo.atomo]);
                comentario_linha = 1; //determina flag como true
            }

            //reconhece comentario de bloco
            else if(infoAtomo.atomo == INICIO_COMENTARIO_BLOCO){
                printf("%d# %s\n",infoAtomo.linha,strAtomo[infoAtomo.atomo]);
                comentario_bloco = 1; //determina flag como true
            }
            
            //reconhece operacao de divisao
            //nao imprime se uma das flags forem true
            else if(!comentario_linha && !comentario_bloco){
                printf("%d# %s\n",infoAtomo.linha,strAtomo[infoAtomo.atomo]);
            }
            break;

        //caso reconheca identificador/variavel
        case '_':
            infoAtomo = reconhece_id();
            infoAtomo.linha = linha;

            //nao imprime se uma das flags forem true
            if(!comentario_linha && !comentario_bloco){
                printf("%d# %s \t| %s\n",infoAtomo.linha,strAtomo[infoAtomo.atomo],infoAtomo.atributo_ID);
            }
            
            break;

        //caso reconheca operadores aritmeticos
        case '+':
        case '-':
        case '*':
            infoAtomo = reconhece_aritmetico();
            infoAtomo.linha = linha;

            //nao imprime se uma das flags forem true
            if(!comentario_linha && !comentario_bloco){
                printf("%d# %s\n",infoAtomo.linha,strAtomo[infoAtomo.atomo]);
            }
            break;
        
        //caso reconheca chaves ou parenteses
        case '(':
        case ')':
        case '{':
        case '}':
            infoAtomo = reconhece_chave_par();
            infoAtomo.linha = linha;

            //nao imprime se uma das flags forem true
            if(!comentario_linha && !comentario_bloco){
                printf("%d# %s\n",infoAtomo.linha,strAtomo[infoAtomo.atomo]);
            }
            break;

        //caso reconheca operadores relacionais
        case '<':
        case '!':
        case '>':
            infoAtomo = reconhece_op_relacional();
            infoAtomo.linha = linha;

            //nao imprime se uma das flags forem true
            if(!comentario_linha && !comentario_bloco){
                printf("%d# %s\n",infoAtomo.linha,strAtomo[infoAtomo.atomo]);
            }            
            break;

        //caso reconheca and/or
        case '&':
        case '|':
            infoAtomo = reconhece_op_logica();
            infoAtomo.linha = linha;

            //nao imprime se uma das flags forem true
            if(!comentario_linha && !comentario_bloco){
                printf("%d# %s\n",infoAtomo.linha,strAtomo[infoAtomo.atomo]);
            }            
            break;
        
        //caso reconheca atribuicao ou igualdade
        case '=':
            infoAtomo = reconhece_igual();
            infoAtomo.linha = linha;

            //nao imprime se uma das flags forem true
            if(!comentario_linha && !comentario_bloco){
                printf("%d# %s\n",infoAtomo.linha,strAtomo[infoAtomo.atomo]);
            }            
            break;
        
        //caso reconheca virgula ou ponto e virgula
        case ',':
        case ';':
            infoAtomo = reconhece_pontuacao();
            infoAtomo.linha = linha;

            //nao imprime se uma das flags forem true
            if(!comentario_linha && !comentario_bloco){
                printf("%d# %s\n",infoAtomo.linha,strAtomo[infoAtomo.atomo]);
            }            
            break;
        
        //caso reconheca numero hexadecimal
        case '0':
            infoAtomo = reconhece_num();
            infoAtomo.linha = linha;

            //nao imprime se uma das flags forem true
            if(!comentario_linha && !comentario_bloco){
                printf("%d# %s \t\t| %d\n",infoAtomo.linha,strAtomo[infoAtomo.atomo],infoAtomo.atributo_numero);
            }            
            break;

        //caso reconheca fim do arquivo
        case '\0':
            infoAtomo.atomo = EOS;
            infoAtomo.linha = linha;

            printf("%d# %s\n",infoAtomo.linha,strAtomo[infoAtomo.atomo]);
            break;
        
        //caso haja erro
        default:
            infoAtomo.atomo = ERRO;
            infoAtomo.linha = linha;

            printf("%d# %s\n",infoAtomo.linha,strAtomo[infoAtomo.atomo]);
    }


    return infoAtomo;
}

//funcao de reconhecer operadores aritmeticos
TInfoAtomo reconhece_aritmetico(){
    TInfoAtomo infoAtomo;
    switch(*buffer){
        case '+':
            infoAtomo.atomo = OP_SOMA;
            buffer++;
            break;

        case '-':
            infoAtomo.atomo = OP_SUBT;
            buffer++;
            break;
        
        case '*':
            infoAtomo.atomo = OP_MULT;
            buffer++;
            break;

        default:
            infoAtomo.atomo = ERRO;
    }
    return infoAtomo;
}

//funcao de reconhecer operacao de divisao ou comentarios
TInfoAtomo reconhece_barra(){
    TInfoAtomo infoAtomo;
    char next = *(buffer+1);

    if(next == '/'){
        buffer++; buffer++;
        infoAtomo.atomo = COMENTARIO_LINHA;
    }
    else if(next == '*'){
        buffer++; buffer++;
        infoAtomo.atomo = INICIO_COMENTARIO_BLOCO;
    }

    else{
        buffer++;
        infoAtomo.atomo = OP_DIV;
    }
    return infoAtomo;
}

//funcao de reconhecer pontuacao
TInfoAtomo reconhece_pontuacao(){
    TInfoAtomo infoAtomo;

    if(*buffer == ','){
        infoAtomo.atomo = VIRGULA;
        buffer++;
    }
    else if(*buffer == ';'){
        infoAtomo.atomo = PONTO_VIRGULA;
        buffer++;
    }
    else {
        infoAtomo.atomo = ERRO;
    }
    return infoAtomo;
}

//funcao de reconhecer operadores logicos
TInfoAtomo reconhece_op_logica(){
    TInfoAtomo infoAtomo;
    char next = *(buffer+1);

    if(*buffer == '&' && next == '&'){
        infoAtomo.atomo = EH;
        buffer++; buffer++;
    }
    else if(*buffer == '|' && next == '|'){
        infoAtomo.atomo = OU;
        buffer++; buffer++;
    }
    else {
        infoAtomo.atomo = ERRO;
    }
    return infoAtomo;
}

//funcao de reconhecer identificadores
TInfoAtomo reconhece_id(){
    TInfoAtomo infoAtomo;
    char* iniLexema = buffer;
    buffer++;
q0:
    if(islower(*buffer) || isupper(*buffer)){
        buffer++;
        goto q1;
    }
    infoAtomo.atomo = ERRO;
    return infoAtomo;

q1:
    if(islower(*buffer) || isupper(*buffer) || isdigit(*buffer)){
        buffer++;
        goto q1;
    }
    
    //verifica se tamanho da string nao passa de 15
    if(buffer-iniLexema <= 15){
        //copiar para atributo_ID iniLexema na quantidade de buffer-iniLexema
        strncpy(infoAtomo.atributo_ID,iniLexema,buffer-iniLexema);

        infoAtomo.atributo_ID[buffer-iniLexema] = '\0'; //finaliza a string p/ nao ter lixo de memoria
        infoAtomo.atomo = IDENTIFICADOR;
    }
    else{
        erro_lexico("identificador muito longo");
    }

    return infoAtomo;
}

//funcao de reconhecer palavra reservada
TInfoAtomo reconhece_palavra_reservada(){
    TInfoAtomo infoAtomo;
    char palavra_reservada[10]; int i = 0;
    char* inilexema = buffer;

    while(islower(*buffer)){//guardar a palavra em uma string separada para comparar depois
        palavra_reservada[i] = *buffer;
        buffer++; i++;
    } 
    palavra_reservada[i] = '\0';

    if(strncmp(palavra_reservada,"bool",strlen(palavra_reservada)) == 0)
        infoAtomo.atomo = BOOL;        
    
    else if(strncmp(palavra_reservada,"else",strlen(palavra_reservada)) == 0)
        infoAtomo.atomo = ELSE;
    
    else if(strncmp(palavra_reservada,"false",strlen(palavra_reservada)) == 0)
        infoAtomo.atomo = FALSE;
    
    else if(strncmp(palavra_reservada,"if",strlen(palavra_reservada)) == 0)
        infoAtomo.atomo = IF;
    
    else if(strncmp(palavra_reservada,"int",strlen(palavra_reservada)) == 0)
        infoAtomo.atomo = INT;
    
    else if(strncmp(palavra_reservada,"main",strlen(palavra_reservada)) == 0)
        infoAtomo.atomo = MAIN;
    
    else if(strncmp(palavra_reservada,"printf",strlen(palavra_reservada)) == 0)
        infoAtomo.atomo = PRINTF;
    
    else if(strncmp(palavra_reservada,"scanf",strlen(palavra_reservada)) == 0)
        infoAtomo.atomo = SCANF;
    
    else if(strncmp(palavra_reservada,"true",strlen(palavra_reservada)) == 0)
        infoAtomo.atomo = TRUE;
    
    else if(strncmp(palavra_reservada,"void",strlen(palavra_reservada)) == 0)
        infoAtomo.atomo = VOID;
    
    else if(strncmp(palavra_reservada,"while",strlen(palavra_reservada)) == 0)
        infoAtomo.atomo = WHILE;
    
    else infoAtomo.atomo = ERRO;
    return infoAtomo;

}

//funcao de reconhecer operadores relacionais
TInfoAtomo reconhece_op_relacional(){
    char next = *(buffer+1);

    if(*buffer == '<' && next == '='){
        buffer++; buffer++;
        infoAtomo.atomo = MENOR_IGUAL;
    }
    
    else if(*buffer == '!' && next == '='){
        buffer++; buffer++;
        infoAtomo.atomo = DIFERENTE;
    }
    else if(*buffer == '>' && next == '='){
        buffer++; buffer++;
        infoAtomo.atomo = MAIOR_IGUAL;
    }
    else if(*buffer == '<'){
        buffer++;
        infoAtomo.atomo = MENOR;
    }
    else if(*buffer == '>'){
        buffer++;
        infoAtomo.atomo = MAIOR;
    }
    else {
        buffer++;
        infoAtomo.atomo = ERRO;
    }

    return infoAtomo;
}

//funcao de reconhecer igualdade ou atribuicao
TInfoAtomo reconhece_igual(){
    TInfoAtomo infoAtomo;
    char next = *(buffer+1);

    if(next == '='){
        buffer++; buffer++;
        infoAtomo.atomo = IGUAL;
    }

    else{
        buffer++;
        infoAtomo.atomo = ATRIBUICAO;
    }

    return infoAtomo;
}

//funcao de reconhecer numero hexadecimal
TInfoAtomo reconhece_num(){
    TInfoAtomo infoAtomo;
    char inilexema[100] = "";
    int numeroDecimal = 0, i = 0;

    if(buffer[0] != '0' && buffer[1] != 'x'){
        infoAtomo.atomo = ERRO;
        return infoAtomo;
    }

q0:
    if(isdigit(*buffer) || *buffer == 'x' ||(*buffer >= 'A' && *buffer <= 'Z') ){
        inilexema[i] = *buffer;
        buffer++; i++;
        goto q0;
    }

    infoAtomo.atributo_numero = (int)strtol(inilexema,NULL,0);
    infoAtomo.atomo = NUMERO;
    return infoAtomo;
}

//funcao de reconhecer chaves ou parenteses
TInfoAtomo reconhece_chave_par(){
    TInfoAtomo infoAtomo;

    if(*buffer == '{'){
        infoAtomo.atomo = ABRE_CHAVE;
        buffer++;
    }
    else if(*buffer == '}'){
        infoAtomo.atomo = FECHA_CHAVE;
        buffer++;
    }
    else if(*buffer == '('){
        infoAtomo.atomo = ABRE_PAR;
        buffer++;
    }
    else if(*buffer == ')'){
        infoAtomo.atomo = FECHA_PAR;
        buffer++;
    }
    else {
        infoAtomo.atomo = ERRO;
    }
    return infoAtomo;
}


//##################### FIM mini Analisador Lexico ##########################

//##################### INICIO Analisador Sintatico ##########################

//funcao de integracao dos analisadores lexicos e sintatico
void consome(TAtomo atomo){
    
    if(lookahead == atomo){
        infoAtomo = obter_atomo(); 
        lookahead = infoAtomo.atomo;
    }

    //se estiver em comentario de linha, continua a comsumir ate reconhecer outro atomo
    else if(lookahead == COMENTARIO_LINHA){
        while(comentario_linha){
            infoAtomo = obter_atomo();
            lookahead = infoAtomo.atomo;
        }
        return;
        // infoAtomo = obter_atomo();
        // lookahead = infoAtomo.atomo;
    }

    //se estiver em comentario de bloco, continua a comsumir ate reconhecer o final do bloco
    else if(lookahead == INICIO_COMENTARIO_BLOCO){
        while(comentario_bloco){
            infoAtomo = obter_atomo();
            lookahead = infoAtomo.atomo;
        }
        //comsumir o fim_comentario_bloco
        infoAtomo = obter_atomo();
        lookahead = infoAtomo.atomo;

        //consome o token da variavel 'atomo' que foi passada
        infoAtomo = obter_atomo();
        lookahead = infoAtomo.atomo;
    }
    else{
        printf("erro sintatico: esperado [%s] encontrado [%s]\n",strAtomo[atomo],strAtomo[lookahead]);
        exit(1);
    }
} 

//inicio do programa
void programa(){
    consome(INT);
    consome(MAIN);
    consome(ABRE_PAR);
    consome(VOID);
    consome(FECHA_PAR);
    consome(ABRE_CHAVE);
    declaracoes(); //declaracoes de identificadores
    comandos(); //comandos com os identificadores
    consome(FECHA_CHAVE);
}

//funcao de declaradores
void declaracoes(){
    while(lookahead == INT || lookahead == BOOL || comentario_linha || comentario_bloco){
        
        //caso reconheca comentario de linha, continue consumindo ate obter atomo do lexico
        while(comentario_linha){
            infoAtomo = obter_atomo();
            lookahead = infoAtomo.atomo;
        }

        //caso reconheca comentario de bloco, continue consumindo are obter atomo do lexico
        while(comentario_bloco){
            infoAtomo = obter_atomo();
            lookahead = infoAtomo.atomo;
        }

        //caso reconheca fim de bloco de comentario, consome-o
        if(lookahead == FIM_COMENTARIO_BLOCO){
            infoAtomo = obter_atomo();
            lookahead = infoAtomo.atomo;
        }

        // se bloco de comentario terminar na parte de <declaracoes> e o proximo atomo seja
        // da parte dos comandos, altera para <comandos>.
        if(lookahead == PONTO_VIRGULA || lookahead == ABRE_CHAVE || lookahead == IDENTIFICADOR || lookahead == IF || 
            lookahead == WHILE || lookahead == SCANF || lookahead == PRINTF) 
            break;
        
        //se nao, entra em uma declaracao de identificador
        declaracao();
    }
}

//declaracao de um identificador
void declaracao(){
    tipo(); //reconhece o tipo do identificador
    consome(IDENTIFICADOR); //reconhece o identificador

    //se houver mais identificadores na mesma linha, consome eles tambem
    while(lookahead == VIRGULA){ 
        consome(VIRGULA);
        lista_variavel(); //os outros identificadores
    }
    consome(PONTO_VIRGULA);
}

//reconhece o tipo do identificador
void tipo(){
    switch(lookahead){
        case INT:
            consome(INT);
            break;

        case BOOL:
            consome(BOOL);
            break;
    }
}

//reconhece outros identificadores na mesma linha
void lista_variavel(){
    consome(IDENTIFICADOR);
    while(lookahead == VIRGULA){
        consome(VIRGULA);
        consome(IDENTIFICADOR);
    }
}

//funcao de comandos
void comandos(){
    
    while(lookahead == PONTO_VIRGULA || lookahead == ABRE_CHAVE || lookahead == IDENTIFICADOR || lookahead == IF || 
          lookahead == WHILE || lookahead == SCANF || lookahead == PRINTF || comentario_linha || comentario_bloco){

        //caso reconheca comentario de linha, continue consumindo ate obter atomo do lexico
        while(comentario_linha){
            infoAtomo = obter_atomo();
            lookahead = infoAtomo.atomo;
        }

        //caso reconheca comentario de bloco, continue consumindo are obter atomo do lexico
        while(comentario_bloco){
            infoAtomo = obter_atomo();
            lookahead = infoAtomo.atomo;
        }

        //caso reconheca fim de bloco de comentario, consome-o
        if(lookahead == FIM_COMENTARIO_BLOCO){
            infoAtomo = obter_atomo();
            lookahead = infoAtomo.atomo;
        }

        //se nao, entra no comando
        comando();
    }
}

//reconhece comando em especifico
void comando(){
    switch(lookahead){
        case PONTO_VIRGULA:
            consome(PONTO_VIRGULA);
            break;

        case ABRE_CHAVE:
            bloco_comandos();
            break;    

        case IDENTIFICADOR:
            atribuicao();
            break;
        
        case IF:
            comando_if();
            break;

        case WHILE:
            comando_while();
            break;

        case SCANF:
            comando_entrada();
            break;

        case PRINTF:
            comando_saida();
            break;

        case FIM_COMENTARIO_BLOCO:
            break;
    }

}

//reconhece varios comandos
void bloco_comandos(){
    consome(ABRE_CHAVE);
    comandos();
    consome(FECHA_CHAVE);
}

//reconhece atribuicao de identificadores
void atribuicao(){
    consome(IDENTIFICADOR);
    consome(ATRIBUICAO);
    expressao();
    consome(PONTO_VIRGULA);
}

//reconhece if
void comando_if(){
    consome(IF);
    consome(ABRE_PAR);
    expressao();
    consome(FECHA_PAR);
    comando();
    if(lookahead == ELSE){ //caso haja else
        consome(ELSE);
        comando();
    }
}

//reconhece while
void comando_while(){
    consome(WHILE);
    consome(ABRE_PAR);
    expressao();
    consome(FECHA_PAR);
    comando();
}

//reconhece scanf
void comando_entrada(){
    consome(SCANF);
    consome(ABRE_PAR);
    lista_variavel();
    consome(FECHA_PAR);
    consome(PONTO_VIRGULA);
}

//reconhece printf
void comando_saida(){
    consome(PRINTF);
    consome(ABRE_PAR);
    expressao();
    while(lookahead == VIRGULA){ //caso haja mais de um identificador para imprimir
        consome(VIRGULA);
        expressao();
    }
    consome(FECHA_PAR);
    consome(PONTO_VIRGULA);
}

//reconhecer expressao
void expressao(){
    expressao_logica();
    while(lookahead == OU){
        consome(OU);
        expressao_logica();
    }
}

// reconhece expressao logica
void expressao_logica(){
    expressao_relacional();
    while(lookahead == EH){
        consome(lookahead);
        expressao_relacional();
    }
}

//reconhece expressao relacional
void expressao_relacional(){
    expressao_adicao();
    if(lookahead == MENOR || lookahead == DIFERENTE ||
        lookahead == IGUAL || lookahead == MENOR_IGUAL || 
        lookahead == MAIOR || lookahead == MAIOR_IGUAL){
        op_relacional();
        expressao_adicao();
    }
}

//consome o operador relacional
void op_relacional(){
    consome(lookahead);
}

//reconhece expreessao de adicao/subtracao
void expressao_adicao(){
    expressao_multi();
    while(lookahead == OP_SOMA || lookahead == OP_SUBT){
        consome(lookahead);
        expressao_multi();
    }
}

//reconhece expressao de multiplicacao/divisao
void expressao_multi(){
    operando();
    while(lookahead == OP_MULT || lookahead == OP_DIV){
        consome(lookahead);
        operando();
    }
}

//reconhece um identificador, numero, true o false
void operando(){
    switch(lookahead){
        case IDENTIFICADOR:
            consome(IDENTIFICADOR);
            break;

        case NUMERO:
            consome(NUMERO);
            break;
        
        case TRUE:
            consome(TRUE);
            break;
        
        case FALSE:
            consome(FALSE);
            break;

        case ABRE_PAR:
            consome(ABRE_PAR);
            expressao();
            consome(FECHA_PAR);
    }
}

//##################### FIM Analisador Sintatico ##########################