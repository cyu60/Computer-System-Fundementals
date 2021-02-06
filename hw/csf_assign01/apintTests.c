/*
 * Unit tests for arbitrary-precision integer data type
 *
 * These tests are by no means comprehensive.  You will need to
 * add more tests of your own!  In particular, make sure that
 * you have tests for more challenging situations, such as
 *
 * - large values
 * - adding/subtracting/comparing values with different lengths
 * - special cases (carries when adding, borrows when subtracting, etc.)
 * - etc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "apint.h"
#include "tctest.h"

typedef struct {
	ApInt *ap0;
	ApInt *ap1;
	ApInt *ap110660361;
	ApInt *max1;
	ApInt *minus1;
	/* TODO: add additional fields of test fixture */
	ApInt *_ap110660361;

	ApInt *hex0;
	ApInt *hexR1;
	ApInt *hexR2;
	ApInt *hexR3;
	ApInt *hexR4;
	ApInt *hex_minus1;

} TestObjs;

TestObjs *setup(void);
void cleanup(TestObjs *objs);

void testCreateFromU64(TestObjs *objs);
void testCreateFromHex(TestObjs *objs);
void testHighestBitSet(TestObjs *objs);
void testComparePosU64(TestObjs *objs);
void testCompareNegU64(TestObjs *objs);
void testFormatAsHex(TestObjs *objs);
void testAdd(TestObjs *objs);
void testSub(TestObjs *objs);
/* TODO: add more test function prototypes */
void testApInt_is_negative(TestObjs *objs);
void testNull(TestObjs *objs);

int main(int argc, char **argv) {
	TEST_INIT();

	if (argc > 1) {
		/*
		 * name of specific test case to execute was provided
		 * as a command line argument
		 */
		tctest_testname_to_execute = argv[1];
	}

	TEST(testCreateFromU64);
	TEST(testHighestBitSet);
	TEST(testComparePosU64);
	TEST(testCompareNegU64);
	TEST(testFormatAsHex);
	TEST(testAdd);
	TEST(testSub);
	/* TODO: use TEST macro to execute more test functions */
	TEST(testApInt_is_negative);
	TEST(testNull);

	TEST_FINI();
}

TestObjs *setup(void) {
	TestObjs *objs = malloc(sizeof(TestObjs));
	objs->ap0 = apint_create_from_u64(0UL);
	objs->ap1 = apint_create_from_u64(1UL);
	objs->ap110660361 = apint_create_from_u64(110660361UL);
	objs->max1 = apint_create_from_u64(0xFFFFFFFFFFFFFFFFUL);
	objs->minus1 = apint_negate(objs->ap1);
	/* TODO: initialize additional members of test fixture */
	objs->_ap110660361 = apint_negate(objs->ap110660361);
	// int testInt = 1;
	// objs->testInt_should_fail = apint_create_from_u64(testInt);

	objs->hexR1 = apint_create_from_hex("f01df2A843bcdb32"); //16
	objs->hexR2 = apint_create_from_hex("bc43f0b6666843bc3421"); //20
	objs->hexR3 = apint_create_from_hex("564268bb"); //8
	objs->hexR4 = apint_create_from_hex("-11112222333344445555666677778888");
	objs->hex0 = apint_create_from_hex("0");
	objs->hex_minus1 = apint_create_from_hex("-1");


	return objs;
}

void cleanup(TestObjs *objs) {
	apint_destroy(objs->ap0);
	apint_destroy(objs->ap1);
	apint_destroy(objs->ap110660361);
	apint_destroy(objs->max1);
	apint_destroy(objs->minus1); // ISSUE?
	/* TODO: destroy additional members of test fixture */
	apint_destroy(objs->_ap110660361);

	apint_destroy(objs->hex0);
	apint_destroy(objs->hexR1);
	apint_destroy(objs->hexR2);
	apint_destroy(objs->hexR3);
	apint_destroy(objs->hexR4);
	apint_destroy(objs->hex_minus1);



	free(objs);
}

void testNull(TestObjs *objs) {
	ApInt * nullptr;
	// try {
		// apint_get_bits(nullptr, 0);
	// } catch {

	// }
	// ASSERT(0UL == apint_get_bits(objs->ap0, 0));
	// ASSERT(1UL == apint_get_bits(objs->ap1, 0));
	// ASSERT(110660361UL == apint_get_bits(objs->ap110660361, 0));
	// ASSERT(0xFFFFFFFFFFFFFFFFUL == apint_get_bits(objs->max1, 0));
	// ASSERT(0xFFFFFFFFFFFFFFFFUL == apint_get_bits(objs->max1, 0));
}

