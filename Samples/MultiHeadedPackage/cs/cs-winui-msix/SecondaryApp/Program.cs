// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;

namespace SecondaryApp
{
    class Program
    {
        static void Main()
        {
            Console.WriteLine("=== Secondary App - Multi-Headed MSIX ===");
            Console.WriteLine();
            Console.WriteLine("This is the Secondary App.");
            Console.WriteLine("Both this app and the Primary App were installed from the same MSIX package.");
            Console.WriteLine();
            Console.WriteLine("Press any key to exit...");
            Console.ReadKey();
        }
    }
}
