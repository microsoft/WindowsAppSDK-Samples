# System Font Collection (Weight-Stretch-Style)

This scenario is the same as the **System Font Collection (Typographic)** scenario except that this system font 
collection uses the *weight-stretch-style* font family model (WSS).

In the weight-stretch-style font family model, fonts within a family can be differentiated only by three properties: 
weight, stretch, and style. To specify the font you want in this model, you specify the desired family name and the 
desired `DWRITE_FONT_WEIGHT`, `DWRITE_FONT_STRETCH`, and `DWRITE_FONT_STYLE` values. Because the grouping of fonts 
depends on the font family model, the family name you specify needs to be the weight-stretch-style family name of 
the font.

# Example

It can be instructive to compare the typographic font collection in the previous scenario with the weight-stretch-style 
font collection in this scenario.

In the typographic font collection, the Sitka font family includes fonts that vary along three axes: weight, italic, 
and optical size. The *optical size* axis does not fit in the weight-stretch-style font family model. In this font 
collection, therefore, the Sitka font family has been split into multiple families, each of which contains fonts in 
a single optical size.

The WSS model does not enable one to specify optical size as an independent parameter, so the only way to select a 
font of the desired optical size is by name. For example, if you're drawing 12-point text, you just have to know that 
the correct font family is Sitka Text rather than, say, Sitka Subhead.
