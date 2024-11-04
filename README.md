# M/o/Vfuscator HTTP Server

A simple HTTP server that uses only the mov instruction, compiled with the M/o/Vfuscator. The server runs on port 8080 and returns "Hello, world".

## Setup

1. Install dependencies:
```bash
sudo apt update
sudo apt install build-essential git bison flex libgmp3-dev libmpfr-dev libmpc-dev texinfo
```

2. Build the compiler:
```bash
cd movfuscator
./build.sh
```

3. Compile the server:
```bash
cd ..
movfuscator/build/movcc -o server server.c
```

4. Run the server:
```bash
chmod +x server
./server
```

5. Test it:
```bash
curl http://localhost:8080/
```

You should see: `<html>hello, world</html>`

## Technical Details
### Implementations and Key Decisions Differing from the Original Code

#### Custom Network Structures and Constants
- **Reason**: The M/o/Vfuscator's movcc compiler cannot handle certain standard headers due to 64-bit constants and constructs.
- **Implementation**: Defined minimal versions of struct sockaddr_in, struct in_addr, and constants like AF_INET, SOCK_STREAM, and INADDR_ANY directly in server.c.

#### Custom Byte Order Functions
- **Reason**: Functions like htons and ntohs are either unsupported or rely on headers that cause issues.
- **Implementation**: Created my_htons and my_ntohs functions to handle byte order conversions.

#### Custom IP Address Conversion Function
- **Reason**: The standard inet_ntoa function depends on headers and implementations not compatible with movcc.
- **Implementation**: Wrote my_inet_ntoa to convert an IP address to a dotted-decimal string without external dependencies.

#### Manual HTTP Request Parsing
- **Reason**: Functions like sscanf may not be fully supported or could introduce complications.
- **Implementation**: Implemented manual parsing of the HTTP request to extract the method, URI, and version.

#### Compliance with C89/C90 Standards
- **Reason**: The movcc compiler adheres to the older C89/C90 standards, requiring variable declarations at the beginning of blocks.
- **Implementation**: Ensured all variables are declared at the start of their respective blocks.

#### Avoidance of Problematic Headers
- **Reason**: Inclusion of headers like <arpa/inet.h> and <netinet/in.h> caused compilation errors due to 64-bit constants and other unsupported features.
- **Implementation**: Excluded these headers and provided necessary functionality through custom code.

#### Simplification of Code
- **Reason**: To minimize compatibility issues with the movcc compiler.
- **Implementation**: Removed unnecessary code and kept the implementation as straightforward as possible.

## Troubleshooting

### If the server won't start:

1. Check if port 8080 is free:
   - Use `sudo lsof -i -P -n | grep LISTEN` to see if another service is using the port.
2. Try a different port:
   - Edit `#define PORT 8080` in server.c to a different port (e.g., 8081), recompile, and run the server again.
3. Verify localhost configuration:
   - Ensure that localhost or 127.0.0.1 is properly set up in your /etc/hosts file.
4. Check firewall settings:
   - Your firewall might be blocking connections to the port. Adjust the settings accordingly.

### If compilation fails:

1. Verify movcc path:
   - Ensure you are using the correct path to movcc: movfuscator/build/movcc.
2. Ensure all dependencies are installed:
   - Revisit the installation step for dependencies to confirm everything is installed.
3. Avoid external headers:
   - Do not include additional headers that may introduce incompatibilities.

## Additional Notes

### Original Implementation Differences

The original server code relied on standard networking headers and functions, such as <arpa/inet.h>, <netinet/in.h>, htons, ntohs, inet_ntoa, and sscanf. These were replaced or reimplemented to ensure compatibility with the M/o/Vfuscator.

### Custom Implementations

By providing custom definitions and functions, we eliminated the need for headers and functions that caused compilation issues, allowing us to successfully compile the server with movcc.

### Why These Changes Were Necessary

The M/o/Vfuscator's movcc compiler has limitations due to its adherence to older C standards and inability to handle certain constructs, especially those involving 64-bit constants or complex library functions. Adjusting the code accordingly was essential to achieve a successful compilation.

### Testing the Server

After starting the server, you can open a web browser and navigate to http://localhost:8080/ to see the "Hello, world" message.

### Stopping the Server

Since the server runs in an infinite loop, you can stop it by pressing Ctrl + C in the terminal where it's running.
