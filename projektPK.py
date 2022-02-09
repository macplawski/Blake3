import sys

def rol(data, shift, size=16):
    shift %= size
    remains = data >> (size - shift)
    body = (data << shift) - (remains << size)
    return (body + remains)

def defoultState(v, w, nrBlock):

    for x in range(4):
        for y in range(4):
            if x == 0:
                v[0][y] = w[y]
            elif x == 1:
                v[1][y] = w[y + 4]
            elif x == 2:
                v[2][0] = int('3553', 16)
                v[2][1] = int('74C1', 16)
                v[2][2] = int('0BDA', 16)
                v[2][3] = int('29F8', 16)
                break
            elif x == 3:
                v[3][0] = 0
                v[3][1] = nrBlock  # numer bloku
                v[3][2] = 0
                v[3][3] = 0
                break
    return v

def makePermutation(m):
    x = [0] * 16

    x[0] = m[1]
    x[1] = m[14]
    x[2] = m[15]
    x[3] = m[13]
    x[4] = m[7]
    x[5] = m[2]
    x[6] = m[3]
    x[8] = m[4]
    x[7] = m[6]
    x[9] = m[8]
    x[10] = m[9]
    x[11] = m[12]
    x[12] = m[0]
    x[13] = m[10]
    x[14] = m[5]
    x[15] = m[11]

    return x

def G(a, b, c, d, x, y):
    a = (((a + b) % 65536) + x) % 65536
    d = rol((d ^ a), 5)
    c = (c + d) % 65536
    b = rol((b ^ c), 7)
    a = (((a + b) % 65536) + y) % 65536
    d = rol((d ^ a), 3)
    c = (c + d) % 65536
    b = rol((b ^ c), 14)
    return a, b, c, d

def makeRound(v, m):
    v[0][0], v[1][0], v[2][0], v[3][0] = G(v[0][0], v[1][0], v[2][0], v[3][0], m[0], m[1])
    v[0][1], v[1][1], v[2][1], v[3][1] = G(v[0][1], v[1][1], v[2][1], v[3][1], m[2], m[3])
    v[0][2], v[1][2], v[2][2], v[3][2] = G(v[0][2], v[1][2], v[2][2], v[3][2], m[4], m[5])
    v[0][3], v[1][3], v[2][3], v[3][3] = G(v[0][3], v[1][3], v[2][3], v[3][3], m[6], m[7])

    v[0][0], v[1][1], v[2][2], v[3][3] = G(v[0][0], v[1][1], v[2][2], v[3][3], m[8], m[9])
    v[0][1], v[1][2], v[2][3], v[3][0] = G(v[0][1], v[1][2], v[2][3], v[3][0], m[10], m[11])
    v[0][2], v[1][3], v[2][0], v[3][1] = G(v[0][2], v[1][3], v[2][0], v[3][1], m[12], m[13])
    v[0][3], v[1][0], v[2][1], v[3][2] = G(v[0][3], v[1][0], v[2][1], v[3][2], m[14], m[15])
    return v

def hashBlock(m, v, w, number):
    v = defoultState(v, w, number)
    for i in range(6):
        v = makeRound(v, m)
        m = makePermutation(m)

    for i in range(4):
        w[i] = w[i] ^ v[0][i] ^ v[2][i]
        w[i+4] = w[i + 4] ^ v[1][i] ^ v[3][i]

    return w, v

def Blake3(wejscie):
    m = []
    w = [0] * 8
    hash = ""
    v = [[""] * 4 for i in range(4)]
    m = prepareData(wejscie, m)
    number = int(len(m) / 16)

    for i in range(number):
        w, v = hashBlock(m[(16 * i):(16 * (i + 1))], v, w, i)

    for i in range(len(w)):
        hash += str(hex(w[i]))[2:6]

    return hash

def prepareData(wejscie, m):
    q = []

    for i in wejscie:  # zamiana na ASCII
        q.append(ord(i))

    dopelnienie = 32 - (len(q) % 32)  # Dodanie dopelnienia
    if dopelnienie != 0:
        q.append(128)
        for i in range(dopelnienie - 1):
            q.append(0)
    else:
        q.append(128)
        for i in range(31):
            q.append(0)

    for i in range(0, len(q), 2):  # Utworzenie blokow po 2 bajty
        m.append((q[i] << 8) ^ q[i + 1])

    return m

def readData(file):
    try:
        open(file, "r")
        file1 = open(file, 'r')
        lines = [line[:-1] for line in file1]
        file1.close()
        return lines
    except IOError:
        print("Error: File does not appear to exist.")
        return 0
    

def saveData(data):
    file = open('save.txt', 'w')
    for line in data:
        file.write(line+'\n')
    file.close()

if __name__ == "__main__":
    lines = readData(sys.argv[1])
    hash = []
    if(lines != 0):
        for i in lines:
            hash.append(Blake3(i))
            saveData(hash)
