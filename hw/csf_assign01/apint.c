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
		return apint_is_negative(left) ? right->len - left->len : left->len - right->len;
	}
	// Careful of flip between neg and non-neg
	// cmp ints - u64
	for (uint32_t i = 0; i < left->len; i++) {
		if (left->data[i] != right->data[i]) {
			// Check needs to be flipped if
			int compareResult = apint_is_negative(left) ? left->data[i] < right->data[i] : left->data[i] > right->data[i];
			// printf("\ncompare res: %i\n", compareResult == 0 ? -1 : 1);
			return compareResult == 0 ? -1 : 1;
		}
	}
	return 0;
}

// Pre: val is not null
ApInt *apint_create_from_u64(uint64_t val) {
	// assert(val);

	// Allocate space for data
	ApInt * new_val = malloc(sizeof(ApInt) + sizeof(uint64_t));
	new_val->len = 1;
	new_val->data[0] = val; 
	
	// Flags are 0 by default (uint64 is pos)
	new_val->flags = 0;

	// printf("val: %lu\n", val);
	// printf("new val: %lu\n", *new_val->data);
	return new_val;
}

// Cases to consider: '-0'?
ApInt *apint_create_from_hex(const char *hex) {
	/* TODO: implement */
	// assert(0);
	printf("hex: %s\n", hex);

	int pos = 0;

	// check neg
	if (hex[0] == '-') {
		pos = 1; // neg
		hex+=1; // no need to consider first char
	}

	// Get len of hex
	int hex_len = strlen(hex);
	// Div by 16, take ceiling for real len
	int num_blocks = (int)ceil(hex_len / 16.0); // need to force float div
	
	printf("len: %i, numbits: %i\n", hex_len, num_blocks);

	// Allocate space for data
	// malloc the size
	ApInt * new_val = malloc(sizeof(ApInt) + num_blocks * sizeof(uint64_t));
	new_val->len = num_blocks;
	new_val->flags = pos;

	// Need to read in from the end?
	// Read in last 16 chars -- form a digit
	// repeat till reach zero

	// Need to form the best of the string?

	char buf[25]; // 16 is needed? 25 is safe?

	for (int i = 0; i < num_blocks; i++) { // careful with boundareies

		int low = hex_len - 16 * (i + 1) >= 0 ? hex_len - 16 * (i + 1) : 0; // need to avoid 0
		int high = hex_len - 16 * i;
		printf("low: %i - %c high-1: %i - %c\n", low, hex[low], high-1, hex[high-1]);

		memcpy(buf, &hex[low], 16);
		// strncpy(buf, &hex[low], high); // strcpy is exclusive upper bound
		int last = high > 16 ? 16 : high;
		buf[last] = '\0'; // enforce null term for safety
		printf("hex: %s buf: %s\n", hex, buf);
		new_val->data[i] = strtoull(buf, NULL, 16);

		printf("digit: %lx\n", new_val->data[i]);
	}

	// printf("pos: %i\n", new_val->flags);
	printf("finished creation\n");
	return new_val;
	// new_val->data[0] = val; 

	// Create the number of spaces
	// read in 16 vals
	// uint64_t val;

	// val = strtoull(hex, NULL, 16);

	// return apint_create_from_u64(val);
}

void apint_destroy(ApInt *ap) {
	// printf("free: %lu\n", ap->data[0]);
	// free(ap->data);
	free(ap);
}

int apint_is_zero(const ApInt *ap) {
	// int val = ap->data[0] == 0 ? 1 : 0;
	// printf("neg: %i\n", val);
	// return val;
	return ap->data[0] == 0 ? 1 : 0;
}

int apint_is_negative(const ApInt *ap) {
	/* TODO: implement */
	return ap->flags;
}

uint64_t apint_get_bits(const ApInt *ap, unsigned n) {
	/* TODO: implement */
	return ap->data[n];
	// Need to implement for large bits!	
}

int apint_highest_bit_set(const ApInt *ap) {
	/* TODO: implement */
	if (apint_is_zero(ap)) 
		return -1;
	// need to check most sig bit
	uint64_t n = ap->data[ap->len - 1];
	// uint64_t n = ap->data[0];
	// printf("n: %lu\n", n);

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
	/* TODO: implement */
	//uint32_t hex_len = (ap->len - 1) * 16 + apint_highest_bit_set(ap) % 64;
	int buffer_len = ap->len * 16 + 1;
	if (apint_is_negative(ap)) { buffer_len++; }
	char * hex = malloc(buffer_len * sizeof(char)); // va errors
	// char * hex = (char*)calloc(hex_len, sizeof(char)); 

	int last_index = 0;

	// Check neg
	if (apint_is_negative(ap)) {
    	hex[0] = '-';
		last_index = 1; // shift by 1
	}

	uint64_t val;
	char * format;
	//uint32_t ptr;
	// printf("len: %i\n", ap->len);

	// Need to fill from the end
	for (uint32_t i = ap->len; i > 0; i--) { 
		val = ap->data[i-1]; // Need to shift by 1 for indexing
		// printf("cur: %lx\n", val);
		// most sig section does not need padding
		format = i == ap->len ? "%lx" : "%016lx";
		// printf("format: %s\n", format);

		printf("last index: %i\n", last_index); // va error
		sprintf(hex + last_index, format, val);
		// Update last index
		last_index = strlen(hex);
		// printf("hex: %s\n", hex);
	}

	// uint64_t val = ap->data[0];
	// // Having null term issue?
	// if (apint_is_negative(ap)) {
    // 	hex[0] = '-';
	// 	hex += 1;
	// 	// Need to have padding 0? 016 except most sig
	// 	sprintf(hex, "%lx", val);
	// 	hex -= 1;
	// 	// printf("neg hex: %s\n", hex);
	// } else {
	// 	// Need to have padding 0? 016
	// 	sprintf(hex, "%lx", val);
	// }

	printf("hex: %s\n", hex);
	return hex;
}

