import os

path = "./frozen/"

def is_empty(file):
    return len(os.listdir(path + file)) ==0


for file in os.listdir(path):
    if is_empty(file):
        print(f'directory {file} is empty')
