



from audioop import reverse


def main():
    # Image

    image_width = 256
    image_height = 256

    # Render
    rgb_string =  ""

    for j in reversed(range(image_height)):
        for i in range(image_width):
            red = i/(image_width-1)
            green = j/(image_height-1)
            blue = 0.25

            red = int(255.99*red)
            green = int(255.99*green)
            blue = int(255.99*blue)

            rgb_string += f" {red} {green} {blue}\n"

    # write to file
    with open("image.ppm", "w+") as image_file:
        header = f"P3\n{image_width} {image_height}\n255\n"

        image_file.write(header)
        image_file.write(rgb_string)

if __name__ == '__main__':
    main()