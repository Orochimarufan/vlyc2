![VLYC² Logo][logo]

# VLYC² #
VLYC² aims to be a performant locally-installed client to cloud-media services.

### Features ###
* Stream videos from various sites
* Supports quality selection
* Built-in browser
* Interface resembling VLC
* Performant playback using LibVLC
* It manages to play 720p youtube videos on a 10 year old Celeron M @1x1.7G Laptop!
* Extensible plugin system
* Write plugins in Python

### Planned ###
* Subtitle suppot
* Playlist support
* MPEG-DASH support (Used by YouTube, among others)
* Chromium interop (Delegate links to vlyc2 from chromium)

## Dependencies ##
* Qt 5 (http://qt-project.org, distro packages)
* libvlc (http://videolan.org, distro packages)
* QtVlc (http://github.com/Orochimarufan/QtVlc)

### Python Plugin support ###
* libpython 3.3+ (http://python.org, distro packages)
* PythonQt (http://github.com/Orochimarufan/PythonQt)
* youtube-dl (http://github.com/rg3/youtube-dl)

### Build-time Dependencies ###
* CMake (http://cmake.org, distro packages)
* Ninja (http://martine.github.io/ninja/, distro packages)

## Build ##
First, get the dependencies:

* (lib)vlc
* Qt5
* python3

Then, start by installing QtVlc:
```bash
git clone https://github.com/Orochimarufan/QtVlc.git
cd QtVlc
git submodule update --init
mkdir build
cd build
cmake .. -G Ninja
ninja
sudo ninja install
```

Next, PythonQt
```bash
git clone https://github.com/Orochimarufan/PythonQt.git
cd PythonQt
mkdir bbuild #build already exists
cd bbuild
cmake .. -G Ninja -DPythonQt_Qt5=ON -DPythonQt_Python3=ON
ninja
sudo ninja install
```

Finally, VLYC² itself
```bash
git clone https://github.com/Orochimarufan/vlyc2.git
cd vlyc2
mkdir build
cd build
cmake .. -G Ninja
ninja
# Enable python plugins
ln -s ../../plugins/python/plugins/{youtube_dl,sbs}.py plugins/
```

Now, you can run VLYC² from the build directory: `./vlyc2`

## License ![GPLv3][gpl-logo]##
    VLYC² is (C) 2013-2014 by Taeyeon Mori.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

[logo]: https://raw.githubusercontent.com/Orochimarufan/vlyc2/master/res/vlyc2.png
[gpl-logo]: https://www.gnu.org/graphics/gplv3-88x31.png