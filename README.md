# Multi-Client Chat Server in C

This project implements a multi-client chat server using TCP socket programming and POSIX threads.

## Features

- Multiple clients can connect simultaneously
- Broadcast messaging between clients
- Username support
- Join and leave notifications

## Technologies Used

- C Programming
- TCP Socket Programming
- POSIX Threads (pthread)
- Linux (Ubuntu / WSL)

## How to Compile

gcc server.c -o server -lpthread
gcc client.c -o client -lpthread

## Run Server

./server

## Run Client

./client
