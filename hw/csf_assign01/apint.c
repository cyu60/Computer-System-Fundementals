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


ApInt *apint_create_from_u64(uint64_t val) {
	ApInt * new_val = malloc(sizeof(ApInt));
	// uint64_t * new_data = malloc(sizeof(uint64_t));
	new_val->len = 1;
	
	// Allocate space for data
	new_val->data = malloc(sizeof(uint64_t)); 
	*new_val->data = val; 
	
	// Old
	// *new_data = val;
	// new_val->data = new_data; // ISSUE?

	// Flags are 0 by default (uint64 is pos)
	new_val->flags = 0;

	// printf("val: %lu\n", val);
	// printf("new val: %lu\n", *new_val->data);
	return new_val;
}

ApInt *apint_create_from_hex(const char *hex) {
	/* TODO: implement */
	// assert(0);
	return NULL;
}

void apint_destroy(ApInt *ap) {
	/* TODO: implement */
	// assert(0);
	printf("free: %lu\n", *ap->data);
	free(ap->data);
	free(ap);

}

int apint_is_zero(const ApInt *ap) {
	// int val = *ap->data == 0 ? 1 : 0;
	// printf("neg: %i\n", val);
	// return val;
	return *ap->data == 0 ? 1 : 0;
}

int apint_is_negative(const ApInt *ap) {
	/* TODO: implement */
	return ap->flags;
}

uint64_t apint_get_bits(const ApInt *ap, unsigned n) {
	/* TODO: implement */
	// assert(0);
	return *ap->data;
	// Need to implement for large bits!
}

int apint_highest_bit_set(const ApInt *ap) {
	/* TODO: implement */
	if (apint_is_zero(ap)) 
		return -1;
	// need to iterate over every bit?
	// Hard code first bit for now
	uint64_t n = *ap->data;
	printf("n: %lu\n", n);

	// unsigned int counter;
	unsigned int counter = 0;
	while (n/2) {
		printf("n: %lu, c: %i\n", n, counter);
		counter++;
		n >>= 1;
	}
	// for (counter = 0; n; n >>= 1) {
	// 	// counter += n & 1;
	// 	counter++;
	// 	printf("n: %lu, c: %i\n", n, counter);
	// }
	// while(n) { // n should not be greater than 63, should use for loop?
	// // while(n > 0) { // n should not be greater than 63, should use for loop?
	// 	// counter += n % 2;
	// 	counter++;
	// 	n >>= 1;
	// 	printf("n: %lu\n", n);	
	// }
	printf("numbit: %i\n", counter);
	return counter;
}

char *apint_format_as_hex(const ApInt *ap) {
	/* TODO: implement */
	// Need to malloc hex?
	char * hex = malloc(20 * sizeof(char)); // need to grow?
	// char hex[] = ""; // need to grow?
	uint64_t val = *ap->data; 
	// Having null term issue?
	if (apint_is_negative(ap)) {
    	hex[0] = '-';
		hex += 1;
		// Need to have padding 0? 016
		sprintf(hex, "%lx", val);
		hex -= 1;
		printf("neg hex: %s\n", hex);
	} else {
		// Need to have padding 0? 016
		sprintf(hex, "%lx", val);
	}

	// printf("hex: %s\n", hex);
	return hex;
}

ApInt *apint_negate(const ApInt *ap) {
	/* TODO: implement */
	// assert(0);
	ApInt * neg_val = malloc(sizeof(ApInt));
	uint64_t * neg_data = malloc(sizeof(uint64_t));
	// if (!apint_is_zero(ap)) {
		neg_val->flags = ap->flags == 0 ? 1 : 0;
	// }
	// TODO: Need to change the vals?
	neg_val->len = ap->len;
	*neg_data = *ap->data;
	neg_val->data = neg_data;

	// printf("org data: %lu\n", *ap->data);
	// printf("new data (saved): %lu\n", *neg_val->data);
	
	return neg_val;
}

