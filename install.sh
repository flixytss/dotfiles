# !/bin/sh

sudo pacman -S nautilus alacritty cmake make

mv .config/waybar ~/.config/waybar
mv .config/hypr ~/.config/hypr
mv swww-wallpaper.py ~/.local/share/nautilus-python/extensions

cd main-color
make all
make install