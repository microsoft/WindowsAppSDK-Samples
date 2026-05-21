// Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.Windows.Search.AppContentIndex;
using Notes.ViewModels;
using Windows.Foundation;
using Windows.Storage;

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
            if (string.IsNullOrWhiteSpace(searchText) || appContentIndexer == null)
            {
                return new List<SearchResult>();
            }

            cancellationToken.ThrowIfCancellationRequested();

            IEnumerable<TextQueryMatch>? textMatches = null;
            IEnumerable<ImageQueryMatch>? imageMatches = null;
            await Task.Run(() =>
            {
                cancellationToken.ThrowIfCancellationRequested();
                AppIndexTextQuery? textQuery = appContentIndexer.CreateTextQuery(searchText);
                AppIndexImageQuery? imageQuery = appContentIndexer.CreateImageQuery(searchText);
                textMatches = textQuery.GetNextMatches(top);
                imageMatches = imageQuery.GetNextMatches(top);
            }, cancellationToken);

            return await SearchMatchesAsync(textMatches, imageMatches, cancellationToken);
        }

        public static async Task<List<SearchResult>> SearchMatchesAsync(IEnumerable<TextQueryMatch>? textMatches, IEnumerable<ImageQueryMatch>? imageMatches, CancellationToken cancellationToken = default)
        {
            var results = new List<SearchResult>();
            var context = await AppDataContext.GetCurrentAsync();

            cancellationToken.ThrowIfCancellationRequested();

            if (textMatches != null)
            {
                foreach (var match in textMatches)
                {
                    cancellationToken.ThrowIfCancellationRequested();

                    SearchResult? ocrResult = await TryCreateOcrSearchResultAsync(match, context, cancellationToken);
                    if (ocrResult != null)
                    {
                        results.Add(ocrResult);
                        continue;
                    }

                    if (match is AppManagedTextQueryMatch textMatch)
                    {
                        if (!int.TryParse(match.ContentId, out int noteId))
                        {
                            Debug.WriteLine($"Skipping text match with non-integer content ID: {match.ContentId}");
                            continue;
                        }

                        var note = await context.Notes.FindAsync(noteId);
                        if (note == null || note.Filename == null)
                        {
                            continue;
                        }

                        Debug.WriteLine($"Text match: {note.Filename}");

                        string matchingData = await NoteViewModel.LoadTextContentByIdAsync(note.Filename);

                        int textOffset = Math.Max(0, Math.Min(textMatch.TextOffset, matchingData.Length));
                        int remainingLength = matchingData.Length - textOffset;

                        if (remainingLength <= 0)
                        {
                            Debug.WriteLine($"Skipping match with invalid TextOffset: {textMatch.TextOffset} for content length: {matchingData.Length}");
                            continue;
                        }

                        int substringLength = Math.Min(500, remainingLength);
                        string matchingString = matchingData.Substring(textOffset, substringLength);
                        var notesFolder = await GetLocalFolderAsync();
                        var textResult = new SearchResult
                        {
                            Content = matchingString,
                            ContentType = ContentType.Note,
                            ContentSubType = ContentSubType.None,
                            SourceId = note.Id,
                            Title = note.Title,
                            MostRelevantSentence = matchingString,
                            Path = notesFolder.Path + "\\" + note.Filename
                        };

                        results.Add(textResult);
                        continue;
                    }

                    Debug.WriteLine($"Skipping unsupported text match type: {match.GetType().Name}");
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

                    if (!int.TryParse(match.ContentId, out int matchContentId))
                    {
                        Debug.WriteLine($"Skipping image match with non-integer content ID: {match.ContentId}");
                        continue;
                    }

                    var image = await context.Attachments.FindAsync(matchContentId);

                    if (image != null)
                    {
                        AppManagedImageQueryMatch? imageMatch = match as AppManagedImageQueryMatch;
                        if (imageMatch != null)
                        {
                            Debug.WriteLine($"Image match: {imageMatch.ContentId}, Region of interest: {imageMatch.RegionOfInterest}");
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

                            // Capture bounding box if present (Region of interest is IReference<Rect>)
                            try
                            {
                                if (imageMatch.RegionOfInterest != null)
                                {
                                    var rect = imageMatch.RegionOfInterest.Value;
                                    searchResult.BoundingBox = rect;
                                }
                            }
                            catch (Exception ex)
                            {
                                Debug.WriteLine("Failed to read image Region of interest: " + ex.Message);
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

        private static async Task<SearchResult?> TryCreateOcrSearchResultAsync(TextQueryMatch match, AppDataContext context, CancellationToken cancellationToken)
        {
            if (!string.Equals(match.GetType().Name, "AppManagedOcrTextQueryMatch", StringComparison.Ordinal))
            {
                return null;
            }

            cancellationToken.ThrowIfCancellationRequested();

            if (!int.TryParse(match.ContentId, out int attachmentId))
            {
                Debug.WriteLine($"Skipping OCR match with non-integer content ID: {match.ContentId}");
                return null;
            }

            var attachment = await context.Attachments.FindAsync(attachmentId);
            if (attachment == null)
            {
                Debug.WriteLine($"Skipping OCR match; attachment not found for content ID: {attachmentId}");
                return null;
            }

            var note = await context.Notes.FindAsync(attachment.NoteId);
            if (note == null)
            {
                Debug.WriteLine($"Skipping OCR match; note not found for attachment ID: {attachmentId}");
                return null;
            }

            // TODO: Replace these reflection-based accesses with direct property access
            // (e.g. `(match as AppManagedOcrTextQueryMatch).Fragment`) once the
            // AppManagedOcrTextQueryMatch type and its Fragment / Subregion properties are
            // projected by the Microsoft.Windows.Search.AppContentIndex SDK. Reflection is
            // used here only as a temporary workaround for the current preview SDK and is
            // NOT a recommended pattern for production code: if the API renames either
            // property in a future preview this would silently return null/empty.
            string fragment = match.GetType().GetProperty("Fragment")?.GetValue(match) as string ?? string.Empty;
            Rect? subregion = GetMatchSubregion(match);
            Debug.WriteLine($"OCR text match: contentId={match.ContentId}, fragment='{fragment}', Subregion={subregion}");

            var attachmentsFolder = await GetAttachmentsFolderAsync();
            return new SearchResult
            {
                Content = fragment,
                ContentType = ContentType.OcrText,
                ContentSubType = ContentSubType.None,
                SourceId = note.Id,
                AttachmentId = attachment.Id,
                Title = note.Title + " (OCR)",
                MostRelevantSentence = fragment,
                Path = attachmentsFolder.Path + "\\" + attachment.RelativePath,
                BoundingBox = subregion
            };
        }

        private static Rect? GetMatchSubregion(TextQueryMatch match)
        {
            try
            {
                // TODO: Same temporary workaround as the `Fragment` access above —
                // replace with direct property access once `Subregion` is projected
                // by the SDK.
                object? subregion = match.GetType().GetProperty("Subregion")?.GetValue(match);
                return subregion is Rect rect ? rect : null;
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Failed to read OCR Subregion: " + ex.Message);
                return null;
            }
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
        public int? AttachmentId { get; set; }
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
                ContentType.OcrText => "🔍",
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
        OcrText = 5,
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
