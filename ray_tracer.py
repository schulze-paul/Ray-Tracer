from tqdm import tqdm
from dataclasses import dataclass
import numpy as np

def write_color(out_file, pixel_color: np.ndarray) -> None:
    red = int(255.999*pixel_color[0])
    green = int(255.999*pixel_color[1])
    blue = int(255.999*pixel_color[2])
    
    rgb_string = f" {red} {green} {blue}\n"
    out_file.write(rgb_string)


def main():
    """main function of the ray tracer"""
    # Image

    image_width = 256
    image_height = 256

    # Render

    # write to file
    with open("image.ppm", "w+") as image_file:
        header = f"P3\n{image_width} {image_height}\n255\n"
        image_file.write(header)

        for j in tqdm(reversed(range(image_height))):
            for i in range(image_width):
                red = i/(image_width-1)
                green = j/(image_height-1)
                blue = 0.25
                color = np.array([red, green, blue])

                write_color(image_file, color)

if __name__ == '__main__':
    main()