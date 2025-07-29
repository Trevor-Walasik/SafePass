# SafePass
# Password manager written in C

Welcome to SafePass, a password manager. Currently SafePass is in a working stage but still early in developement.
Current code was written in early 2024 and I recently decided to revisit and improve on the program. 

Currently SafePass is ran from a shell and is a terminal based program. It will take keyboard inputs to modify a file 
containing log in credentials to various services for the user. 

One important thing to note: currently the program stores credentials in plaintext, this includes passwords. While the storage
of this text is local to the users computer, this does pose a possible security threat if the wrong person gets a hold of
the text file.

I intent to implement AES-256 Encryption of users password, and many other features detailed in the "Roadmap" file.