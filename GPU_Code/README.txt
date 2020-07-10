input.txt has a format as follows;
1st line is first base key. Each digit represents 4 bits (hexadecimal format). Total 64 bits. Please use digits in this range: (0-1...9-A-...-E-F)
2nd line is second base key. Each digit represents 4 bits (hexadecimal format). Total 64 bits. Please use digits in this range: (0-1...9-A-...-E-F)
3rd line is third base key. Each digit represents 4 bits (hexadecimal format). Total 64 bits. Please use digits in this range: (0-1...9-A-...-E-F)
4th line represents how many 64bit blocks the plaintext has. For example: 4 blocks means there is a 4*64 bit plaintext in other words 32 chars in 5th line.
5th line is the actual plaintext in ascii char. Each char represents here 8 bits . 8 chars make 1 block (64 bit). Modify 4th and 5th line accordingly to satifsy the block size, plaintext size constraint.

User can modify those lines with the related guideline.
