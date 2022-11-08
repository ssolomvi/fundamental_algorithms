#include "header4.h"

read_message_statuses read_message(FILE* fi, message* mess, unsigned int* last_id, char* stop_lexeme)
{
	if (!fi) 
		return read_message_incorrect_ptr_to_file;

	if (!stop_lexeme)
		return read_message_incorrect_ptr_to_stop_lexeme;

	int size = 0, size_to_read = 1024, actual_size = 0;
	char* tmp = NULL, * message_str = NULL;

	while (!feof(fi)) {
		size += size_to_read;
		if (!(tmp = (char*)realloc(message_str, sizeof(char) * size))) {
			if (message_str) {
				free(message_str);
			}
			return read_message_realloc_error;
		}
		message_str = tmp;

		if (!fgets(message_str, size_to_read, fi)) {
			free(message_str);
			return read_message_fgets_error;
		}

		actual_size = strlen(message_str);
		// the end of file / string found before reading all size_to_read chars
		if (actual_size % (size_to_read - 1) != 0) {
			break;
		}
	}

	if (message_str[actual_size - 1] == '\n') {
		if (!(tmp = (char*)realloc(message_str, sizeof(char) * actual_size))) {
			free(message_str);
			return read_message_realloc_error;
		}
		message_str = tmp;
		message_str[actual_size - 1] = '\0';
	}
	else {
		if (!(tmp = (char*)realloc(message_str, sizeof(char) * (actual_size + 1)))) {
			free(message_str);
			return read_message_realloc_error;
		}
		message_str = tmp;
		message_str[actual_size] = '\0';
	}


	int i, j, ind = 0, stop_lex_len = strlen(stop_lexeme);
	for (i = 0; i < actual_size; i++) {
		if ((i + stop_lex_len) <= actual_size && message_str[i] == stop_lexeme[ind]) {
			j = i;
			while (message_str[j] == stop_lexeme[ind] && ind < stop_lex_len) {
				j++; ind++;
			}
			if (ind == stop_lex_len) {
				break; // stop lexeme found
			}
			else {
				ind = 0;
			}
		}
	}

	(*mess).id = (*last_id);
	(*last_id)++;

	if (i != actual_size) {
		if (!(tmp = (char*)realloc(message_str, sizeof(char) * (i + 1)))) {
			free(message_str);
			return read_message_realloc_error;
		}
		message_str = tmp;
		message_str[i] = '\0';

		(*mess).message = message_str;
		(*mess).len_bytes = strlen(message_str) + 1;

		if (i == 0) {
			return read_message_stop_word_found;
		}

		return read_message_stop_word_found;
	}
	
	(*mess).message = message_str;
	(*mess).len_bytes = strlen(message_str) + 1;
	return read_message_no_stop_word_found;
}

in_csv_file_statuses in_csv_file(FILE* fi, message mess)
{
	if (!fi) {
		return in_csv_file_incorrect_file_ptr;
	}

	if (!mess.message || mess.id < 0 || mess.len_bytes < 0) {
		return in_csv_file_incorrect_message_passed;
	}

	char* to_put = NULL, * tmp = NULL, ch = 0;
	int mess_len = strlen(mess.message), total_size = mess_len, i = 0, j = 0;
	
	if (!(to_put = (char*)malloc(sizeof(char) * (mess_len + 1)))) {
		return in_csv_file_malloc_error;
	}

	for (i; i < mess_len; i++) {
		if (j >= mess_len - 1) {
			if (!(tmp = (char*)realloc(to_put, sizeof(char) * (total_size + 1)))) {
				free(to_put);
				return in_csv_file_realloc_error;
			}
			to_put = tmp;
		}
		ch = mess.message[i];
		if (ch == '"') {
			total_size++;
			to_put[j++] = ch;
		}

		to_put[j++] = ch;
	}

	if (!(tmp = (char*)realloc(to_put, sizeof(char) * (total_size + 1)))) {
		free(to_put);
		return in_csv_file_realloc_error;
	}
	to_put = tmp;
	to_put[total_size] = '\0';

	fprintf(fi, "%u,%u,\"%s\"\n", mess.id, mess.len_bytes, to_put);

	free(to_put);
	return in_csv_file_ok;
}