void testCreateFromU64(TestObjs *objs) {
	ASSERT(0UL == apint_get_bits(objs->ap0, 0));
	ASSERT(1UL == apint_get_bits(objs->ap1, 0));
	ASSERT(110660361UL == apint_get_bits(objs->ap110660361, 0));
	ASSERT(0xFFFFFFFFFFFFFFFFUL == apint_get_bits(objs->max1, 0));
	ASSERT(0xFFFFFFFFFFFFFFFFUL == apint_get_bits(objs->max1, 0));
}

void testCreateFromHex(TestObjs *objs) {
	ASSERT(0UL == apint_get_bits(objs->hex0, 0));
	ASSERT(0xf01df2A843bcdb32UL == apint_get_bits(objs->hexR1, 0));
	ASSERT(0xf0b6666843bc3421UL == apint_get_bits(objs->hexR2, 0));
	ASSERT(0xbc43UL == apint_get_bits(objs->hexR2, 1));
	ASSERT(0x564268bbUL == apint_get_bits(objs->hexR3, 0));
}

void testNegateApint(TestObjs *objs) {
	// Test 0
	ASSERT(0 == apint_is_negative(objs->ap0));
	// Test pos
	ASSERT(0 == apint_is_negative(objs->ap1));
	ASSERT(0 == apint_is_negative(objs->ap110660361));
	ASSERT(0 == apint_is_negative(objs->max1));
	// Test neg
	ASSERT(1 == apint_is_negative(objs->minus1));
}

void testApInt_is_negative(TestObjs *objs) {
	// Test 0
	ASSERT(0 == apint_is_negative(objs->ap0));
	// Test pos
	ASSERT(0 == apint_is_negative(objs->ap1));
	ASSERT(0 == apint_is_negative(objs->ap110660361));
	ASSERT(0 == apint_is_negative(objs->max1));
	// Test neg
	ASSERT(1 == apint_is_negative(objs->minus1));
}

void testHighestBitSet(TestObjs *objs) {
	ASSERT(-1 == apint_highest_bit_set(objs->ap0));
	ASSERT(0 == apint_highest_bit_set(objs->ap1));
	ASSERT(26 == apint_highest_bit_set(objs->ap110660361));
	ASSERT(63 == apint_highest_bit_set(objs->max1));
}

void testComparePosU64(TestObjs *objs) {
	/* 1 > 0 */
	ASSERT(apint_compare(objs->ap1, objs->ap0) > 0);
	/* 0 < 1 */
	ASSERT(apint_compare(objs->ap0, objs->ap1) < 0);
	/* 110660361 > 0 */
	ASSERT(apint_compare(objs->ap110660361, objs->ap0) > 0);
	/* 110660361 > 1 */
	ASSERT(apint_compare(objs->ap110660361, objs->ap1) > 0);
	/* 0 < 110660361 */
	ASSERT(apint_compare(objs->ap0, objs->ap110660361) < 0);
	/* 1 < 110660361 */
	ASSERT(apint_compare(objs->ap1, objs->ap110660361) < 0);
}

void testCompareNegU64(TestObjs *objs) {
	/* 0 > -1 */
	ASSERT(apint_compare(objs->ap0, objs->minus1) > 0);
	/* -1 < 1 */
	ASSERT(apint_compare(objs->minus1, objs->ap1) < 0);
	/* 110660361 > -1 */
	ASSERT(apint_compare(objs->ap110660361, objs->minus1) > 0);
	/* -1 < 110660361 */
	ASSERT(apint_compare(objs->minus1, objs->ap110660361) < 0);

	// Additional tests big num
	/* 1 < bc43f0b6666843bc3421*/
	ASSERT(apint_compare(objs->ap1, objs->hexR2) < 0);
	/* bc43f0b6666843bc3421 > 0*/
	ASSERT(apint_compare(objs->hexR2, objs->ap0) > 0);
	
	/* f01df2A843bcdb32 < bc43f0b6666843bc3421*/
	ASSERT(apint_compare(objs->hexR1, objs->hexR2) < 0);
	/* bc43f0b6666843bc3421 = bc43f0b6666843bc3421 */
	ASSERT(apint_compare(objs->hexR2, objs->hexR2) == 0);

	ApInt *a = apint_create_from_hex("bc43b0b6666843bc3421");
	/* bc43f0b6666843bc3421 > bc43b0b6666843bc3421 */
	ASSERT(apint_compare(objs->hexR2, a) > 0);
	apint_destroy(a);
}

