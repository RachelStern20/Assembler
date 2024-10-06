#include "first_pass.h"
#include "second_pass.h"

/* updating the lables adress if dc then to cur. addrees +ic+100 if ic to  cur. adrees +100 */
void address_update(lable lable_arr[], int IC, int lable_count)
{
    int i;
    if(lable_arr[0].name == NULL) /*if there are no labels*/
    {
        return;
    }
    /*Going through all the labels and updating the address */
    for(i = 0; i < lable_count; i++)
    {
        if(lable_arr[i].type == 2)  /* A label that is defined before a directive */
        {
            lable_arr[i].address += IC+100;
        }
        if(lable_arr[i].type == 1) /* A lable that is defined befor a command */
        {
            lable_arr[i].address +=100;
        }
    }
}


int check_entries(entry_l *entries, lable *lable_arr, int lable_count, char *file_name,int entries_count)
{
    char *cur_entry;
    int line_num, i,j, found = 0, err_flag = 0;
    
    for(j=0;j<entries_count;j++)
    {
        found = 0;
        cur_entry =entries[j].name;
        line_num = entries[j].line_num; 

        for (i = 0; i < lable_count; i++)
        {
            if (strcmp(cur_entry, lable_arr[i].name) == 0)
            {
                found = 1;
                if (lable_arr[i].type == 3)
                {
                    printf("\nERROR in line %d in file %s : lable set as entry and extern\n", line_num, file_name);
                    err_flag = 1;
                }
                break;
            }
        }
        
        if (found == 0 || lable_count==0)
        {
            printf("\nERROR in line %d in file %s : lable %s was not initialized\n", line_num, file_name, cur_entry);
            err_flag = 1;
        }

        free(cur_entry);        
    }
    if (err_flag)
    {
        return 1;
    }

    return 0;
}

int second_pass_update(code_word * codeword_arr ,unknown_lables * unknown_lables_arr , int unknown_lbl_count,lable *lable_arr,int lable_count,char* file_name)
{
    int i,j,found=0,err_flag=0;
    for(i=0; i<unknown_lbl_count;i++)
    {
       found=0;
       for(j=0;j<lable_count;j++)
        {
            if(strcmp(unknown_lables_arr[i].name,lable_arr[j].name)==0 && unknown_lables_arr[i].index!=-1)
            {
               codeword_arr[unknown_lables_arr[i].index].value|=(lable_arr[j].address<<3);
               if(lable_arr[j].type==2 || lable_arr[j].type==1){
                 codeword_arr[unknown_lables_arr[i].index].value|=LABLE_ARE;
               }
               else if(lable_arr[j].type==3)/* if lable is extern */
                 codeword_arr[unknown_lables_arr[i].index].value|=EXTERN_ARE;
               found=1;
               break; /* exeting loop when lable found */
            } 
        } 
        if((!found && unknown_lables_arr[i].index!=-1) || lable_count==0){
            err_flag=1;
            printf("\nERROR in line %d in file %s :lable %s is not initialized \n",unknown_lables_arr[i].line_num,file_name,unknown_lables_arr[i].name);
            } 
    }
    if(err_flag)
      return 1;
    return 0;
}

