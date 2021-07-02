# System Font Collection (Typographic)

A font collection is an object that represents a set of fonts grouped into font families. The *system font collection*
is a special font collection that contains all the fonts installed on the computer, including fonts installed by the
current user.

You can use the system font collection to enumerate available font families, as in the list at right. The system font 
collection is also the default font collections used by text layout and font fallback to mach font family names and 
other properties to available fonts.

# Typographic Font Family Model

Fonts in a font collection are grouped into families according to *the font family model,* which is specified as 
a parameter to the `GetSystemFontCollection` factory method. The two font family models are *typographic* and
*weight-stretch-style*. This system font collection uses the typographic model.

In the typographic font family model, fonts within a family can be differentiated by any number of properties. These 
are called *axes of variation,* with each axis identified by a four-character OpenType tag. The `DWRITE_FONT_AXIS_VALUE` 
structure encapsulates an axis tag and associated value. To specify the font you want in this model, you specified the 
desired family name and an array of `DWRITE_FONT_AXIS_VALUE` structures. The font family name in this case should be the 
*typographic family name* of the font.

# Variable Fonts

The typographic font family model is closely associated with variable fonts introduced in OpenType 1.8, although it works
with static fonts as well. One of the advantages of the typographic model is that you can select arbitrary instances of 
variable fonts. For example, if a variable font supports a continuous range of weights, you can select any weight in that 
range by specifying the weight axis value you want. If you use the weight-stretch-style model, DirectWrite selects the 
nearest *named* instance to the specified weight, stretch, and style.
