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
MAX_CANDIDATES = 128


def find_match(data: bytes, pos: int, positions, extra_position=None):
    """Return the longest match at pos, optionally including one new history byte."""
    if pos + MIN_MATCH > len(data):
        return 0, 0

    key = data[pos:pos + MIN_MATCH]
    candidates = positions.get(key)
    if candidates:
        oldest = pos - WINDOW
        while candidates and candidates[0] < oldest:
            candidates.popleft()
        recent = list(candidates)[-MAX_CANDIDATES:]
    else:
        recent = []

    # Used for lazy matching at pos + 1: if pos is emitted as a literal, it is
    # available in the decoder's history before the lookahead match starts.
    if (extra_position is not None and
            data[extra_position:extra_position + MIN_MATCH] == key):
        recent.append(extra_position)
        recent = recent[-MAX_CANDIDATES:]

    best_len = 0
    best_distance = 0
    limit = min(MAX_MATCH, len(data) - pos)
    for previous in reversed(recent):
        length = 0
        while length < limit and data[previous + length] == data[pos + length]:
            length += 1
        if length > best_len:
            best_len = length
            best_distance = pos - previous
            if length == limit:
                break

    return best_len, best_distance


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

            best_len, best_distance = find_match(data, pos, positions)

            # One-byte lazy matching: emit a literal when doing so exposes a
            # longer match at the next position. This changes only encoder
            # decisions; the DZ1 stream format remains exactly the same.
            use_literal = best_len < MIN_MATCH
            if not use_literal and pos + 1 < len(data):
                next_len, _ = find_match(data, pos + 1, positions,
                                         extra_position=pos)
                use_literal = next_len > best_len

            if use_literal:
                flags |= 1 << bit
                body.append(data[pos])
                consumed = 1
            else:
                code = (best_distance - 1) | ((best_len - MIN_MATCH) << 12)
                body.extend((code & 0xFF, code >> 8))
                consumed = best_len

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
