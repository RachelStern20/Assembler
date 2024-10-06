#ifndef INSTERRORCHEK_H
#define INSTERRORCHEK_H

#define EXIT_ERROR exit(1)
/**
 * @brief correctness of the command name
 *
 * The function is checking the correctness of the command name
 *
 * @param str The line that we are cheking.
 * @param num_of_line The number of the line that we are cheking (in the am file).
 * @param file_name The name of the file we are at.
 * 
 * @return The function returns 0 if the line is valid and 1 otherwise.
 */
int command_name(char *str, int num_of_line, char *file_name);

/**
 * @brief correctness of the two operands functions
 *
 * The function checks the correctness of operands and commas in functions that receive 2 operands
 *
 * @param str The line we are checking does not include the name of the command.
 * @param num_of_line The number of the line that we are cheking (in the am file).
 * @param file_name The name of the file we are at.
 * @param instructions The array of instruction or directive names.
 * 
 * @return The function returns 0 if the operands and commas are valid and 1 otherwise.
 */
int check_two_arguments(char *str, char *command_name, int num_of_line, char *file_name, char *instructions[]);

/**
 * @brief correctness of the one operand functions
 *
 * The function checks the correctness of the operand in functions that receive 1 arguments
 *
 * @param str The line we are checking does not include the name of the command.
 * @param num_of_line The number of the line that we are cheking (in the am file).
 * @param file_name The name of the file we are at.
 * @param instructions The array of instruction or directive names.
 * 
 * @return The function returns 0 if the operand is valid and 1 otherwise.
 */
int check_one_argument(char *str, char *command_name, int num_of_line, char *file_name, char *instructions[]);

/**
 * @brief Same as instruction or directive
 *
 * The function checks whether the operand passed to it is the same as an instruction or directive 
 *
 * @param str The operand from one of the functions (2-operands or 1-operand functions) which we compare to 
 * instruction and directive names
 * @param start The starting index for the array - from which index to start checking.
 * @param finish The ending index for the array - up to which index to check.
 * @param instructions The array of instruction or directive names.
 * 
 * @return The function returns 1 if the operand is equal to instruction\directive and 0 otherwise.
 */
int same_as_directive_or_instruction(char *str, int start, int finish, char *instructions[]);

/**
 * @brief validation of the first operand in the two operands functions
 *
 * The function checks the correctness of the addressing method for the first operand - the source operand,
 *  in functions that accept 2 operands
 *
 * @param str The first operand from one of the two operands functions - which we are cheking.
 * @param command_name The name of the command whose operand we are checking
 * @param num_of_line The number of the line that we are cheking (in the am file).
 * @param file_name The name of the file we are at.
 * @param instructions The array of instruction or directive names.
 * 
 * @return The function returns 0 if the operand is valid and 1 otherwise.
 */
int chek_first_operand_validation(char *str, char *command_name, int num_of_line, char *file_name, char *instructions[]);

/**
 * @brief validation of the second operand in the two operands functions
 *
 * The function checks the correctness of the addressing method for the second operand - the target operand,
 *  in functions that accept 2 operands
 *
 * @param str The second operand from one of the two operands functions - which we are cheking.
 * @param command_name The name of the command whose operand we are checking
 * @param num_of_line The number of the line that we are cheking (in the am file).
 * @param file_name The name of the file we are at.
 * @param instructions The array of instruction or directive names.
 * 
 * @return The function returns 0 if the operand is valid and 1 otherwise.
 */
int chek_second_operand_validation(char *str, char *command_name, int num_of_line, char *file_name, char *instructions[]);

/**
 * @brief validation of the operand in the one operand functions
 *
 * The function checks the correctness of the addressing method for the operand 
 *  in functions that accept 1 operand
 *
 * @param str The operand from one of the one operands functions - which we are cheking.
 * @param command_name The name of the command whose operand we are checking
 * @param num_of_line The number of the line that we are cheking (in the am file).
 * @param file_name The name of the file we are at.
 * @param instructions The array of instruction or directive names.
 * 
 * @return The function return 0 if the operand is valid and 1 otherwise.
 */
int chek_operand_validation(char *str, char *command_name, int num_of_line, char *file_name, char *instructions[]);

#endif
