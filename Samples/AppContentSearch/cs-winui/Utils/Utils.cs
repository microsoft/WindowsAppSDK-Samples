// Copyright (c) Microsoft Corporation. All rights reserved.

using Microsoft.Extensions.AI;
using Microsoft.Windows.AI.Search.Experimental.AppContentIndex;
using Notes.ViewModels;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading.Tasks;
using Windows.Storage;
using Windows.Foundation;
using System.Threading;

namespace Notes
{
    internal partial class Utils
    {
        public static readonly string FolderName = "MyNotes";
        public static readonly string FileExtension = ".txt";
        public static readonly string StateFolderName = ".notes";
        public static readonly string AttachmentsFolderName = "attachments";

        private static string localFolderPath = string.Empty;

        public static async Task<string> GetLocalFolderPathAsync()
        {
            if (string.IsNullOrWhiteSpace(localFolderPath))
            {
                localFolderPath = (await GetLocalFolderAsync()).Path;
            }

            return localFolderPath;
        }

        public static async Task<StorageFolder> GetLocalFolderAsync()
        {
            return await KnownFolders.DocumentsLibrary.CreateFolderAsync(FolderName, CreationCollisionOption.OpenIfExists);
        }

        public static async Task<StorageFolder> GetStateFolderAsync()
        {
            var notesFolder = await GetLocalFolderAsync();
            return await notesFolder.CreateFolderAsync(StateFolderName, CreationCollisionOption.OpenIfExists);
        }

        public static async Task<StorageFolder> GetAttachmentsFolderAsync()
        {
            var notesFolder = await GetLocalFolderAsync();
            return await notesFolder.CreateFolderAsync(AttachmentsFolderName, CreationCollisionOption.OpenIfExists);
        }

        public static async Task<StorageFolder> GetAttachmentsTranscriptsFolderAsync()
        {
            var notesFolder = await GetStateFolderAsync();
            return await notesFolder.CreateFolderAsync(AttachmentsFolderName, CreationCollisionOption.OpenIfExists);
        }

