# Bitmapped font editor

This is a little utility to create and edit the binary font file format that I needed for another project.

The mapping is done for 1bpp, so a character that takes a matrix of 8x8 bits to be represented will occupy 8 bytes in the file.

## Usage

There are two possible options to perform operations: `-n` to create a new file, and `-w` to write to an existing file.

### Create new file

When creating a new file you have to enter the number of characters, and the width and height in pixels of each character and the name of the font file you want to create. The file will be created, padded with 0s. Character widths are fixed to multiples of 8, so if a width of 5 is entered, each row will still take a complete byte, or if a width of 9 is entered each row will take 2 bytes.

An example of creating a file is:

    $ bfe -n 128 9 8 myfontfile.bin

which will create a new empty file, with 2048 bytes of size, as each character takes 2 bytes per row * 8 * 128.

### Write to file

Since the format is so simple it has no header, so when you want to write in a character with `-w` you have to specify again the character dimensions, followed by the number of the character you're editing and the name of the font.

Once you're in writing-mode the program will take as many lines as rows per character from the standard input, each line taking a minimum of 0 values for the line to be completely zeroed-out, and a maximum of the character's width plus any padding that can still fit in the row's size for values.

Values are entered in binary, and the parsing is very permissive so any garbage in the input that cannot be made into a binary number will be taken as zero.

An example of writing to a file is:

```
$ bfe -w 5 8 65 myfontfile.bin
00000
00100
01010
10001
10001
11111
10001
10001
```

which will write into space 65, of a 5 * 8 font (that's at byte offset 0x208), the entered binary sequence.
