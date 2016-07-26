#! /bin/bash

if [ $# -ne 2 ]; then
    echo "Usage: $0 <config> <elf>"
    exit 1
fi

openocd \
    -f "$1" \
    -c init \
    -c targets \
    -c "reset init" \
    -c "halt" \
    -c "flash write_image erase "$2"" \
    -c "verify_image "$2"" \
    -c "reset run" \
    -c shutdown
