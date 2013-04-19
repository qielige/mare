Mare
====

Mare (Make replacement) is a build automation tool similar to Make that can be used to automatize the build process of software projects. Therefore, it manages dependencies between source files and targets using build rules (or recipes) for each source file. Using these rules, Mare can determine which files to recompile in order to apply changes in source files. Unlike Make, Mare relies on a file format that was designed with needs of modern software projects in mind.

Motivation
----------

Make is part of almost every build system for UNIX-like systems. It is simple, versatile and easy to use but it lacks certain features as required for modern software projects. Hence it is mostly used in conjunction with Makefile-generators (like automake, cmake, qmake, prebuild etc.). 

* Many software projects make extensive use of the file system and version control systems like SVN or Git. Files are stored in directories and rules on compiling source files can be associated to the directory in which the files are stored. However, Make requires a list of files of each directory that can not be generated automatically without using any extensions or external command line tools like "find". Often these file lists are managed manually or generated by a Makefile-generator.

* In a Makefile the rules on how to compile files are specified without any conventions or additional information. The structure of the project can hence not be derived from the Makefile alone, since some information like the purpose of a rule is missing. Other third party tools like IDEs use their own files formats to manage such information. This leads to non-centralized project configuration management that is unnecessarily complex and tedious to maintain.

* Many software projects use different configuration sets (e.g. for debuggable and for optimized code). Although Make is capable of handling multiple configurations this can lead to badly structured Makefiles. Even the support for multiple configurations in some of the popular build systems is limited in some measure.

* While Makfile-generators add most the functionality that is missing in Make they compromise the simplicity of the build system. Many Makefile-generates were build to address a certain issue and the result is not always in favor of the software project that ends up using them.

How does mare work?
--------------

Mare is a small stand alone tool that has to be called in its working directory. There, it searches for a file named Marefile. This file declares rules on how to compile the source files of a software project into build targets. Mare determines which targets to recompile by comparing the file modification timestamp of the source files and previously generated build targets. In case the build target is missing or older then one of its source files it is recompiled using a command line tool as declared by the build rules. Instead of executing the build process directly, mare can also be used to generate project files for other tools like Visual Studio, CodeBlocks, CodeLite, NetBeans, Make and cmake.

An example: A Marefile consists of three lists: "targets", "configurations", "platforms". "configurations" lists configurations (e.g. "Debug" and "Release") in which the build targets can be build. "targets" list all the build targets and contains the source files for each target and the rules to compile them. "platforms" is normally not used unless the project utilizes a cross-compiler where the target platform differs from the host platform. Here is an example of a Marefile for a simple c++ application where all source files are stored in the directory "src":

```
configurations = { Debug, Relase }
targets = {
  myApplication = cppApplication + {
    files = {
      "src/**.cpp" = cppSource
    }
  }
}
```

Compiling mare
-----------

### Windows

Git and Visual Studio 2010 (or newer) are required to compile Mare on Windows (e.g. Windows Xp, Windows 7). https://github.com/jeffco/mare.git has the be cloned into a working directory. The directory "VS2010" of the working copy will then contain project files for Visual Studio that can be used to compile the program. The resulting binary file (mare.exe) can be found in the directory "Debug" or "Release" depending on which configuration was used.

### GNU/Linux

Git, a compiler compatible with g++ and Make are required to compile Mare on GNU/Linux (or another GNU/Linux-like operating system). Mare can then be cloned and compiled with:

```
$ cd /your/working/directory
$ git clone https://github.com/jeffco/mare.git mare
$ cd mare
$ make
```

The generated executable (mare) will be located in the directory "Debug". It can be used to compile mare without debug symbols:

```
$ cd /your/working/directory/mare
$ Debug/mare config=Release
```

The Marefile
------------

A Marefile contains nested associative list where keys are strings and the value associated to a key can be another associative list. However, keys do not have to point on subordinate associative list. An associate list can also be interpreted as a string by concatenating each key separated by space characters. Within an associative list multiple subordinate associative lists can be defined by separating multiple keys with space characters. An associative list interpreted as string can therefore be inserted into a key. Associative lists can also be composed of other associative lists and different cases for multiple configurations can be handled when declaring a list.

