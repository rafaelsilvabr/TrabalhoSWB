#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 255
int aux=1, aux1=1;

void traduzirfuncao();
void attr();

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
    printf("# Tradução BPL to Assembly\n\n");
    while(scanf("%[^\n]\n", line) != EOF){ //Le linhas
     // printf("%s\n",line);
        word = strtok(line," ");          //Quebra a linha em palavras p/ num1entificar 
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
    printf("\n.globl %s\n\n%s:\n\n",word,word);
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
       // printf("||p%c%d||\n",word[1],word[2]-48);
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
        [3].registrador="%r13d",
        [4].registrador="%r14d",
        [5].registrador="%r15d",
        [1].tipo=0,
        [2].tipo=0,
        [3].tipo=0,
        [4].tipo=0,
        [5].tipo=0,
        [1].tamanho=0,
        [2].tamanho=0,
        [3].tamanho=0,
        [4].tamanho=0,
        [5].tamanho=0
            
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
   
        
    //Aumenta Pilha da Função
    int tamanho_pilha = 0; char tmp[10];
    strcpy(buffer,"\n");
    for(int i=1; i<=3; i++){ //Verifica PARAMETROS Declarados Na função p/alocar na pilha
        if(parametros[i].tipo!=0){
          // 
         // parametros[i].tamanho+=8; 
          tamanho_pilha+=parametros[i].tamanho;
           parametros[i].tamanho=tamanho_pilha;
            /*
            strcat(buffer,"movq ");
            strcat(buffer,parametros[i].registrador);
            strcat(buffer,", -");
            sprintf(tmp,"%d",tamanho_pilha);
            strcat(buffer,tmp);
            strcat(buffer,"(%rsp)\n");*/
        }
    }
    for(int i=1; i<=5; i++){
        if(variaveis[i].tipo!=0){ //Verifica VARIAVEIS Declarados Na função p/alocar na pilha
        //   printf("tamanho da pilha= %d\n", tamanho_pilha);
           //  
            tamanho_pilha+=variaveis[i].tamanho; 
            /*variaveis[i].tamanho=tamanho_pilha;
           strcat(buffer,"movq ");
            strcat(buffer,variaveis[i].registrador);
            strcat(buffer,", -");
            sprintf(tmp,"%d",tamanho_pilha);
            strcat(buffer,tmp);
            strcat(buffer,"(%rsp)\n");*/
        }
    }
/*
         //Debug Variaveis Locais
        printf("%c / %s / %d\n",variaveis[1].tipo, variaveis[1].registrador, variaveis[1].tamanho);
        printf("%c / %s / %d\n",variaveis[2].tipo, variaveis[2].registrador, variaveis[2].tamanho);
        printf("%c / %s / %d\n",variaveis[3].tipo, variaveis[3].registrador, variaveis[3].tamanho);
        printf("%c / %s / %d\n",variaveis[4].tipo, variaveis[4].registrador, variaveis[4].tamanho);
        printf("%c / %s / %d\n",variaveis[5].tipo, variaveis[5].registrador, variaveis[5].tamanho);
        */
    if(tamanho_pilha%16 != 0) tamanho_pilha+=(16-(tamanho_pilha%16)); //Alinha pilha a 16
    printf("subq $%d, %%rsp\n", tamanho_pilha);
    printf("%s", buffer);

//Corpo da Função

//Atribuição
while(scanf("%[^\n]\n", line), strcmp(line,"end")!=0){
  
    int r, l1;
    char num1;
    r = sscanf(line, "if %ci%d", &num1, &l1); 

    if(r==2){
        while(scanf("%[^\n]\n", line), strcmp(line,"endif")!=0){

        }
    }
    else{
          attr(line, variaveis, parametros);
    }

//Finaliza Função
}
}

