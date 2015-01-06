# Totally-Not-A-Virus

Keylogger with remote command execution and web interface.


The keylogger itself is written in C++.
The backend is a website written in JavaScript (with jQuery) and PHP.

## Disclaimer

We are not responsible for any harm or damage caused by this software.
Use on your own risk.


The software should only be used for educational purposes.

## Capabilities

* Key logging & screenshots
* Copies itself into autostart locations
* Remote command execution
* Great web interface with user overview and command console

## Remote command execution
The communication with the server is bi-directional;
the server can transmit commands to the program which will be executed quickly.
The commands can be entered in the backend terminal.

* "batch" - Execute a Windows command


  Usage: ```batch <command>```


  Example: killing explorer.exe ```batch taskkill /f /im explorer.exe```


* "msgbox" - Display a message box


  Usage: ```msgbox <text>```


* "screenshot" - Takes a screenshot and then sends it back


  Usage: ```screenshot```


* "remove" - Removes the virus from autostart


  Usage: ```remove```


* "exit" - Stops the program


  Usage: ```exit```

Multiple commands are separated with a semicolon.


  Example: ```batch winver;screenshot;exit```

## Web Interface
Such a design was chosen on purpose.
As a virus owner you wouldn't want to host the server on your own computer.
There are hundreds of free PHP servers on the internet.
The owner can use the web interface through a proxy and stay safe.

# Installation

1. Upload all files in 'Backend' to a web server with PHP.

2. Change the server URLs in the following file:
   /Hax/Hax/Settings.h

3. Compile the project 'Hax' using Visual Studio. Support for C++11 is required.
