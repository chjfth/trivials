#!/usr/bin/env python3
"""
File block scanner with hex dump for non-zero blocks
"""

#[2026-09-03] Code by Deepseek

import sys
import os
import argparse
from typing import Optional


# Pre-compute lookup tables at module level (done once)
HEX_TRANS = bytearray(256 * 3)
for i in range(256):
    HEX_TRANS[i*3:(i+1)*3] = f'{i:02x} '.encode('ascii')

ASCII_TRANS = bytearray(256)
for i in range(256):
    ASCII_TRANS[i] = i if 32 <= i <= 126 else 0x2e  # '.'

def hex_dump(data: bytes, offset: int, bytes_per_line: int = 16) -> None:
    """
    Faster hex dump using pre-computed translation tables.
    Clean and safe implementation.
    """
    # Chj Note: This hexdump code is time consuming, even if you use '> foo.txt' to redirect it to file. 
    # So, if you have quite many blocks to dump, the program progresses very slow.

    length = len(data)
    
    # Build output using bytearray
    output = bytearray()
    
    # Header
    output.extend(f"Offset: 0x{offset:08x}\n".encode('ascii'))
    output.extend(b'-' * 60)
    output.append(0x0a)
    
    # Process each line
    for i in range(0, length, bytes_per_line):
        chunk = data[i:i + bytes_per_line]
        chunk_len = len(chunk)
        
        # Offset: "  XXXXXXXX  "
        output.extend(b'  ')
        output.extend(f'{i+offset:08x}'.encode('ascii'))
        output.extend(b'  ')
        
        # Hex part
        for b in chunk:
            start = b * 3
            output.extend(HEX_TRANS[start:start+3])
        
        # Pad hex part if needed
        if chunk_len < bytes_per_line:
            output.extend(b' ' * ((bytes_per_line - chunk_len) * 3))
        
        # Separator
        output.extend(b'  ')
        
        # ASCII part
        for b in chunk:
            output.append(ASCII_TRANS[b])
        
        # Newline
        output.append(0x0a)
    
    # Extra newline
    output.append(0x0a)
    
    # Single write
    sys.stdout.buffer.write(output)


def is_non_zero_block(data: bytes) -> bool:
    """
    Check if the block contains any non-zero bytes - Highly optimized.
    Using C-level operations for speed.
    """
    # This uses C-level memory operations - extremely fast
    return data != b'\x00' * len(data)


def scan_file(file_path: str, block_size: int = 4096, 
              skip_zero: bool = True, bytes_per_line: int = 16,
              progress_interval: Optional[float] = None) -> None:
    """
    Scan a file block by block and hex dump non-zero blocks.
    
    Args:
        file_path: Path to the file to scan
        block_size: Size of each block in bytes
        skip_zero: If True, skip blocks that are all zeros
        bytes_per_line: Number of bytes per line in hex dump
        progress_interval: Report progress every N MiB (None for no progress)
    """
    if block_size <= 0:
        raise ValueError("Block size must be positive")
    
    if bytes_per_line <= 0:
        raise ValueError("Bytes per line must be positive")
    
    if progress_interval is not None and progress_interval <= 0:
        raise ValueError("Progress interval must be positive")
    
    try:
        # Get file size
        file_size = os.path.getsize(file_path)
        
        # Send initial info to stderr
        sys.stderr.write(f"File: {file_path}\n")
        sys.stderr.write(f"File size: {file_size} bytes (0x{file_size:x})\n")
        sys.stderr.write(f"Block size: {block_size} bytes\n")
        sys.stderr.write(f"Total blocks: {(file_size + block_size - 1) // block_size}\n")
        if progress_interval is not None:
            sys.stderr.write(f"Progress reporting: every {progress_interval} MiB\n")
        sys.stderr.write("-" * 60 + "\n")
        
        # Check if file is empty
        if file_size == 0:
            sys.stderr.write("File is empty.\n")
            return
        
        blocks_printed = 0
        blocks_checked = 0
        bytes_read_total = 0
        
        # Progress tracking
        last_progress_report = 0
        progress_bytes = int(progress_interval * 1024 * 1024) if progress_interval is not None else None
        
        with open(file_path, 'rb') as f:
            while True:
                offset = f.tell()
                data = f.read(block_size)
                
                # End of file
                if not data:
                    break
                
                bytes_read_total += len(data)
                blocks_checked += 1
                
                # Check if block is all zeros
                if skip_zero and not is_non_zero_block(data):
                    # Still report progress if needed
                    if progress_bytes is not None:
                        bytes_since_last_report = bytes_read_total - last_progress_report
                        if bytes_since_last_report >= progress_bytes:
                            percent = (bytes_read_total / file_size) * 100
                            sys.stderr.write(f"Progress: {bytes_read_total:,} bytes read "
                                           f"({percent:.1f}%) at offset 0x{offset:08x}\n")
                            last_progress_report = bytes_read_total
                    continue
                
                # Print block offset and hex dump to stdout
                blocks_printed += 1
                print(f"Block #{blocks_printed} at offset 0x{offset:08x} "
                      f"({offset} bytes)")
                hex_dump(data, offset, bytes_per_line)
                
                # Report progress after processing the block
                if progress_bytes is not None:
                    bytes_since_last_report = bytes_read_total - last_progress_report
                    if bytes_since_last_report >= progress_bytes:
                        percent = (bytes_read_total / file_size) * 100
                        sys.stderr.write(f"Progress: {bytes_read_total:,} bytes read "
                                       f"({percent:.1f}%) at offset 0x{offset:08x}\n")
                        last_progress_report = bytes_read_total
        
        # Final progress report if needed
        if progress_bytes is not None and bytes_read_total > last_progress_report:
            percent = (bytes_read_total / file_size) * 100
            sys.stderr.write(f"Progress: {bytes_read_total:,} bytes read "
                           f"({percent:.1f}%) - Complete\n")
        
        # Summary to stderr
        sys.stderr.write("-" * 60 + "\n")
        sys.stderr.write(f"Scan complete: {blocks_checked:,} blocks checked, "
                        f"{blocks_printed:,} non-zero blocks printed.\n")
              
    except FileNotFoundError:
        sys.stderr.write(f"Error: File '{file_path}' not found.\n")
        sys.exit(1)
    except PermissionError:
        sys.stderr.write(f"Error: Permission denied to read '{file_path}'.\n")
        sys.exit(1)
    except IOError as e:
        sys.stderr.write(f"Error reading file: {e}\n")
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
  %(prog)s -p 10 file.bin    # Report progress every 10 MiB
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
        '-p', '--progress',
        type=float,
        metavar='MIB',
        help='Report progress every MIB MiB (e.g., -p 10 for every 10 MiB)'
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
            bytes_per_line=args.bytes_per_line,
            progress_interval=args.progress
        )
    except ValueError as e:
        sys.stderr.write(f"Error: {e}\n")
        sys.exit(1)
    except KeyboardInterrupt:
        sys.stderr.write("\nScan interrupted by user.\n")
        sys.exit(1)


if __name__ == '__main__':
    main()