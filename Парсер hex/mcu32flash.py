with open("firmware.hex",
          "r", encoding='utf-8') as f:
    lines = f.readlines()

memory_blocks = {}
bytes = []
block_addr = 0
is_error = False
byte_len = 0

for i in range(lines.__len__()):
    line = lines[i]
    if line[0] != ':':
        print("Error: unexpected start symbol on line %i, expect \':\', get \'%c\'" % (
            i+1, line[0]))
        is_error = True
        break
    data_len = int(line[1:3], base=16)
    line_addr = int(line[3:7], base=16)
    line_type = int(line[7:9], base=16)
    match line_type:
        case 0:
            data_bytes_line = line[9:data_len*2 + 9]
            data_bytes = []
            data_words = []
            for i in range(data_len):
                data_byte = data_bytes_line[i*2:i*2+2]
                data_bytes.append(data_byte)
                byte_len += 1
            for i in range(data_len//4):
                data_word = data_bytes[i*4:i*4+4]
                # data_word.reverse()
                data_words.append(data_word)
            
            for word in data_words:
                for byte in word:
                    bytes.append(byte)
            # for i in range(data_len//4):
            #     data_bytes = word_bytes.reverse()
            # print("data words: ", data_words)
        case 1:
            print("end of file")
        case 2:
            print("Error: unimplemented record type 2 on line %i" % (i+1))
            is_error = True
            break
        case 4:
            print("extended address")
        case 5:
            print("program start")
        case _:
            print("Error: unexpected record type %i on line %i" %
                  (line_type, i+1))
            is_error = True
            break
    # print("line %i data_bytes=%i line_addr=%i" % (i+1, data_bytes, line_addr))
c_data = "char bin_data[] = { "
for byte in bytes:
    c_data += "0x%s, " % byte
c_data = c_data[:-2]
c_data += " };"

print("byte_len=%i" % byte_len)

with open("array.h",
          "w", encoding='utf-8') as f:
    f.write(c_data)

# import subprocess
# subprocess.call(['.\\openocd\\bin\\openocd.exe'])

# def write_raw_sequence(tn, seq):
#     sock = tn.get_socket()
#     if sock is not None:
#         sock.send(seq)

# import telnetlib
# with telnetlib.Telnet('localhost', 4444) as tn:
#     write_raw_sequence(tn, b"targets\n")
