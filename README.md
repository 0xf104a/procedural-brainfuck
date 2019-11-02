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
|`,`                     | Read char from `stdin`  |
|`.`                     | Put char to `stdout `   |

<b>*</b> – Not yet tested <br>
## Procedures
Procedure contains some brainfuck code which will be executed when procedure called. Procedure name could containt letters, numbers and underscores. To call porcedure you should put it name and `;` after it. Procedures could call other procedures. You are free to define procedure inside a loop, but **this could lead to a severe memory leak.**<br>
Definition example:<br>
```
procedure_name{...}
```
<br>
Call example:<br>

```
procedure_name;
```
## Arguments
| Argument                       |                       Description     |
|--------------------------------|---------------------------------------|
| `-h` `--help`                  | Display help message                  |
| `<filename>`                   | Path to filename with code            |
| `--security <arg>`             | Set security mode                     |

## Security mode
To set a security mode put it's name after `--security` argument.<br>

| Mode               | Description                      |
|--------------------|----------------------------------|
| `none`             |Do none when program exceeds array size. |
| `loop`             |Cycle over array                  |
| `die`              |Print an error and exit           |

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
 $ ./pbf ../examples/hello_world.pbf
 Hello world!
 $
 ```
 <p>
 You could check out some more examples <a href="https://github.com/Andrewerr/procedural-brainfuck/tree/master/examples"> here</a>
 </p>
 
## ToDo
* Implement server mode which will allow to put a sequence of files to run
