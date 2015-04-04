# Totally-Not-A-Virus

Keylogger with remote command execution and web interface.


The client (virus) itself is written in C++.
The backend is a website written in JavaScript (with jQuery) and PHP.


Everyone is welcome to join the project. Just message us. Don't be shy.

## Disclaimer

We are not responsible for any harm or damage caused by this software.
Use on your own risk.


The software should only be used for educational purposes.

## Capabilities

* Key logging & screenshots
* Copies itself into autostart locations
* Remote command execution
* Remote error log
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

1. Upload all files in 'Server' to a web server with PHP.

2. Change the location of AuthUserFile in ctrl/.htaccess to the correct location of ctrl/.htpasswd
   The path is relative to the server root.

2. Use CMake to generate a project file for your Visual Studio version.

3. Project Settings: Character Set: 'Use Unicode Character Set'

4. Change the server URLs in the following file:
   /Client/src/Settings.h

5. Compile the project using Visual Studio. Support for C++11 is required.
