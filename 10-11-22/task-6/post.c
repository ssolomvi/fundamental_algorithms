#include "post6.h"
#include <ctype.h>

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

int is_valid_time(char to_valid[time_count_of_numbers])
{
	int day, month, year, date = 0;
	int hour, min, sec, time = 0;

	if (sscanf(to_valid, "%d.%d.%d", &day, &month, &year) != 3 ||
		sscanf(to_valid, "%d:%d:%d", &hour, &min, &sec) != 3 || to_valid[10] != ' ') {
		return 0;
	}

	if (year < 2000 || (hour > 23 || hour < 0) || (min > 59 || min < 0) || (sec > 59 || sec < 0)) {
		return 0;
	}

	if (month == 2 && (day <= 29 || day > 0)) {
		if (year % 4 == 0)
			return (day <= 29 && day > 0);
		else
			return (day < 29 && day > 0);
	}

	int days_31[7] = { 1, 3, 5, 7, 8, 10, 12 }, days_30[4] = { 4, 6, 9, 11 }, i;

	for (i = 0; i < 7; i++) {
		if (month == days_31[i])
			return (day <= 31 && day > 0);
	}

	for (i = 0; i < 4; i++) {
		if (month == days_30[i])
			return (day <= 30 && day > 0);
	}
}

post_statuses add_mail(post this_post, mail to_add)
{
	mail* tmp = NULL;
	if (!(tmp = (mail*)realloc(this_post.this_post_mails, sizeof(mail) * (this_post.count_of_mails + 1)))) {
		return post_realloc_error;
	}

	this_post.this_post_mails = tmp;

	this_post.this_post_mails[this_post.count_of_mails++] = to_add;
	return post_ok;
}

int is_valid_address(address to_val)
{
	int i;
	if (!isupper(*(to_val.city.str)) || !isupper(*(to_val.street.str))) {
		return 0;
	}

	for (i = 1; i < to_val.city.length; i++) {
		if (!islower(to_val.city.str[i])) {
			return 0;
		}
	}

	for (i = 1; i < to_val.street.length; i++) {
		if (!islower(to_val.street.str[i])) {
			return 0;
		}
	}

	for (i = 0; i < index_count_of_numbers; i++) {
		if (!isdigit(to_val.index[i])) {
			return 0;
		}
	}
	return 1;
}

void delete_address(address* to_del)
{
	delete_string(&(to_del->city));
	delete_string(&(to_del->street));
}

