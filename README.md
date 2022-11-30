# dvd-overlay
Bouncing DVD Logo Overlay

## Screenshot
<p align="center">
  <img src="https://github.com/Daxterapid/dvd-overlay/raw/master/screenshot.png">
</p>

## Building

### Windows
Make sure MinGW and GLFW 3.4 (unstable) are installed on your system, then run:
```
make
```

### Linux
**Though untested, it's likely this only works with certain window managers.**

You'll probably have to install GLFW 3.4 (unstable) and run `make`.

---

*GLFW 3.4 is required for input passthrough.*

## Running

### Windows
```
build\dvd_overlay.exe
```

### Linux
```
build/dvd_overlay
```

## Contributing
Feel free to open an issue or create a pull request if you find any issues or improvements!

## Known issues
* Image appears to have a white outline on bright backgrounds.