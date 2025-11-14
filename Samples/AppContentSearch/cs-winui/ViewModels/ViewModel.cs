// Copyright (c) Microsoft Corporation. All rights reserved.

using Notes.Models;
using System;
using System.Collections.ObjectModel;
using System.Linq;
using System.Threading.Tasks;
using Windows.Storage;

namespace Notes.ViewModels
{
    public class ViewModel
    {
        public ObservableCollection<NoteViewModel> Notes { get; set; } = new();

        public ViewModel()
        {
            _ = LoadNotes();
        }

        public async Task<NoteViewModel> CreateNewNote()
        {
            var title = "New note";

            var folder = await Utils.GetLocalFolderAsync();
            var file = await folder.CreateFileAsync(title + Utils.FileExtension, CreationCollisionOption.GenerateUniqueName);

            var note = new Note()
            {
                Title = title,
                Created = DateTime.Now,
                Modified = DateTime.Now,
                Filename = file.Name
            };

            var noteViewModel = new NoteViewModel(note);
            Notes.Insert(0, noteViewModel);
            var dataContext = await AppDataContext.GetCurrentAsync();
            dataContext.Notes.Add(note);
            await dataContext.SaveChangesAsync();

            return noteViewModel;
        }

        public async Task RemoveNoteAsync(NoteViewModel noteViewModel)
        {
            // Remove all attachments associated with the Note.
            foreach (var attachment in noteViewModel.Attachments.ToList())
            {
                if (noteViewModel != null && attachment != null)
                {
                    await noteViewModel.RemoveAttachmentAsync(attachment);
                }
            }
              
            // Remove from the observable collection.
            Notes.Remove(noteViewModel!);

            // Delete the file from local storage.
            var folder = await Utils.GetLocalFolderAsync();
            var file = await folder.TryGetItemAsync(noteViewModel?.Note.Filename) as StorageFile;

            if (file != null)
            {
                await file.DeleteAsync();
            }

            // Remove from data context and index.
            var dataContext = await AppDataContext.GetCurrentAsync();
            var note = dataContext.Notes.FirstOrDefault(n => n.Filename == noteViewModel!.Note.Filename);

            if (note != null)
            {
                dataContext.Notes.Remove(note);
                await dataContext.SaveChangesAsync();
                await noteViewModel!.RemoveNoteFromIndexAsync();
            }            
        }

        private async Task LoadNotes()
        {
            var dataContext = await AppDataContext.GetCurrentAsync();
            var savedNotes = dataContext.Notes.Select(note => note).ToList();

            StorageFolder notesFolder = await Utils.GetLocalFolderAsync();
            var files = await notesFolder.GetFilesAsync();
            var filenames = files.ToDictionary(f => f.Name, f=> f);

            foreach (var note in savedNotes)
            {
                if (note.Filename != null)
                {
                    if (filenames.ContainsKey(note.Filename))
                    {
                        filenames.Remove(note.Filename);
                        Notes.Add(new NoteViewModel(note));
                    }
                    else
                    {
                        // delete note from db
                        dataContext.Notes.Remove(note);
                    }
                }
            }

            foreach (var filename in filenames)
            {
                if (filename.Key.EndsWith(Utils.FileExtension))
                {
                    var file = filename.Value;
                    var note = new Note()
                    {
                        Title = file.DisplayName,
                        Created = file.DateCreated.DateTime,
                        Filename = file.Name,
                        Modified = DateTime.Now
                    };
                    dataContext.Notes.Add(note);
                    Notes.Add(new NoteViewModel(note));
                }
            }

            await dataContext.SaveChangesAsync();
        }
    }
}
