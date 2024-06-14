# Compilador C Lite
Este é um trabalho de Faculdade desenvolvido para a disciplina de Compiladores do 6º semestre.  
Este trabalho teve 2 entregas: parte 1 e parte 2. Sendo que cada uma das partes envolvia etapas diferentes do processo de compilação de código.  
# Sobre a Gramática da linguagem
A linguagem a ser compilada é uma versão mais simplificada de C: C Lite  
Nesta gramática, existem apenas **inteiros** e **boleanos**.  
Além disso, a gramática aceita palavras-chave como: if e else, while, true e false, int e bool, printf e scanf, void e main. Cada palavra reservada é um átomo específico para o analisador léxico.  
  
Os **identificadores/variáveis** sempre serão declarados **antes** de serem executados comandos com eles. Cada identificador deve começar com o **Underline** '_'  em seguida de quaisquer outros caracteres, com tamanho máximo de 15 caracteres. Caso contrário, será impresso um **ERRO** no terminal.  
  
Os números devem estar na forma **hexadecimal: 0xn** para serem reconhecidos, sendo 'n' um **dígito** ou uma **letra** de A até F. Mas, na impressão do número, ele deve estar na forma decimal.  
Por exemplo: 0xA -> 10  

# Parte 1 do trabalho
Esta parte possui a implementação das fases de Análise Léxica e Análise Sintática.  
O resultado final é a impressão dos **TOKENS** detectados na entrada definida.  
Caso algum erro seja detectado durante a execução, a impressão dos tokens é **interrompida** e um ERRO é impresso no terminal, indicando qual tipo de erro foi detectado: sintático ou léxico.  
# Parte 2 do trabalho
Esta parte, além de possuir as fases anteriores, tem a implementação das fases de Análise Semântica e Geração de Código Intermediário.  
Um detalhe que diferencia esta impementação da anterior é que boleanos não existem. Para a facilitar a implementação da Geração de Código Intermediário, os valores boleanos foram retirados da gramática.  
O resultado final é a impressão do código intermediário em MEPA(Máquina de Execução para Pascal).  
Caso ocorra haja mais de uma variável declarada ou uma variável que não foi declarada está sendo utilizada, ocorre um erro semântico e a execução do compilador é **interrompida**, imprimindo um **ERRO**.  
# Executar o compilador
1. Tenha um compilador C instalado. Caso não tenha, pode instalar seguindo as orientações da página do VS Code: [Using GCC with MinGW](https://code.visualstudio.com/docs/cpp/config-mingw)
2. Clone o repositório.
3. Abra o terminal e entre em uma das pastas com _cd .\parte 1_ ou _cd .\parte 2_ .
4. Execute o comando _.\compilador_de_CLite.exe input.txt_ . O input.txt é o arquivo com um programa em **C Lite**.
5. Caso o executável não esteja presente em uma das pastas, basta compilar o arquivo C. Comando: _gcc compilador_de_CLite.c -o nome_do_executavel.exe_
