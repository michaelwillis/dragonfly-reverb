/**
Editable textbox, adapted from DPlug's PBR textbox. 

Copyright: Michael Willis 2020.
Copyright: Ethan Reker 2017.
Copyright: Copyright Auburn Sounds 2015-2017.
License:   $(LINK2 http://www.boost.org/LICENSE_1_0.txt, Boost License 1.0)
*/
module widgets.textbox;

import dplug.gui.element;
import dplug.core.nogc;
import dplug.core.vec;
import dplug.window.window : getCharFromKey;

private import core.stdc.stdlib : malloc, free;
private import core.stdc.stdio : snprintf, printf;
private import core.stdc.string : strcmp, strlen;

class UITextBox : UIElement
{
public:
nothrow:
@nogc:

    this(UIContext context, Font font, int textSize, RGBA textColor = RGBA(200, 200, 200, 255), RGBA backgroundColor = RGBA(0, 0, 0, 255))
    {
        super(context, flagRaw);
        _font = font;
        _textSize = textSize;
        _textColor = textColor;
        _backgroundColor = backgroundColor;
        charBuffer = makeVec!char();
    }

    ~this()
    {
    }

    @property const(char)[] getText()
    {
        return displayString();
    }

    override void onDrawRaw(ImageRef!RGBA rawMap, box2i[] dirtyRects)
    {
        float textPosx = position.width * 0.5f;
        float textPosy = position.height * 0.5f;

        foreach(dirtyRect; dirtyRects)
        {
            auto cropped = rawMap.cropImageRef(dirtyRect);
            vec2f positionInDirty = vec2f(textPosx, textPosy) - dirtyRect.min;

            cropped.fillAll(_backgroundColor);
            cropped.fillText(_font, displayString(), _textSize, 0.5, _textColor, positionInDirty.x, positionInDirty.y);
        }
    }

    override bool onMouseClick(int x, int y, int button, bool isDoubleClick, MouseState mstate)
    {
        // Left click
        _isActive = true;

        setDirtyWhole();
        return true;
    }

    override void onMouseEnter()
    {
        setDirtyWhole();
    }

    override void onMouseExit()
    {
        _isActive = false;
        setDirtyWhole();
    }

    override bool onKeyDown(Key key)
    {
        if(_isActive)
        {
            const char c = cast(char)getCharFromKey(key);
            if(c == '\t')
            {
                if (charBuffer.length > 0)
                {
                    charBuffer.popBack();
                }
            }
            else if(c != '\0')
                charBuffer.pushBack(c);
            setDirtyWhole();
            return true;
        }

        return false;
    }

private:

    Font _font;
    int _textSize;
    RGBA _textColor;
    RGBA _backgroundColor;
    bool _isActive;
    Vec!char charBuffer;

    const(char)[] displayString() nothrow @nogc
    {
        return charBuffer[];
    }

}