A target declared in a Mare (e.g. "Example1") contains a list of input files, output files and a command line to compile the input files into the output files:

```
targets = {
  Example1 = {
    input = "file1.o file2.o"
    output = "Example1"
    command = "g++ file1.o file2.o -o Example1"
  }
}
```

The files listed in "input" and "output" are listed twice. To avoid this "input" and "output" can be inserted into "command":

```
targets = {
  Example1 = {
    input = "file1.o file2.o"
    output = "Example1"
    command = "g++ $(input) -o $(output)"
  }
}
```

In this example the object files (intermediate targets) should be created by compiling some source files. A target contains a list named "files" that describes how the intermediate targets are created:

```
targets = {
  Example1 = {
    input = "file1.o file2.o"
    output = "Example1"
    command = "g++ $(input) -o $(output)"
    
    cppFlags = "-O3"
    
    files = {
      "file1.cpp" = {
        input = "file1.cpp"
        output = "file1.o"
        command = "g++ $(cppFlags) -c $(input) -o $(output)"
      }
      "file2.cpp" = {
        input = "file2.cpp"
        output = "file2.o"
        command = "g++ $(cppFlags) -c $(input) -o $(output)"
      }
    }
  }
}
```

The rules on compiling "file1.cpp" and "file2.cpp" can be combined:

```
targets = {
  Example1 = {
    input = "file1.o file2.o"
    output = "Example1"
    command = "g++ $(input) -o $(output)"
    
    cppFlags = "-O3"
    
    files = {
      "file1.cpp file2.cpp" = {
        input = "$(file)"
        output = "$(patsubst %.cpp,%.o,$(file))"
        command = "g++ $(cppFlags) -c $(input) -o $(output)"
      }
    }
  }
}
```

To improve clearness the rule on compiling a .cpp file can be swapped out:

```
targets = {
  Example1 = {
    input = "file1.o file2.o"
    output = "Example1"
    command = "g++ $(input) -o $(output)"
    
    cppFlags = "-O3"
    
    files = {
      "file1.cpp file2.cpp" = myCppSource
    }
  }
}

myCppSource = {
  input = "$(file)"
  output = "$(patsubst %.cpp,%.o,$(file))"
  command = "g++ $(cppFlags) -c $(input) -o $(output)"
}
```

The list of object files ("file1.o file2.o") can be generated automatically by altering the list of source files. The name of the output file "Example" can be derived from name of the target:

```
targets = {
  Example1 = {
    input = "$(patsubst %.cpp,%.o,$(files))"
    output = "$(target)"
    command = "g++ $(input) -o $(output)"
    
    cppFlags = "-O3"
    
    files = {
      "file1.cpp file2.cpp" = myCppSource
    }
  }
}
...
```

That way the rule on linking the program can also be swapped out:

```
targets = {
  Example1 = myCppApplication + {
    cppFlags = "-O3"
    files = {
      "file1.cpp file2.cpp" = myCppSource
    }
  }
}

myCppSource = {
  input = "$(file)"
  output = "$(patsubst %.cpp,%.o,$(file))"
  command = "g++ $(cppFlags) -c $(input) -o $(output)"
}

myCppApplication = {
  input = "$(patsubst %.cpp,%.o,$(files))"
  output = "$(target)"
  command = "g++ $(input) -o $(output)"
}
```

The list of source files can be generated automatically using a list of files that exist in the file system and matches a wildcard pattern:

```
targets = {
  Example1 = myCppApplication + {
    cppFlags = "-O3"
    files = {
      "**.cpp" = myCppSource
    }
  }
}
...
```

Keys of swapped out lists can be overwritten when used. For example "output" could be overwritten subsequently if the name of the output file should not be "Example1":

```
targets = {
  Example1 = myCppApplication + {
    output = "Example1.blah"
    cppFlags = "-O3"
    files = {
      "**.cpp" = myCppSource
    }
  }
}
...
```

