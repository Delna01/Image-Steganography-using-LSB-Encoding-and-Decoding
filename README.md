# LSB Image Steganography in C

A command-line tool written in C that hides secret data inside BMP images using the **Least Significant Bit (LSB)** technique — and extracts it back without visually altering the image.

---

## What is Steganography?

Steganography is the practice of concealing a message within another file so that its existence is undetectable. Unlike encryption (which scrambles data), steganography **hides the fact that a secret even exists** by embedding it invisibly inside a carrier file — in this case, a BMP image.

---

## How It Works

Each pixel in a BMP image is stored as 3 bytes (Red, Green, Blue). This project encodes one bit of secret data into the **least significant bit (LSB)** of each byte. Since flipping the LSB changes a color value by at most 1 out of 255, the resulting image looks identical to the human eye.

### Encoding layout (after the 54-byte BMP header):

| Segment | Size |
|---|---|
| Magic string (`#*`) | 2 bytes × 8 bits = 16 LSBs |
| Extension length | 32 LSBs |
| File extension (e.g., `.txt`) | extension\_length × 8 LSBs |
| Secret file size | 32 LSBs |
| Secret file data | file\_size × 8 LSBs |
| Remaining image data | unchanged |

---

## Project Structure

```
.
├── main.c        # Entry point; routes to encode or decode
├── encode.c      # Encoding logic
├── encode.h      # EncodeInfo struct and function declarations
├── decode.c      # Decoding logic
├── decode.h      # DecodeInfo struct and function declarations
├── types.h       # Shared types: Status enum, OperationType enum, uint
├── common.h      # Magic string definition (#*)
├── beautiful.bmp # Sample cover image
├── secret.txt    # Sample secret file
└── output.sh     # Helper script
```

---

## Build

```bash
gcc main.c encode.c decode.c -o stego
```

Or using the existing compiled binary:

```bash
./a.out
```

---

## Usage

### Encoding — Hide a secret file inside a BMP image

```bash
./a.out -e <source_image.bmp> <secret_file> <output_image.bmp>
```

**Example:**
```bash
./a.out -e beautiful.bmp secret.txt stego.bmp
```

Supported secret file types: `.txt`, `.c`, `.sh`

If no output filename is provided, the result is saved as `default.bmp`.

### Decoding — Extract the hidden file from a stego image

```bash
./a.out -d <stego_image.bmp> [output_filename]
```

**Example:**
```bash
./a.out -d stego.bmp recovered
```

The extension is recovered automatically from the stego image, so the output file will be saved as `recovered.txt` (or whatever the original extension was).

---

## Sample Run

**Encoding:**
```
width = 1024
height = 768
Successfully validated encoding arguments.
File opened successfully.
Excellent storage capacity.
Header copied successfully.
Magic string encoded successfully.
Secret file extension size encoded successfully.
Secret file extension encoded successfully.
Secret file size encoded successfully.
Secret file data encoded successfully.
Remaining data copied successfully.
Encoding process completed successfully.
```

**Decoding:**
```
You Have Chosen Decoding...
All Command Line Arguments Validated Successfully
Destination File Opened Successfully
Decoded Magic String : #*
Magic String Decoded Successfully from Destination File
Decoded Secret File Extension Size : 4
Secret File Extension Decoded Successfully from Destination File
Decoded Secret File Data Size : 11 bytes
Decoded Secret File Data : Hello world
Decoding Process is Completed Successfully
```

---

## Requirements

- GCC compiler
- Linux / Unix environment
- A 24-bit BMP image large enough to hold the secret data

---

## Limitations

- Only **BMP** images are supported as the cover file.
- Secret files must be `.txt`, `.c`, or `.sh`.
- The cover image must have enough pixel data to store the entire secret. The capacity check ensures: `image_capacity > 16 + 32 + 32 + 32 + (secret_size × 8)` bits.

---

## Author

**Delna Joseph**  
