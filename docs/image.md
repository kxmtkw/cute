
# Program Image Format (OUTDATED)

A program image file contains the following sections:

## Header

Used to parse the rest of the image file. Consists of the following components:

+ Magic: Used to check whether the file is a valid image
+ Constant Pool Start: The file offset from the start where the constant pool starts.
+ Constant Count: Number of constants.
+ Function Table Start: The offset from where the function table starts.
+ Function Count: Number of functions in the table.
+ Instruction Blob Start: The offset where the instruction blob starts.
+ Instruction Count: Number of instructions in the blob.


## Constant Pool 

Contains constants. Note that as int and float types are stored directly in the bytecode, the constant pool is used for types that will cause the bytecode to bloat. Examples include strings and primitive arrays.

Constants are present in the following format inside the bytecode:
```
[type] [info] [data]
```
For constant arrays of primitives.
```
[0] [len, size] [size[len]] 
```
For string, this will be:
```
[1] [len] [chars[len]]
```
Both len and size are uint32.

For arrays of strings, the strings are stored in the constant pool but since string is a container type, the compiler needs to generate bytecode that will first make the string containers using the chars in bytecode and then manually add each string to the array.

Same priniciple applies to other complex arrays.

## Function Table

A table of defined functions. Each function consists of the following format:
```
[function_id] [locals_count] [args_count] [instruction_address]
```

## Instruction Blob

A continuous array of instructions and operands. Each instruction documents its operand count and size in instr.md