There is set of build-in rules (cApplication, cppApplication, cDynamicLibrary, cppDynamicLibrary, cStaticLibrary, cppStaticLibrary, cSource und cppSource) available which can be used for simple c and c++ applications. (see section "Build-in Rules")

### Specialization

An "if &lt;expr&gt; &lt;statements&gt; [else &lt;statements&gt;]" expression can be used when declaring a list. This allows adding customization for certain configurations:

```
cppFlags = "-mmmx"
if configuration == "Release" {
  cppFlags += "-O3"
}
...
```

or

```
cppFlags = {
  "-mmmx"
  if configuration == "Release" { "-O3" }
}
...
```

or

```
cppFlags = "-mmmx -O3"
if configuration != "Release" {
  cppFlags -= "-O3"
}
...
```

or

```
cppFlags = {
  "-mmmx -O3"
  if configuration != "Release" { -"-O3" }
}
...
```

Therefore, Mare provides the following "variables":
* "configuration" � the name of the configuration currently built (e.g. "Debug", "Release", ...) 
* "platform" � the name of the target platform (e.g. "Win32", "Linux", "MacOSX", ...) 
* "host" � the name of the host platform (e.g. "Win32", "Linux", "MacOSX", ...) 
* "tool" � the name of a translator (declared when the mare is translated into another format) (e.g. "vcxproj", "codelite", "codeblocks", "cmake", "netbeans") 
* "target" � the name of the target currently handled
* "architecture" � the architecture of the host system (e.g. "i686", "x86_64")

### Include

Instead of declaring a list it is also possible to include a list from an external Marefile:

```
targets = {
  include "Example1.mare"
}
...
```

### File Name Wildcards

When wildcards in file names are used, the wildcard pattern will be replaced by a list of matching files found in the file system. In the following example the "**.cpp" will be replaced with "file1.cpp file2.cpp" given these two files exist:

```
targets = {
  Example1 = myCppApplication + {
    files = {
      "**.cpp" = myCppSource
    }
  }
}
...
```

The wildcard pattern may contain the following placeholders:
* * - matches any string withing the file name (e.g. "*.cpp" matches "ab.cpp", "bcd.cpp")
* ? - matches a single character withing the file name (e.g. "a?.cpp" matches "ab.cpp", "ac.cpp" aber nicht mit "aef.cpp") 
* ** - matches any path withing the file name (e.g. "**.cpp" matches "aa.cpp", "bb.cpp", "subdir/bbws.cpp", "subdir/subdir/bassb.cpp") 

### Using Space in Keys

The space character with in a key can be used to assign multiple keys at once. However, if a key should actually contain a space character (for instance for a file name contains a space character) then the whole string can be enclosed with quotation marks:

```
myKey = "\"file name.txt\""
```

(This feature is experimentell.)

### ',' and ';'

Each assignment in a Marefile can be separated with an optional comma or semicolon:

```
targets = {
  Example1 = myCppApplication + {
    files = {
      "*.cpp" = myCppSource;
    },;;,,
  },
};
...
```

### Functions

