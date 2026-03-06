# !/bin/sh

sudo pacman -S nautilus alacritty cmake make lua

cp .config/waybar ~/.config/waybar
cp .config/hypr ~/.config/hypr

git clone https://github.com/flixytss/swww-setter-general.git setter
git clone https://github.com/flixytss/MainColor.git main-color
git clone https://github.com/flixytss/nautilus-plugin-swww.git plugin

cd main-color
mkdir build
make all
make install
cd ..
cd setter
mkdir build
make all
make install
cd ..
cd plugin
cp plugin.py ~/.local/share/nautilus-python/extensions/swww-wallpaper.py
cd ..