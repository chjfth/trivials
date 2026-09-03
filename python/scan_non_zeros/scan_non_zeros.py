#!/usr/bin/env python3
"""
File block scanner with hex dump for non-zero blocks
([2026-09-03] Provided by Deepseek)
"""

import sys
import os
import argparse
from typing import Optional


def hex_dump(data: bytes, offset: int, bytes_per_line: int = 16) -> None:
    """
    Print a hex dump of the data with offset information.
    
    Args:
        data: Bytes to dump
        offset: Starting offset in the file
        bytes_per_line: Number of bytes to display per line
    """
    print(f"Offset: 0x{offset:08x}")
    print("-" * 60)
    
    for i in range(0, len(data), bytes_per_line):
        chunk = data[i:i + bytes_per_line]
        hex_part = ' '.join(f'{b:02x}' for b in chunk)
        
        # ASCII representation (printable only)
        ascii_part = ''.join(chr(b) if 32 <= b <= 126 else '.' for b in chunk)
        
        # Format the output
        print(f"  {i+offset:08x}  {hex_part:<48}  {ascii_part}")
    
    print()


def is_non_zero_block(data: bytes) -> bool:
    """
    Check if the block contains any non-zero bytes.
    
    Args:
        data: Block of bytes to check
    
    Returns:
        True if block has any non-zero byte, False otherwise
    """
    return any(b != 0 for b in data)


def scan_file(file_path: str, block_size: int = 4096, 
              skip_zero: bool = True, bytes_per_line: int = 16) -> None:
    """
    Scan a file block by block and hex dump non-zero blocks.
    
    Args:
        file_path: Path to the file to scan
        block_size: Size of each block in bytes
        skip_zero: If True, skip blocks that are all zeros
        bytes_per_line: Number of bytes per line in hex dump
    """
    if block_size <= 0:
        raise ValueError("Block size must be positive")
    
    if bytes_per_line <= 0:
        raise ValueError("Bytes per line must be positive")
    
    try:
        # Get file size
        file_size = os.path.getsize(file_path)
        print(f"File: {file_path}")
        print(f"File size: {file_size} bytes (0x{file_size:x})")
        print(f"Block size: {block_size} bytes")
        print(f"Total blocks: {(file_size + block_size - 1) // block_size}")
        print("-" * 60)
        
        # Check if file is empty
        if file_size == 0:
            print("File is empty.")
            return
        
        blocks_printed = 0
        blocks_checked = 0
        
        with open(file_path, 'rb') as f:
            while True:
                offset = f.tell()
                data = f.read(block_size)
                
                # End of file
                if not data:
                    break
                
                blocks_checked += 1
                
                # Check if block is all zeros
                if skip_zero and not is_non_zero_block(data):
                    continue
                
                # Print block offset and hex dump
                blocks_printed += 1
                print(f"Block #{blocks_printed} at offset 0x{offset:08x} "
                      f"({offset} bytes)")
                hex_dump(data, offset, bytes_per_line)
        
        # Summary
        print("-" * 60)
        print(f"Scan complete: {blocks_checked} blocks checked, "
              f"{blocks_printed} non-zero blocks printed.")
              
    except FileNotFoundError:
        print(f"Error: File '{file_path}' not found.", file=sys.stderr)
        sys.exit(1)
    except PermissionError:
        print(f"Error: Permission denied to read '{file_path}'.", file=sys.stderr)
        sys.exit(1)
    except IOError as e:
        print(f"Error reading file: {e}", file=sys.stderr)
        sys.exit(1)


def main() -> None:
    """Main entry point with argument parsing."""
    parser = argparse.ArgumentParser(
        description="Scan a file block by block and hex dump non-zero blocks.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s file.bin
  %(prog)s -b 1024 file.bin
  %(prog)s -b 2048 --show-zero file.bin
  %(prog)s -b 4096 -l 8 file.bin
        """
    )
    
    parser.add_argument(
        'file',
        help='Path to the file to scan'
    )
    
    parser.add_argument(
        '-b', '--block-size',
        type=int,
        default=4096,
        help='Block size in bytes (default: 4096)'
    )
    
    parser.add_argument(
        '-z', '--show-zero',
        action='store_true',
        help='Also show blocks that are all zeros (default: skip zero blocks)'
    )
    
    parser.add_argument(
        '-l', '--bytes-per-line',
        type=int,
        default=16,
        help='Number of bytes per line in hex dump (default: 16)'
    )
    
    parser.add_argument(
        '--version',
        action='version',
        version='%(prog)s 1.0.0'
    )
    
    args = parser.parse_args()
    
    try:
        scan_file(
            file_path=args.file,
            block_size=args.block_size,
            skip_zero=not args.show_zero,
            bytes_per_line=args.bytes_per_line
        )
    except ValueError as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)
    except KeyboardInterrupt:
        print("\nScan interrupted by user.", file=sys.stderr)
        sys.exit(1)


if __name__ == '__main__':
    main()

