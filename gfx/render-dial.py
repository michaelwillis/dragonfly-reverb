from PIL import Image

b = Image.open("knob-background.png")
l = Image.open("knob-line.png")
bs = Image.new("RGBA", (32, 3232))
ls = Image.new("RGBA", (32, 3232))

for row in range(101):
    offset = (0, row * 32)
    bs.paste(b, offset)
    ls.paste(l.copy().rotate(135.0 - (float(row) * 0.01 * 270)), offset)

bs.save("knob-backgrounds.png")
ls.save("knob-lines.png")

