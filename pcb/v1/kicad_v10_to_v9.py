#!/usr/bin/env python3
"""
KiCad v10 to v9 PCB file downgrade script.
Usage: python3 kicad_v10_to_v9.py input.kicad_pcb output.kicad_pcb
"""

import re
import sys


def build_net_map(content):
    """
    In v10, the net list at the top only has names, no numbers:
        (net "GND")
        (net "/HFXOUT")
    We collect all unique net names from the whole file and assign numbers,
    putting GND/empty-string first (net 0 = unconnected, net 1 = GND by convention).
    """
    # Collect every net name mentioned anywhere in the file
    all_names = []
    seen = set()
    # Match (net "name") or (net N "name") anywhere
    for m in re.finditer(r'\(net\s+(?:\d+\s+)?"([^"]*)"\)', content):
        name = m.group(1)
        if name not in seen:
            seen.add(name)
            all_names.append(name)

    # Also grab bare (net "name") top-level entries
    # (already covered above, but be safe)

    # Sort: empty string first, then GND, then everything else alphabetically
    def sort_key(n):
        if n == '':
            return (0, n)
        if n == 'GND':
            return (1, n)
        return (2, n)

    all_names.sort(key=sort_key)

    net_map = {}  # name -> number
    for i, name in enumerate(all_names):
        net_map[name] = i + 1  # start at 1; 0 is reserved for unconnected

    return net_map


def build_net_list(net_map):
    """Generate the net list block to insert at the top of the file."""
    lines = []
    for name, num in sorted(net_map.items(), key=lambda x: x[1]):
        lines.append(f'\t(net {num} "{name}")')
    return '\n'.join(lines)


def downgrade(content):
    # 1. Downgrade version number
    content = re.sub(r'\(version\s+\d+\)', '(version 20241229)', content)

    # 2. Downgrade generator version
    content = re.sub(r'\(generator_version\s+"10\.[^"]*"\)', '(generator_version "9.0")', content)

    # 3. Remove v10-only fields
    content = re.sub(r'\n\s+\(duplicate_(pin|pad)_numbers_are_jumpers\s+(yes|no)\)', '', content)
    content = re.sub(r'\n\s+\(in_pos_files\s+(yes|no)\)', '', content)
    content = re.sub(r'\n\s+\(body_style\s+\d+\)', '', content)

    # 4. Fix tenting format
    content = re.sub(r'\(tenting\s+\(front\s+yes\)\s+\(back\s+yes\)\s*\)', '(tenting front back)', content)
    content = re.sub(r'\(tenting\s+\(front\s+no\)\s+\(back\s+no\)\s*\)',   '(tenting none)',       content)
    content = re.sub(r'\(tenting\s+\(front\s+no\)\s+\(back\s+yes\)\s*\)',  '(tenting back)',       content)
    content = re.sub(r'\(tenting\s+\(front\s+yes\)\s+\(back\s+no\)\s*\)',  '(tenting front)',      content)

    # 5. Remove capping/filling/plugging/covering sections
    content = re.sub(r'\n\s+\((capping|filling|plugging|covering)\s+\(front\s+(yes|no)\)\s+\(back\s+(yes|no)\)\s*\)', '', content)
    content = re.sub(r'\n\s+\((capping|filling|plugging|covering)\s+(yes|no)\)', '', content)

    # 6. Build net map from all net names in the file
    net_map = build_net_map(content)

    # 7. Remove the old v10 net list entries (name-only, no number)
    #    These look like:  \t(net "GND")  at the top level
    content = re.sub(r'\n\t\(net\s+"[^"]*"\)', '', content)

    # 8. Insert the new numbered net list before (setup ...)
    net_list_str = build_net_list(net_map)
    content = re.sub(r'(\n\t\(setup\s)', '\n' + net_list_str + r'\1', content, count=1)

    # 9. Fix net references in segments/vias: (net "name") or (net N "name") -> (net N)
    def strip_net(block_text):
        # (net N "name") -> (net N)
        block_text = re.sub(r'\(net\s+(\d+)\s+"[^"]*"\)', r'(net \1)', block_text)
        # (net "name") -> (net N)
        def by_name(m):
            name = m.group(1)
            num = net_map.get(name)
            if num is not None:
                return f'(net {num})'
            print(f"Warning: unknown net '{name}'", file=sys.stderr)
            return m.group(0)
        block_text = re.sub(r'\(net\s+"([^"]*)"\)', by_name, block_text)
        return block_text

    # 10. Fix net references in pads: (net "name") -> (net N "name")
    def fix_pad_net(block_text):
        # Already numbered: leave alone
        # Name-only: add number
        def by_name(m):
            name = m.group(1)
            num = net_map.get(name)
            if num is not None:
                return f'(net {num} "{name}")'
            print(f"Warning: unknown net '{name}'", file=sys.stderr)
            return m.group(0)
        block_text = re.sub(r'\(net\s+"([^"]*)"\)', by_name, block_text)
        return block_text

    # 11. Walk the file block by block
    def process_blocks(text):
        result = []
        i = 0
        while i < len(text):
            m = re.search(r'\n(\t+)\((segment|via|pad)\s', text[i:])
            if not m:
                result.append(text[i:])
                break

            block_type = m.group(2)
            abs_start = i + m.start()
            result.append(text[i:abs_start])

            block_start = abs_start + 1
            depth = 0
            j = block_start
            while j < len(text):
                if text[j] == '(':
                    depth += 1
                elif text[j] == ')':
                    depth -= 1
                    if depth == 0:
                        j += 1
                        break
                j += 1

            block = text[block_start:j]
            if block_type in ('segment', 'via'):
                block = strip_net(block)
            elif block_type == 'pad':
                block = fix_pad_net(block)

            result.append('\n' + block)
            i = j

        return ''.join(result)

    content = process_blocks(content)
    return content


def main():
    if len(sys.argv) != 3:
        print("Usage: python3 kicad_v10_to_v9.py input.kicad_pcb output.kicad_pcb")
        sys.exit(1)

    in_path = sys.argv[1]
    out_path = sys.argv[2]

    with open(in_path, 'r', encoding='utf-8') as f:
        content = f.read()

    if not content.strip().startswith('(kicad_pcb'):
        print("Error: Not a valid KiCad PCB file (.kicad_pcb)")
        sys.exit(1)

    if not re.search(r'\(generator_version\s+"10\.', content):
        print("Error: File does not appear to be a KiCad v10 file")
        sys.exit(1)

    print(f"Converting {in_path} ...")
    result = downgrade(content)

    with open(out_path, 'w', encoding='utf-8') as f:
        f.write(result)

    print(f"Done! Saved to {out_path}")
    print("Please open in KiCad v9 and run DRC to verify.")


if __name__ == '__main__':
    main()