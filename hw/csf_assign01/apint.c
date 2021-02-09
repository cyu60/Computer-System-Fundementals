/*
 * CSF Assignment 1
 * Arbitrary-precision integer data type
 * Function implementations
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "apint.h"
// Personal libs
#include <stdio.h>
#include <string.h>
#include <math.h>

// Private funcs

int apint_abs_compare(const ApInt *left, const ApInt *right) {
	// cmp len 
	if (left->len != right->len) {
		return left->len - right->len;
	}
	// Check from most sig bit?
	for (uint32_t i = left->len; i > 0; i--) {
		if (left->data[i-1] != right->data[i-1]) {
			int compareResult = left->data[i-1] > right->data[i-1]; //???
			// printf("\ncompare res: %i\n", compareResult == 0 ? -1 : 1);
			return compareResult == 0 ? -1 : 1;
		}
	}
	return 0;
}

// Pre: val is not null
ApInt *apint_create_from_u64(uint64_t val) {
	// Allocate space for data
	ApInt * new_val = malloc(sizeof(ApInt) + sizeof(uint64_t));
	new_val->len = 1;
	new_val->data[0] = val; 
	// Flags are 0 by default (uint64 is pos)
	new_val->flags = 0;
	return new_val;
}

// Cases to consider: '-0'?
ApInt *apint_create_from_hex(const char *hex) {
	// printf("hex: %s\n", hex);
	int pos = 0;
		// check neg
		if (hex[0] == '-')
		{
			pos = 1;  // neg
			hex += 1; // no need to consider first char
		}
		
		// Need to handle zero padding?
		while (hex[0] == '0')
		{
			hex += 1;
			// printf("cur: %c  ", hex[0]);
			// Need to check for 00000000000000000
			if (hex[0] == '\0') {
				return apint_create_from_u64(0UL);
			}
		}
		
		// Get len of hex
		int hex_len = strlen(hex);
		// Div by 16, take ceiling for real len
		int num_blocks = (int)ceil(hex_len / 16.0); // need to force float div

		// printf("len: %i, numbits: %i\n", hex_len, num_blocks);

		// Allocate space for data
		ApInt *new_val = malloc(sizeof(ApInt) + num_blocks * sizeof(uint64_t));
		new_val->len = num_blocks;
		new_val->flags = pos;

		char buf[25]; // 16 is needed? 25 is safe?

		for (int i = 0; i < num_blocks; i++)
		{ // careful with boundareies

			int low = hex_len - 16 * (i + 1) >= 0 ? hex_len - 16 * (i + 1) : 0; // need to avoid 0
			int high = hex_len - 16 * i;
			// printf("low: %i - %c high-1: %i - %c\n", low, hex[low], high-1, hex[high-1]);

			memcpy(buf, &hex[low], 16);
			int last = high > 16 ? 16 : high;
			buf[last] = '\0'; // enforce null term for safety
			new_val->data[i] = strtoull(buf, NULL, 16);

			// printf("digit: %lx\n", new_val->data[i]);
		}

		// printf("pos: %i\n", new_val->flags);
		// printf("finished creation\n");
		return new_val;
	}

void apint_destroy(ApInt *ap) {
	free(ap);
}

int apint_is_zero(const ApInt *ap) {
	return ap->data[0] == 0 ? 1 : 0;
}

int apint_is_negative(const ApInt *ap) {
	return ap->flags;
}

uint64_t apint_get_bits(const ApInt *ap, unsigned n) {
	return ap->data[n];
}

int apint_highest_bit_set(const ApInt *ap) {
	if (apint_is_zero(ap)) 
		return -1;
	// need to check most sig bit
	uint64_t n = ap->data[ap->len - 1];
	// unsigned int counter;
	unsigned int counter = 0;
	while (n/2) {
		counter++;
		n >>= 1;
	}
	counter += (ap->len - 1) * 64;
	return counter;
}


char *apint_format_as_hex(const ApInt *ap) {

	int buffer_len = ap->len * 16 + 1;
	if (apint_is_negative(ap)) { buffer_len++; }
	char * hex = malloc(buffer_len * sizeof(char)); // va errors

	int last_index = 0;

	// Check neg
	if (apint_is_negative(ap)) {
    	hex[0] = '-';
		last_index = 1; // shift by 1
	}

	uint64_t val;
	char * format;

	// Need to fill from the end
	for (uint32_t i = ap->len; i > 0; i--) { 
		val = ap->data[i-1]; // Need to shift by 1 for indexing
		// most sig section does not need padding
		format = i == ap->len ? "%lx" : "%016lx";

		// printf("last index: %i\n", last_index); // va error
		sprintf(hex + last_index, format, val);
		// Update last index
		last_index = strlen(hex);
		// printf("hex: %s\n", hex);
	}

	// printf("hex: %s\n", hex);
	return hex;
}

ApInt *apint_negate(const ApInt *ap) {
	ApInt * neg_val = malloc(sizeof(ApInt) + ap->len * sizeof(uint64_t));
	neg_val->len = ap->len;
	for (uint32_t i = 0; i < ap->len; i++) {
		neg_val->data[i] = ap->data[i];
	}
	if (apint_is_zero(ap)) {
		neg_val->flags = 0;
		return neg_val;
	}
	
	neg_val->flags = ap->flags == 0 ? 1 : 0;	
	return neg_val;
}

// Private func
uint64_t check_overflow(uint64_t a, uint64_t b, uint64_t result) {
	return result < a && result < b ? 1UL : 0UL;
}

ApInt* same_sign_add(const ApInt *min, const ApInt *max, ApInt *result) {

	uint64_t carryover = 0UL;
	uint64_t max_val = 0xFFFFFFFFFFFFFFFFUL; // Should declare as macro?
	
	for (uint64_t i = 0; i < max->len; i++) {
			if (i < min->len) {
				result->data[i] = min->data[i] + max->data[i] + carryover;
				
				// Check special case -- max, max, +1
				if (min->data[i] == max_val && max->data[i] == max_val && carryover == 1UL) {
					carryover = 1UL; // There is overflow
				} else {
					carryover = check_overflow(min->data[i], max->data[i], result->data[i]);
				}
			} else {
				result->data[i] = max->data[i] + carryover; // add carryover?
				// Carryover set to 0?
				carryover = 0UL;
			}
			// printf("\nres: %lx\n", result->data[i]);
			// printf("carryover: %lu\n", carryover);
		}
		// Need to check final val overflow?  
		if (min->len == max->len && carryover == 1UL) {
			result = realloc(result, (sizeof(ApInt) + (max->len + 1) * sizeof(uint64_t)));
			result->len++;
			result->data[max->len] = 1UL;
		}

		// Determine sign
		result->flags = apint_is_negative(max) ? 1 : 0;

		return result;
}

ApInt *apint_add(const ApInt *a, const ApInt *b) {

	// printf("start add\n");
	// printf("\ndata: %lx, %lx", a->data[0], b->data[0]);
	// printf("\nsign: %i, %i", a->flags, b->flags);
	// printf("\nb[1]: %lx", b->data[1]);

	// Need to find num of blocks
	const ApInt * min = a->len > b->len ? b : a;
	const ApInt * max = a->len > b->len ? a : b;

	// printf("max: %i min: %i", max, min->len);

	ApInt * result = malloc(sizeof(ApInt) + (max->len) * sizeof(uint64_t));
	result->len = max->len;

	// Same sign
	if (apint_is_negative(a) == apint_is_negative(b)) {
		// same_sign_add(min, max, result);
		result = same_sign_add(min, max, result);
	} else {
		// Diff sign -- 5f20766963b0251c
		// -1, 1 ? -1, 0
		const ApInt * abs_bigger = apint_abs_compare(a, b) >= 0 ? a : b;
		const ApInt * abs_smaller = apint_abs_compare(a, b) < 0 ? a : b;
		// printf("bigger: %s", apint_format_as_hex(abs_bigger)); // BIGGER IS WRONG????
		uint64_t carryover = 0UL;
		for (uint64_t i = 0; i < abs_bigger->len; i++) {
			if (i < abs_smaller->len) {
				result->data[i] = abs_bigger->data[i] - abs_smaller->data[i] - carryover;
				// need to consider small data max? -- 9 + 1 = overflow 0? -- special case
				// printf("%x", UINT16_MAX);
				if (abs_smaller->data[i] == UINT64_MAX && carryover == 1UL) {
					printf("--------------------------------------------------------%lx", abs_smaller->data[i]);
					carryover = 1UL;
				} else {
					carryover = abs_bigger->data[i] >= abs_smaller->data[i] + carryover ? 0UL : 1UL;
				}
			} else {
				// 0 - 1
				result->data[i] = abs_bigger->data[i] - carryover;
				carryover = 0UL;
			}
		}
		
		result->flags = apint_is_zero(result) ? 0 : abs_bigger->flags;
			// Determine flag
			/*if (abs)
			
			if (abs_bigger == a) { // a >= b?
			// if (apint_abs_compare(a, b) >= 0) { // a >= b?
			// if (a->data[min->], b) >= 0) { // a >= b?
				// printf("\na: %lu, b: %lu, evl: %i\n", a->data[0], b->data[0], apint_compare(a, b) >= 0);
				// printf("\na: %i, b: %i, : %i\n", a->flags, b->flags, apint_is_negative(a));
				result->flags = apint_is_negative(a) && result->data[0] != 0UL ? 1 : 0;
			} else {
				result->flags = !apint_is_negative(a) && result->data[0] != 0UL ? 1 : 0;
			}*/
		// printf("\ndata: %lx, %lx", a->data[0], b->data[0]);
		// printf("\nexpected: %lx", a->data[0] - b->data[0]);
		// printf("\nsign: %i, %i", a->flags, b->flags);
		// // Compute
		// printf("\nresult[0]: %lx flag: %i\n", result->data[0], result->flags);
		// if (result->len > 1)
		// 	printf("result[1]: %lx\n", result->data[1]);
		// if (result->len > 2)
		// 	printf("result[2]: %lx\n", result->data[2]);
	}
	//need to deal with over carry of msb	

	// printf("\ndata: %lu, %lu", a->data[0], b->data[0]);


	return result;
}


