## Cute 

**Cute** is a cute little language runtime that I am working on.

### Features

Cute is a typed virtual machine. Though I am still finalizing on the architecture.

Here is a basic program in the cute asm:

```

0 [
	loadi r0 10  ; load an integar with value 10 to r0
	loadi r1 5   ; load an integar with value 5 to r1 
	addi r0 r1   ; add r0 and r1 and store the result back to r0
	out r0 0     ; output the value of r0, the 0 is the format.
	halt 0       ; halt the engine with code 0
]

; will print [ int 15 ]

```

Here is one with more than one procedures:

```

0 [
	loadi r0 5
	call 1
	out r0 0 
	halt 0
]

1 [
	muli r0 r0
	ret
]

; procedure(1) acts a square function in this example
; will print out [ int 25 ]

```



