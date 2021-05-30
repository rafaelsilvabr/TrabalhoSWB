#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 255

void traduzirfuncao();
void attr();
void acessArray();

typedef struct{
    char tipo;
    char registrador[5];
    char memoria[10];
    int tamanho;
    }var;

int main(){
    char line[MAX];
    char *word;
    printf("# Tradução BPL to Assembly\n\n");
    while(scanf("%[^\n]\n", line) != EOF){ //Le linhas
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

/*---------------------------------------------------------*/

void traduzirfuncao(char *word){
    //Inicializa a pilha da função

    //Nome da Função
    word = strtok(NULL," "); //Pula p/próxima palavra
    printf("\n.globl %s\n\n%s:\n\n",word,word);
    printf("pushq %%rbp\nmovq %%rsp, %%rbp\n");
    word = strtok(NULL," ");

    //Parametros da Função
    var parametros[4]={
        [1].registrador="%edi",
        [2].registrador="%esi",
        [3].registrador="%edx",
        [1].tamanho=8,
        [2].tamanho=8,
        [3].tamanho=8,
        [1].tipo=0,
        [2].tipo=0,
        [3].tipo=0
    };

    while(word != NULL){ //Salva tipo do parametro na struct (array or int)
        parametros[word[2]-48].tipo = word[1];
        word = strtok(NULL," ");
    }

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
    int tamanho_pilha = 0;
    for(int i=1; i<=3; i++){ //Verifica PARAMETROS Declarados Na função p/alocar na pilha
        if(parametros[i].tipo!=0){
            tamanho_pilha+=parametros[i].tamanho; //incrementa tamanho do parametro no tamanho da pilha
            parametros[i].tamanho=tamanho_pilha; //aloca parametro na pilha
        }
    }
    for(int i=1; i<=5; i++){
        if(variaveis[i].tipo!=0){ //Verifica VARIAVEIS Declarados Na função p/alocar na pilha
            tamanho_pilha+=variaveis[i].tamanho; 
            variaveis[i].tamanho=tamanho_pilha;
        }
    }

    if(tamanho_pilha%16 != 0) tamanho_pilha+=(16-(tamanho_pilha%16)); //Alinha pilha a 16
    if(tamanho_pilha != 0) printf("subq $%d, %%rsp\n", tamanho_pilha);

/*---------------------------------------------------------*/

//Corpo da Função

//Atribuição
int n_if=0;
while(scanf("%[^\n]\n", line), strcmp(line,"end")!=0){
  
    int r, num;
    char l;

    r = sscanf(line, "if %ci%d", &l, &num); 

    if(r==2){
        n_if++;
      //  printf("#if %ci%d\n", l, num);
        while(scanf("%[^\n]\n", line), strcmp(line,"endif")!=0){
           int tmp;
           tmp = ( l=='p') ? parametros[num].tamanho : variaveis[num].tamanho;

            printf("movl -%d(%%rbp), %%ecx\n", tmp);
            printf("cmpl $0, %%ecx\n");
            printf("je d_if%d\n\n",n_if);

            attr(line, variaveis, parametros);

            printf("d_if%d:\n",n_if);
        }
    }
    else{
          attr(line, variaveis, parametros);

    }
    
}
//end
printf("end_func:\n");
printf("leave\nret\n");

return;
//Finaliza Função
}

void attr(char * line,  var * variaveis, var * parametros){

    int r, s, num1, num2, num3;
    char op, l, ll;
   
    r = sscanf(line, "vi%d = %ci%d %c %ci%d", &num1, &l, &num2, &op, &ll, &num3); 
           // printf("vi%d = %ci%d %c %ci%d\n", num1, l, num2, op, ll, num3); 

    int n, f, n1, n2, n3;
    char l1, l11, l2, l22, l3, l33;

    s = sscanf(line, "vi%d = call f%d %c%c%d %c%c%d %c%c%d", &n, &f, &l1, &l11, &n1, &l2, &l22, &n2, &l3, &l33, &n3);

/*-----------------------------------------------------------------------------------*/

    //<attr> -> <varint> = <valint> (atribuição simples)
    if(r==3){
        if(l == 'c'){
        //    printf("#vi%d = ci%d\n", num1, num2);
            printf("movl $%d, -%d(%%rbp)\n\n", num2, variaveis[num1].tamanho);
            return;
        }
        if(l == 'v'){
         //   printf("#vi%d = vi%d\n", num1, num2);
            printf("movl -%d(%%rbp), %%eax\n", variaveis[num2].tamanho);
            printf("movl %%eax, -%d(%%rbp)\n\n", variaveis[num1].tamanho);
            return;
        }
        if(l == 'p'){
          //  printf("#vi%d = pi%d\n", num1, num2);
            printf("movl %s, -%d(%%rbp)\n", parametros[num2].registrador, parametros[num2].tamanho);   
            printf("movl -%d(%%rbp), %%eax\n", parametros[num2].tamanho);
            printf("movl %%eax, -%d(%%rbp)\n\n", variaveis[num1].tamanho);  
        
            return;
        }
    }
   
/*-----------------------------------------------------------------------------------*/

    //<attr> -> <varint> = <oper> (Atribuiçao por expressão)
    if(r==6){ 

        //printf("vi%d = %ci%d %c %ci%d\n", num1, l, num2, op, ll, num3); 

        //Primeira parte da expressao em %eax
        if(l == 'v'){
            printf("movl -%d(%%rbp), %%eax\n", variaveis[num2].tamanho);
        }
        else if(l == 'c'){
            printf("movl $%d, %%eax\n", num2);  
        }
        else if(l == 'p'){
            printf("movl %s, %%eax\n", parametros[num2].registrador);
        }

        //Segunda parte da expressao em %ecx
        if(ll == 'v'){
            printf("movl -%d(%%rbp), %%ecx\n", variaveis[num3].tamanho);
        }
        else if(ll == 'c'){
            printf("movl $%d, %%ecx\n", num3);  
        }
        else if(ll == 'p'){
            printf("movl %s, %%ecx\n", parametros[num3].registrador);
        }

          if(op == '+')
          printf("addl %%ecx, %%eax\n");
          if(op == '-')
          printf("subl %%ecx, %%eax\n");
          if(op == '*')
          printf("imull %%ecx, %%eax\n");
          if(op == '/'){      
            printf("cltd\n");
            printf("idivl %%ecx\n");
          } 
          
        //<varint> = <exp>
        printf("movl %%eax, -%d(%%rbp)\n\n", variaveis[num1].tamanho); 
        return;
    }

/*-----------------------------------------------------------------------------------*/

    //<attr> -> <varint> = <call>

    if(s==2){ // chamada de função com 0 parâmetros 
      //  printf("#vi%d = f%d\n", n, f);
        printf("call f%d\n", f);
        printf("movl %%eax, -%d(%%rbp)\n\n", variaveis[n].tamanho);  
        return;
    }

    if(s > 4 && s < 12){ // chamada de função com 1 parâmetros a 3 parâmetros
        
        int tmp1, tmp2, tmp3;

        if(l1=='v' || l1 =='p' || l1 =='c'){ //Primeiro parâmetro
            if(l1 =='c')
            printf("movl $%d, %%edi\n", n1); 
            if(l11=='i' && l1 != 'c'){
            tmp1 = ( l1 =='p') ? parametros[n1].tamanho : variaveis[n1].tamanho;
            printf("movl -%d(%%rbp), %%edi\n", tmp1);
            }
            if(l11=='a'){
                if(l1=='p')
                printf("movq -%d(%%rbp), %%rdi\n", parametros[n1].tamanho);
                if(l1=='v'){
                printf("leaq -%d(%%rbp), %%rdi\n", variaveis[n1].tamanho);
                }
            }

        }

        if(s==5){ //Finaliza a função caso tenha 1 parâmetro
        printf("call f%d\n", f);
        printf("movl %%eax, -%d(%%rbp)\n\n", variaveis[n].tamanho); 
        return;
        }

        if(l2=='v' || l2 =='p' || l2 =='c'){//Segundo parâmetro
            if(l2== 'c'){
            printf("movl $%d, %%esi\n", n2);   
            }
            if(l22=='i' && l2 != 'c'){
            tmp2 = ( l2 =='p') ? parametros[n2].tamanho : variaveis[n2].tamanho;
            printf("movl -%d(%%rbp), %%esi\n", tmp2);
            }
            if(l22=='a'){
                if(l2=='p')
                printf("movq -%d(%%rbp), %%rsi\n", parametros[n2].tamanho);
                if(l2=='v'){
                printf("leaq -%d(%%rbp), %%rsi\n", variaveis[n2].tamanho);
                }
            }
        }

        if(s==8){//Finaliza a função caso tenha 2 parâmetros
        printf("call f%d\n", f);
        printf("movl %%eax, -%d(%%rbp)\n\n", variaveis[n].tamanho); 
        return;
        }

        if(l3=='v' || l3 =='p' || l3=='c'){//Terceiro parâmetro
            if(l3== 'c'){
            printf("movl $%d, %%edx\n", n3);   
            }
            if(l33=='i' && l3 != 'c'){
            tmp3 = ( l3 =='p') ? parametros[n3].tamanho : variaveis[n3].tamanho;
            printf("movl -%d(%%rbp), %%edx\n", tmp3);
            }
            if(l33=='a'){
                if(l3=='p')
                printf("movq -%d(%%rbp), %%rdx\n", parametros[n3].tamanho);
                if(l3=='v'){
                printf("leaq -%d(%%rbp), %%rdx\n", variaveis[n3].tamanho);
                }
            }
        }

        if(s==11){ //Finaliza a função caso tenha 3 parâmetro
        printf("call f%d\n", f);
        printf("movl %%eax, -%d(%%rbp)\n\n", variaveis[n].tamanho); 
        return;
        }
     
    }

/*-----------------------------------------------------------------------------------*/
    char g, arr, ret;
    int arrN, ciN, varN;

    g = sscanf(line, "%cet %ca%d index ci%d to vi%d", &g, &arr, &arrN, &ciN, &varN); 
    
    if(g==5){ // <arrayget> → 'get' <array> 'index' <const> 'to' <varint>
        if(arr=='p') acessArray(ciN,parametros[arrN]);
        if(arr=='v') acessArray(ciN,variaveis[arrN]);
        printf("movl (%%rax), %%ecx\n");
        printf("movl %%ecx, -%d(%%rbp)\n\n", variaveis[varN].tamanho);
    }
    
    s = sscanf(line, "%cet %ca%d index ci%d with %ci%d", &g, &arr, &arrN, &ciN, &ret, &varN);

    if(s==6){ // <arrayset> → 'set' <array> 'index' <const> 'with' <valint>
        if(arr=='p') acessArray(ciN,parametros[arrN]);
        if(arr=='v') acessArray(ciN,variaveis[arrN]);

        if(ret == 'p'){
            printf("movl -%d(%%rbp), %%ecx\n", parametros[varN].tamanho);
        }
        if(ret == 'v'){
            printf("movl -%d(%%rbp), %%ecx\n", variaveis[varN].tamanho);
        }
        if(ret == 'c'){
            printf("movl $%d, %%ecx\n", varN);
        }

        printf("movl %%ecx, (%%rax)\n\n");
    }

/*-----------------------------------------------------------------------------------*/
    int rtn;
    char str;
    rtn = sscanf(line, "%ceturn %ci%d",&str, &ret, &varN);
    if(rtn==3){
        printf("movl ");
        if(ret == 'p'){
            printf("-%d(%%rbp), %%eax\n", parametros[varN].tamanho);
        }
        if(ret=='v'){
            printf("-%d(%%rbp), %%eax\n", variaveis[varN].tamanho);
        }
        if(ret=='c'){
            printf("$%d, %%eax\n", varN);
        }
        printf("jmp end_func\n\n");
    }
    if(str=='r' && rtn!=3){
        printf("jmp end_func\n\n");
    }

}

void acessArray(int index, var array){
    printf("movq $%d, %%rax\n", index);
    printf("imulq $4, %%rax\n");
    printf("leaq -%d(%%rbp), %%rcx\n", array.tamanho);
    printf("addq %%rcx, %%rax\n");
}
