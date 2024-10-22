# List of packages to be installed for debian or fedora
packages_debian=(
  "libfreetype6-dev"
  "libx11-dev"
  "libxcursor-dev"
  "libxrandr-dev"
  "libudev-dev"
  "libgl1-mesa-dev"
  "libflac-dev"
  "libogg-dev"
  "libvorbis-dev"
  "libvorbisenc2"
  "libvorbisfile3"
  "libopenal-dev"
  "libpthread-stubs0-dev"
)

packages_redhat=(
  "freetype-devel"
  "libX11-devel"
  "libXcursor-devel"
  "libXrandr-devel"
  "systemd-devel"
  "mesa-libGL-devel"
  "flac-devel"
  "libogg-devel"
  "libvorbis-devel"
  "openal-soft-devel"
  "glibc-headers"
)

# Detect the distro and install the needed packages
if [ -x "$(command -v apt)" ]; then
    echo "Detected a Debian-based system, installing SFML dependencies."
    sudo apt install -y "${packages_debian[@]}"

elif [ -x "$(command -v dnf)" ]; then
    echo "Detected a Fedora-based system, installing SFML dependencies."
    sudo dnf install -y "${packages_redhat[@]}"
else
    echo "Unsupported Linux distribution!"
    exit 1
fi

echo "Packages installation complete."
