// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

using System.Runtime.InteropServices;

// This file provides helpers to be able to use dynamic dependencies, which will enable this unpackaged app
// to consume the Project Reunion APIs in the Project Reunion framework package.

namespace Microsoft.Windows.ApplicationModel
{
    public struct PackageVersion
    {
        ushort Major;
        ushort Minor;
        ushort Build;
        ushort Revision;

        public PackageVersion(ushort major) :
            this(major, 0, 0, 0)
        {
        }
        public PackageVersion(ushort major, ushort minor) :
            this(major, minor, 0, 0)
        {
        }
        public PackageVersion(ushort major, ushort minor, ushort build) :
            this(major, minor, build, 0)
        {
        }
        public PackageVersion(ushort major, ushort minor, ushort build, ushort revision)
        {
            Major = major;
            Minor = minor;
            Build = build;
            Revision = revision;
        }

        public PackageVersion(ulong version) :
            this((ushort)(version >> 48), (ushort)(version >> 32), (ushort)(version >> 16), (ushort)version)
        {
        }

        public ulong ToVersion()
        {
            return (((ulong)Major) << 48) | (((ulong)Minor) << 32) | (((ulong)Build) << 16) | ((ulong)Revision);
        }
    };

    public class MddBootstrap
    {
        public static int Initialize(uint majorMinorVersion)
        {
            return Initialize(majorMinorVersion, null);
        }

        public static int Initialize(uint majorMinorVersion, string versionTag)
        {
            var minVersion = new PackageVersion();
            return Initialize(majorMinorVersion, versionTag, minVersion);
        }

        public static int Initialize(uint majorMinorVersion, string versionTag, PackageVersion minVersion)
        {
            return MddBootstrapInitialize(majorMinorVersion, versionTag, minVersion);
        }

        [DllImport("Microsoft.ProjectReunion.Bootstrap.dll", CharSet = CharSet.Unicode)]
        private static extern int MddBootstrapInitialize(uint majorMinorVersion, string versionTag, PackageVersion packageVersion);

        public static void Shutdown()
        {
            MddBootstrapShutdown();
        }

        [DllImport("Microsoft.ProjectReunion.Bootstrap.dll")]
        private static extern void MddBootstrapShutdown();
    }
}
