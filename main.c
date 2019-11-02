#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h> //bzero
#include <ctype.h>
#include <errno.h>
#include <unistd.h> //access

#include "hashtable.h"
#include "array.h"
/**Configuration**/
#define ARRAY_SIZE 65535
#define HASHTBL_SZ 255
#define ARRAY_CAPACITY //initial capacity for array_t
/**End configuration**/
typedef struct{
  int cp;
  char *code;
} procedure_t;

typedef enum{
   SECURITY_NONE, SECURITY_LOOP, SECURITY_DIE
} security;

bool is_regular(char c){ //regular chars are commands of pbf
    return c=='>'||c=='<'||c=='['||c==']'||c=='+'||c=='-'||c=='!'||c==','||c=='.';
}

int8_t *array;
int ap;//array ptr
security security_mode;
hashtable *funs;

void free_procedure(procedure_t *procedure){
     free(procedure->code);
     free(procedure);
}

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
            if(ap>=ARRAY_SIZE&&security_mode!=SECURITY_NONE){
               if(security_mode == SECURITY_LOOP){
                  ap = ap % ARRAY_SIZE;
               }else{
                  fprintf(stderr, "Security policy forbids exiting out of array size, which is set to %d\n", ARRAY_SIZE);
                  exit(-1);
               }
             }
            return true;
         }
         if(cmd=='<'){
            ap--;
            if(ap<0&&security_mode!=SECURITY_NONE){
               if(security_mode == SECURITY_LOOP){
                  ap = ARRAY_SIZE - 1;
               }else{
                  fprintf(stderr, "Security policy forbids exiting out of array size, which is set to %d\n", ARRAY_SIZE);
                  exit(-1);
               }
            }
            return true;
         }
         if(cmd=='.'){
            putchar_unlocked(array[ap]);
            return true;
         }
         if(cmd==','){
            fflush(stdin);
            array[ap] = getchar_unlocked();
            return true;
         }
         return false;
}

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

void help(char *name){ //Help message
    printf("Usage: %s [--help -h] <filname> [--security none|loop|die]\n",name);
}

void init(void){ //initialize intepreter
    funs = hashtbl_create(HASHTBL_SZ);
    array = (int8_t *)malloc(sizeof(int8_t)*ARRAY_SIZE);
    bzero(array, ARRAY_SIZE*sizeof(int8_t));
}

void reset(void){ //reset inerpreter
      size_t i;
    //free procedures
    for(i=0; i<funs->sz; ++i){
        free_procedure(funs->values->base[i]);
    }
    hashtbl_destroy(funs);
    //free array
    free(array);
    init();//initialize it again
}

char *readf(char *fname){
     FILE *f = fopen(fname, "rb");
     char *buffer = NULL;
     if(!f){
       return NULL;
     }
     fseek(f, 0, SEEK_END);
     size_t len = ftell(f);
     fseek(f, 0, SEEK_SET);
     buffer = malloc(len + 1);
     fread(buffer, 1, len, f);
     fclose(f);
     buffer[len]='\0';
     return buffer;
}

int main(int argc, char *argv[]){
    /*Parse args*/
    if(argc<2){
       help(argv[0]);
       return -1;
    }
    if(!strcmp(argv[1],"-h")||!strcmp(argv[1],"--help")){
       help(argv[0]);
       return 0;
    }    
    char *filename=argv[1];
    if( access(filename, F_OK) == -1){
        fprintf(stderr,"%s: access: %s(%d).\n",argv[0], strerror(errno), errno);  
        return -1;
    }
    char *code=readf(filename);
    if(!code){
       fprintf(stderr,"%s: fopen: %s(%d).\n",argv[0], strerror(errno), errno);
    }
    security_mode = SECURITY_LOOP;
    if(argc > 2){
       if(!strcmp(argv[2],"--security")){
          if(argc < 4){
             fprintf(stderr,"`--security` requires an argument.\n");
             return -1;
          }
          if(!strcmp(argv[3], "none")){
             security_mode = SECURITY_NONE;
          }else if(!strcmp(argv[3], "loop")){
             security_mode = SECURITY_LOOP;
          }else if(!strcmp(argv[3], "die")){
             security_mode = SECURITY_DIE;
          }else{
             fprintf(stderr,"Unrecognized security mode: `%s`",argv[3]);
             return -1;
          } 
       }else{
         fprintf(stderr,"Unrecognized command-line argument: `%s.`\n",argv[2]);
         return -1;
       }
    }
    /*Run code */
    init();
    eval(code);  
    return  0;
}

