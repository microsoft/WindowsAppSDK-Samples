# Custom Font Collection

A *custom font collection* is a font collection created by an application, containing
font files specified by the application.

You can enumerate fonts in a custom font collection in the same way as the system font
collection. You can also use a custom font collection with the text layout API by passing
your custom font collection instead of nullptr as the *fontCollection* parameter to
`IDWriteFactory::CreateTextFormat` or `IDWriteTextLayout::SetFontCollection`.

The easiest way to create a custom font collection is to create a custom font set using
the `IDWriteFontSetBuilder` interface and then call the `CreateFontCollectionFromFontSet`
factory method.

Optionally, a font collection can contain *virtual font files* that are not really file 
system files. For example, a virtual font file might be embedded in a document, or it
might be a binary resource in the executable image, as in this example. To use virtual
font files, you need to implement the `IDWriteFontFileLoader` callback interface.
