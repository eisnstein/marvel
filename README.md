# Query the Marvel API in your Terminal

## Setup
To be able to use this tool you need a Marvel Developer account. Register and copy your API pblic and private key into the .env file.
You also the the *openssl* lib installed (`sudo apt install openssl`).

`cp .env.example .env`     

## Build
`make`    

## Test
`make && make tests`    
You can also run the tests with *Valgrind* by directly calling the **runtests.sh** script.   
`VALGRIND=valgrind ./runtests.sh`    
The output will be stored in **./tests/error_log.txt**.

## Usage
`./build/marvel <query>`    
The `<query>` part is basically just the path of the API without the preceding */v1/public/*. (see also developer.marvel.com/docs) For example:    
`./build/marvel comics`     
`./build/marvel comics/{comicId}`     
`./build/marvel comics/{comicId}/characters`     
...

The JSON response will be stored in **output.json**
