import os, threading
from random import randint

file = open("sample.txt", 'rb')

data = file.read()

# start is inclusive, end is exclusive
def generate_samples(_starting_file_index, _ending_index):
    count = 0
    file_index = _starting_file_index

    for i in range(_ending_index):
        offset = 0
        size = 0
        print(f"Iteration #{i}")

        set_root_path = f"samples/set_{file_index}"

        if not os.path.exists(set_root_path):
            os.makedirs(set_root_path)
        
        for f in range(167):
            while 1:
                size = randint(5, 100)
                sample = data[offset:offset + size]
                offset += size

                if sample == b'':
                    break

                try:
                    to_write = sample.decode()
                    file = open(f"{set_root_path}/sample_{count}", 'a')
                    file.write(to_write)
                    file.close()
                    count += 1
                except:
                    pass

            offset = 0

        file_index += 1
        count = 0

generate_samples(0, 10000000000)
