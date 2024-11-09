import yaml
import argparse
import subprocess
from os import remove

magic_number = 0xF908EC

def load_yaml(filename : str) -> dict:
    try:
        with open(filename, 'r') as file:
            data = yaml.safe_load(file)
        return data
    except FileNotFoundError:
        print(f"Error: The file '{filename}' does not exist.")
    except yaml.YAMLError as e:
        print(f"Error: Failed to parse YAML file. {e}")
    except Exception as e:
        print(f"An unexpected error occurred: {e}")

def verify_pack(data : dict) -> bool:
    if ('id' not in data):
        print("Error: no three-character id provided for level pack.")
        return False
    if (len(data['id']) != 3):
        print("Error: level pack id must be three characters long.")
        return False
    if ('name' not in data):
        data['name'] = 'Untitled Level Pack'
    if ('levels' not in data):
        print("Error: no levels in pack!")
        return False
    data['count'] = len(data['levels'])
    return True

def serialize(data : dict) -> None:
    global file_name; file_name = f"DDLVL{data['id']}"
    global bin_name; bin_name = f"{file_name}.bin"
    global out_name; out_name = f"{file_name}.8xv"

    level_lengths : list = list(map(lambda level: len(level['name']) + 3 * len(level['monsters']) + 2 * len(level['chests']) + 19 + 8, data['levels']))
    title_offset : int = 3
    toc_offset : int = title_offset + len(data['name']) + 2
    first_level_offset : int = toc_offset + 2 * len(data['levels'])
    level_offsets : list = [None] * len(level_lengths)
    for i in range(len(level_lengths)):
        level_offsets[i] = first_level_offset
        for j in range(i):
            level_offsets[i] += level_lengths[j]

    with open(bin_name, 'wb') as file:
        file.write(magic_number.to_bytes(3, 'little'))
        file.write(bytes(data['name'], "ascii"))
        file.write(b'\x00')
        file.write(len(data['levels']).to_bytes(1))
        for level_offset in level_offsets:
            file.write(level_offset.to_bytes(2, 'little'))
        for level in data['levels']:
            file.write(bytes(level['name'], "ascii"))
            file.write(b'\x00')
            for i in range(8):
                file.write(b'\x00')
            for i in range(8):
                file.write(level['cols'][i].to_bytes(1))
            for i in range(8):
                file.write(level['rows'][i].to_bytes(1))
            for monster in level['monsters']:
                file.write(monster[0].to_bytes(1))
                file.write(monster[1].to_bytes(1))
                file.write(monster[2].to_bytes(1))
            file.write(b'\xff')
            for chest in level['chests']:
                file.write(chest[0].to_bytes(1))
                file.write(chest[1].to_bytes(1))
            file.write(b'\xff')

def convbin():
    command = ["convbin", "-j", "bin", "-k", "8xv", "-i", bin_name, "-o", out_name, "-n", file_name]
    try:
        subprocess.run(command)
    except FileNotFoundError:
        print("Error: ensure that convbin is in your $PATH.")
    remove(bin_name)

def main():
    parser = argparse.ArgumentParser(description="Compile a level pack for Dungeons and Diagrams CE from a YAML file.")
    parser.add_argument("inputfile", help="The name of the YAML file to load.")
    args = parser.parse_args()
    data = load_yaml(args.inputfile)
    print("Loading file...")
    if not verify_pack(data):
        return
    if data:
        print(f"Loaded level pack '{data['name']}' with {data['count']} levels.")
        print("Compiling...")
        serialize(data)
        print("Converting...")
        convbin()

if __name__ == "__main__":
    main()