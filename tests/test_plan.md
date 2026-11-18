# Test Plan for msh

| Input|                            | Expected behaviour |                    | Actual result |

| (blank line)                      | New prompt, no error                    | Displayed msh[0]>  with no error 
| echo hello world                  | Prints text; status 0                   | Printed hello world; prompt showed `msh[0]> ` 
| ls /nonexistent                   | Error from ls; non-zero status          | Printed `ls: cannot access '/nonexistent': No such file or directory`; prompt showed `msh[2]> ` 
| nosuchcommand                     | Error message; status 127               | Printed `nosuchcommand: No such file or directory`; prompt showed `msh[127]> ` 
| cd /tmp then pwd                  | Prints `/tmp`                           | Printed `/tmp`; prompt showed `msh[0]> ` 
| sleep 3 &                         | Prompt returns at once; PID shown       | Printed `[bg] started PID 78512` and returned `msh[0]> ` immediately 
| echo hi > f.txt then cat f.txt    | File contains `hi`                      | `cat f.txt` printed `hi`; prompt showed `msh[0]> ` 
| sleep 100 then `Ctrl-C`           | Sleep stops; shell survives; status 130 | `sleep` stopped; shell survived and showed `msh[130]> ` 
| Ctrl-D` at the prompt             | Shell exits cleanly                     | Printed newline and exited cleanly to bash 
| cd (no arguments) then pwd        | Changes to `$HOME` directory            | Printed `/home/<administrator>`; prompt showed `msh[0]> ` 
| echo second > f.txt then cat f.txt| Truncates existing file (`O_TRUNC`)     | Printed `second` only; prompt showed `msh[0]> ` 
| echo    spaced                    | Ignores extra spaces and tabs           | Printed `spaced`; prompt showed `msh[0]> ` 