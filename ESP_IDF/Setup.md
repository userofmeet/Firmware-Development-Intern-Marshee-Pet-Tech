# Getting Started with ESP-IDF 5.4.1 – Blink Example

This guide will walk you through setting up ESP-IDF version 5.4.1 on Windows, building the **blink** example, and using Visual Studio Code for development.

---

## 🚀 Installation Steps

1. **Download the ESP-IDF Installer**

   Get the latest ESP-IDF Tools Installer for Windows (version 4.4 or above) from the official Espressif site:

   👉 [Download Installer](https://dl.espressif.com/dl/esp-idf/?idf=4.4)

2. **Installation Directory Structure**

   - Set the **ESP-IDF directory** to:
     ```
     C:\esp\esp-idf-v5.4.1
     ```

   - Set the **Destination location** for tools to:
     ```
     C:\esp\tools\espressif
     ```

3. **Important Note During Installation**
   - **Uncheck** the following option before completing the installation:
     ```
     Register the ESP-IDF Tools executables as Windows Defender exclusions
     ```

---

## 🛠 Building the Blink Example

1. Open the **ESP-IDF 5.4 CMD**.

2. Navigate to the `blink` example directory:
   ```bash
   cd C:\esp\esp-idf\examples\get-started\blink
   ```
3. Run the build command:
   ```bash
   idf.py build
   ```

## 💡 Using Visual Studio Code
1. Download and install [Visual Studio Code](https://code.visualstudio.com/)
2. Inside VS Code:
   - Go to File -> Open Foler
   - Select the folder:
     ```bash
     C:\esp\esp-idf\examples\get-started\blink
     ```

## 🧰 Youtube Video Reference
If you're facing issues with the IDF terminal in VS Code, refer to the following YouTube tutorials:

[Installing the ESP-IDF](https://youtu.be/5IuZ-E8Tmhg?feature=shared)

[Terminal Fix](https://youtu.be/N93RvZz6dEc?feature=shared)
   
