#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX 255

void traduzirfuncao();

typedef struct{
    char tipo;
    char registrador[5];
    char memoria[10];
    int tamanho;
    }var;

int main(){
    int i;
    char line[255];
    char *word;
    printf("# Tradução BNF to Assembly\n");
    while(scanf("%[^\n]\n", line) != EOF){ //Le linhas
        //printf("%s\n",line);
        word = strtok(line," ");          //Quebra a linha em palavras p/ identificar
        while(word!=NULL){
            if(strcmp(word,"function")==0){
                traduzirfuncao(word);     //Inicializa Tradução de uma função
            }
            word = strtok(NULL," ");
        }
    }
    return 0;
}

void traduzirfuncao(char *word){
    char buffer[MAX];
//Inicializa a pilha da função

    //Nome da Função
    word = strtok(NULL," "); //Pula p/próxima palavra
    printf(".globl %s\n%s:\n",word,word);
    printf("pushq %%rbp\nmovq %%rsp, %%rbp\n");
    word = strtok(NULL," ");

    //Parametros da Função
    var parametros[4]={
        [1].registrador="%rdi",
        [2].registrador="%rsi",
        [3].registrador="%rdx",
        [1].tamanho=8,
        [2].tamanho=8,
        [3].tamanho=8,
        [1].tipo=0,
        [2].tipo=0,
        [3].tipo=0
    };

    while(word != NULL){
        char tipoParam, numParam;
        //printf("||p%c%d||\n",word[1],word[2]-48);
        parametros[word[2]-48].tipo = word[1];
        word = strtok(NULL," ");
    }

        //Debug Parametros
        /*
        printf("%c / %s / %s\n",parametros[1].tipo, parametros[1].registrador, parametros[1].memoria);
        printf("%c / %s / %s\n",parametros[2].tipo, parametros[2].registrador, parametros[2].memoria);
        printf("%c / %s / %s\n",parametros[3].tipo, parametros[3].registrador, parametros[3].memoria);
        */

//Declara as variaveis locais
    char line[MAX];
    
    var variaveis[6] = {
        [1].registrador="%ebx",
        [2].registrador="%r12d",
        [3].registrador="%r14d",
        [4].registrador="%r15d",
        [5].registrador="%r16d",
        [1].tipo=0,
        [2].tipo=0,
        [3].tipo=0,
        [4].tipo=0,
        [5].tipo=0        
    };

    while(scanf("%[^\n]\n", line), strcmp(line,"enddef")!=0){
        variaveis[line[6]-'0'].tipo = line[5];
        variaveis[line[6]-'0'].tamanho = 4;
        if(strlen(line)>7){ // é do tipo = vet vaN size ciN
            int num=0; int sinal = 1; int i=15; //leitura número (positivo/negativo)
            if(!(line[i]>='0' && line[i]<='9')){
                sinal = -1; //negativo
                i++;
            }
            while(line[i]>='0' && line[i]<='9'){
                num = num*10 + line[i]-'0';
                i++;
            }
            num*=sinal;
            variaveis[line[6]-'0'].tamanho *= num;
        }
    }
        //Debug Variaveis Locais
        /*printf("%c / %s / %s\n",variaveis[1].tipo, variaveis[1].registrador, variaveis[1].memoria);
        printf("%c / %s / %s / %d\n",variaveis[2].tipo, variaveis[2].registrador, variaveis[2].memoria, variaveis[2].tamanho);
        printf("%c / %s / %s\n",variaveis[3].tipo, variaveis[3].registrador, variaveis[3].memoria);
        printf("%c / %s / %s\n",variaveis[4].tipo, variaveis[4].registrador, variaveis[4].memoria);
        printf("%c / %s / %s\n",variaveis[5].tipo, variaveis[5].registrador, variaveis[5].memoria);
        */
        
    //Aumenta Pilha da Função
    int tamanho_pilha = 0; char tmp[10];
    strcpy(buffer,"\n");
    for(int i=1; i<=3; i++){ //Verifica PARAMETROS Declarados Na função p/alocar na pilha
        if(parametros[i].tipo!=0){
            tamanho_pilha+=parametros[i].tamanho;

            strcat(buffer,"movq ");
            strcat(buffer,parametros[i].registrador);
            strcat(buffer,", -");
            sprintf(tmp,"%d",tamanho_pilha);
            strcat(buffer,tmp);
            strcat(buffer,"(%rsp)\n");
        }
    }
    for(int i=1; i<=5; i++){
        if(variaveis[i].tipo!=0){ //Verifica VARIAVEIS Declarados Na função p/alocar na pilha
            tamanho_pilha+=variaveis[i].tamanho; 

            strcat(buffer,"movq ");
            strcat(buffer,variaveis[i].registrador);
            strcat(buffer,", -");
            sprintf(tmp,"%d",tamanho_pilha);
            strcat(buffer,tmp);
            strcat(buffer,"(%rsp)\n");
        }
    }
    if(tamanho_pilha%16 != 0) tamanho_pilha+=(16-(tamanho_pilha%16)); //Alinha pilha a 16
    printf("\nsubq $%d, %%rsp\n", tamanho_pilha);
    printf("%s\n", buffer);

//Corpo da Função

//Finaliza Função
}