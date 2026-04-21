# rxm

A simple CLI x86 disassembler written in C that reads a raw binary file,
extracts bytes and tries to identify x86 opcodes.

---

## Setup

```bash
git clone https://github.com/jdias2019/rxm
cd rxm
chmod +x build.sh
./build.sh
```

## Usage

```bash
./rxm <file.bin>
```

## Example

```bash
nasm -f bin tests/test.asm -o tests/test.bin
./rxm tests/test.bin
```

