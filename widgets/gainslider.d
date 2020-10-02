/**
Horizontal gain slider, Adapted from DPlug's flatslider.d 
Copyright: Michael Willis 2020
Copyright: Guillaume Piolat 2015-2018
Copyright: Ethan Reker 2017
License:   $(LINK2 http://www.boost.org/LICENSE_1_0.txt, Boost License 1.0)
*/

module widgets.gainslider;

import std.math;
import std.algorithm.comparison;

import dplug.core.math;
import dplug.graphics.drawex;
import dplug.gui.bufferedelement;
import dplug.client.params;

class GainSlider : UIElement, IParameterListener
{
public:
nothrow:
@nogc:

  this(UIContext context, FloatParameter param, OwnedImage!RGBA filmstrip, int numFrames, int pixelMargin)
    {
        super(context, flagRaw);
        _param = param;
        _filmstrip = filmstrip;
        _numFrames = numFrames;
        _pixelMargin = pixelMargin;
        _disabled = false;
    }

    ~this()
    {
        _param.removeListener(this);
    }

    override void onDrawRaw(ImageRef!RGBA rawMap, box2i[] dirtyRects)
    {
        setCurrentImage();
        auto _currentImage = _filmstrip.crop(box2i(_imageX1, _imageY1, _imageX2, _imageY2));
        foreach(dirtyRect; dirtyRects)
        {
            auto croppedRawIn = _currentImage.crop(dirtyRect);
            auto croppedRawOut = rawMap.crop(dirtyRect);

            int w = dirtyRect.width;
            int h = dirtyRect.height;

            for(int j = 0; j < h; ++j)
            {
                RGBA[] input = croppedRawIn.scanline(j);
                RGBA[] output = croppedRawOut.scanline(j);

                for(int i = 0; i < w; ++i)
                {
                    ubyte alpha = input[i].a;

                    RGBA color = blendColor(input[i], output[i], alpha);
                    output[i] = color;
                }
            }

        }
    }

    void setCurrentImage()
    {
        float value = _param.getNormalized();
        currentFrame = cast(int)(round(value * (_numFrames - 1)));

        if(currentFrame < 0) currentFrame = 0;

        _imageX1 = 0;
        _imageY1 = (_filmstrip.h / _numFrames) * currentFrame;

        _imageX2 = _filmstrip.w;
        _imageY2 = _imageY1 + (_filmstrip.h / _numFrames);

    }

    override bool onMouseClick(int x, int y, int button, bool isDoubleClick, MouseState mstate)
    {
        _param.beginParamEdit();
        // double-click => set to default
        if (isDoubleClick || mstate.altPressed)
        {
            if (auto p = cast(FloatParameter)_param)
                p.setFromGUI(p.defaultValue());
            else if (auto p = cast(IntegerParameter)_param)
                p.setFromGUI(p.defaultValue());
            else
                assert(false); // only integer and float parameters supported
        }
        else
        {
          onMouseDrag(x, y, 0, 0, mstate);
        }

        _param.endParamEdit();
        return true; // to initiate dragging
    }

    // Called when mouse drag this Element.
    override void onMouseDrag(int x, int y, int dx, int dy, MouseState mstate)
    {
        if(!_disabled)
        {
            double newParamValue = clamp(cast(float)(x/* - _pixelMargin*/) / (_filmstrip.w - (2 * _pixelMargin)), 0.0, 1.0);
            
            if (mstate.altPressed)
                newParamValue = _param.getNormalizedDefault();

            if (newParamValue != _param.getNormalized())
                _param.setFromGUINormalized(newParamValue);

            setDirtyWhole();
        }
    }

    // For lazy updates
    override void onBeginDrag()
    {
        _param.beginParamEdit();
        setDirtyWhole();
    }

    override void onStopDrag()
    {
        _param.endParamEdit();
        setDirtyWhole();
    }

    override void onMouseEnter()
    {
    }

    override void onMouseExit()
    {
    }

    override void onParameterChanged(Parameter sender)
    {
        setDirtyWhole();
    }

    override void onBeginParameterEdit(Parameter sender)
    {
    }

    override void onEndParameterEdit(Parameter sender)
    {
    }

    void disable()
    {
        _disabled = true;
    }

protected:

    FloatParameter _param;

    OwnedImage!RGBA _filmstrip;
    int _numFrames;
    int _pixelMargin;
    int _imageX1, _imageX2, _imageY1, _imageY2;
    int currentFrame;

    bool _disabled;
}
