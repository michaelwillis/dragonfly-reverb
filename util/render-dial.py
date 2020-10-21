from PIL import Image

b = Image.open("knob-background-52.png")
l = Image.open("knob-line-52.png")
bs = Image.new("RGBA", (52, 5252))
ls = Image.new("RGBA", (52, 5252))

for row in range(101):
    offset = (0, row * 52)
    bs.paste(b, offset)
    ls.paste(l.copy().rotate(135.0 - (float(row) * 0.01 * 270)), offset)

bs.save("knob-backgrounds-52.png")
ls.save("knob-lines-52.png")


