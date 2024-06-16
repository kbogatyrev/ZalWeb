Build the NodeJS server using Visual Studio 2022 (C++ core) and Visual Studio Code (NodeJS add-on and JavaScript components)

1. Build the core component (Visual Studio 2022)   
-- Clone the Zal-Code repo from the GitHub: https://github.com/kbogatyrev/Zal-Core targeting a standard Windows folder   
-- Open the Zal-Core directory as a CMake project   
-- Select "WSL:Ubuntu-..." connection as the target system and "Linux Debug" or "Linux Release" as the current configuration   
-- Build the Linux Binary   

3. Build the Node C++ add-on (Visual Studio Code)   
-- Open an WSL Ubuntu connection   
-- Clone the Zal-Web repo from the GitHub: https://github.com/kbogatyrev/Zal-Web targeting a WSL Linux directory   
-- Use VSC to open the Zal-Web directory as a CMake project or --   
-- Open a WSL Ubuntu window and cd to Zal-Web   
-- Say **cmake-js** to build the NodeJS server   
-- Start the server by entering node **node index.js**   
