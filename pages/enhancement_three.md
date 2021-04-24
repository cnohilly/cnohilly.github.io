# Enhancement Three
## Databases 

### Explanation
For this category, I used the same artifact that I utilized for the first enhancement which was a personal project written in Spring of 2020. The project was a C++ program meant to be a stat tracker for a game called Fall Guys. This program was text-based in a console window where the user presses numerical inputs to make choices and selections to track stats. The game itself had no formal state tracking built in, at least none accessible to the players, so this was a solution for my self to track win/loss for various maps in the game. I chose this to show an ability to use SQL and databases and learn new things when I need to. 

### Enhancement Plan
The enhancements to be made to this artifact were to rewrite the program from C++ into Java and include an interface to a MySQL database using SQL. This is because I am comfortable with both C++ and Java but find Java to be easier to use to create user interfaces for programs to simplify use. In the C++ version, all of the stat tracking is done by reading and writing to a single text file, but in the Java version I could use a JDBC driver to interface with a database and use SQL to query and update data. I prefered using SQL to use a database with MySQL to store and manage the data because I believe it was a better solution than reading and writing a file to then store in arrays. I wrote the program to be able to use the database dynamically if I were to add additional maps or map-types to the database. It can not handle major edits to the database but it is flexible to handle a few additions.

### Outcomes
This enhancement of the artifact me allowed me to learn a completely new topic where I can interface with a database through my Java programs. Much of the challenges for the enhancement were learning and becoming familiar with this new topic and learning the way in which the driver for JDBC had to be used. The other minor part was refamiliarizing myself with the language a bit after not using it for a long period. This part did not take long because I was able to use older assignments and projects for examples. Overall, this was a great enhancement that provided a chance to use two languages or topics that I knew and take the challenge to learn to combine them. 

### Enhancement One Files
[Github containing C++ and Java Versions](https://github.com/cnohilly/cnohilly.github.io/tree/main/Enhancement_Three_Files)

### Navigation
- [Professional Self-Assessment](../index.md)
- [Enhancement One](./enhancement_one.md)
- [Enhancement Two](./enhancement_two.md)
