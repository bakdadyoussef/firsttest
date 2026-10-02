is this a safe script to execute in my pc
and is it python:
import hashlib

def mac_to_password(mac):
    md5 = hashlib.md5()
    md5.update(mac.upper().encode())
    md5.update(b"AEJLY")  # static salt
    digest = md5.hexdigest()

    UPPER = "ACDFGHJMNPRSTUWXY"
    LOWER = "abcdfghjkmpstuwxy"
    DIGIT = "2345679"
    SYMBOL = "!@$&%"

    vals = [int(c, 16) for c in digest[:20]]
    password = [''] * 16

    for i in range(16):
        v = vals[i]
        case = v % 4
        if case == 0: password[i] = UPPER[(v*2) % 17]
        elif case == 1: password[i] = LOWER[(v*2+1) % 17]
        elif case == 2: password[i] = DIGIT[6 - (v%7)]
        elif case == 3: password[i] = SYMBOL[4 - (v%5)]

    # Enforce all character classes at positions derived from vals[16-19]
    # [enforcement logic with collision avoidance]

    return ''.join(password)