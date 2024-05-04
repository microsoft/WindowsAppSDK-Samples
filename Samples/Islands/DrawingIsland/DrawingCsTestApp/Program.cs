using DrawingIslandComponents;
using System;

// See https://aka.ms/new-console-template for more information
Console.WriteLine("Hello, World!");

Microsoft.UI.Composition.Compositor compositor = new();
DrawingIsland drawing = new(compositor);
_= drawing.Island;