read_n_write_csv_statuses read_n_write_csv(FILE* _in, FILE* _out, char* stop_lexeme)
{
	if (!_in || !_out) {
		return read_n_write_file_ptr_error;
	}

	int last_id = 0;
	message mess;
	read_message_statuses read_message_s = -1;
	in_csv_file_statuses in_csv_file_s = -1;

	do {
		read_message_s = read_message(_in, &mess, &last_id, stop_lexeme);
		switch (read_message_s)
		{
		case read_message_incorrect_ptr_to_file:
			return read_n_write_file_ptr_error;
		case read_message_incorrect_ptr_to_stop_lexeme:
			return read_n_write_incorrect_stop_lexeme;
		case read_message_realloc_error:
			return read_n_write_realloc_error;
		case read_message_fgets_error:
			return read_n_write_fgets_error;
		default:
			break;
		}

		if (!(mess.len_bytes == 1 && read_message_s == read_message_stop_word_found)) {
			in_csv_file_s = in_csv_file(_out, mess);
			switch (in_csv_file_s)
			{
			case in_csv_file_incorrect_file_ptr:
				free(mess.message);
				return read_n_write_file_ptr_error;
			case in_csv_file_incorrect_message_passed:
				free(mess.message);
				return read_n_write_message_error;
			case in_csv_file_malloc_error:
				free(mess.message);
				return read_n_write_malloc_error;
			case in_csv_file_realloc_error:
				free(mess.message);
				return read_n_write_realloc_error;
			default:
				break;
			}
		}

		free(mess.message);

	} while (read_message_s != read_message_stop_word_found);
	return read_n_write_ok;
}

char* gen_file_name()
{
	char* fileName = (char*)malloc(sizeof(char) * (fileNameLen + 1));
	int i;
	for (i = 0; i < fileNameLen - 4; i++) {
		if (rand() % 2) {
			fileName[i] = rand() % 10 + '0';
		}
		else {
			if (rand() % 2) {
				fileName[i] = rand() % 26 + 'a';
			}
			else {
				fileName[i] = rand() % 26 + 'A';
			}
		}
	}
	// 1.csv len -- 5
	fileName[fileNameLen - 4] = '.'; fileName[fileNameLen - 3] = 'c';
	fileName[fileNameLen - 2] = 's'; fileName[fileNameLen - 1] = 'v';
	fileName[fileNameLen] = '\0';

	return fileName;
}

void clear(message** messes, int* mess_count)
{
	int i;
	for (i = 0; i < (*mess_count); i++) {
		free((*messes)[i].message);
	}
	free(*messes);
}

read_from_csv_file_statuses read_from_csv_file(FILE* fi, message** messes, int* mess_count)
{
	if (!fi) {
		return read_from_csv_file_incorrect_ptr_to_file;
	}

	if (*messes) {
		return read_from_csv_file_incorrect_ptr_to_messages_arr;
	}

	(*mess_count) = 0;

	message* tmp = NULL;
	int size = 8, i = 0;

	if (!((*messes) = (message*)malloc(sizeof(message) * size))) {
		return read_from_csv_file_malloc_error;
	}

	char ch = 0, prev_ch = 0;
	unsigned int id = 0, len_in_bytes = 0;

	while (!feof(fi)) {
		id = 0; len_in_bytes = 0;
		if ((*mess_count) >= size - 1) {
			size *= 2;
			if (!(tmp = (message*)realloc((*messes), sizeof(message) * size))) {
				clear(messes, mess_count);
				return read_from_csv_file_realloc_error;
			}
			(*messes) = tmp;
		}

		(*messes)[(*mess_count)].id = 0;
		while ((ch = fgetc(fi)) != ',') {
			if (ch == EOF) {
				return read_from_csv_file_ok;
			}
			id *= 10;
			id += ch - '0';
		}
		(*messes)[(*mess_count)].id = id;

		(*messes)[(*mess_count)].len_bytes = 0;
		while ((ch = fgetc(fi)) != ',') {
			len_in_bytes *= 10;
			len_in_bytes += ch - '0';
		}
		(*messes)[(*mess_count)].len_bytes = len_in_bytes;

		if (!((*messes)[(*mess_count)].message = (char*)malloc(sizeof(char) * len_in_bytes))) {
			clear(messes, mess_count);
			return read_from_csv_file_realloc_error;
		}

		fgetc(fi);
		i = 0;
		while ((ch = fgetc(fi)) != '\n' && ch != EOF) {
			if (prev_ch == '"' && ch == '"') {
				prev_ch = 0;
				continue;
			}
			(*messes)[(*mess_count)].message[i++] = ch;
			
			prev_ch = ch;
		}
		(*messes)[(*mess_count)++].message[len_in_bytes - 1] = '\0';
	}

	return read_from_csv_file_ok;
}

void print_message_arr(FILE* stream, message* mess, int mess_count)
{
	if (!mess_count) {
		fprintf(stream, "0 messages were read\n");
		return;
	}

	if (!mess) {
		fprintf(stream, "Incorrect ptr passed\n");
		return;
	}

	int i;
	for (i = 0; i < mess_count; i++) {
		fprintf(stream, "%u:: %s\nLen in bytes: %u\n\n", mess[i].id, mess[i].message, mess[i].len_bytes);
	}
}