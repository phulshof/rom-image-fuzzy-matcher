Rename all your screenshots (or box scan images) to match your ROM names. For MAME front ends like WahCade or other console emulators.

Usage: `RomImageFuzzyMatcher [-m match_threshold 0.0 to 1.0]  [-p perfect_match_threshold 0.0 to 1.0]  [-f force bad matches to be made anyhow]-s art_dir_path -r rom_dir_path -d dest_renamed_path`

**-m Match Threashold:**  Levenshtein distance counts the number of inversions, insertions etc between two strings.  The bad match percentage is defined as:  (number\_of\_inversions\_to\_art\_name / number\_of\_characters\_in\_rom\_name).

If the bad match percentage is above the threshold it will be flagged as a bad match.

Some liberties are taken: e.g. No white space counts as inversions.  Most punctuation is deleted.  Garbage like `[USA] !` and file extensions are deleted before comparisons, etc.


**-p perfect\_match\_threshold:** is used to cull out all other art files in the search, within a certain match threshold. Default is 0 = "real" perfect match distance needed to cull the rest of a search.

If you don't care about accurate matches, since matching an entire dir of art is taking too long, you can increase the perfect match threshold to make matching happen faster, at the expense of getting less accurate matches.

**-f force\_bad\_matches:** will force matches to be created that are below the threshold.  Use this if you don't care about a few bad matches, but want an art image for all your ROMS anyhow.

Example:

`RomImageFuzzyMatcher -m 0.50 -s /home/kyle2/Games/Roms/SNES/SNES_Box_Scans/ -r /home/kyle2/Games/Roms/SNES/Roms/ -d /home/kyle2/Games/Roms/SNES/SNES_Corrected_box_scans/`