void attr(char * line,  var * variaveis, var * parametros){

    int r, num1, num2, num3;
    char op, l1, l2;
    
   
        r = sscanf(line, "vi%d = %ci%d %c %ci%d", &num1, &l1, &num2, &op, &l2, &num3); 
           // printf("vi%d = %ci%d %c %ci%d\n", num1, l1, num2, op, l2, num3); 


    if(r==3){
        if(l1 == 'c'){
         //   printf("#vi%d = ci%d\n", num1, num2);
            printf("movl $%d, -%d(%%rbp)\n\n", num2, variaveis[num1].tamanho);
            return;
        }
        if(l1 == 'v'){
         //   printf("#vi%d = vi%d\n", num1, num2);
            printf("movl -%d(%%rbp), %%eax\n", variaveis[num2].tamanho);
            printf("movl %%eax, -%d(%%rbp)\n\n", variaveis[num1].tamanho);
            return;
        }
        if(l1 == 'p'){
         //   printf("#vi%d = pi%d\n", num1, num2);
            printf("movq %s, -%d(%%rbp)\n", parametros[num2].registrador, parametros[num2].tamanho);   
            printf("movl -%d(%%rbp), %%eax\n", parametros[num2].tamanho);
            printf("movl %%eax, -%d(%%rbp)\n\n", variaveis[num1].tamanho);  
        
            return;
        }
    }
   
    if(r == 6){

        if(l1 == 'v'){

            //printf("#vi%d = vi%d %c %ci%d\n", num1, num2, op, l2, num3); 

            if(l2 == 'c')
            printf("movl -%d(%%rbp), %%eax\n", variaveis[num2].tamanho);

            if(l2 == 'v'){ 
            printf("movl -%d(%%rbp), %%eax\n", variaveis[num3].tamanho);
                if(op == '+')
                printf("addl -%d(%%rbp), %%eax\n", variaveis[num2].tamanho) ;
                if(op == '-')
                printf("subl -%d(%%rbp), %%eax\n", variaveis[num2].tamanho);
                if(op == '*')
                printf("imull -%d(%%rbp), %%eax\n", variaveis[num2].tamanho);
            }

            if(l2 == 'p') {
          
            if(aux==1){  printf("movq %s, -%d(%%rbp)\n\n", parametros[num2].registrador, parametros[num2].tamanho);   
            aux+=2;
            }

            printf("movq -%d(%%rbp), %%rax\n", parametros[num3].tamanho);
            printf("movl %%eax, %%edx\n"); 
            printf("movl -%d(%%rbp), %%eax\n", variaveis[num2].tamanho); 

                if(op == '+')
                printf("addl %%edx, %%eax\n");
                if(op == '-')
                printf("subl %%edx, %%eax\n");
                if(op == '*')
                printf("imull %%edx, %%eax\n");
                if(op == '/') 
                printf("FAZER\n");
                printf("movl %%eax, -%d(%%rbp)\n\n", variaveis[num1].tamanho); 

                return;
            }
                if(op == '+' && l2 == 'c') 
                printf("addl $%d, %%eax\n", num3);
                if(op == '-'&& l2 == 'c')
                printf("subl $%d, %%eax\n", num3);
                if(op == '*'&& l2 == 'c')
                printf("imull $%d, %%eax, %%eax\n", num3);
                if(op == '/') 
                printf("FAZER\n");
            printf("movl %%eax, -%d(%%rbp)\n\n", variaveis[num1].tamanho); 
            return;
        }

        if(l1 == 'c'){
          //  printf("#vi%d = vi%d %c %ci%d\n", num1, num2, op, l2, num3); 

            if(l2 == 'c'){
                int tmp=0;
                if(op=='+') tmp=num2+num3;
                else if(op=='-') tmp=num2-num3;
                else if(op=='*') tmp=num2*num3;

            printf("movl $%d, -%d(%%rbp)\n\n", tmp ,variaveis[num1].tamanho);  
            return;
            }

            if(l2 == 'v') 
            printf("movl -%d(%%rbp), %%eax\n", variaveis[num3].tamanho);


           if(l2 == 'p') {
               if(aux==1){  printf("movq %s, -%d(%%rbp)\n\n", parametros[num3].registrador, parametros[num3].tamanho);   
            aux+=2;}
                printf("movq -%d(%%rbp), %%rax\n", parametros[num3].tamanho);

           // printf("movl $%d, %%eax\n", num2);  
                if(op == '+')
                printf("addl $%d, %%eax\n", num2);
                if(op == '-')
                printf("subl $%d, %%eax\n", num2);
                if(op == '*')
                printf("imull $%d, %%eax\n", num2);
                if(op == '/') 
                printf("FAZER\n");
                printf("movl %%eax, -%d(%%rbp)\n\n", variaveis[num1].tamanho); 

                return;
            }
                if(op == '+')
                printf("addl $%d, %%eax\n", num2);  
                if(op == '-')
                printf("subl $%d, %%eax\n", num2);  
                if(op == '*')
                printf("imull $%d, %%eax\n", num2);  
                if(op == '/') 
                printf("FAZER\n");
            printf("movl %%eax, -%d(%%rbp)\n\n", variaveis[num1].tamanho); 
            return;
        }

        if(l1 == 'p'){
           

          if(aux==1){  printf("movq %s, -%d(%%rbp)\n\n", parametros[num2].registrador, parametros[num2].tamanho);   
            aux+=2;

          }
           //printf("#vi%d = vi%d %c %ci%d\n", num1, num2, op, l2, num3); 

            if(l2 == 'c'){
            printf("movl $%d, %%eax\n", num3);             }

            if(l2 == 'v') 
            printf("movl -%d(%%rbp), %%eax\n", variaveis[num3].tamanho);

            if(l2 == 'p'){ 
                printf("movq %s, -%d(%%rbp)\n", parametros[num3].registrador, parametros[num3].tamanho);   

                if(op != '*')
            printf("movl -%d(%%rbp), %%eax\n", parametros[num3].tamanho);

            }
                if(op == '+')
                printf("addl -%d(%%rbp), %%eax\n", parametros[num2].tamanho);
                if(op == '-')
                printf("subl -%d(%%rbp), %%eax\n", parametros[num2].tamanho);
                if(op == '*' && l2 != 'v'){
                printf("imulq -%d(%%rbp), %%rax\n", parametros[num2].tamanho);
                }else if (op == '*' && l2 == 'v'){
                printf("movslq %%eax, %%rax\n");
                printf("imulq -%d(%%rbp), %%rax\n", parametros[num2].tamanho);

                }
                if(op == '/') 
                printf("FAZER\n");
            printf("movl %%eax, -%d(%%rbp)\n\n", variaveis[num1].tamanho); 
            return;
        }

    }
        
}


