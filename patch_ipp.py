import sys

exe_path = r'E:\ipp-sharing\target\x86_64-pc-windows-gnu\release\ipp-sharing.exe'
out_path = r'E:\ipp-sharing\target\x86_64-pc-windows-gnu\release\ipp-sharing_patched.exe'

with open(exe_path, 'rb') as f:
    data = bytearray(f.read())

old_func = b'GetSystemTimePreciseAsFileTime'
new_func = b'GetSystemTimeAsFileTime'

count = 0
pos = 0
while True:
    pos = data.find(old_func, pos)
    if pos < 0:
        break
    count += 1
    print(f'[{count}] Found at offset: {pos} (0x{pos:x})')
    print(f'    Old: {bytes(data[pos:pos+len(old_func)])}')
    # Overwrite with new function name + null padding
    data[pos:pos+len(old_func)] = new_func + b'\x00' * (len(old_func) - len(new_func))
    verify = bytes(data[pos:pos+len(new_func)])
    print(f'    New: {verify}')
    pos += len(old_func)

if count == 0:
    print('ERROR: GetSystemTimePreciseAsFileTime not found!')
    sys.exit(1)

print(f'\nTotal replacements: {count}')
print(f'Old string length: {len(old_func)} bytes')
print(f'New string length: {len(new_func)} bytes')

# Final check
if data.find(old_func) >= 0:
    print(f'ERROR: Still {data.count(old_func)} instance(s) of old string remaining!')
    sys.exit(1)

with open(out_path, 'wb') as f:
    f.write(data)

print(f'\nDone! Written to: {out_path}')
print(f'Size: {len(data)} bytes')

# Verify the output
with open(out_path, 'rb') as f:
    verify_data = f.read()
if verify_data.find(new_func) >= 0:
    print(f'VERIFIED: GetSystemTimeAsFileTime present in output')
if verify_data.find(old_func) < 0:
    print(f'VERIFIED: GetSystemTimePreciseAsFileTime absent from output')
