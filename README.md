# Corpy chat 

Corpy's chat application was developed in just one month by a committed team of five developers. During the development process, the team employed a range of advanced technologies and tools, 
including SQLite3 for effective data management, cJSON for smooth JSON parsing, OpenSSL for strong security measures, and GTK4 for crafting a modern and responsive user interface. Notably, 
GTK4 enabled the team to harness its robust features to create a visually appealing and user-friendly application for Linux.

## Installation and start working with app

### For Linux

***IMPORTANT*** - you need Ubuntu 2022 or newer version!

***IMPORTANT*** - minimal screen ration is 1280x720!

***IMPORTANT*** - after first admin added users, only then they can login in messenger

## First time setup
# First of all check is your operating system has all required libraries, for that follow this steps:
1. cd ./folder_of_project/
2. Run in console "./setup_dependencies.sh" (You might need password for installation)
# After previous steps done well
3. Run command: make

## To start server:
1. cd ./server/
2. ./uchat_server 8000(or other available port)
3. congratulation: YOU ARE AN ADMIN OF YOUR OWN MESSENGER
# Credentials login: FirstAdmin; password: -you can see it in console or in the file "./server/admin.txt"-
# Later you can change password in Corpy's chat application settings(which you can find after logining successfully in program).
# If FirstAdmin lost his password, you can recover it by deleting the file "./server/admin.txt" and rebooting server(new password will be created in new admin.txt file).

## To start client for FirstAdmin:
1. cd ./client/
2. ./uchat_client localhost 8000(the same port as for the server)
3. Enter login and password provide for FirstAdmin
4. Press setting icon, and create new user

***IMPORTANT*** - only admin can add new users, only admin can ban/unban users

***IMPORTANT*** - only admin can recover user password

## To start client:
1. cd ./client/
2. ./uchat_client localhost 8000(the same port as for the server)
3. Enter login and password provided from admin

### Functionalities:

## User:
- can chat with other users
- change own username and password
- create and delete groups
- add and delete users from group(if he/she is owner of that group)
- copy, delete and edit own messages
- can search for contact or group

## Admin:
- can do everything that user can
- can add new users/admins
- can ban/unban users accounts
- can recover user password
