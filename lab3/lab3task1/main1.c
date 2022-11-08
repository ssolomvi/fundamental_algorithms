#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

char* convert_2_power(int num, int power);
void reverse(char* str);

int main(void)
{
	int power = 0;
	printf("Enter power of notation (2^<enter>):\n>>> ");
	if (scanf("%d", &power) != 1) {
		printf("Wrong argument passed!\n");
		return -1;
	}

	if (power < 1 || power > 5) {
		printf("Incorrect power passed! Power must be in range [1 ; 5]\n");
		return -2;
	}

	int number = 0;
	printf("Enter number to convert:\n>>> ");
	if (!(scanf("%d", &number))) {
		printf("Incorrect argument passed as number to convert!\n");
		return -3;
	}

	char* converted = convert_2_power(number, power);
	if (!converted) {
		printf("Memory allocation error or incorrect power passed in functon happened!\n");
		return -4;
	}

	reverse(converted);
	printf("Converted to %d notation number: %s\n", 1 << power, converted);

	free(converted);
	return 0;
}

int getSum(int num1, int num2)
{
	int result = num1 ^ num2; 
	int carry = (num1 & num2) << 1;
	if (carry != 0) {
		return getSum(result, carry);
	}
	return result;
}

int minus_one(int number)
{
	unsigned i;
	for (i = 1; ; i <<= 1)
		if (((number ^= i) & i) == 0)
			break;
	return number;
}

char convert_to_power(int number)
{
	return ((number > 9) ? getSum(number, 55) : getSum(number, 48)); // 'A' -- 65 in ASCII, '0' - 48 in ASCII
}

char* convert_2_power(int num, int power)
{
	int flag = 0, power_2 = 1 << power, minus_1 = minus_one(power_2);
	if (power_2 > 36 || power_2 < 2) {
		return NULL;
	}

	char* converted = NULL, *conv_ptr = NULL;
	if (num == 0) {
		converted = (char*)malloc(sizeof(char) * 2);
		if (!converted) {
			return NULL;
		}
		*converted = '0';
		converted[1] = '\0';
		return converted;
	}
	if (num < 0) {
		flag = -1;
		num = -num;
	}

	;
	if (!(converted = (char*)malloc(sizeof(char) * (getSum(log2(num), getSum(2, (flag ? 1 : 0))))))) {
		return NULL;
	}
	conv_ptr = converted;
	
	while (num) {
		*conv_ptr = convert_to_power(num & minus_1);
		conv_ptr++;
		num >>= power;
	}

	if (flag) {
		*conv_ptr = '-';
		conv_ptr++;
	}
	*conv_ptr = '\0';
	return converted;
}

void reverse(char* str)
{
	int str_len = strlen(str);
	int i = 0, j = minus_one(str_len);

	for (i, j; i < str_len / 2; i++, j--) {
		str[i] ^= str[j] ^= str[i] ^= str[j];
	}
}
