# DWriteCore Sample Gallery

This sample application demonstrates the DWriteCore API, which is a reimplementation of the Windows DirectWrite API.
Select items from the *Scenario* menu to see pages that demonstrate various API functionality.

DWriteCore is a low-level API for formatting and rendering text. It is a nano-COM API, meaning it uses COM-style 
interfaces (derived from `IUnknown`) but does not actually use the COM run-time. It is therefore not necessary to
call `CoCreateInstance` before using DWriteCore. Instead, call the `DWriteCreateFactory` function to create a factory
object (`IDWriteFactory7`), and then call factory methods to create other objects or perform other actions.

# API Layers

The **text layout API** is the highest layer of the DWriteCore API. It includes *text format* objects (`IDWriteTextFormat4`),
which encapsulate formatting properties, and *text layout* objects (`IDWriteTextLayout4`), which represents formatted text
strings. A text layout object exposes methods for drawing, getting metrics, hit testing, and so on. Each paragraph on this
page is a text layout object.

The **font API** exposes information about fonts, and provides functionality needed for text layout and rendering. It includes
*font collection* objects (`IDWriteFontCollection3`), which are collections of fonts grouped into families, *font set* objects
(`IDWriteFontSet3`), which are flat collections of fonts, and *font face* objects (`IDWriteFontFace6`), which represent specific
fonts.

The **text rendering API** provides interfaces uses for rendering text. When you call a text layout object's `Draw` method,
it calls back to an interface (`IDWriteTextRenderer1`), which must be implemented by the application or by some other library.
The use of an abstract callback interface enables DWriteCore to be decoupled from any particular graphics engine. The text
renderer implementation can use text rendering APIs provided by DWriteCore to help with rendering glyphs. The `TextRenderer` 
class in this application provides a sample implementation of the `IDWriteTextRenderer1` interface.

The **text analysis API** provides low-level APIs for sophisticated applications that implement their own text layout engines.
This includes script analysis, bidi analysis, shaping, and so on.
