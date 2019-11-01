# procedural-brainfuck
## Description
Version of brainfuck interpreter with procedures. Usual brainfuck description: https://en.wikipedia.org/wiki/Brainfuck<br>
Tested only on Mac OS X Mojave.
## Building it
```
./build.sh all
```
<br>
Now the pbf interpreter should be inside of `bin/` directory.
<br>


## Command table

|Command                 |Description              |
|------------------------|-------------------------|
|`>`                     | Move array ptr right.   |
|`<`                     | Move array ptr left.    |
|`+`                     | Add 1 to current array value|
|`-`                     | Substract 1 from current array value|
|`[`                     | Begin loop              |
|`]`                     | End loop                |
|`!`                     | Exit procedure          |
|`?`*                    | `!` if current value is 0 |
|`;`                     | States the end of name that should be called |

<b>*</b> – Not yet tested 
## Procedures
Procedure contains some brainfuck code which will be executed when procedure called. Procedure names could containt letters, numbers and underscores. To call porcedure you should put it name and `;` after it. Procedures could call other procedures. You are free to define procedure inside a loop, but **this could lead to a severe memory leak.**<br>
Definition example:<br>
```
procedure_name{...}
```
<br>
Call example:<br>

```
procedure_name;
```
## Security
The interpreter **DOES NOT CHECK** whether current cell is inside of allocated array. 
## Hello, world!
Here is code of "Hello, world!" app with use of procdeures.
```
h{++++++++++[>+++++++>++++++++++>+++>+<<<<-]>++
 .>+.+++++++..+++.>++.<<+++++++++++++++.>.+++.
 ------.--------.>+.>.}h;
 ```
 To test this code:<br> 
 ```
 $ cd bin/
 $ ./pbf < ../examples/hello_world.pbf
 Hello world!
 $
 ```
## ToDo
* Implement security
* Implement server mode which will allow to put a sequenc of files to execute
