import random
import string

def generate_random_text_file(filename="0_input.txt", size_mb=1100):
    size_bytes = size_mb * 1024 * 1024
    chunk_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 abcdefghijklmnopqrstuvwxyz"
    
    total_written = 0
    with open(filename, "w") as f:
        while total_written < size_bytes:
            line_length = random.randint(100, 500)
            line = ''.join(random.choices(chunk_chars, k=line_length)) + '\n'
            f.write(line)
            total_written += len(line)

    print(f"Generated '{filename}' with approximately {size_mb} MB of random content.")

generate_random_text_file()