ApInt *apint_add(const ApInt *a, const ApInt *b) {
	/* TODO: implement */
	// printf("\ndata: %lu, %lu", *a->data, *b->data);
	// Allocate space
	ApInt * result = malloc(sizeof(ApInt));
	uint64_t * result_data = malloc(sizeof(uint64_t));
	
	// unint64_t abs_a = *a->data;
	// unint64_t abs_b = *b->data;
	
	// Same sign
	if (apint_is_negative(a) == apint_is_negative(b)) {
		*result_data = *a->data + *b->data;
		// neg
		result->flags = apint_is_negative(a) ? 1 : 0;
		// if (apint_is_negative(a)) {
		// 	result->flags = 1;
		// }
	} else {
	// Diff sign
		if (*a->data >= *b->data) { // need to check abs
			*result_data = *a->data - *b->data;
			result->flags = apint_is_negative(a) && *result_data != 0UL ? 1 : 0;
		} else {
			*result_data = *b->data - *a->data;
			result->flags = !apint_is_negative(a) && *result_data != 0UL ? 1 : 0;
		}
		// if (apint_compare(a,b) > 0) { // a > b
		// 	// a = 0, b = -1? a=1, b=-1
		// 	// if (*a->data >= *b->data) { // need to check abs
		// 	// 	*result_data = *a->data - *b->data;
		// 	// } else {
		// 	// 	*result_data = *b->data - *a->data;
		// 	// }
		// 	result->flags = apint_is_negative(a) ? 1 : 0;
		// 	printf("a > b");
		// } else { // b > a 
		// 	*result_data = *b->data - *a->data;
		// 	result->flags = !apint_is_negative(a) ? 1 : 0;
		// }
		// Old
			// // neg pos
			// if (apint_is_negative(a)) {
			// 	// cmp 
			// 	if (apint_compare(a, b) > 0) { // a > b
			// 		*result_data = *a->data - *b->data;
			// 		result->flags = 1;
			// 	} else { // b > a 
			// 		*result_data = *b->data - *a->data;
			// 	}
			// } else { // pos neg
			// 	if (apint_compare(a, b) > 0) {
			// 		*result_data = *a->data - *b->data;
			// 	} else {
			// 		*result_data = *b->data - *a->data;
			// 		result->flags = 1;
			// 	}
			// }
	}

	printf("\ndata: %lu, %lu", *a->data, *b->data);
	// Compute
	// *result_data = a->data * multiplierA + b->data * multiplierB;
	// *result_data = *a->data + *b->data;
	result->data = result_data;
	printf("\nresult: %lu flag: %i\n", *result_data, result->flags);

	return result;
}


ApInt *apint_sub(const ApInt *a, const ApInt *b) {
	// Need to check free the opp?
	ApInt * opp = apint_negate(b);
	printf("\nb: %i %lu opp: %i %lu\n", b->flags, *b->data, opp->flags, *opp->data);
	ApInt * result = apint_add(a, opp);
	apint_destroy(opp);
	return result;
	// return apint_add(a, apint_negate(b));

	// // Allocate space
	// ApInt * result = malloc(sizeof(ApInt));
	// uint64_t * result_data = malloc(sizeof(uint64_t));
	
	// // Diff sign
	// if (apint_is_negative(a) != apint_is_negative(b)) {
	// 	*result_data = *a->data + *b->data;
	// 	// neg
	// 	result->flags = apint_is_negative(a) ? 1 : 0;
	// } else {
	// // Same sign
	// 	if (apint_compare(a, b) > 0) { // a > b
	// 		*result_data = *a->data - *b->data;
	// 		result->flags = !apint_is_negative(a) ? 1 : 0;
	// 	} else { // b > a 
	// 		*result_data = *b->data - *a->data;
	// 		result->flags = apint_is_negative(a) ? 1 : 0;
	// 	}
	// }

	// printf("\ndata: %lu, %lu", *a->data, *b->data);
	// // Allocate
	// result->data = result_data;
	// printf("\nresult: %lu\n", *result_data);

	// return result;
}

// @return left > right ? + : - or 0
int apint_compare(const ApInt *left, const ApInt *right) {
	// printf("\ndata: %lu, %lu", *left->data, *right->data);
	
	// cmp sign
	// Check if sign is diff
	if (left->flags != right->flags) {
		// flags = 1 if neg, 0 if pos
		return right->flags - left->flags;
	}
	// cmp len - TODO
	// cmp ints - u64
	if (*left->data != *right->data) {
		int compareResult = left->data > right->data;
		// printf("\ncompare res: %i\n", compareResult == 0 ? -1 : 1);
		return compareResult == 0 ? -1 : 1;
	}
	return 0;
}