Within keys a functions can be used with the syntax "$(function arguments)". These are similar to the functions available in Make (see http://www.gnu.org/software/make/manual/make.html#Functions) but some of these are not implemented yet. The currently implemented functions are:
* subst, patsubst, filter, filter-out, firstword, lastword, dir, notdir, suffix, basename, addsuffix, addprefix, if, foreach, origin 

Mare introduces some additional functions:
* lower � transforms a string into lower case letters ("$(lower AbC)" becomes "abc")
* upper � transforms a string into upper case letters ("$(upper dDd)" becomes "DDD")
* readfile � inserts the content of plain text file (e.g. "$(readfile anyfile.d)") 

### Build-in Rules

There is a set of build-in rules available suitable for simple c and c++ applications, dynamic libraries and static libraries. The Translators (see section "Translators") interpret them accordingly to allow a "native" translation into project or build files of the utilized translator.
* cppSource, cSource - rules for c/cpp source files 
* cppApplication, cApplication - rules for c/cpp executables
* cppDynamicLibrary, cDynamicLibrary � rules for c/cpp DLLs or "shared objects" 
* cppStaticLibrary, cStaticLibrary � rules for static c/cpp libraries 

These rules can be customized using the following lists:
* linker � the program used to link a c/cpp application or DLL/shared object (default is "gcc" for cApplication or cDynamicLibrary, "g++" for cppApplication or cppDynamicLibrary and "ar" for cppStaticLibrary or cStaticLibrary) 
* linkFlags, libPaths, libs - flags passed to the linker
* cCompiler, cppCompiler � the compiler used to compile c/cpp files (default is "gcc" for cApplication, cDynamicLibrary or cStaticLibrary and "g++" for cppApplication, cppDynamicLibrary or cppStaticLibrary)
* cFlags, cppFlags, defines, includePaths � flags passed to the compiler
* buildDir � the directory used to store intermediate files (default is "$(configuration)") 

A simple Marefile like

```
targets = {
  Example1 = cppApplication + {
    defines = { "NDEBUG" }
    libs = { "jpeg" }
    includePaths = { "anypath1", "anypath2" }
    files = {
      "*.cpp" = cppSource
    }
  }
}
```

could be handled by mare and translated into project files for Visual Studio and build files for Make or cmake. The other translators (for CodeLite, CodeBlocks, NetBeans) are not yet advanced enough to support these build-in rules properly. However, these IDEs allow using an external build system and hence mare can be used as replacement for their primary build systems. The command line to build, rebuild or clean a target can be specified using the keys "buildCommand", "reBuildCommand" and "cleanCommand" within a target:

```
targets = {
  Example1 = cppApplication + {
    defines = { "NDEBUG" }
    libs = { "jpeg" }
    includePaths = { "anypath1", "anypath2" }
    files = {
      "*.cpp" = cppSource
    }
    
    if tool = "codelite" || tool == "codeblocks" {
      buildCommand = "./mare $(target) config=$(configuration)"
      cleanCommand = "./mare clean $(target) config=$(configuration)"
      reBuildCommand = "./mare rebuild $(target) config=$(configuration)"
    }
  }
}
```

Translators
-----------

A Marefile can be translated into project files for Visual Studio, CodeLite, CodeBlocks and NetBeans and into build files for cmake and Make. However currently, these translators are not fully functional. Here is a brief overview over the state of the development:
<table>
  <tr>
    <td></td>
    <td>mare&nbsp;1)</td>
    <td>vcxproj</td>
    <td>make</td>
    <td>codelite</td>
    <td>codeblocks</td>
    <td>cmake</td>
    <td>netbeans</td>
  </tr>
  <tr>
    <td>configurations</td>
    <td>works</td>
    <td>works</td>
    <td>works</td>
    <td>works</td>
    <td>works</td>
    <td></td>
    <td>geht</td>
  </tr>
  <tr>
    <td>platforms</td>
    <td>works</td>
    <td>?</td>
    <td>works</td>
    <td>?</td>
    <td>?</td>
    <td></td>
    <td></td>
  </tr>
  <tr>
    <td>{c,cpp}{Source,Application,DynamicLibrary,StaticLibrary}</td>
    <td>works</td>
    <td>works</td>
    <td>works</td>
    <td></td>
    <td></td>
    <td>works</td>
    <td></td>
  </tr>
  <tr>
    <td>{c,cpp}Compiler</td>
    <td>works</td>
    <td></td>
    <td>works</td>
    <td></td>
    <td></td>
    <td>works</td>
    <td></td>
  </tr>
  <tr>
    <td>linker</td>
    <td>works</td>
    <td></td>
    <td>works</td>
    <td></td>
    <td></td>
    <td></td>
    <td></td>
  </tr>
  <tr>
    <td>{build,clean,reBuild}Command</td>
    <td></td>
    <td>works</td>
    <td></td>
    <td>works</td>
    <td>works</td>
    <td></td>
    <td>geht</td>
  </tr>
</table>

1) not a translator
