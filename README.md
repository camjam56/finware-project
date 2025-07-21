# Finware-Project - A mock stocks trading platform


## What the project is

This is my first 'completed github project', and so because of this it took me many weeks working on it directly, but also many days spent simply learning wtihout any action.

## My goal

My main production goal was to achieve a rough prototype of a mock-stocks trading platform, with working and well validated sign-in and register functionality, and with a database to store both users and trades made.

However, my primary goal in general was to learn; to fundamentally understand network programming and programming and safety, both in theory, and in practice in the language of C.


## Full-Stack Web App:

### Tech-Stack

**Front-End** : React, Vite, Node.js - JavaScript, CSS, HTML(jsx)
**Back-End** : C, SQLite - cJSON and sqlite3 libraries for C server


### Skills and Techniques

#### Back-End

**C**
I first attained a grasp with low-level programming fundamentals, understanding pointers fully and memory management. Another new concept for me was catching and checking for errors in the code manually instead of letting the code do it automatically.
Learning to make the code 'safe' was a big priority, and I remain a beginner in the world of C programming. Yet, in comparison to where I was when I started this project, I have come very far and feel very confident.  

**Network/Socket Programming**
As mentioned, lots of time was spent on theory, reading through documentation.
Specifically studying 'Beej's Guide to Network Programming: Using Internet Sockets' that can be found in scrollable single-page HTML format here: https://beej.us/guide/bgnet/html/.

Here I learned in-depth about TCP & UDP, IPv4 & IPv6, subnets & ports and byte orders. Shortly after I learned how this integrates into netwrok programming with C. 
For example, the arguments in getaddrinfo() and using socket(). Then using bind(), connect(), listen() and so forth. 
It was after I was deeper in to the project that I had to begin referring back to the documentation and researching further the slightly more advanced ideas in network programming.

**JSON Parsing using cJSON - HTTP Protocol Understanding - RESTful API**
Persevering with the desire to learn computing from the ground-up, I continued to work in my back-end in C, and I did so by parsing JSON from the front-end with cJSON.
This taught me not just the basics of the cJSON library, but also how messages accross HTTP are formatted, sent and received.
Incidentally, this approach led me to learn through bug fixing, research and trial and error issues to do with Cross-Origin Resource Sharing (CORS Prefilght Request errors).
After nearing the end of the project when more calls and requests were beign made from the front-end, I refactored my back-end code to be _RESTful_, which I also spent time learning about the benefits of before-hand.

**SQLite Database Integration**
Here I gained a lot of hands-on experience and familiarity with SQLite syntax, and also its workflow, such as creating 'init' files for more clean and modular workflows with databases.
My two databases are users.db, which I created earlier in the project and manually, and account_trades.db, which was made with an init file that can be found in the same directory. This was after learnign about better SQLite practices.
In the spirirt of learning cyber security and code security in general, I engaged in better than basic practices with my SQL commands, by utilising sqlite3_prepare_v2() in order to avoid simple SQL injection attacks.

**Password and Username Validation/Authentication**
On both the front-end and back-end, however more importantly security wise the back-end, I employed understanding of _Regex_. 
The purpose being to ensure best practices with usernames and passwords when registering, and I also endeavoured to create a duplicate username check by scanning the users.db database before writing the account to the database.

#### Front-End

**React.js**
I created three different main pages using both .jsx and .css components ('Login', 'Register', and 'Dashboard').
Utilising hooks such as useNaviagte, useState, useContext, useMemo etc, I created smoothly transitioning pages and global states (for user).
As mentioned in 'Password and Username Valdiation', I also learned to utilise the Regex function on the front-end to quick check for validity before requesting more from the server (efficiency and security purposes).
I also learned of the standard component lifecycle of Initialisation, Mount, Update, Unmount. 
Of course, in order for the front-end and back-end to communicate fluidly - or even at all for that matter - I spent lots of effort learning in and out the very basics of a fetch() based API from the front-end.

### Development Environment and Tools

**Git Version Control**
Engaged frequently in commits and messages, with multiple different branches for safety, in git version control. I even had a specific moment where I compiled my server file incorrectly and it deleted.
Only because of git was I able to pull down my code again, which taught me a very valuable lesson on the importance of version control.
On a side note, I utilised SSH keys for pushing and pulling my work from seperate devices as I worked from different locations. The project was made across Linux and MacOS.

**Linux (Fedora) - Bash**
As my entry to proffesional software engineering, I dual booted Linux on a second SSD and spent a fair bit of time (roughly a couple days) researching the differences between Linux and Windows.
I decided on Linux Fedora due to the type of future projects I will be working on, the frequent updates, and the choice of DE.
Through Linux I have learned _Bash_ to a decent level, I can confidently traverse my filebase through command line interface and create and delete directories and files where needed.
I have also utilisied the package management system and learned to install packages and libraries throguh command line.


### Summary

**This project deeply enhanced my proficiency in**

    -  Writing low-level networked back-end services in C

    -  Managing raw TCP socket communication and HTTP protocol details manually
 
    -  Using cJSON and SQLite in C for JSON and persistent storage handling
    
    -  Building a modern, user-friendly front-end with React.js and client-server API communication

    -  Navigating cross-platform development, package management, and compilation challenges

    -  Applying version control best practices with Git and GitHub


## How to run the code

1)  Clone the repository to your machine.

2)  Locate the server.c file in the back-end directory and compile it.
        
        -   Libraries Required: SQLite3 and cJSON

        -   When compiling on MacOS and Linux the command I used was:
            gcc server.c -o server -lsqlite3 -lcjson

3) Run the server file

        -   ./server in bash terminal will run it if in the right directory

4) Run the front-end

        -   In a new terminal window, locate the 'front-end' folder at path 'finware-project/front-end' and enter it

        -   Once in the front-end folder, you ust install Node Package Manager, typically with command 'npm install'

        -   Upon installation completion, execute command 'npm run dev' from the same directory

5) Open the Web App

        -   Now with the front-end and back-end open and running,  by using 'o + Enter' in the front-end terminal, the web app will open.



### Issues, Problems and Fixes for Future Projects

        -   After creating an account there is no instant login, one must return to the login screen and login with their new details
        
        -   Upon making a trade(s) in the Dashboard, one must Sign Out and re-login in order to see the new trade(s) appear on the Trade History Column

        -   As time went on and I worked more, my code became less commented and orderly, I intend to focus heavily in my next project and onwards to write extremely readable and comprehendable code!

        -   HTTP instead of HTTPS

        -   Plaintext password storage, meaning no encryption and therefore not practical and safe password management

        -   Despite using await and async well and having a continuous while loop in the back-end, it is a single threaded server, therefore only capable of one client at a time

        -   No session management or token system implemented, using web-tokens for user authentication would be more efficient and safer










