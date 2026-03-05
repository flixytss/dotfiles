from gi.repository import Nautilus, GObject
import subprocess
import os

class ImageCommandExtension(GObject.GObject, Nautilus.MenuProvider):

    def run_command(self, menu, file):
        path = file.get_location().get_path()

        with open(f"{os.getenv("HOME")}/.local/state/initback.sh", "w") as file:
            file.write(f"#!/bin/bash\nswww img \"{path}\" --transition-type none")
            file.close()

        subprocess.run(f"{os.getenv("HOME")}/.local/state/initback.sh")

        # modifying waybar color
        output = subprocess.run(
            ["main-color", "-png" if path.endswith("png") else "-jpeg", path],
            capture_output=True
        )

        rgb = output.stdout.decode("utf-8").strip().split()
        new_first_line = f"@define-color bg rgba({rgb[0]}, {rgb[1]}, {rgb[2]}, 0.7);"

        css_path = os.path.join(os.getenv("HOME"), ".config/waybar/style.css")

        with open(css_path, "r") as file_r:
            lines = file_r.readlines()

        if lines:
            lines[0] = new_first_line + "\n"
        else:
            lines = [new_first_line + "\n"]

        with open(css_path, "w") as file_w:
            file_w.writelines(lines)

        os.system("killall waybar && nohup waybar &")

    def get_file_items(self, files):
        if len(files) != 1:
            return

        file = files[0]

        mime_type = file.get_mime_type()

        if mime_type and mime_type.startswith("image/"):
            item = Nautilus.MenuItem(
                name="ImageCommandExtension::RunImage",
                label="Set SWWW Wallpaper",
                tip="Put this image as the swww background"
            )

            item.connect("activate", self.run_command, file)

            return [item]