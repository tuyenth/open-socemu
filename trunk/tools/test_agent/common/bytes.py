########################################################################################
# ByteString toolbox definition
########################################################################################

def partition(string, size):
    if len(string) % size:
        parts = len(string) / size + 1
    else:
        parts = len(string) / size
    return [string[index*size:index*size+size] for index in range(parts)]

def pad_right(string, length, padding=' '):
        return string + padding[0] * (length - len(string))

def convert_hex(string):
    return ' '.join([hex(ord(character))[2:].upper().zfill(2) \
                     for character in string])

def dump(string):
    """
    Convert a byte string to its hex string representation e.g. for output.
    """
    parts = partition(string, 16)
    
    result = ''
    
    for index in range(len(parts)):
        result += ' | '.join([hex(index)[2:].upper().zfill(7)[-7:] + '0', \
                          pad_right(convert_hex(parts[index]), 47)]) + '\n'

    return result