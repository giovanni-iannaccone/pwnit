from pwn import *

{{ binaries }}

context.binary = {{ elf_var }}

HOST = "addr"
PORT = 1337

gdbscript = """
set follow-fork-mode child
"""

args.LOCAL = True
args.DEBUG = True

def conn():
    if args.LOCAL:
        {{ remote }} = process(exe.path)
        if args.DEBUG:
            gdb.attach({{ remote }}, gdbscript=gdbscript)
    else:
        {{ remote }} = remote(HOST, PORT)

    return {{ remote }}

def main():
    {{ remote }}  = conn()

    # good luck pwning :D

    {{ remote }}.interactive()

if __name__ == "__main__":
    main()
