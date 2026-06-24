# Cute (,,>﹏<,,)

**Cute** is a cute little language runtime that I am working on.

## Arch

The project is divided into two parts:
- Engine
- Assembler


### `Engine`
The main runtime that executes the bytecode.
+ Register-based execution
+ Typed instruction set
+ Dynamic containers (objects) supported along with reference GC

See the instruction set at: [instructions](docs/arch.md)

### `Assembler`
A simple assembler that turns human readable instructions into bytecode.
The assembler will be slowly evolved into a proper compiler.

Examples of the Cute assembly language are below.


### Example

Here is a basic program in the cute asm:

```

proc 0 [
	seti r0 10;      # Set r0 to int 10
	seti r1 5;       # Set r1 to int 5
	addi r2 r0 r1;   # Add r0 + r1 and store the result to r2
	out r2 0;        # Output the value in r2
	setu r255 0;     # Set r255 to 0
	halt r255;       # Halt engine with return code 0
]

; will print [ int 15 ]

```

Here is one with more than one procedures:

```

proc 0 [
	seti r0 5;
	setu r16 1;
	call r16;
	out r32 0; 
	setu r255 0
	halt r255;
]

proc 1 [
	muli r32 r0 r0;
	ret;
]

; procedure(1) acts a square function in this example
; will print out [ int 25 ]

```



