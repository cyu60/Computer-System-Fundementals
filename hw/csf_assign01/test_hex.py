
import os,binascii
import random

def hex_rad():
    ran = random.randrange(10**80)
    return "%064x" % ran

def print_test(a, b, result):
    code = f"""
    a = apint_create_from_hex("{a}");
	b = apint_create_from_hex("{b}");
	sum = apint_add(a, b);
	ASSERT(0 == strcmp("{result}",
       	(s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	apint_destroy(b);
	apint_destroy(a);
	free(s);
    """
    print(code)

print("// Neg Addition")
for _ in range(10):
    a = hex_rad()
    b = hex_rad()

    # myhex1 = hex(a) + hex(b)
    myhex1 = int(a, 16) + int(b, 16)
    # myhex1 = hex(int(a,16)) + hex(int(b, 16))

    # print(a, b, format(myhex1, 'x'))
    # print_test(a, b, format(myhex1, 'x'))
    print_test("-"+a, "-" + b, "-" + format(myhex1, 'x'))

print("// Diff signs")
for _ in range(10):
    a = hex_rad()
    b = hex_rad()

    myhex1 = int(a, 16) - int(b, 16)

    # print(a, '-' + b, format(myhex1, 'x'))
    print_test('-' + b, a, format(myhex1, 'x'))
    # print_test(a, '-' + b, format(myhex1, 'x'))

# # print(hex(binascii.b2a_hex(os.urandom(15))))

# def randhex():

#     return "\\x" + str(random.choice("0123456789ABCDEF")) + str(random.choice("0123456789ABCDEF"))


# # test = ''.join(random.choice('0123456789abcdef') for n in range(30)
# test = randhex()
# print(test)

sum = 0xffffffffffffffff11111111111111111111111111111111 - 0xffffffffffffffffffffffffffffffff
print(hex(sum))

