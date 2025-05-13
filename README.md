## Dungeons and Diagrams CE

*Dungeons and Dragons CE* is a nonogram-like puzzle designed by Zach Barth. This adaptation is created by euphory.

### Gameplay

This is a puzzle game about filling in eight-by-eight maps of dungeons while observing four simple rules:

1. Every row and column of the grid must contain exactly the labeled number of walls.
2. A cell contains a monster if and only if it is a dead end; i.e., exactly one orthogonally adjacent cell is open.
3. A treasure chest must be contained in a three-by-three area of open space with only one entrance or exit.
4. No two-by-two area of open space may exist outside of a treasure room. 

### Controls

Global:
 - `on` to immediately exit program ("boss button")

Pack select (skipped if only one pack detected):
 - Number keys to select corresponding pack (as shown on-screen)

Level select:
 - Arrow keys to navigate levels
 - `2nd` to play level
 - `clear` to return to pack select (or exit, if only one pack present on calculator)

Gameplay:
 - Arrow keys to navigate grid
 - `2nd` to place wall
 - `alpha` to place gem (reminder of no wall in cell; not checked for level completion)
 - `clear` to return to level select

### Technical Info

This program relies on LibLoad as well as the C Standard Library, available [here](https://github.com/CE-Programming/libraries/releases/latest). If launching this program fails with messages about "need LibLoad" or "missing AppVar," check that these are installed on your calculator.

This is an ASM program written in C targeting the TI-84 Plus CE and TI-83 Premium CE. It uses 16.6 kB of RAM and no ARC (unless archived). By default, it must be run with the `Asm(` token.

### Credits

The original "Dungeons and Diagrams" was designed by Zach Barth, whose original levels are available for free online [here](https://www.trashworldnews.com/files/advanced_dungeons_and_diagrams.pdf) and reproduced as DDLVLZOP.8xv, "Zach's Original Pack."

The visual design of this program is inspired by that in the version of "Dungeons and Diagrams" included in Zachtronics' game *Last Call BBS*.

All graphical assets used in this program are created either by euphory or an anonymous friend. All code is written by euphory.

### Licensing

This program is licensed under the GNU General Public License version 3.0. A copy of the license text should have been distributed along with the program. If not, it is available here: https://www.gnu.org/licenses/gpl-3.0.html.
