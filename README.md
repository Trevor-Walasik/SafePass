# SafePass
# Password manager written in C

Welcome to SafePass, a password manager. The program has been in development since late 2024. Current code is written for 
C17.

Currently SafePass is ran from a shell and is a terminal based program. It will take keyboard inputs to modify a file 
containing log in credentials to various services for the user. 

AES 256 encryption is used to store the passwords in a plaintext folder. retrievel is based on a master key entered by the 
user.