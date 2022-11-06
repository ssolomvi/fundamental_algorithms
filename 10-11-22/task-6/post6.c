#include "post6.h"

void delete_address(address* to_del)
{
	if (!to_del) {
		return;
	}
	delete_string(&(to_del->city));
	delete_string(&(to_del->street));
}

void delete_mail_array(mail** arr, size_t* count_of_mails)
{
	if (!(*count_of_mails) || !(*arr)) {
		return;
	}

	size_t i;
	for (i = 0; i < (*count_of_mails); i++) {
		delete_address(&(*arr)[i].sender);
		delete_address(&(*arr)[i].recipient);
	}
	free(*arr);
	*arr = NULL;
	*count_of_mails = 0;
}

void delete_post(post* to_delete)
{
	if (!to_delete || to_delete->count_of_mails == 0) {
		return;
	}
	delete_address(to_delete->this_post_address);

	delete_mail_array(&(to_delete->this_post_mails), &(to_delete->count_of_mails));

	to_delete->this_post_mails = NULL;
}

int is_valid_mail_id(char mail_id[mail_id_count_of_numbers])
{
	int i;
	for (i = 0; i < mail_id_count_of_numbers; i++) {
		if (!isdigit(mail_id[i])) {
			return 0;
		}
	}
	return 1;
}

int is_valid_time(time to_val)
{
	if (to_val.year < 2000 || (to_val.hour > 23 || to_val.hour < 0) || (to_val.min > 59 || to_val.min < 0) || (to_val.sec > 59 || to_val.sec < 0)) {
		return 0;
	}

	if (to_val.month == 2) {
		if ((to_val.year % 400 == 0) || (to_val.year % 4 == 0 && to_val.year % 100 != 0))
			return (to_val.day <= 29 && to_val.day > 0);
		else
			return (to_val.day < 29 && to_val.day > 0);
	}

	int days_31[7] = { 1, 3, 5, 7, 8, 10, 12 }, days_30[4] = { 4, 6, 9, 11 }, i;

	for (i = 0; i < 7; i++) {
		if (to_val.month == days_31[i])
			return (to_val.day <= 31 && to_val.day > 0);
	}

	for (i = 0; i < 4; i++) {
		if (to_val.month == days_30[i])
			return (to_val.day <= 30 && to_val.day > 0);
	}

	return 0;
}

int is_upper(char ch)
{
	return (ch >= 'A' && ch <= 'Z');
}

int is_lower(char ch)
{
	return (ch >= 'a' && ch <= 'z');
}

int is_digit(char ch)
{
	return (ch >= '0' && ch <= '9');
}

int is_valid_address(address to_val)
{
	size_t i;
	if (!is_upper(*(to_val.city.str)) || !is_upper(*(to_val.street.str)) || !to_val.house || !to_val.flat) {
		return 0;
	}

	for (i = 1; i < to_val.city.length; i++) {
		if (!is_lower(to_val.city.str[i])) {
			return 0;
		}
	}

	for (i = 1; i < to_val.street.length; i++) {
		if (!is_lower(to_val.street.str[i])) {
			return 0;
		}
	}

	for (i = 0; i < index_count_of_numbers; i++) {
		if (!is_digit(to_val.index[i])) {
			return 0;
		}
	}
	return 1;
}

int is_valid_mail(mail to_val)
{
	if (to_val.weigth <= 0 || !is_valid_mail_id(to_val.mail_id) || !is_valid_time(to_val.time_of_creation) || !is_valid_time(to_val.delivery_time)) {
		return 0;
	}
	return 1;
}

void get_address(FILE* stream, address* to_get)
{
	to_get->city = create_string(stream, 0, ' ');
	to_get->street = create_string(stream, 0, ' ');

	if (fscanf(stream, "%u %u %s", &(to_get->house), &(to_get->flat), to_get->index) != 3 ||
		!to_get->city.length || !to_get->street.length) {
		delete_address(&to_get);
		to_get->house = 0;
		return;
	}
	fgetc(stream);

	if (!is_valid_address(*to_get)) {
		delete_address(to_get);
		to_get->house = 0;
	}
}

int get_time(FILE* stream, mail* to_get)
{
	if (fscanf(stream, "%u.%u.%u %u:%u:%u", &(to_get->time_of_creation.day), &(to_get->time_of_creation.month), &(to_get->time_of_creation.year),
		&(to_get->time_of_creation.hour), &(to_get->time_of_creation.min), &(to_get->time_of_creation.sec)) != 6) {
		return -1;
	}

	if (!is_valid_time(to_get->time_of_creation)) {
		return -2;
	}

	if (fscanf(stream, "%u.%u.%u %u:%u:%u\n", &(to_get->delivery_time.day), &(to_get->delivery_time.month), &(to_get->delivery_time.year),
		&(to_get->delivery_time.hour), &(to_get->delivery_time.min), &(to_get->delivery_time.sec)) != 6) {
		return -1;
	}

	if (!is_valid_time(to_get->delivery_time)) {
		return -2;
	}
	return 0;
}

