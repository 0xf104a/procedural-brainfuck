#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h> //bzero
#include <ctype.h>

#include "hashtable.h"
#include "array.h"
/**Configuration**/
#define ARRAY_SIZE 65535
#define HASHTBL_SZ 255
/**End configuration**/
typedef struct{
  int cp;
  char *code;
} procedure_t;

bool is_regular(char c){ //regular chars are commands of pbf
    return c=='>'||c=='<'||c=='['||c==']'||c=='+'||c=='-'||c=='!'||c==','||c=='.';
}

int8_t *array;
int ap;//array ptr

bool eval_cmd(char cmd){
         if(cmd=='+'){
            array[ap]++;
            return true;
         }
         if(cmd=='-'){
            array[ap]--;
            return true;
         }
         if(cmd=='>'){
            ap++;
            return true;
         }
         if(cmd=='<'){
            ap--;
            return true;
         }
         if(cmd=='.'){
            printf("%c",array[ap]);
            return true;
         }
         if(cmd==','){
            scanf("%c",&array[ap]);
            return true;
         }
         return false;
}
hashtable *funs;
void eval(char *code){
     array_t *loop_stack=array_create(1);
     size_t csz = strlen(code);
     size_t cp = 0;// code pointer
     while(cp<csz){
         char cmd=code[cp];
         if(cmd==' '||cmd=='\n'){
            ++cp;
         }
         if(eval_cmd(cmd)){
            ++cp;
            continue;
         }
         if(cmd=='!'){
            return ;
         }
         if(cmd=='?'){
            if(!array[ap]){
               return ;
            }else{
                ++cp;
                continue;
            }
         }
         if(cmd=='['){
            size_t *_cp=(size_t *)malloc(sizeof(size_t));//return addr for loop
            *_cp=cp+1;
            array_add(loop_stack, _cp);
            ++cp;
            continue;
          }
         if(cmd==']'){
            if(array[ap]){
               int ret = *(int*)loop_stack->base[loop_stack->sz-1];
               cp=ret;
            }else{
               int *ret = (int*)loop_stack->base[loop_stack->sz-1];
               free(ret);  
               cp++;
               array_pop(loop_stack);
            }
            continue;
         }
         if(cmd=='{'||cmd=='}'||cmd==';'){
             printf("Unexpected token: `%c`\n",cmd);
             exit(-1);
         }
         char *name=(char *)malloc(sizeof(char));
         name[0]='\0';
         size_t sz=1;
         while(isalpha(code[cp])||code[cp]==' '||code[cp]=='_'||isdigit(code[cp])){
             if(code[cp]==' '){
                 cp++;
                 continue;
             }
             name[sz-1]=code[cp];
             sz++;
             name=(char *)realloc(name, sz*sizeof(char));
             name[sz-1]='\0';
             cp++;
         }
         if(code[cp]==';'){
             if(!hashtbl_check_key(funs, name)){
                printf("Undefined procedure: `%s`\n", name);
                exit(-1);
             }
             procedure_t *proc = hashtbl_get(funs, name);
             proc->cp=0;
             eval(proc->code);
             free(name);
             ++cp;
             continue;
         }
         if(code[cp]=='{'){
             ++cp;
             procedure_t *proc=(procedure_t *)malloc(sizeof(procedure_t));
             proc->code=(char *)malloc(sizeof(char));
             proc->code[0]='\0';
             size_t sz=1;
             do{
                 proc->code[sz-1]=code[cp];
                 ++sz;
                 proc->code=(char*)realloc(proc->code, sz*sizeof(char));
                 proc->code[sz-1]='\0';
                 ++cp;
             } while (code[cp]!='}');
             hashtbl_add(funs, name, proc);
             free(name);
             ++cp;
         }
     }
}
int main(){
    funs=hashtbl_create(HASHTBL_SZ);
    array=(int8_t *)malloc(sizeof(int8_t)*ARRAY_SIZE);
    char *code=(char *)malloc(sizeof(char));
    code[0]='\0';
    size_t sz=1;
    for(;;){
        char c=getc(stdin);
        if(c==EOF){
            break;
        }
        code[sz-1]=c;
        code=realloc(code, sizeof(char)*(++sz));
        code[sz-1]='\0';
    }
    bzero(array, sizeof(int8_t)*ARRAY_SIZE);
    eval(code);
    free(code);
    return  0;
}

