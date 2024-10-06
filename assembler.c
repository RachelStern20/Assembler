#include "first_pass.h"
#include "second_pass.h"
#include "pre_processor.h"



int main(int argc, char *argv[])
{
   FILE *fd, *am;
   int i, mc, lable_count, IC, DC, unknown_lbl_count, error,am_line_count,entries_count; 
   char *file_name, *file_name_am;
   size_t filename_len, len_am_file;
   Node *macro_list;
   lable *lable_arr;
   entry_l *entries;
   int data_arr[3996];
   code_word codeword_arr[3996];
   unknown_lables * unknown_lables_arr;  


   if(argc < 2)
   {
       printf("\nERROR: no input files\n");
       EXIT_ERROR;
   }   
  
   /* Go through all the transferred files in the command line */
   for(i = 1; i < argc; i++)
   {
     
       lable_count = 0;
       IC = 0;
       DC = 0;
       unknown_lbl_count = 0;
       macro_list = NULL;
       entries_count=0;  


       lable_arr = (lable *)malloc(1 * sizeof(lable));
       unknown_lables_arr = (unknown_lables *)malloc(1 * sizeof(unknown_lables));
       entries = (entry_l *)malloc(1 * sizeof(entry_l)); /* Initialize as an empty string */


       if(!unknown_lables_arr || !lable_arr || !entries)
       {
           fprintf(stderr, "\nMemory allocation error\n");
           EXIT_ERROR;
       }


       /* Opening the as file to read */
       filename_len = strlen(argv[i]) + 4; /* .as and null terminated */
       file_name = (char *)malloc(filename_len * sizeof(char));
       if(file_name == NULL)
       {
           fprintf(stderr, "\nMemory allocation error\n");
           EXIT_ERROR;
       }
      
       strcpy(file_name, argv[i]);
       strcat(file_name, ".as");


      
       if(!(fd = fopen(file_name, "r")))
       {
           fprintf(stderr, "\nCan't open file %s for read\n", file_name);
           free_macro_list(&macro_list);
           free(file_name);
           free(lable_arr);
           free(unknown_lables_arr);
           free(entries);
           continue;
       }


      /* Sending to macro function - A function that interprets the macros */
       mc = macro(fd, argv[i], file_name, &macro_list, &am_line_count);
       if(mc) /* If there was error in the macro */
       {
           fclose(fd);
           free_macro_list(&macro_list);
           free(file_name);
           free(lable_arr);
           free(unknown_lables_arr);
           free(entries);
           continue;
       }
       fclose(fd);
 
       /* Opening the am file to read */
       len_am_file = strlen(argv[i]) + 4; /* Add 4 bytes for ".am" extension */
       file_name_am = (char *)malloc(len_am_file * sizeof(char));
       if(file_name_am == NULL)
       {
           fprintf(stderr, "\nMemory allocation error\n");
           free(file_name);
           free(lable_arr);
           free(unknown_lables_arr);
           free(entries);
           free_macro_list(&macro_list);
           EXIT_ERROR;
       }


       strcpy(file_name_am, argv[i]);
       strcat(file_name_am, ".am");
       if(!(am = fopen(file_name_am, "r")))
       {
           fprintf(stderr, "\nCan't open file %s for read\n", file_name_am);
           free_macro_list(&macro_list);
           free(file_name);
           free(lable_arr);
           free(unknown_lables_arr);
           free(entries);
           continue;
       }

       /* Sending to the first pass function */
       error = first_pass(am, &lable_arr, &lable_count, &IC, &DC, &entries, codeword_arr, &unknown_lables_arr, &unknown_lbl_count, file_name_am, data_arr, macro_list,am_line_count,&entries_count);
       /* Sending to the pre second pass function */
       address_update(lable_arr, IC, lable_count);

       /* Sending to the second pass function*/
       error |= second_pass_update(codeword_arr, unknown_lables_arr, unknown_lbl_count, lable_arr, lable_count, file_name_am);
       error |= check_entries(entries, lable_arr, lable_count, file_name,entries_count);
      
       /* If there wasn't error - sending to the create files function */
       if(!error)
       {
           create_files(argv[i], lable_arr, lable_count, IC, DC, entries_count, codeword_arr, data_arr,unknown_lables_arr,unknown_lbl_count);
       }
      
       /* Freeing all the memory for the next as file */
       free_macro_list(&macro_list);
       macro_list = NULL; /* Reset the head for the next file */
       free(file_name);
       free_lables_array(lable_arr,lable_count,unknown_lables_arr,unknown_lbl_count,entries,entries_count);
       free(file_name_am);
       fclose(am);
   }
   return 0;
}


void free_lables_array(lable * lable_arr,int lable_count, unknown_lables * unknown_lables_arr, int unknown_lbl_count,entry_l * entries,int entries_count)
{
   int i;
   for(i=0;i<lable_count;i++)
   {
       free(lable_arr[i].name);
   }
   free(lable_arr);
   for(i=0;i<unknown_lbl_count;i++)
   {
       free(unknown_lables_arr[i].name);
   }
   free(unknown_lables_arr);
   
   free(entries);
}







