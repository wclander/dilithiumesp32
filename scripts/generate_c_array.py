def generate_c_array(i : int):
    h_string = format(i, 'x')
    if (len(h_string) % 2 != 0):
        h_string = '0' + h_string
    # reverse string since we need little endian representation
    start = "".join(reversed([ '0x'+h_string[i:i+2]+', ' for i in range(2, len(h_string), 2) ]))
    # no comma on last byte
    last = '0x'+h_string[:2]
    return '{'+start + last + '}'

def main():
    m = 2**2048 + 1
    print(generate_c_array(m))
    print(generate_c_array(2**8192 % m))

if __name__ == "__main__":
    main()