ApInt *apint_sub(const ApInt *a, const ApInt *b) {
	// Need to check free the opp?
	ApInt * opp = apint_negate(b);
	// printf("\nb: %i %lu opp: %i %lu\n", b->flags, b->data[0], opp->flags, *opp->data);
	ApInt * result = apint_add(a, opp);
	apint_destroy(opp);
	return result;
}

// @return left > right ? + : - or 0
int apint_compare(const ApInt *left, const ApInt *right) {
	// printf("\ndata: %lx %i, %lx %i\n", left->data[0], left->len, right->data[0], right->len);
	
	// cmp sign
	// Check if sign is diff
	if (left->flags != right->flags) {
		// flags = 1 if neg, 0 if pos
		return right->flags - left->flags;
	}
	// cmp len
	if (left->len != right->len)
	{
		return apint_is_negative(left) ? right->len - left->len : left->len - right->len;
	}
	// Careful of flip between neg and non-neg
	// cmp ints - u64
	// for (uint32_t i = 0; i < left->len; i++)
	// {
	// 	if (left->data[i] != right->data[i])
	// 	{
	// 		// Check needs to be flipped if
	// 		int compareResult = apint_is_negative(left) ? left->data[i] < right->data[i] : left->data[i] > right->data[i]; //???
	// 		// printf("\ncompare res: %i\n", compareResult == 0 ? -1 : 1);
	// 		return compareResult == 0 ? -1 : 1;
	// 	}
	// }

	for (uint32_t i = left->len; i > 0; i--)
	{
		if (left->data[i - 1] != right->data[i - 1])
		{
			// Check needs to be flipped if
			int compareResult = apint_is_negative(left) ? left->data[i - 1] < right->data[i - 1] : left->data[i - 1] > right->data[i - 1]; //???
			// printf("\ncompare res: %i\n", compareResult == 0 ? -1 : 1);
			return compareResult == 0 ? -1 : 1;
		}
	}
	return 0;
	// return apint_abs_compare(left, right);	
}

ApInt *apint_lshift(ApInt *ap) {
 /* TODO: implement */
 assert(0);
 return NULL;
}

ApInt *apint_lshift_n(ApInt *ap, unsigned n) {
 /* TODO: implement */
 assert(0);
 return NULL;
}