address* get_address(FILE* stream)
{
	address* new_address;
	new_address->city = create_string(stream, 0, ' ');
	new_address->street = create_string(stream, 0, ' ');

	if (fscanf(stream, "%u %u %s\n", &(new_address->house), &(new_address->flat), new_address->index) != 3 ||
		(!new_address->city.length && !new_address->city.str) || (!new_address->street.length && !new_address->street.str)) {
		delete_address(new_address);
		new_address->house = nan;
		return new_address;
	}

	if (!is_valid_address(*new_address)) {
		delete_address(new_address);
		new_address->house = nan;
	}
	return new_address;
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

int is_valid_mail(mail to_val)
{
	if (to_val.weigth <= 0 || !is_valid_mail_id(to_val.mail_id) || !is_valid_time(to_val.time_of_creation) || !is_valid_time(to_val.delivery_time)) {
		return 0;
	}
	return 1;
}

post_statuses read_mails(FILE* stream, post* this_post)
{
	if (!stream) {
		return post_incorrect_ptr_to_file_passed;
	}

	size_t size = this_post->count_of_mails + 4;
	mail* tmp = NULL;

	if (!(tmp = (mail*)realloc(this_post->this_post_mails, sizeof(mail) * size))) {
		return post_realloc_error;
	}
	this_post->this_post_mails = tmp;

	while (!feof(stream)) {
		if (this_post->count_of_mails >= size - 1) {
			size *= 2;
			if (!(tmp = (mail*)realloc(this_post->this_post_mails, sizeof(mail) * size))) {
				delete_mail_array(&(this_post->this_post_mails), &(this_post->count_of_mails));
				return post_realloc_error;
			}
			this_post->this_post_mails = tmp;
		}

		this_post->this_post_mails[this_post->count_of_mails].sender = *(get_address(stream));
		this_post->this_post_mails[this_post->count_of_mails].recipient = *(get_address(stream));
		if (fscanf(stream, "%lf %s %s %s\n", &(this_post->this_post_mails[this_post->count_of_mails].weigth), this_post->this_post_mails[this_post->count_of_mails].mail_id, this_post->this_post_mails[this_post->count_of_mails].time_of_creation, this_post->this_post_mails[this_post->count_of_mails].delivery_time) != 4 || 
			isnan(this_post->this_post_mails[this_post->count_of_mails].sender.house) ||
			isnan(this_post->this_post_mails[this_post->count_of_mails].recipient.house)) {
			this_post->count_of_mails++;
			delete_mail_array(&(this_post->this_post_mails), &(this_post->count_of_mails));
			return post_incorrect_input_file_format;
		}

		// address is validated inside get_address
		if (!is_valid_mail(this_post->this_post_mails[this_post->count_of_mails])) {
			this_post->count_of_mails++;
			delete_mail_array(&(this_post->this_post_mails), &(this_post->count_of_mails));
			return post_incorrect_input_file_format;
		}
		this_post->count_of_mails++;
		fscanf(stream, "\n");
	}

	if (!(tmp = (mail*)realloc(this_post->this_post_mails, sizeof(mail) * this_post->count_of_mails))) {
		delete_mail_array(&(this_post->this_post_mails), &(this_post->count_of_mails));
		return post_realloc_error;
	}
	this_post->this_post_mails = tmp;
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
	for (i = 0; i < this_post.count_of_mails; i++) {
		if (!compare(to_find, this_post.this_post_mails[i])) {
			(*ind) = i;
			return &(this_post.this_post_mails[i]);
		}
	}
	(*ind) = nan;
	return NULL;
}

void delete_mail(mail to_delete, post* this_post)
{
	size_t ind = 0, i;
	mail* found = find_mail(to_delete, *this_post, mail_id_comparator, &ind);
	
	if (!found || isnan(ind)) {
		return;
	}
	
	delete_address(&(found->sender));
	delete_address(&(found->recipient));

	for (i = ind; i < ((*this_post).count_of_mails - 1); i++) {
		(*this_post).this_post_mails[i] = (*this_post).this_post_mails[i + 1];
	}
}

void delete_post(post* to_delete)
{
	int i;
	delete_address(to_delete->this_post_address);

	for (i = 0; i < to_delete->count_of_mails; i++) {
		delete_address(&(to_delete->this_post_mails[i].sender));
		delete_address(&(to_delete->this_post_mails[i].recipient));
	}

	if (to_delete->this_post_mails) {
		free(to_delete->this_post_mails);
	}

	to_delete->this_post_mails = NULL;
	to_delete->this_post_address = NULL;
	to_delete->count_of_mails = 0;
}

int recipient_n_mail_id_comparator(const void * ptr1, const void * ptr2)
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
int time_comparator(char first[time_count_of_numbers], char second[time_count_of_numbers])
{
	int day1, month1, year1, day2, month2, year2, date1 = 0, date2 = 0;
	if (sscanf(first, "%d.%d.%d", &day1, &month1, &year1) == 3 &&
		sscanf(second, "%d.%d.%d", &day2, &month2, &year2) == 3) {
		date1 = year1 * 10000 + month1 * 100 + day1;
		date2 = year2 * 10000 + month2 * 100 + day2;
	}

	if (date1 > date2 || date1 < date2)
		return date1 - date2;

	int hour1, hour2, min1, min2, sec1, sec2, time1 = 0, time2 = 0;
	if (sscanf(first, "%d:%d:%d", &hour1, &min1, &sec1) == 3 &&
		sscanf(second, "%d:%d:%d", &hour2, &min2, &sec2) == 3) {
		time1 = hour1 * 10000 + min1 * 100 + sec1;
		time2 = hour2 * 10000 + min2 * 100 + sec2;
	}

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

post_statuses find_delivered_mails(mail** found, unsigned int* count_of_found, post this_post, char current_time[time_count_of_numbers])
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
		return post_ok;
	}

	if (!(tmp = (mail*)realloc((*found), sizeof(mail) * (*count_of_found)))) {
		free(*found);
		return post_realloc_error;
	}
	(*found) = tmp;

	qsort(*found, *count_of_found, sizeof(mail), time_of_creation_n_mail_id_comparator);
	return post_ok;
}

post_statuses find_expired_mails(mail** found, unsigned int* count_of_found, post this_post, char current_time[time_count_of_numbers])
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
		return post_ok;
	}

	if (!(tmp = (mail*)realloc((*found), sizeof(mail) * (*count_of_found)))) {
		free(*found);
		return post_realloc_error;
	}
	(*found) = tmp;

	qsort(*found, *count_of_found, sizeof(mail), time_of_creation_n_mail_id_comparator);
	return post_ok;
}

void print_address(FILE* stream, address to_pr)
{
	if (is_valid_address) {
		fprintf(stream, "%s %s %u %u %s\n", to_pr.city.str, to_pr.street.str, to_pr.house, to_pr.flat, to_pr.index);
	}
}

void print_mail(FILE* stream, mail to_pr)
{
	if (is_valid_mail(to_pr)) {
		fprintf("Mail id: %s", to_pr.mail_id);
		fprintf(stream, "Sender: ");
		print_address(stream, to_pr.sender);
		fprintf(stream, "Recipient:");
		print_address(stream, to_pr.recipient);
		printf("Weight: %lf\nTime of creation: %s\nDelivery time: %s\n\n", to_pr.weigth, to_pr.time_of_creation, to_pr.delivery_time);
	}
}

void print_mail_collection(FILE* stream, mail* to_pr, size_t count)
{
	if (count == 0) {
		fprint("There are 0 mails at your post\n");
		return;
	}
	size_t i;
	for (i = 0; i < count; i++) {
		print_mail(stream, to_pr[i]);
	}
}

void print_post(FILE* stream, post to_pr)
{
	fprintf(stream, "Your post address:\n");
	print_address(stream, *(to_pr.this_post_address));
	fprintf(stream, "<--------- Mails start --------->\n");

	size_t i;
	print_mail_collection(stream, to_pr.this_post_mails, to_pr.count_of_mails);
	fprintf(stream, "<---------  Mails end  --------->\n");
}