int get_mail(FILE* stream, mail* to_get)
{
	get_address(stream, &(to_get->sender));
	get_address(stream, &(to_get->recipient));

	if (fscanf(stream, "%f %s", &(to_get->weigth), to_get->mail_id) != 2 || !to_get->sender.house || !to_get->recipient.house
		|| to_get->weigth <= 0 || !is_valid_mail_id(to_get->mail_id)) {
		return -1;
	}

	if (get_time(stream, to_get)) {
		return -2;
	}
	return 0;
}

post_statuses read_mails(FILE* in, post* my_post)
{
	if (!in)
		return post_incorrect_ptr_to_file_passed;

	size_t size = my_post->count_of_mails + 4;
	mail* tmp = NULL;

	if (!(tmp = (mail*)realloc(my_post->this_post_mails, sizeof(mail) * size))) {
		return post_realloc_error;
	}
	my_post->this_post_mails = tmp;

	while (!feof(in)) {
		if (my_post->count_of_mails >= size - 1) {
			size *= 2;
			if (!(tmp = (mail*)realloc(my_post->this_post_mails, sizeof(mail) * size))) {
				my_post->count_of_mails--;
				// TODO delete_mail_array(&(my_post->this_post_mails), &(my_post->count_of_mails));
				return post_realloc_error;
			}
			my_post->this_post_mails = tmp;
		}

		if (get_mail(in, &(my_post->this_post_mails[my_post->count_of_mails]))) {
			// TODO just return error, in main free all my_post mails before this index (count of mails)
			my_post->count_of_mails--;
			return post_incorrect_input_file_format;
		}

		my_post->count_of_mails++;

	}

	if (!(tmp = (mail*)realloc(my_post->this_post_mails, sizeof(mail) * my_post->count_of_mails))) {
		// TODO delete_mail_array(&(my_post->this_post_mails), &(my_post->count_of_mails));
		return post_realloc_error;
	}

	my_post->this_post_mails = tmp;

	return post_ok;
}

int mail_id_comparator(mail first, mail second)
{
	int i;
	for (i = 0; i < mail_id_count_of_numbers; i++) {
		if (first.mail_id[i] > second.mail_id[i]) {
			return 1;
		}

		if (first.mail_id[i] < second.mail_id[i]) {
			return -1;
		}
	}
	return 0;
}

mail* find_mail(mail to_find, post this_post, int compare(mail, mail), size_t* ind)
{
	int i;
	if (this_post.count_of_mails != 0)
		for (i = 0; i < this_post.count_of_mails; i++) {
			if (!compare(to_find, this_post.this_post_mails[i])) {
				(*ind) = i;
				return &(this_post.this_post_mails[i]);
			}
		}
	(*ind) = nan;
	return NULL;
}

post_statuses delete_mail(mail to_delete, post* this_post)
{
	if (!this_post->count_of_mails) {
		return post_empty_post;
	}
	size_t ind = 0, i;
	mail* found = find_mail(to_delete, *this_post, mail_id_comparator, &ind), *tmp = NULL;

	if (!found || isnan(ind)) {
		return post_no_match;
	}

	delete_address(&(found->sender));
	delete_address(&(found->recipient));

	for (i = ind; i < ((*this_post).count_of_mails - 1); i++) {
		(*this_post).this_post_mails[i] = (*this_post).this_post_mails[i + 1];
	}

	this_post->count_of_mails--;
	if (!(tmp = (mail*)realloc(this_post->this_post_mails, sizeof(mail) * this_post->count_of_mails))) {
		return post_realloc_error;
	}
	this_post->this_post_mails = tmp;

	return post_ok;
}

int recipient_n_mail_id_comparator(const void* ptr1, const void* ptr2)
{
	// index of recipient and mail_id
	mail* first = (mail*)ptr1, * second = (mail*)ptr2;
	int i;
	for (i = 0; i < index_count_of_numbers; i++) {
		if (first->recipient.index[i] > second->recipient.index[i]) {
			return 1;
		}

		if (first->recipient.index[i] < second->recipient.index[i]) {
			return -1;
		}
	}

	return mail_id_comparator(*first, *second);
}

void sort_recipient_n_mail_id(post this_post)
{
	qsort(this_post.this_post_mails, this_post.count_of_mails, sizeof(mail), recipient_n_mail_id_comparator);
}

// returns < 0 if first is earlier second; > 0 if first is later second
int time_comparator(time first, time second)
{
	int date1 = first.year * 10000 + first.month * 100 + first.day, date2 = second.year * 10000 + second.month * 100 + second.day;
	int time1 = first.hour * 10000 + first.min * 100 + first.sec, time2 = second.hour * 10000 + second.min * 100 + second.sec;

	if (date1 > date2 || date1 < date2)
		return date1 - date2;

	if (time1 > time2 || time1 < time2)
		return time1 - time2;

	return 0;
}

int time_of_creation_n_mail_id_comparator(const void* ptr1, const void* ptr2)
{
	mail* first = (mail*)ptr1, * second = (mail*)ptr2;
	int result = 0;

	if ((result = time_comparator(first->time_of_creation, second->time_of_creation)))
		return result;

	return mail_id_comparator(*first, *second);
}

