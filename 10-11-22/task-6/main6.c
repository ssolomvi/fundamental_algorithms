#include "post6.h"
#define VALID_USER_INPUT(x) ((x) == '1' || (x) == '2' || (x) == '3' || (x) == '4' || (x) == '5' || (x) == '6' || (x) == '7' || (x) == 8 || (x) == 9)
#define SIZBUF 8

void help();
void help_file_format();

int main(int argc, char** argv)
{
	// TODO: demonstrate string.h functionality
	// TODO: add mail_id check (if id is alreday in use)
	if (argc != 2) {
		printf("Incorrect count of command line arguments.\nPass only the current time date in format {day}.{month}.{year} {hour}:{minutes}:{seconds}\n");
		return -1;
	}

	if (!is_valid_time(argv[1])) {
		printf("Invalid time passed in command line\n. Pass the current time date in format {day}.{month}.{year} {hour}:{minutes}:{seconds}\n");
		return -2;
	}

	FILE* in = NULL, * out = NULL;
	string user_input, user_stream;
	user_input.str = NULL; user_stream.str = NULL;
	string stdin_string, stdout_string;
	stdin_string.str = "stdin", stdout_string.str = "stdout";
	stdin_string.length = 5; stdout_string.length = "6";

	help();

	post my_post;
	my_post.count_of_mails = 0;
	my_post.this_post_address = NULL;
	my_post.this_post_mails = NULL;
	
	post_statuses post_s = -1;
	mail* collection = NULL, to_find;
	size_t to_find_ind = 0;
	unsigned int collection_count = 0;
	
	int exited = 1;

	while (exited)
	{
		printf(">>> ");
		
		user_input = create_string(stdin, SIZBUF, '\n');
		if (!user_input.length && !user_input.str) {
			printf("Error reading user input\n");
			delete_post(&my_post);
			return -4;
		}

		if (!VALID_USER_INPUT(*(user_input.str)) || user_input.length != 1) {
			printf("Invalid input! Try again...\n\n");
			delete_string(&user_input);
			delete_post(&my_post);
			continue;
		}

		switch (*(user_input.str) - '0')
		{
		case 1:
			if (my_post.count_of_mails == 0) {
				printf("Enter address where you want your post to be in format\n\t{string} {string} {value > 0} {value > 0} {6 digits}\nRemember, it must be correct\n>>> ");
				my_post.this_post_address = get_address(stdin);
				if (isnan(my_post.this_post_address->house)) {
					printf("Error reading post address\n");
					delete_string(&user_input);
					delete_post(&my_post);
					return -5;
				}
			}

			printf("Enter stream from where you want to add mails to post:\n>>> ");

			user_stream = create_string(stdin, FILENAME_MAX, '\n');
			if (!user_stream.length && !user_stream.str) {
				printf("Error reading user input\n");
				delete_string(&user_input);
				delete_post(&my_post);
				return -6;
			}

			if (!compare_strings(user_stream, stdin_string, lexic_comparator)) {
				post_s = read_mails(stdin, &my_post);
			}
			else {
				if (!(in = fopen(user_stream.str, "r"))) {
					printf("File open error\n");
					delete_string(&user_input);
					delete_string(&user_stream);
					delete_post(&my_post);
					return -7;
				}

				post_s = read_mails(in, &my_post);

				fclose(in);
			}
			delete_string(&user_stream);

			switch (post_s)
			{
			case post_incorrect_ptr_to_file_passed:
				printf("Incorrect ptr to file passed in function read_mails\n");
				delete_string(&user_input);
				delete_post(&my_post);
				return -8;
			case post_realloc_error:
				printf("Memory reallocation error happened\n");
				delete_string(&user_input);
				delete_post(&my_post);
				return -9;
			case post_incorrect_input_file_format:
				printf("Your input data is incorrect. Type 9 for help next time\n");
				delete_string(&user_input);
				delete_post(&my_post);
				return -10;
			default:
				printf("New mails were successfully added to your post\n");
				break;
			}
			post_s = -1;
			break;

		case 2:
			printf("Enter mail_id to delete:\n>>> ");
			if (scanf("%s", to_find.mail_id) != 1) {
				printf("Error readind mail_id\n");
				delete_string(&user_input);
				delete_post(&my_post);
				return -11;
			}
			if (!is_valid_mail_id(to_find.mail_id)) {
				printf("Incorrect mail_id passed. Try again!\n");
			}
			else {
				delete_mail(to_find, &my_post);
				printf("A mail was successfully deleted from your post\n");
			}

			break;

		case 3:
			printf("Enter mail_id to print:\n>>> ");
			if (scanf("%s", to_find.mail_id) != 1) {
				printf("Error readind mail_id\n");
				delete_string(&user_input);
				delete_post(&my_post);
				return -12;
			}
			if (!is_valid_mail_id(to_find.mail_id)) {
				printf("Incorrect mail_id passed. Try again!\n");
			}
			else {
				printf("Enter stream where you want to print a mail:\n>>> ");

				user_stream = create_string(stdin, FILENAME_MAX, '\n');
				if (!user_stream.length && !user_stream.str) {
					printf("Error reading user input\n");
					delete_string(&user_input);
					delete_post(&my_post);
					return -13;
				}

				if (!compare_strings(user_stream, stdout_string, lexic_comparator)) {
					print_mail(stdout, *(find_mail(to_find, my_post, mail_id_comparator, &to_find_ind)));
				}
				else {
					if (!(in = fopen(user_stream.str, "w"))) {
						printf("File open error\n");
						delete_string(&user_input);
						delete_string(&user_stream);
						delete_post(&my_post);
						return -14;
					}

					print_mail(in, *(find_mail(to_find, my_post, mail_id_comparator, &to_find_ind)));
					fclose(in);
					printf("The mail was successfully printed in %s file\n", user_stream.str);
				}
				delete_string(&user_stream);
			}
			break;

		case 4:
			printf("Enter stream where you want to print your post:\n>>> ");

			user_stream = create_string(stdin, FILENAME_MAX, '\n');
			if (!user_stream.length && !user_stream.str) {
				printf("Error reading user input\n");
				delete_string(&user_input);
				delete_post(&my_post);
				return -15;
			}

			if (!compare_strings(user_stream, stdout_string, lexic_comparator)) {
				print_post(stdout, my_post);
			}
			else {
				if (!(in = fopen(user_stream.str, "w"))) {
					printf("File open error\n");
					delete_string(&user_input);
					delete_string(&user_stream);
					delete_post(&my_post);
					return -16;
				}
				
				print_post(in, my_post);
				fclose(in);
				printf("Your post was successfully printed in %s file\n", user_stream.str);
			}
			delete_string(&user_stream);
			break;

		case 5:
			post_s = find_delivered_mails(&collection, &collection_count, my_post, argv[1]);
			switch (post_s)
			{
			case post_incorrect_ptr_to_collection_passed:
				printf("Incorrect pointer to collection passed. It must be NULL\n");
				delete_string(&user_stream);
				delete_post(&my_post);
				return -17;
			case post_realloc_error:
				printf("Memory reallocation error happened\n");
				delete_string(&user_stream);
				delete_post(&my_post);
				return -18;
			default:
				if (!collection_count) {
					printf("There are 0 found delivered mails at your post\n");
				}
				else {
					printf("Enter stream where you want to print your found delivered mails:\n>>> ");

					user_stream = create_string(stdin, FILENAME_MAX, '\n');
					if (!user_stream.length && !user_stream.str) {
						printf("Error reading user input\n");
						if (collection)
							free(collection);
						delete_string(&user_input);
						delete_post(&my_post);
						return -19;
					}

					if (!compare_strings(user_stream, stdout_string, lexic_comparator)) {
						print_mail_collection(stdout, collection, collection_count);
					}
					else {
						if (!(in = fopen(user_stream.str, "w"))) {
							printf("File open error\n");
							if (collection)
								free(collection);
							delete_string(&user_input);
							delete_string(&user_stream);
							delete_post(&my_post);
							return -20;
						}

						print_mail_collection(in, collection, collection_count);
						fclose(in);
						printf("Found delivered mails were successfully printed in %s file\n", user_stream.str);
					}
					delete_string(&user_stream);
				}
				break;
			}
			post_s = -1;
			if (collection)
				free(collection);
			collection = NULL;
			collection_count = 0;
			break;

		case 6:
			post_s = find_expired_mails(&collection, &collection_count, my_post, argv[1]);
			switch (post_s)
			{
			case post_incorrect_ptr_to_collection_passed:
				printf("Incorrect pointer to collection passed. It must be NULL\n");
				delete_string(&user_stream);
				delete_post(&my_post);
				return -17;
			case post_realloc_error:
				printf("Memory reallocation error happened\n");
				delete_string(&user_stream);
				delete_post(&my_post);
				return -18;
			default:
				if (!collection_count) {
					printf("There are 0 found delivered mails at your post\n");
				}
				else {
					printf("Enter stream where you want to print your found expired mails:\n>>> ");

					user_stream = create_string(stdin, FILENAME_MAX, '\n');
					if (!user_stream.length && !user_stream.str) {
						printf("Error reading user input\n");
						if (collection)
							free(collection);
						delete_string(&user_input);
						delete_post(&my_post);
						return -19;
					}

					if (!compare_strings(user_stream, stdout_string, lexic_comparator)) {
						print_mail_collection(stdout, collection, collection_count);
					}
					else {
						if (!(in = fopen(user_stream.str, "w"))) {
							printf("File open error\n");
							if (collection)
								free(collection);
							delete_string(&user_input);
							delete_string(&user_stream);
							delete_post(&my_post);
							return -20;
						}

						print_mail_collection(in, collection, collection_count);
						fclose(in);
						printf("Found expired mails were successfully printed in %s file\n", user_stream.str);
					}
					delete_string(&user_stream);
				}
				break;
			}
			post_s = -1;
			if (collection)
				free(collection);
			collection = NULL;
			collection_count = 0;
			break;

		case 7:
			delete_post(&my_post);
			break;
		case 8:
			exited = 0;
			break;
		case 9:
			help_file_format();
			break;
		}

		delete_string(&user_input);
	}

	if (my_post.this_post_mails) {
		delete_post(&my_post);
	}

	return 0;
}

