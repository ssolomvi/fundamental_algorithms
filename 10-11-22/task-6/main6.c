#include "post6.h"
#define VALID_USER_INPUT(x) ((x) == '1' || (x) == '2' || (x) == '3' || (x) == '4' || (x) == '5' || (x) == '6' || (x) == '7' || (x) == '8' || (x) == '9')
#define SIZBUF 8

void help();
void help_file_format();

void init_address(address* my_init)
{
	*my_init = (address){ .city.str = NULL, .street.str = NULL, .house = 0, .flat = 0 };
}

int main(int argc, char** argv)
{
	printf("Demonstration of my string.h functionality:\n");
	string my_str, copy_str, _new_str, double_str, catted_string;
	printf("Enter something for my_str:\n>>> ");
	my_str = create_string(stdin, 0, '\n');
	printf("Enter something once more for copy_str:\n>>> ");
	copy_str = create_string(stdin, 0, '\n');
	printf("my_str data:\n");
	print_string(stdout, my_str);
	printf("copy_str data:\n");
	print_string(stdout, copy_str);
	int strs_diff = 0;
	if ((strs_diff = compare_strings(my_str, copy_str, lexic_comparator) == 0)) {
		printf("my_str is equal to copy_str!\n");
	}
	else if (strs_diff > 0) {
		printf("my_str is lexicographically greater than copy_str!\n");
	}
	else {
		printf("my_str is lexicographically less than copy_str!\n");
	}
	printf("Enter something once again for _new_str:\n>>> ");
	_new_str = create_string(stdin, 0, '\n');
	printf("Let's concatenate my_str, copy_str and _new_str:\n");
	catted_string = cat_strings(3, &my_str, &copy_str, &_new_str);
	print_string(stdout, catted_string);
	printf("Let's copy my_str data to copy_str:\n");
	copy_string(&copy_str, my_str);
	print_string(stdout, copy_str);
	printf("Let's double _new_str:\n");
	double_str = double_string(_new_str);
	print_string(stdout, _new_str);
	delete_string(&my_str);
	delete_string(&copy_str);
	delete_string(&_new_str);
	delete_string(&double_str);
	delete_string(&catted_string);
	printf("That's all. Now the post functionality!\n\n");

	if (argc != 3) {
		printf("Incorrect count of command line arguments.\nPass only the current time date in format {day}.{month}.{year} {hour}:{minutes}:{seconds}\n");
		return -1;
	}

	time current_time = (time){ .year = 0, .month = 0, .day = 0, .hour = 0, .min = 0, .sec = 0 };

	if ((sscanf(argv[1], "%u.%u.%u", &(current_time.day), &(current_time.month), &(current_time.year))) != 3 ||
		(sscanf(argv[2], "%u:%u:%u", &(current_time.hour), &(current_time.min), &(current_time.sec))) != 3 ||
		!is_valid_time(current_time)) {
		printf("Error reading current time from command line\n");
		return -2;
	}

	help();
	int exited = 0;

	post my_post = (post){ .count_of_mails = 0, .this_post_mails = NULL };
	my_post.this_post_address = &(address) { .city.str = NULL, .street.str = NULL, .house = 0, .flat = 0 };
	mail to_delete, to_find, * collection = NULL;
	size_t count_collection = 0, to_find_ind = 0;

	post_statuses post_s = -1;
	FILE* in = NULL, * out = NULL;
	string user_input, user_stream, stdin_string = (string){ .length = 5, .str = "stdin" }, stdout_string = (string){ .length = 6, .str = "stdout" };

	while (!exited)
	{
		printf(">>> ");

		user_input = create_string(stdin, SIZBUF, '\n');
		if (!user_input.length && !user_input.str) {
			printf("Error reading user input\n");
			delete_post(&my_post);
			return -3;
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
			if (!my_post.count_of_mails) {
				printf("Enter address where you want your post to be in format\n\t{string} {string} {value > 0} {value > 0} {6 digits}\nRemember, it must be correct\n>>> ");
				// e.g. Moscow Bolshoy 10 12 892047
				get_address(stdin, my_post.this_post_address);
				if (!my_post.this_post_address->house) {
					printf("Error reading post address\n");
					delete_string(&user_input);
					delete_post(&my_post);
					return -4;
				}
			}

			printf("Enter stream from where you want to add mails to post:\n>>> ");
			user_stream = create_string(stdin, FILENAME_MAX, '\n');
			if (!user_stream.length && !user_stream.str) {
				printf("Error reading user input\n");
				delete_string(&user_input);
				delete_post(&my_post);
				return -5;
			}

			if (!compare_strings(user_stream, stdin_string, lexic_comparator)) {
				printf("Enter mails data:\n");
				post_s = read_mails(stdin, &my_post);
				/*
				Kak Sdat 8 19 561324
Labs Ilya 7 6 434241
36.56 12151417181920 12.10.2022 15:15:23 18.10.2022 18:19:22
				*/
			}
			else {
				if (!(in = fopen(user_stream.str, "r"))) {
					printf("File open error\n");
					delete_string(&user_input);
					delete_string(&user_stream);
					delete_post(&my_post);
					return -6;
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
				return -7;
			case post_realloc_error:
				printf("Memory reallocation error happened\n");
				delete_string(&user_input);
				delete_post(&my_post);
				return -8;
			case post_incorrect_input_file_format:
				printf("Your input data is incorrect. Type 9 for help next time\n");
				delete_string(&user_input);
				delete_post(&my_post);
				return -9;
			default:
				printf("New mails were successfully added to your post\n");
				break;
			}
			post_s = -1;
			sort_recipient_n_mail_id(my_post);
			break;
		case 2:
			if (!my_post.count_of_mails) {
				printf("There are no mails in your post\n");
				break;
			}
			printf("Enter mail id of mail to delete:\n>>> ");
			if (fscanf(stdin, "%s", to_delete.mail_id) != 1) {
				printf("Error readind mail_id\n");
				delete_string(&user_input);
				delete_post(&my_post);
				return -10;
			}
			if (!is_valid_mail_id(to_delete.mail_id)) {
				printf("Incorrect mail_id passed. Try again!\n");
			}
			else {
				post_s = delete_mail(to_delete, &my_post);
				if (post_s == post_empty_post)
					printf("There are no mails in your post\n");
				if (post_s = post_realloc_error) {
					printf("Memory reallocation error happened\n");
					delete_string(&user_input);
					delete_post(&my_post);
					return -11;
				}
				if (post_s == post_no_match) {
					printf("There are no mails with such mail_id in your post\n");
					delete_string(&user_input);
				}
				else
					printf("A mail was successfully deleted from your post\n");
			}
			fgetc(stdin);
			break; 
		case 3:
			if (!my_post.count_of_mails) {
				printf("There are no mails in your post\n");
				break;
			}
			printf("Enter mail id of mail to print:\n>>> ");
			if (fscanf(stdin, "%s", to_find.mail_id) != 1) {
				printf("Error readind mail_id\n");
				delete_string(&user_input);
				delete_post(&my_post);
				return -12;
			}
			if (!is_valid_mail_id(to_find.mail_id)) {
				printf("Incorrect mail_id passed. Try again!\n");
			}
			else {
				fgetc(stdin);
				printf("Enter stream where you want to print a mail:\n>>> ");

				user_stream = create_string(stdin, FILENAME_MAX, '\n');
				if (!user_stream.length && !user_stream.str) {
					printf("Error reading user input\n");
					delete_string(&user_input);
					delete_post(&my_post);
					return -13;
				}

				if (!compare_strings(user_stream, stdout_string, lexic_comparator)) {
					collection = (find_mail(to_find, my_post, mail_id_comparator, &to_find_ind));
					if (!collection) {
						printf("There are no mails with such mail_id in your post\n");
						delete_string(&user_stream);
						break;
					}
					print_mail(stdout, *collection);
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
			post_s = find_delivered_mails(&collection, &count_collection, my_post, current_time);
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
				if (!count_collection) {
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
						print_mail_collection(stdout, collection, count_collection);
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

						print_mail_collection(in, collection, count_collection);
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
			count_collection = 0;
			break; 
		case 6:
			post_s = find_expired_mails(&collection, &count_collection, my_post, current_time);
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
				if (!count_collection) {
					printf("There are 0 found expired mails at your post\n");
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
						print_mail_collection(stdout, collection, count_collection);
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

						print_mail_collection(in, collection, count_collection);
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
			count_collection = 0;
			break; 
		case 7:
			delete_post(&my_post);
			printf("Your post was deleted\n");
			break; 
		case 8:
			exited++;
			break; 
		case 9:
			help_file_format();
			break; 
		}
	}

	delete_post(&my_post);

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
