from PIL import Image

b = Image.open("slider-background.png")
bar = Image.open("slider-bar.png")
slider = Image.new("RGBA", (146, 143 * 16))
margin = 2

for row in range(143):
    slider.paste(b, (0, row * 16))
    for i in range(row):
        slider.paste(bar, (margin + i, margin + row * 16))

slider.save("slider.png")
