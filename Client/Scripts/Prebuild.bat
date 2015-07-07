REM This downloads a new Tor expert bundle from torproject.org
REM Then we strip the Data folder of it, so that only the binaries remain

cd Scripts

if exist "../res/Tor.zip" goto end

REM Add Tools directory to path
PATH=%PATH%;%~dp0..\..\Tools\bin-w32;

REM FIXME: need a mirror that points to the latest tor version!
curl -k --output "../res/Tor.zip" --url "https://dist.torproject.org/torbrowser/4.5.3/tor-win32-0.2.6.9.zip"

7za x -y ../res/Tor.zip -o../res/

del "../res/Tor.zip"

rmdir "../res/Data" /s /q

7za a -tzip ../res/Tor.zip ../res/Tor/*.*

rmdir "../res/Tor" /s /q

:end

REM HACK! Visual Studio keeps saying something went wrong when it clearly didn'that
REM To go around this, set errorlevel to 0
set errorlevel=0