void create_files(char* file_name ,lable *lable_arr, int lable_count, int IC, int DC, int entries_count, code_word *codeword_arr,int * data_arr,unknown_lables * unknown_lables_arr, int unknown_lbl_count)
{
    FILE *ob,*entry,* extern_f;
    char *file_ob = NULL,*file_ent=NULL,*file_ext=NULL;
    size_t fileob_len = 0, file_e_len=0;    
    int i,j,first_ext=0,first_ent=0;
    unsigned int u_num;
    /* Creating ob file */
    fileob_len = strlen(file_name + 4); /*.ob and null terminated*/
    file_ob = (char *) malloc(fileob_len * sizeof(char));
    if(file_ob == NULL)
    {
        fprintf(stdout, "\nMemory allocation error\n");
        EXIT_ERROR;
    }


    strcpy(file_ob, file_name);
    strcat(file_ob, ".ob");
    ob = fopen(file_ob, "w");

    if(!ob)
    {
        printf("\nCan't open ob file for file %s\n", file_name);
        free(file_ob);
        EXIT_ERROR;
    }
    
    /* Writing the ic and dc in the begining of the ob file */
    fprintf(ob, "   %d ", IC);
    fprintf(ob, "%d\n", DC);
    for(i=0;i<IC;i++)
    { 
        if(i<1000)
        {
           fprintf(ob, "0%d ", (100 + i));  /* printing the adrress with 0 before*/
        }
        else
        {
            fprintf(ob, "%d ", (100 + i));  /* printing the adrress */
        }
        u_num=(unsigned) codeword_arr[i].value;
        fprintf(ob, "%05o\n",(u_num& 077777)); /* printing codeword */
    }

    for(i=0;i<DC;i++)
    {
        if((IC+100+i)<1000)
        {
           fprintf(ob, "0%d ",(IC+100+i));  /* printing the adrress with 0 before*/
        }
        else
        {
            fprintf(ob, "%d ", (IC+100+i));  /* printing the adrress */
        }
        u_num=(unsigned) data_arr[i];
        fprintf(ob, "%05o\n", (u_num & 077777)); /* printing codeword */
    }
    free(file_ob);
    fclose(ob);
    
    file_e_len = strlen(file_name + 5); /*.ent /ext  and null terminated*/

    /*checking and creating entry file (if there are entries) */   
    if(entries_count!=0)
    {
       /*create file*/
       file_ent = (char *) malloc(file_e_len * sizeof(char));
       if(file_ent == NULL)
       {
          fprintf(stdout, "\nMemory allocation error\n");
          EXIT_ERROR;
        }

       
       for(i=0;i<unknown_lbl_count;i++)
       {
        
          for(j=0;j<lable_count;j++)
          {
            if(strcmp(unknown_lables_arr[i].name,lable_arr[j].name)==0 && unknown_lables_arr[i].index==-1) /* if lable is entry */
            {
               
                if(first_ent==0)/* if file wasn't created yet */
                {
                    /*create file*/
                    strcpy(file_ent, file_name);
                    strcat(file_ent, ".ent");
                    entry = fopen(file_ent, "w");

                    if(!entry)
                    {
                        printf("\nCan't open entry file for file %s\n", file_name);
                        free(file_ent);
                        EXIT_ERROR;
                    }
                    first_ent=1;
                }
                /* updating the ext_file with lable data */
                fprintf(entry,"%s  %04d\n", unknown_lables_arr[i].name,(lable_arr[j].address));

            }  
          } 
        }
        free(file_ent);
        fclose(entry);
    }
    
    
    for(i=0;i<unknown_lbl_count;i++)
    {
        for(j=0;j<lable_count;j++)
        {
            if(strcmp(unknown_lables_arr[i].name,lable_arr[j].name)==0 && lable_arr[j].type==3) /* if lable is extern */
            {
               
                if(first_ext==0)/* if file wasn't created yet */
                {
                     /*create file*/
                    file_ext = (char *) malloc(file_e_len * sizeof(char));
                    if(file_ext == NULL)
                    {
                        fprintf(stdout, "\nMemory allocation error\n");
                        EXIT_ERROR;
                    }

                    strcpy(file_ext, file_name);
                    strcat(file_ext, ".ext");
                    extern_f = fopen(file_ext, "w");

                    if(!extern_f)
                    {
                        printf("\nCan't open extern file for file %s\n", file_name);
                        free(file_ext);
                        EXIT_ERROR;
                    }
                    first_ext=1;
                }
                /* updating the ext_file with lable data */
                fprintf(extern_f,"%s  %04d\n", unknown_lables_arr[i].name,(unknown_lables_arr[i].index+100));
            }  
        } 
    }
    if(first_ext!=0)
    {
        free(file_ext);
        fclose(extern_f);
    }
}



