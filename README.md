# UberExporter3000-Mobile

UE3k-mobile is a particular "version" of [UE3k](https://github.com/Golax/UberExporter3000), it is one of the few if not the only app that can programmable transfer content from a portable device like a smartphone or a tablet to your pc via USB cable.

### Premise!

I do **NOT** guarantee the correct working of the app and/or the right expected results from any of the usage you make from it.
I had a really hard time to implement this, because I had to deal with the old MS WPD library and with the MS ecosystem wich were really *tedious* in my opinion, so I apologize for the REALLY BAD code or for the probably wrong approaches but in the end I was a bit tired.

### Installation

If you want to compile the code and create yourself the executable file you need to compile this solution with vs2015. A normal installation should provide you of all the libraries and dlls this project needs.
Or if you trust me, you can download and double click the executable file I made wich *should* be portable and inlcude all the dlls in itself. Link

### pattern.uber file

You need to write yourself the pattern.uber file wich link a specific extension with is respective destination folder you choose.
It is harder to say it than doing it:
```
//Example

Images //destination folder the will be created inside the root destination folder
.jpg .jpeg .png //specific files Images folder will contain

Music
.mp3
//see the patter.uber file in the repository for a complete example.
```

### Usage/workflow

- Once you double click the .exe it should display all the portable devices connected to your pc, choose the right one.
- It should now display the read pattern.uber file that you wrote, for a final check.
- Once you confirm it starts analyzing all the files in the device and then "cutting" them to the appropiate location defined in the pattern file. I tested it with a full 64GB device and the hole process *should* take from 1 to 2 hours.

### Final thought

Though UE3k-mobile is an ended project (I won't work on it for a while) I will be avaible for every request or advice or issues you may have.