ApInt *apint_negate(const ApInt *ap) {

	ApInt * neg_val = malloc(sizeof(ApInt) + ap->len * sizeof(uint64_t));
	neg_val->len = ap->len;
	// TODO: Need to change the vals?
	for (uint32_t i = 0; i < ap->len; i++) {
		neg_val->data[i] = ap->data[i];
		// neg_val->data[0] = ap->data[0];

	}

	if (apint_is_zero(ap)) {
		neg_val->flags = 0;
		return neg_val;
	}
	
	// printf("org data: %lu\n", ap->data[0]);
	// printf("new data (saved): %lu\n", *neg_val->data);
	neg_val->flags = ap->flags == 0 ? 1 : 0;	
	return neg_val;
}

// Private func
uint64_t check_overflow(uint64_t a, uint64_t b, uint64_t result) {
	// uint64_t max_val = 0xFFFFFFFFFFFFFFFFUL; // Should declare as macro?
	// // Check special case -- max, max
	// if (a == max_val && b == max_val) {
	// 	return 1UL;
	// } else {
		return result < a && result < b ? 1UL : 0UL;
	// }
	// return NULL; // Should not reach here
	
}

ApInt* same_sign_add(const ApInt *min, const ApInt *max, ApInt *result) {
// void same_sign_add(const ApInt *min, const ApInt *max, ApInt *result) {

	uint64_t carryover = 0UL;
	uint64_t max_val = 0xFFFFFFFFFFFFFFFFUL; // Should declare as macro?
	
	for (uint64_t i = 0; i < max->len; i++) {
			if (i < min->len) {
				result->data[i] = min->data[i] + max->data[i] + carryover;
				
				// Check special case -- max, max, +1
				if (min->data[i] == max_val && max->data[i] == max_val && carryover == 1UL) {
					carryover = 1UL;
				} else {
					carryover = check_overflow(min->data[i], max->data[i], result->data[i]);
				}
			} else {
				result->data[i] = max->data[i] + carryover; // add carryover?
				// Carryover set to 0?
				carryover = 0UL;
			}
			printf("\nres: %lx\n", result->data[i]);
			printf("carryover: %lu\n", carryover);
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

	printf("start add\n");
	printf("\ndata: %lx, %lx", a->data[0], b->data[0]);
	// printf("\nsign: %i, %i", a->flags, b->flags);
	// printf("\nb[1]: %lx", b->data[1]);

	// Need to find most sig bit?
	// simple addition
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

		uint64_t carryover = 0UL;
		for (uint64_t i = 0; i < abs_bigger->len; i++) {
			if (i < abs_smaller->len) {
				result->data[i] = abs_bigger->data[i] - abs_smaller->data[i] - carryover;
				carryover = abs_bigger->data[i] >= abs_smaller->data[i] + carryover ? 0 : 1;
			} else {
				result->data[i] = abs_bigger->data[i] - carryover;
				carryover = 0;
			}
		}
			// Determine flag
			if (abs_bigger == a) { // a >= b?
			// if (apint_abs_compare(a, b) >= 0) { // a >= b?
			// if (a->data[min->], b) >= 0) { // a >= b?
				printf("\na: %lu, b: %lu, evl: %i\n", a->data[0], b->data[0], apint_compare(a, b) >= 0);
				printf("\na: %i, b: %i, : %i\n", a->flags, b->flags, apint_is_negative(a));
				result->flags = apint_is_negative(a) && result->data[0] != 0UL ? 1 : 0;
			} else {
				result->flags = !apint_is_negative(a) && result->data[0] != 0UL ? 1 : 0;
			}
		}
	//need to deal with over carry of msb	

	
	// // Allocate space
	// ApInt * result = malloc(sizeof(ApInt) + sizeof(uint64_t));
	// result->len = 1;
	// // Same sign
	// if (apint_is_negative(a) == apint_is_negative(b)) {
	// 	result->data[0] = a->data[0] + b->data[0];
	// 	// neg
	// 	result->flags = apint_is_negative(a) ? 1 : 0;
	// } else {
	// // Diff sign
	// 	if (a->data[0] >= b->data[0]) { // need to check abs
	// 		result->data[0] = a->data[0] - b->data[0];
	// 		result->flags = apint_is_negative(a) && result->data[0] != 0UL ? 1 : 0;
	// 	} else {
	// 		result->data[0] = b->data[0] - a->data[0];
	// 		result->flags = !apint_is_negative(a) && result->data[0] != 0UL ? 1 : 0;
	// 	}
	// }

	// printf("\ndata: %lu, %lu", a->data[0], b->data[0]);
	// Compute
	printf("\nresult[0]: %lx flag: %i\n", result->data[0], result->flags);
	if (result->len > 1)
		printf("result[1]: %lx\n", result->data[1]);
	if (result->len > 2)
		printf("result[2]: %lx\n", result->data[2]);

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

	return apint_abs_compare(left, right);	
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