        public static async Task<List<SearchResult>> SearchAsync(AppContentIndexer appContentIndexer, string searchText, int top = 5, CancellationToken cancellationToken = default)
        {
            var results = new List<SearchResult>();

            if (string.IsNullOrWhiteSpace(searchText) || appContentIndexer == null)
            {
                return results;
            }

            cancellationToken.ThrowIfCancellationRequested();
            
            var context = await AppDataContext.GetCurrentAsync();

            IEnumerable<AppIndexQueryMatch>? textMatches = null;
            IEnumerable<AppIndexQueryMatch>? imageMatches = null;
            await Task.Run(() =>
            {
                cancellationToken.ThrowIfCancellationRequested();
                AppIndexQuery? query = appContentIndexer.CreateQuery(searchText);
                textMatches = query.GetNextTextMatches(top);
                imageMatches = query.GetNextImageMatches(top);
            }, cancellationToken);

            cancellationToken.ThrowIfCancellationRequested();

            if (textMatches != null)
            {
                foreach (var match in textMatches)
                {
                    cancellationToken.ThrowIfCancellationRequested();
                    
                    int matchcontentId = int.Parse(match.ContentId);
                    var note = await context.Notes.FindAsync(matchcontentId);

                    if (note != null && note.Filename != null)
                    {
                        Debug.WriteLine($"Text match: {note.Filename}");

                        AppManagedTextQueryMatch? textMatch = match as AppManagedTextQueryMatch;

                        if (textMatch != null)
                        {
                            string matchingData = await NoteViewModel.LoadTextContentByIdAsync(note.Filename);
                            string matchingString = matchingData.Substring(textMatch.TextOffset, Math.Min(500, matchingData.Length - textMatch.TextOffset));
                            var searchResult = new SearchResult
                            {
                                Content = matchingString,
                                ContentType = ContentType.Note,
                                ContentSubType = ContentSubType.None,
                                SourceId = note.Id,
                                Title = note.Title,
                                MostRelevantSentence = matchingString
                            };

                            results.Add(searchResult);
                        }
                    }
                }
            }
            else
            {
                Debug.WriteLine("TextMatches is null");
            }

            cancellationToken.ThrowIfCancellationRequested();

            if (imageMatches != null)
            {
                foreach (var match in imageMatches)
                {
                    cancellationToken.ThrowIfCancellationRequested();
                    
                    int matchContentId = int.Parse(match.ContentId);
                    var image = await context.Attachments.FindAsync(matchContentId);

                    if (image != null)
                    {
                        AppManagedImageQueryMatch? imageMatch = match as AppManagedImageQueryMatch;
                        if (imageMatch != null)
                        {
                            Debug.WriteLine($"Image match: {imageMatch.ContentId}, Subregion: {imageMatch.Subregion}");
                            var searchResult = new SearchResult
                            {
                                ContentType = ContentType.Image,
                                SourceId = image.Id
                            };

                            if (image.Filename != null)
                            {
                                byte[] matchingData = await NoteViewModel.LoadImageContentByIdAsync(image.Filename);
                                searchResult.ContentBytes = matchingData;
                                searchResult.Title = image.Filename;

                                var lower = image.Filename.ToLower();
                                if (lower.EndsWith(".png"))
                                {
                                    searchResult.ContentSubType = ContentSubType.Png;
                                }
                                else if (lower.EndsWith(".jpeg") || lower.EndsWith(".jpg"))
                                {
                                    searchResult.ContentSubType = ContentSubType.Jpeg;
                                }
                                else if (lower.EndsWith(".gif"))
                                {
                                    searchResult.ContentSubType = ContentSubType.Gif;
                                }
                                else if (lower.EndsWith(".bmp"))
                                {
                                    searchResult.ContentSubType = ContentSubType.Bmp;
                                }
                            }

                            var attachmentsFolder = await GetAttachmentsFolderAsync();
                            searchResult.Path = attachmentsFolder.Path + "\\" + image.RelativePath;

                            // Capture bounding box if present (Subregion is IReference<Rect>)
                            try
                            {
                                if (imageMatch.Subregion != null)
                                {
                                    var rect = imageMatch.Subregion.Value;
                                    searchResult.BoundingBox = rect;
                                }
                            }
                            catch (Exception ex)
                            {
                                Debug.WriteLine("Failed to read image Subregion: " + ex.Message);
                            }
                            results.Add(searchResult);
                        }
                    }
                }
            }
            else
            {
                Debug.WriteLine("ImageMatches is null");
            }
            return results;
        }
    }

    public record SearchResult
    {
        public string? Title { get; set; }
        public string? Content { get; set; }
        public byte[]? ContentBytes { get; set; }
        public string? MostRelevantSentence { get; set; }
        public string? Path { get; set; }
        public int SourceId { get; set; }
        public ContentType ContentType { get; set; }
        public ContentSubType ContentSubType { get; set; }
        public Rect? BoundingBox { get; set; }

        public static string ContentTypeToGlyph(ContentType type)
        {
            return type switch
            {
                ContentType.Note => "📝",
                ContentType.Image => "🖼️",
                ContentType.Audio => "🎙️",
                ContentType.Video => "🎞️",
                ContentType.Document => "📄",
                _ => throw new NotImplementedException()
            };
        }

        public static string ContentSubTypeToMimeType(ContentSubType subType)
        {
            return subType switch
            {
                ContentSubType.None => "text/plain",
                ContentSubType.Png => "image/png",
                ContentSubType.Jpeg => "image/jpeg",
                ContentSubType.Gif => "image/gif",
                ContentSubType.Bmp => "image/bmp",
                _ => throw new NotImplementedException()
            };
        }
    }

    public enum ContentType
    {
        Image = 0,
        Audio = 1,
        Video = 2,
        Document = 3,
        Note = 4,
    }

    public enum ContentSubType
    {
        None = 0,
        Png = 1,
        Jpeg = 2,
        Gif = 3,
        Bmp = 4
    }
}