void help()
{
	printf("The programm works with post struct. Choices:\n");
	printf("\t1 -- add mail(s)\n");
	printf("\t2 -- delete certain mail\n");
	printf("\t3 -- print certain mail\n");
	printf("\t4 -- print all mails of this post\n");
	printf("\t5 -- find and print a collection of delivered mails\n");
	printf("\t6 -- find and print a collection of expired mails\n");
	printf("\t7 -- delete the whole post\n");
	printf("\t8 -- exit\n");
	printf("\t9 -- get help about input file format\n");
	printf("You choose:\n");
}

void help_file_format()
{
	printf("Fields of structures are divided with space\nThere must be an empty string between two mails\n");
	printf("City, street are strings with their first letters capital and other lowercase\n");
	printf("House, flat are nonnegative whole numbers\n");
	printf("Index, mail_id are of %d and %d length arrays of numbers\n", index_count_of_numbers, mail_id_count_of_numbers);
	printf("Weight is a floating point number > 0\n");
	printf("time_of_creation and delivery_time are strings in format {day}.{month}.{year} {hour}:{minutes}:{seconds}\n");
	printf("Correct file format:\n");
	printf("{city} {street} {house} {flat} {index}\n");
	printf("{city} {street} {house} {flat} {index}\n");
	printf("{weigth} {mail_id} {time_of_creation} {delivery_time}\n\n"); 
	printf("{city} {street} {house} {flat} {index}\n");
	printf("{city} {street} {house} {flat} {index}\n");
	printf("{weigth} {mail_id} {time_of_creation} {delivery_time}\n\n");
	printf("...\n\n");
}