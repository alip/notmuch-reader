#include "notmuch.h"
#include "config.h"
#include <errno.h>
#include <string.h>
#include "assert.h"

static int notmuch_thread_callback(void* ctx, int type, const JSON_value* value){
    struct nmr_notmuch * nm=(struct nmr_notmuch*) ctx;

    switch (nm->state){
        case 0:
            if(type!=JSON_T_ARRAY_BEGIN)
                break;
            nm->state=1;
            break;
        case 1:
            if (type!=JSON_T_OBJECT_BEGIN)
                break; 
            nm->next_thread = malloc(sizeof(struct nmr_notmuch_thread));
            memset(nm->next_thread,0,sizeof(struct nmr_notmuch_thread));
            nm->state=2;
            break;
        case 2:
            if (type==JSON_T_KEY){
                if(strcmp(value->vu.str.value,"thread")==0){
                    nm->state=3;
                }else if(strcmp(value->vu.str.value,"subject")==0){
                    nm->state=4;
                }else if(strcmp(value->vu.str.value,"matched")==0){
                    nm->state=5;
                }else if(strcmp(value->vu.str.value,"total")==0){
                    nm->state=6;
                }else if(strcmp(value->vu.str.value,"authors")==0){
                    nm->state=7;
                }else if(strcmp(value->vu.str.value,"tags")==0){
                    nm->state=8;
                }else{
                    nm->state=99;
                }
            }else if (type==JSON_T_OBJECT_END){
                nm->callback(nm->next_thread);
                nm->next_thread=0;
                nm->state=1;
            }
            break;
        case 3:
            if(type!=JSON_T_STRING){
                fprintf(stderr,"corrupted json in state 3 (key:thread). got: %i \n",type);
                abort();;
            }
            if(nm->next_thread->thread_id)
                return -3;
            nm->next_thread->thread_id=malloc(sizeof(char*)*strlen(value->vu.str.value));
            strcpy(nm->next_thread->thread_id,value->vu.str.value);
            nm->state=2;
            break;
        case 4:
            if(type!=JSON_T_STRING)
                break;
            if(nm->next_thread->subject)
                return -4;
            nm->next_thread->subject=malloc(sizeof(char*)*strlen(value->vu.str.value));
            strcpy(nm->next_thread->subject,value->vu.str.value);
            nm->state=2;
            break;
        case 5:
            if (type!=JSON_T_INTEGER){
                fprintf(stderr,"corrupted json in state 5 (key:matched). got: %i \n",type);
                abort();
            }
            nm->next_thread->matched=value->vu.integer_value;
            nm->state=2;
            break;
        case 6:
            if (type!=JSON_T_INTEGER)
                break;
            nm->next_thread->total=value->vu.integer_value;
            nm->state=2;
            break;
        case 7:
            if(type!=JSON_T_STRING)
                break;
            if(nm->next_thread->subject)
                return -7;
            nm->next_thread->authors=malloc(sizeof(char*)*strlen(value->vu.str.value));
            strcpy(nm->next_thread->authors,value->vu.str.value);
            nm->state=2;
            break;
        case 8:
            if(type!=JSON_T_ARRAY_BEGIN){
                fprintf(stderr,"corrupted json in state 8 (key:tags). got: %i \n",type);
                abort();
            }
            nm->next_thread->tags=malloc(sizeof(void*));
            nm->state=9;
            break;
        case 9:
            if(type==JSON_T_STRING){
                nm->next_thread->tags=realloc(nm->next_thread->tags,(nm->next_thread->tags_l+1)*sizeof(void*));
                char * str= malloc(strlen(value->vu.str.value));
                strcpy(str,value->vu.str.value);
                nm->next_thread->tags[nm->next_thread->tags_l++]=str;
            }else if(type==JSON_T_ARRAY_END){
                nm->state=2;
            }else{
                fprintf(stderr,"corrupted json in state 8 (key:tags). got: %i \n",type);
                abort();
            }
            
        default:
            if(type==JSON_T_OBJECT_BEGIN || type==JSON_T_ARRAY_BEGIN){
                ++nm->state;
            }else if(type==JSON_T_OBJECT_END || type==JSON_T_ARRAY_END){
                if((--nm->state)<99){
                    nm->state=2;
                    notmuch_thread_callback(ctx,type,value);
                }
            }else if(nm->state==99){
                nm->state=2;
            }
    }

    return 1;
}

int nmr_notmuch_search(struct  nmr_notmuch * notmuch,const char * query){
    notmuch->state=0;

    const char * cmd=config_notmuch_call();
    char * fc=malloc( (strlen(cmd)+strlen(query)+24)*sizeof(char));
    sprintf(fc,"%s search --format=json %s",cmd,query);
    notmuch->p=popen(fc,"r");
    free(fc);
    assert(notmuch->p);
    JSON_config config;
    init_JSON_config(&config);
    config.depth                  = 19;
    config.callback               = &notmuch_thread_callback;
    config.callback_ctx           = (void*)notmuch;
    config.allow_comments         = 1;
    config.handle_floats_manually = 0;

    notmuch->jc = new_JSON_parser(&config);
    return 0;
}

int nmr_notmuch_activate(struct nmr_notmuch * notmuch){
    int next_char = fgetc(notmuch->p);
    if( next_char ==EOF){
        if (!JSON_parser_done(notmuch->jc)) {
            fprintf(stderr, "JSON_parser_end: syntax error\n");
            abort();
        }
        pclose(notmuch->p);
        notmuch->p=NULL;
    }
    if (next_char <= 0) {
        return errno;
    }
    if (!JSON_parser_char(notmuch->jc, next_char)) {
        fprintf(stderr, "JSON_parser_char: syntax error. %c\n",next_char);
        abort();
    }
}


void nmr_notmuch_free(struct nmr_notmuch * notmuch){
    if(notmuch->p!=NULL)
        pclose(notmuch->p);
    delete_JSON_parser(notmuch->jc);
}