void testFormatAsHex(TestObjs *objs) {
	char *s;

	ASSERT(0 == strcmp("0", (s = apint_format_as_hex(objs->ap0))));
	free(s);

	ASSERT(0 == strcmp("1", (s = apint_format_as_hex(objs->ap1))));
	free(s);

	ASSERT(0 == strcmp("6988b09", (s = apint_format_as_hex(objs->ap110660361))));
	free(s);

	ASSERT(0 == strcmp("ffffffffffffffff", (s = apint_format_as_hex(objs->max1))));
	free(s);

	// Neg
	ASSERT(0 == strcmp("-1", (s = apint_format_as_hex(objs->minus1))));
	free(s);

	ASSERT(0 == strcmp("bc43f0b6666843bc3421", (s = apint_format_as_hex(objs->hexR2))));
	free(s);

	ASSERT(0 == strcmp("-11112222333344445555666677778888", (s = apint_format_as_hex(objs->hexR4))));
	free(s);

}

// Use diff vals!
void testAdd(TestObjs *objs) {
	// Simple 64x -- need to allocate and free!
	/* 0 + 0 = 0 */
	// ASSERT(0UL == apint_get_bits(apint_add(objs->ap0, objs->ap0), 0));
	// ASSERT(1UL == apint_get_bits(apint_add(objs->ap1, objs->ap0), 0));
	// Plus neg
	// ASSERT(0UL == apint_get_bits(apint_add(objs->ap1, objs->minus1), 0));

	ApInt *sum, *a, *b;
	char *s;

	/* 0 + 0 = 0 */
	sum = apint_add(objs->ap0, objs->ap0);
	ASSERT(0 == strcmp("0", (s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	free(s);

	/* 1 + 0 = 1 */
	sum = apint_add(objs->ap1, objs->ap0);
	ASSERT(0 == strcmp("1", (s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	free(s);

	/* 1 + 1 = 2 */
	sum = apint_add(objs->ap1, objs->ap1);
	ASSERT(0 == strcmp("2", (s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	free(s);

	/* 110660361 + 1 = 110660362 */
	sum = apint_add(objs->ap110660361, objs->ap1);
	ASSERT(0 == strcmp("6988b0a", (s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	free(s);

	/* 1 + -1 = 0 */
	sum = apint_add(objs->ap1, objs->minus1);
	ASSERT(0 == strcmp("0", (s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	free(s);

	/* -1 + 1 = 0 */
	sum = apint_add(objs->minus1, objs->ap1);
	ASSERT(0 == strcmp("0", (s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	free(s);

	// large sum
	/* FFFFFFFFFFFFFFFF + 1 = 10000000000000000 */
	sum = apint_add(objs->max1, objs->ap1);
	ASSERT(0 == strcmp("10000000000000000", (s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	free(s);

	// Additional tests
	/* 1 + FFFFFFFFFFFFFFFF = 10000000000000000 */
	sum = apint_add(objs->ap1, objs->max1);
	ASSERT(0 == strcmp("10000000000000000", (s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	free(s);

	/* bc43f0b6666843bc3421 + 0 = bc43f0b6666843bc3421 */
	sum = apint_add(objs->ap0, objs->hexR2);
	ASSERT(0 == strcmp("bc43f0b6666843bc3421", (s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	free(s);

	// Additional tests
	/* test involving larger values */
	a = apint_create_from_hex("7e5ff912c8ede6ccff0d56ae5a9b5459804f9");
	b = apint_create_from_hex("6057bccd860546f03fd51bf5488d50cca96");
	sum = apint_add(a, b);
	ASSERT(0 == strcmp("7ec050cf9673ec13ef4d2bca4fe3e1aa4cf8f",
       	(s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	apint_destroy(b);
	apint_destroy(a);
	free(s);

	a = apint_create_from_hex("439284b10da826ceef1ca7435a3f300a7");
	b = apint_create_from_hex("e460e18a60c7345781147ab3ca122fd4c6b2c9dda98d2245431005e64614500cd8610d129c9bdb9cdecfa38358eb02");
	sum = apint_add(a, b);
	ASSERT(0 == strcmp("e460e18a60c7345781147ab3ca122fd4c6b2c9dda98d2245431005e64614544600ac1ded1f08ca8ea943d9274beba9",
       	(s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	apint_destroy(b);
	apint_destroy(a);
	free(s);

	a = apint_create_from_hex("c8ed2c2ef8832a");
	b = apint_create_from_hex("3bac3c2a7dd6df38b3fc4c0165d8a0ef31f523cb7614c6efe900");
	sum = apint_add(a, b);
	ASSERT(0 == strcmp("3bac3c2a7dd6df38b3fc4c0165d8a0ef31f524946340f5e86c2a",
       	(s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	apint_destroy(b);
	apint_destroy(a);
	free(s);

	a = apint_create_from_hex("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
	b = apint_create_from_hex("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
	sum = apint_add(a, b);
	ASSERT(0 == strcmp("1fffffffffffffffffffffffffffffffe",
       	(s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	apint_destroy(b);
	apint_destroy(a);
	free(s);

	// Additional tests
	/* neg test involving larger values */
	a = apint_create_from_hex("-7e5ff912c8ede6ccff0d56ae5a9b5459804f9");
	b = apint_create_from_hex("-6057bccd860546f03fd51bf5488d50cca96");
	sum = apint_add(a, b);
	ASSERT(0 == strcmp("-7ec050cf9673ec13ef4d2bca4fe3e1aa4cf8f",
       	(s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	apint_destroy(b);
	apint_destroy(a);
	free(s);

	a = apint_create_from_hex("-439284b10da826ceef1ca7435a3f300a7");
	b = apint_create_from_hex("-e460e18a60c7345781147ab3ca122fd4c6b2c9dda98d2245431005e64614500cd8610d129c9bdb9cdecfa38358eb02");
	sum = apint_add(a, b);
	ASSERT(0 == strcmp("-e460e18a60c7345781147ab3ca122fd4c6b2c9dda98d2245431005e64614544600ac1ded1f08ca8ea943d9274beba9",
       	(s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	apint_destroy(b);
	apint_destroy(a);
	free(s);

	a = apint_create_from_hex("-c8ed2c2ef8832a");
	b = apint_create_from_hex("-3bac3c2a7dd6df38b3fc4c0165d8a0ef31f523cb7614c6efe900");
	sum = apint_add(a, b);
	ASSERT(0 == strcmp("-3bac3c2a7dd6df38b3fc4c0165d8a0ef31f524946340f5e86c2a",
       	(s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	apint_destroy(b);
	apint_destroy(a);
	free(s);

	a = apint_create_from_hex("-FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
	b = apint_create_from_hex("-FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
	sum = apint_add(a, b);
	ASSERT(0 == strcmp("-1fffffffffffffffffffffffffffffffe",
       	(s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	apint_destroy(b);
	apint_destroy(a);
	free(s);
}

void testSub(TestObjs *objs) {
	ApInt *a, *b, *diff;
	char *s;

	/* subtracting 1 from ffffffffffffffff is fffffffffffffffe */
	diff = apint_sub(objs->max1, objs->ap1);
	ASSERT(0 == strcmp("fffffffffffffffe", (s = apint_format_as_hex(diff))));
	apint_destroy(diff);
	free(s);

	/* subtracting 0 from 1 is 1 */
	diff = apint_sub(objs->ap1, objs->ap0);
	ASSERT(0 == strcmp("1", (s = apint_format_as_hex(diff))));
	ASSERT(0 == apint_compare(diff, objs->ap1));
	apint_destroy(diff);
	free(s);

	/* subtracting 1 from 1 is 0 */
	diff = apint_sub(objs->ap1, objs->ap1);
	ASSERT(0 == strcmp("0", (s = apint_format_as_hex(diff))));
	ASSERT(0 == apint_compare(diff, objs->ap0));
	apint_destroy(diff);
	free(s);

	/* subtracting 1 from 0 is -1 */
	diff = apint_sub(objs->ap0, objs->ap1);
	ASSERT(0 == (strcmp("-1", (s = apint_format_as_hex(diff)))));
	ASSERT(0 == apint_compare(diff, objs->minus1));
	apint_destroy(diff);
	free(s);

	/* test involving larger values */
	a = apint_create_from_hex("7e35207519b6b06429378631ca460905c19537644f31dc50114e9dc90bb4e4ebc43cfebe6b86d");
	b = apint_create_from_hex("9fa0fb165441ade7cb8b17c3ab3653465e09e8078e09631ec8f6fe3a5b301dc");
	diff = apint_sub(a, b);
	ASSERT(0 == strcmp("7e35207519b6afc4883c6fdd8898213a367d73b918de95f20766963b0251c622cd3ec4633b691",
		(s = apint_format_as_hex(diff))));
	apint_destroy(diff);
	apint_destroy(b);
	apint_destroy(a);
	free(s);

	/* test involving larger values (with a negative difference) */
	a = apint_create_from_hex("9fa0fb165441ade7cb8b17c3ab3653465e09e8078e09631ec8f6fe3a5b301dc");
	b = apint_create_from_hex("7e35207519b6b06429378631ca460905c19537644f31dc50114e9dc90bb4e4ebc43cfebe6b86d");
	diff = apint_sub(a, b);
	ASSERT(0 == strcmp("-7e35207519b6afc4883c6fdd8898213a367d73b918de95f20766963b0251c622cd3ec4633b691",
		(s = apint_format_as_hex(diff))));
	apint_destroy(diff);
	apint_destroy(b);
	apint_destroy(a);
	free(s);
}

/* TODO: add more test functions */

