#!/usr/bin/env python3
"""Tiny LZSS compressor for DOS Zine DAT files (Python standard library only).

The stream is: b'DZ1\0', little-endian uint32 unpacked size, then groups of
one flag byte and up to eight items. A set flag bit is a literal; a clear bit
is a two-byte (12-bit distance-1, 4-bit length-3) match.
"""

import argparse
import struct
from collections import defaultdict, deque

MAGIC = b"DZ1\0"
WINDOW = 4096
MIN_MATCH = 3
MAX_MATCH = 18


def compress(data: bytes) -> bytes:
    positions = defaultdict(deque)
    body = bytearray()
    pos = 0

    while pos < len(data):
        flag_at = len(body)
        body.append(0)
        flags = 0

        for bit in range(8):
            if pos >= len(data):
                break

            best_len = 0
            best_distance = 0
            key = data[pos:pos + MIN_MATCH]
            candidates = positions.get(key)
            if candidates:
                oldest = pos - WINDOW
                while candidates and candidates[0] < oldest:
                    candidates.popleft()

                # Recent matches tend to be good; cap work on repetitive data.
                for previous in reversed(list(candidates)[-128:]):
                    limit = min(MAX_MATCH, len(data) - pos)
                    length = 0
                    while length < limit and data[previous + length] == data[pos + length]:
                        length += 1
                    if length > best_len:
                        best_len = length
                        best_distance = pos - previous
                        if length == MAX_MATCH:
                            break

            if best_len >= MIN_MATCH:
                code = (best_distance - 1) | ((best_len - MIN_MATCH) << 12)
                body.extend((code & 0xFF, code >> 8))
                consumed = best_len
            else:
                flags |= 1 << bit
                body.append(data[pos])
                consumed = 1

            # Add every consumed position so overlapping/repetitive matches work.
            end = min(pos + consumed, len(data))
            for added in range(pos, end):
                if added + MIN_MATCH <= len(data):
                    positions[data[added:added + MIN_MATCH]].append(added)
            pos = end

        body[flag_at] = flags

    return MAGIC + struct.pack("<I", len(data)) + body


def main() -> None:
    parser = argparse.ArgumentParser(description="compress a DOS Zine DAT file")
    parser.add_argument("input")
    parser.add_argument("output")
    parser.add_argument("--force", action="store_true",
                        help="write DZ1 even when it is not smaller than the raw file")
    args = parser.parse_args()

    with open(args.input, "rb") as source:
        raw = source.read()
    packed = compress(raw)

    use_packed = args.force or len(packed) < len(raw)
    result = packed if use_packed else raw
    with open(args.output, "wb") as target:
        target.write(result)

    kind = "DZ1" if use_packed else "raw"
    saving = 100.0 * (len(raw) - len(result)) / len(raw) if raw else 0.0
    print(f"{args.output}: {len(raw)} -> {len(result)} bytes ({kind}, {saving:.1f}% saved)")


if __name__ == "__main__":
    main()