post_statuses find_delivered_mails(mail** found, size_t* count_of_found, post this_post, time current_time)
{
	if (*found) {
		return post_incorrect_ptr_to_collection_passed;
	}

	size_t i, size = 8;
	mail* tmp = NULL;

	if (!(tmp = (mail*)realloc((*found), sizeof(mail) * size))) {
		return post_realloc_error;
	}
	(*found) = tmp;
	(*count_of_found) = 0;

	for (i = 0; i < this_post.count_of_mails; i++) {
		if ((*count_of_found) >= size - 1) {
			size *= 2;
			if (!(tmp = (mail*)realloc((*found), sizeof(mail) * size))) {
				free(*found);
				(*found) = NULL;
				return post_realloc_error;
			}
			(*found) = tmp;
		}

		if (time_comparator(current_time, this_post.this_post_mails[i].delivery_time) >= 0) {
			(*found)[(*count_of_found)++] = this_post.this_post_mails[i];
		}
	}

	if (!(*count_of_found)) {
		free(*found);
		(*found) = NULL;
		return post_ok;
	}

	if (!(tmp = (mail*)realloc((*found), sizeof(mail) * (*count_of_found)))) {
		free(*found);
		(*found) = NULL;
		return post_realloc_error;
	}
	(*found) = tmp;

	qsort(*found, *count_of_found, sizeof(mail), time_of_creation_n_mail_id_comparator);
	return post_ok;
}

post_statuses find_expired_mails(mail** found, size_t* count_of_found, post this_post, time current_time)
{
	if (*found) {
		return post_incorrect_ptr_to_collection_passed;
	}

	int i, size = 8;
	mail* tmp = NULL;

	if (!(tmp = (mail*)realloc((*found), sizeof(mail) * size))) {
		return post_realloc_error;
	}
	(*found) = tmp;
	(*count_of_found) = 0;


	for (i = 0; i < this_post.count_of_mails; i++) {
		if ((*count_of_found) >= size - 1) {
			size *= 2;
			if (!(tmp = (mail*)realloc((*found), sizeof(mail) * size))) {
				free(*found);
				(*found) = NULL;
				return post_realloc_error;
			}
			(*found) = tmp;
		}

		if (time_comparator(current_time, this_post.this_post_mails[i].delivery_time) < 0) {
			(*found)[(*count_of_found)++] = this_post.this_post_mails[i];
		}
	}

	if (!(*count_of_found)) {
		free(*found);
		(*found) = NULL;
		return post_ok;
	}

	if (!(tmp = (mail*)realloc((*found), sizeof(mail) * (*count_of_found)))) {
		free(*found);
		(*found) = NULL;
		return post_realloc_error;
	}
	(*found) = tmp;

	qsort(*found, *count_of_found, sizeof(mail), time_of_creation_n_mail_id_comparator);
	return post_ok;
}

void print_address(FILE* stream, address to_pr)
{
	if (is_valid_address(to_pr)) {
		fprintf(stream, "%s %s %u %u %s\n", to_pr.city.str, to_pr.street.str, to_pr.house, to_pr.flat, to_pr.index);
	}
}

void print_mail(FILE* stream, mail to_pr)
{
	fprintf(stream, "Mail id: %s\n", to_pr.mail_id);
	fprintf(stream, "Sender:           ");
	print_address(stream, to_pr.sender);
	fprintf(stream, "Recipient:        ");
	print_address(stream, to_pr.recipient);

	fprintf(stream, "Weight:           %lf\n", to_pr.weigth);
	fprintf(stream, "Time of creation: %2u.%2u.%4u %2u:%2u:%2u\n", to_pr.time_of_creation.day, to_pr.time_of_creation.month, to_pr.time_of_creation.year,
		to_pr.time_of_creation.hour, to_pr.time_of_creation.min, to_pr.time_of_creation.sec);
	fprintf(stream, "Delivery time:    %2u.%2u.%4u %2u:%2u:%2u\n\n", to_pr.delivery_time.day, to_pr.delivery_time.month, to_pr.delivery_time.year,
		to_pr.delivery_time.hour, to_pr.delivery_time.min, to_pr.delivery_time.sec);
}

void print_mail_collection(FILE* stream, mail* to_pr, size_t count)
{
	if (count == 0) {
		fprintf(stream, "There are 0 mails at your post\n");
		return;
	}
	size_t i;
	for (i = 0; i < count; i++) {
		print_mail(stream, to_pr[i]);
	}
}

void print_post(FILE* stream, post to_pr)
{
	if (!to_pr.this_post_address || !to_pr.this_post_mails) {
		printf("There is no post of yours\n");
		return;
	}
	fprintf(stream, "Your post address:\n");
	print_address(stream, *(to_pr.this_post_address));
	fprintf(stream, "<--------- Mails start --------->\n");

	size_t i;
	print_mail_collection(stream, to_pr.this_post_mails, to_pr.count_of_mails);
	fprintf(stream, "<---------  Mails end  --------->\n");
}
