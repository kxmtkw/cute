## Instruction Set

Each instruction has the following format:

```
instruction [destination] [operands]
```
Instructions are divided into different categories.
The prefix `rx` will represent any register, `sx` will represent any slot. `lb` is used for literal bytes, i.e. for ints, uints, floats in the bytecode.

In total, there are `67` instructions.

### Engine Control

These are used to control and manipulate the engine and get debug reports etc.

#### Halt `0x00`
Causes the engine to halt. Ends all contexts. Nuclear option really.
```
halt rx
```

#### Null `0x01`
Does nothing. Can be used as a safe placeholder, if writing a compiler.
```
null
```

#### Out `0x10`
Used to display the type and data held by an atom.
```
out rx
```

### Memory Operations

These instructions are used to transfer data.

#### Mov `0x20`
Moves (or rather copies) data from one atom to another.
```
mov rx ry
```

#### Load `0x21`
Loads an atom from a slot to a register.
```
load rx sx
```

#### Store `0x22`
Stores an atom from a register to a slot.
```
store sx rx
```

#### LoadG `0x23`
Loads an atom from a global slot to a register.
```
loadg rx sx
```

#### StoreG `0x24`
Stores an atom from a register to a global slot.
```
storeg sx rx
```

#### SetI `0x25`
Sets an `int` atom to a register.
```
seti rx lb
```

#### SetU `0x26`
Sets an `uint` atom to a register.
```
setu rx lb
```

#### SetF `0x27`
Sets a `float` atom to a register.
```
setf rx lb
```

#### SetB `0x28`
Sets an `bool` atom to a register.
```
setb rx lb
```

#### SetC `0x29`
Sets an `char` atom to a register.
```
setc rx lb
```

#### SetN `0x2A`
Sets an `none` atom to a register.
```
setu rx
```

### Arithmetic

#### AddI `0x30`

#### SubI `0x31`

#### MulI `0x32`

#### DivI `0x33`

#### ModI `0x34`

#### NegI `0x35`

#### AddU `0x40`

#### SubU `0x41`

#### MulU `0x42`

#### DivU `0x43`

#### ModU `0x44`

#### AddF `0x50`

#### SubF `0x51`

#### MulF `0x52`

#### DivF `0x53`

#### NegF `0x54`

All of these are self-explantory.
- The binary operations have the format:
```
opcode rx ry rz
```
- The unary operations have the format:
```
opcode rx ry
```
Where `rx` is the dest and,`ry` and `rz` are the source registers.

All operands are registers. `src`s are type checked before the operation is performed.

### Logic

#### LogicAnd `0x60`

#### LogicOr `0x61`

#### LogicNot `0x62`

All of these are self-explantory.
- The binary operations have the format:
```
opcode rx ry rz
```
- The unary operations have the format:
```
opcode rx ry
```
Where `rx` is the dest and,`ry` and `rz` are the source registers.

The logic operations only work on the `bool` atom.

### Bitwise

#### BitAnd `0x70`

#### BitOr `0x71`

#### BitNot `0x73`

#### BitXor `0x74`

#### BitShl `0x75`

#### BitShr `0x76`

All of these are self-explantory.
- The binary operations have the format:
```
opcode rx ry rz
```
- The unary operations have the format:
```
opcode rx ry
```
Where `rx` is the dest and,`ry` and `rz` are the source registers.

The logic operations only work on the `uint` atom.

### Comparison

#### CmpI `0x80`

#### CmpU `0x81`

#### CmpF `0x82`

All of these are self-explantory.
- The binary operations have the format:
```
cmpi/cmpu/cmpf rx ry rz
```

Where `rx` stores a difference between the values as `int`. This is usless unless you use the following cmp resolvers which turn this difference into a boolean value:

#### Eq `0x90`

#### NotEq `0x91`

#### Less `0x92`

#### LessEq `0x93`

#### Greater `0x94`

#### GreaterEq `0x95`

All of these have the format:
```
opcode rx
```
The register `rx` is mutated in place and this instruction has no destination register.

### Control Flow

#### Jmp `0xA0`
Jump to a specific offset.
```
jmp lb
```
`lb` is read as an int and the instruction pointer is offset as required.

#### JmpIf `0xA1`

#### JmpIfNot `0xA2`
Similar to `jmp` but conditional.
```
jmpif/jmpifnot rx lb
```
Where rx must hold a `bool` atom.

#### JmpAbs `0xA3`
Jump to a absolute address.
```
jma lb
```
`lb` is read as an uint and the instruction pointer is offset as required.

#### JmpAbsIf `0xA4`

#### JmpAbsIfNot `0xA5`

Similar to `jma` but conditional.
```
jmaif/jmaifnot rx lb
```
Where rx must hold a `bool` atom.

> All jmp instructions are designed to jump anywhere in the bytecode but the assembler is so made that jumps can only be done within a procedure.

### Procedures

#### Call `0xB0`
Call a prcodeure.
```
call rx
```
Where `rx` stores the Id of the procedure to be called. It must be a `uint`

#### Return `0xB1`
Return from a procedure
```
ret
```
Note that values can't be returned. This just returns to the caller.


### Containers

#### ConNew `0xC1`
Make a new container with a given size.
```
connew rx
```
Where `rx` is a `uint` that holds the size of the required container.

#### ConDel `0xC2`
Delete the particular reference of the container.
```
condel rx
```
Where rx must hold a `container` atom. After deletion, `rx` holds `none`.
> This does NOT delete the container but just deletes the targeted reference and decreases the refcount.

#### ConGet `0xC3`
Get an atom from the container.
```
conget rx ry rz
```
Where `rx` is the dest register, `ry` holds the container and `rz` holds the index required. 

#### ConSet `0xC4`
Set an atom in a container
```
conset rx ry rz
```
Where `rx` holds the container, `ry` holds the index and `rz` holds the atom to be stored. 

#### ConLen `0xC5`
Get the length or size of a container.
```
conlen rx ry
```
Where `rx` is the dest register and `ry` holds the container.

#### ConResize `0xC6`
Resize a container in-place.
```
conresize rx ry
```
Where `rx` holds the container and `ry` holds the updated size.
The runtime will:
- Expand normally if `new_size` > `current_size`
- Truncate atoms if `new_size` < `current_size`
- Skip if `new_size` == `current_size`


#### ConClone `0xC7`
Create a shallow clone of a container.
```
conclone rx ry
```
Where `rx` is the dest register and `ry` holds the container to be cloned.

> Sub Containers are NOT cloned, however their refcount is incremented as usual.


